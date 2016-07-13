/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright (c) 1998, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2014, Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/vm.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <sys/kmem.h>
#include <sys/mem.h>
#include <sys/mman.h>
#include <sys/vnode.h>
#include <sys/errno.h>
#include <sys/memlist.h>
#include <sys/dumphdr.h>
#include <sys/dumpadm.h>
#include <sys/ksyms.h>
#include <sys/compress.h>
#include <sys/stream.h>
#include <sys/strsun.h>
#include <sys/cmn_err.h>
#include <sys/bitmap.h>
#include <sys/modctl.h>
#include <sys/utsname.h>
#include <sys/systeminfo.h>
#include <sys/vmem.h>
#include <sys/log.h>
#include <sys/var.h>
#include <sys/debug.h>
#include <sys/sunddi.h>
#include <fs/fs_subr.h>
#include <sys/fs/snode.h>
#include <sys/ontrap.h>
#include <sys/panic.h>
#include <sys/dkio.h>
#include <sys/vtoc.h>
#include <sys/errorq.h>
#include <sys/fm/util.h>
#include <sys/fs/zfs.h>

#include <vm/hat.h>
#include <vm/as.h>
#include <vm/page.h>
#include <vm/pvn.h>
#include <vm/seg.h>
#include <vm/seg_kmem.h>
#include <sys/clock_impl.h>
#include <sys/hold_page.h>

/*
 * exported vars
 */
kmutex_t	dump_lock;		/* lock for dump configuration */
dumphdr_t	*dumphdr;		/* dump header */
int		dump_conflags = DUMP_KERNEL; /* dump configuration flags */
vnode_t		*dumpvp;		/* dump device vnode pointer */
u_offset_t	dumpvp_size;		/* size of dump device, in bytes */
char		*dumppath;		/* pathname of dump device */
int		dump_timeout = 120;	/* timeout for dumping pages */
int		dump_timeleft;		/* portion of dump_timeout remaining */
int		dump_ioerr;		/* dump i/o error */
char		*dump_stack_scratch;	/* scratch area for saving stack summary */

/*
 * Tunables for dump.  These can be set via /etc/system.
 *
 * dump_metrics_on	if set, metrics are collected in the kernel, passed
 *	to savecore via the dump file, and recorded by savecore in
 *	METRICS.txt.
 */

/* tunables for pre-reserved heap */
uint_t dump_kmem_permap = 1024;
uint_t dump_kmem_pages = 8;

/*
 * Compression metrics are accumulated nano-second subtotals. The
 * results are normalized by the number of pages dumped. A report is
 * generated when dumpsys() completes and is saved in the dump image
 * after the trailing dump header.
 *
 * Metrics are always collected. Set the variable dump_metrics_on to
 * cause metrics to be saved in the crash file, where savecore will
 * save it in the file METRICS.txt.
 */
#define	PERPAGES \
	PERPAGE(bitmap) PERPAGE(map) PERPAGE(unmap) \
	PERPAGE(compress) \
	PERPAGE(write)

typedef struct perpage {
#define	PERPAGE(x) hrtime_t x;
	PERPAGES
#undef PERPAGE
} perpage_t;

/*
 * If dump_metrics_on is set to 1, the timing information is passed to
 * savecore via the crash file, where it is appended to the file
 * dump-dir/METRICS.txt.
 */
uint_t dump_metrics_on = 0;	/* set to 1 to enable recording metrics */

#define	HRSTART(v, m)		v##ts.m = gethrtime()
#define	HRSTOP(v, m)		v.m += gethrtime() - v##ts.m

static char dump_osimage_uuid[36 + 1];

#define	isdigit(ch)	((ch) >= '0' && (ch) <= '9')
#define	isxdigit(ch)	(isdigit(ch) || ((ch) >= 'a' && (ch) <= 'f') || \
			((ch) >= 'A' && (ch) <= 'F'))

/*
 * configuration vars for dumpsys
 */
typedef struct dumpcfg {
	char *page;			/* buffer for page copy */
	char *lzbuf;			/* lzjb output */

	char *cmap;			/* array of input (map) buffers */
	ulong_t *bitmap;		/* bitmap for marking pages to dump */
	pgcnt_t bitmapsize;		/* size of bitmap */
	pid_t *pids;			/* list of process IDs at dump time */

	/*
	 * statistics
	 */
	perpage_t perpage;		/* per page metrics */
	perpage_t perpagets;		/* per page metrics (timestamps) */
	pgcnt_t npages;			/* subtotal of pages dumped */
	pgcnt_t pages_mapped;		/* subtotal of pages mapped */
	pgcnt_t pages_used;		/* subtotal of pages used per map */
	size_t nwrite;			/* subtotal of bytes written */
	hrtime_t elapsed;		/* elapsed time when completed */
	hrtime_t iotime;		/* time spent writing nwrite bytes */
	hrtime_t iowait;		/* time spent waiting for output */
	hrtime_t iowaitts;		/* iowait timestamp */

	/*
	 * I/O buffer
	 *
	 * There is one I/O buffer used by dumpvp_write and dumvp_flush. It
	 * is sized according to the optimum device transfer speed.
	 */
	struct {
		vnode_t	*cdev_vp;	/* VCHR open of the dump device */
		len_t	vp_limit;	/* maximum write offset */
		offset_t vp_off;	/* current dump device offset */
		char	*cur;		/* dump write pointer */
		char	*start;		/* dump buffer address */
		char	*end;		/* dump buffer end */
		size_t	size;		/* size of dump buf in bytes */
		size_t	iosize;		/* best transfer size for device */
	} buf;
} dumpcfg_t;

static dumpcfg_t dumpcfg;	/* config vars */

/*
 * The dump I/O buffer must be at least one page, at most xfer_size bytes,
 * and should scale with physmem in between.  The transfer size passed in
 * will either represent a global default (maxphys) or the best size for the
 * device.  The size of the dump I/O buffer is limited by dumpbuf_limit (8MB
 * by default) because the dump performance saturates beyond a certain size.
 * The default is to select 1/4096 of the memory.
 */
static int	dumpbuf_fraction = 12;	/* memory size scale factor */
static size_t	dumpbuf_limit = 8 << 20;	/* max I/O buf size */

static size_t
dumpbuf_iosize(size_t xfer_size)
{
	size_t iosize = ptob(physmem >> dumpbuf_fraction);

	if (iosize < PAGESIZE)
		iosize = PAGESIZE;
	else if (iosize > xfer_size)
		iosize = xfer_size;
	if (iosize > dumpbuf_limit)
		iosize = dumpbuf_limit;
	return (iosize & PAGEMASK);
}

/*
 * resize the I/O buffer
 */
static void
dumpbuf_resize(void)
{
	char *old_buf = dumpcfg.buf.start;
	size_t old_size = dumpcfg.buf.size;
	char *new_buf;
	size_t new_size;

	ASSERT(MUTEX_HELD(&dump_lock));

	new_size = dumpbuf_iosize(MAX(dumpcfg.buf.iosize, maxphys));
	if (new_size <= old_size)
		return; /* no need to reallocate buffer */

	new_buf = kmem_alloc(new_size, KM_SLEEP);
	dumpcfg.buf.size = new_size;
	dumpcfg.buf.start = new_buf;
	dumpcfg.buf.end = new_buf + new_size;
	kmem_free(old_buf, old_size);
}

/*
 * dump_update_clevel is called when dumpadm configures the dump device.
 *	Allocate the minimum configuration for now.
 *
 * When the dump file is configured we reserve a minimum amount of
 * memory for use at crash time. But we reserve VA for all the memory
 * we really want in order to do the fastest dump possible. The VA is
 * backed by pages not being dumped, according to the bitmap. If
 * there is insufficient spare memory, however, we fall back to the
 * minimum.
 *
 * Live dump (savecore -L) always uses the minimum config.
 */
static void
dump_update_clevel()
{
	dumpcfg_t *old = &dumpcfg;
	dumpcfg_t newcfg = *old;
	dumpcfg_t *new = &newcfg;

	ASSERT(MUTEX_HELD(&dump_lock));

	/*
	 * Free the previously allocated bufs and VM.
	 */
	if (old->lzbuf)
		kmem_free(old->lzbuf, PAGESIZE);
	if (old->page)
		kmem_free(old->page, PAGESIZE);

	if (old->cmap)
		/* VM space for mapping pages */
		vmem_xfree(heap_arena, old->cmap, PAGESIZE);

	/*
	 * Allocate new data structures and buffers, and also figure the max
	 * desired size.
	 */
	new->lzbuf = kmem_alloc(PAGESIZE, KM_SLEEP);
	new->page = kmem_alloc(PAGESIZE, KM_SLEEP);

	new->cmap = vmem_xalloc(heap_arena, PAGESIZE, PAGESIZE,
				0, 0, NULL, NULL, VM_SLEEP);

	/*
	 * Reserve memory for kmem allocation calls made during crash
	 * dump.  The hat layer allocates memory for each mapping
	 * created, and the I/O path allocates buffers and data structs.
	 * Add a few pages for safety.
	 */
	kmem_dump_init(dump_kmem_permap + (dump_kmem_pages * PAGESIZE));

	/* set new config pointers */
	*old = *new;
}

/*
 * Define a struct memlist walker to optimize bitnum to pfn
 * lookup. The walker maintains the state of the list traversal.
 */
typedef struct dumpmlw {
	struct memlist	*mp;		/* current memlist */
	pgcnt_t		basenum;	/* bitnum base offset */
	pgcnt_t		mppages;	/* current memlist size */
	pgcnt_t		mpleft;		/* size to end of current memlist */
	pfn_t		mpaddr;		/* first pfn in memlist */
} dumpmlw_t;

/* initialize the walker */
static inline void
dump_init_memlist_walker(dumpmlw_t *pw)
{
	pw->mp = phys_install;
	pw->basenum = 0;
	pw->mppages = pw->mp->ml_size >> PAGESHIFT;
	pw->mpleft = pw->mppages;
	pw->mpaddr = pw->mp->ml_address >> PAGESHIFT;
}

/*
 * Lookup pfn given bitnum. The memlist can be quite long on some
 * systems (e.g.: one per board). To optimize sequential lookups, the
 * caller initializes and presents a memlist walker.
 */
static pfn_t
dump_bitnum_to_pfn(pgcnt_t bitnum, dumpmlw_t *pw)
{
	bitnum -= pw->basenum;
	while (pw->mp != NULL) {
		if (bitnum < pw->mppages) {
			pw->mpleft = pw->mppages - bitnum;
			return (pw->mpaddr + bitnum);
		}
		bitnum -= pw->mppages;
		pw->basenum += pw->mppages;
		pw->mp = pw->mp->ml_next;
		if (pw->mp != NULL) {
			pw->mppages = pw->mp->ml_size >> PAGESHIFT;
			pw->mpleft = pw->mppages;
			pw->mpaddr = pw->mp->ml_address >> PAGESHIFT;
		}
	}
	return (PFN_INVALID);
}

static pgcnt_t
dump_pfn_to_bitnum(pfn_t pfn)
{
	struct memlist *mp;
	pgcnt_t bitnum = 0;

	for (mp = phys_install; mp != NULL; mp = mp->ml_next) {
		if (pfn >= (mp->ml_address >> PAGESHIFT) &&
		    pfn < ((mp->ml_address + mp->ml_size) >> PAGESHIFT))
			return (bitnum + pfn - (mp->ml_address >> PAGESHIFT));
		bitnum += mp->ml_size >> PAGESHIFT;
	}
	return ((pgcnt_t)-1);
}

static void
dumphdr_init(void)
{
	pgcnt_t npages;

	ASSERT(MUTEX_HELD(&dump_lock));

	if (dumphdr == NULL) {
		dumphdr = kmem_zalloc(sizeof (dumphdr_t), KM_SLEEP);
		dumphdr->dump_magic = DUMP_MAGIC;
		dumphdr->dump_version = DUMP_VERSION;
		dumphdr->dump_wordsize = DUMP_WORDSIZE;
		dumphdr->dump_pageshift = PAGESHIFT;
		dumphdr->dump_pagesize = PAGESIZE;
		dumphdr->dump_utsname = utsname;
		(void) strcpy(dumphdr->dump_platform, platform);
		dumpcfg.buf.size = dumpbuf_iosize(maxphys);
		dumpcfg.buf.start = kmem_alloc(dumpcfg.buf.size, KM_SLEEP);
		dumpcfg.buf.end = dumpcfg.buf.start + dumpcfg.buf.size;
		dumpcfg.pids = kmem_alloc(v.v_proc * sizeof (pid_t), KM_SLEEP);
		dump_stack_scratch = kmem_alloc(STACK_BUF_SIZE, KM_SLEEP);
		(void) strncpy(dumphdr->dump_uuid, dump_get_uuid(),
		    sizeof (dumphdr->dump_uuid));
	}

	npages = num_phys_pages();

	if (dumpcfg.bitmapsize != npages) {
		void *map = kmem_alloc(BT_SIZEOFMAP(npages), KM_SLEEP);

		if (dumpcfg.bitmap != NULL)
			kmem_free(dumpcfg.bitmap, BT_SIZEOFMAP(dumpcfg.
			    bitmapsize));
		dumpcfg.bitmap = map;
		dumpcfg.bitmapsize = npages;
	}
}

/*
 * Establish a new dump device.
 */
int
dumpinit(vnode_t *vp, char *name, int justchecking)
{
	vnode_t *cvp;
	vattr_t vattr;
	vnode_t *cdev_vp;
	int error = 0;

	ASSERT(MUTEX_HELD(&dump_lock));

	dumphdr_init();

	cvp = common_specvp(vp);
	if (cvp == dumpvp)
		return (0);

	/*
	 * Determine whether this is a plausible dump device.  We want either:
	 * (1) a real device that's not mounted and has a cb_dump routine, or
	 * (2) a swapfile on some filesystem that has a vop_dump routine.
	 */
	if ((error = VOP_OPEN(&cvp, FREAD | FWRITE, kcred, NULL)) != 0)
		return (error);

	vattr.va_mask = AT_SIZE | AT_TYPE | AT_RDEV;
	if ((error = VOP_GETATTR(cvp, &vattr, 0, kcred, NULL)) == 0) {
		if (vattr.va_type == VBLK || vattr.va_type == VCHR) {
			if (devopsp[getmajor(vattr.va_rdev)]->
			    devo_cb_ops->cb_dump == nodev)
				error = ENOTSUP;
			else if (vfs_devismounted(vattr.va_rdev))
				error = EBUSY;
			if (strcmp(ddi_driver_name(VTOS(cvp)->s_dip),
			    ZFS_DRIVER) == 0 &&
			    IS_SWAPVP(common_specvp(cvp)))
				error = EBUSY;
		} else {
			if (vn_matchopval(cvp, VOPNAME_DUMP, fs_nosys) ||
			    !IS_SWAPVP(cvp))
				error = ENOTSUP;
		}
	}

	if (error == 0 && vattr.va_size < 2 * DUMP_LOGSIZE + DUMP_ERPTSIZE)
		error = ENOSPC;

	if (error || justchecking) {
		(void) VOP_CLOSE(cvp, FREAD | FWRITE, 1, (offset_t)0,
		    kcred, NULL);
		return (error);
	}

	VN_HOLD(cvp);

	if (dumpvp != NULL)
		dumpfini();	/* unconfigure the old dump device */

	dumpvp = cvp;
	dumpvp_size = vattr.va_size & -DUMP_OFFSET;
	dumppath = kmem_alloc(strlen(name) + 1, KM_SLEEP);
	(void) strcpy(dumppath, name);
	dumpcfg.buf.iosize = 0;

	/*
	 * If the dump device is a block device, attempt to open up the
	 * corresponding character device and determine its maximum transfer
	 * size.  We use this information to potentially resize dump buffer
	 * to a larger and more optimal size for performing i/o to the dump
	 * device.
	 */
	if (cvp->v_type == VBLK &&
	    (cdev_vp = makespecvp(VTOS(cvp)->s_dev, VCHR)) != NULL) {
		if (VOP_OPEN(&cdev_vp, FREAD | FWRITE, kcred, NULL) == 0) {
			size_t blk_size;
			struct dk_cinfo dki;
			struct dk_minfo minf;

			if (VOP_IOCTL(cdev_vp, DKIOCGMEDIAINFO,
			    (intptr_t)&minf, FKIOCTL, kcred, NULL, NULL)
			    == 0 && minf.dki_lbsize != 0)
				blk_size = minf.dki_lbsize;
			else
				blk_size = DEV_BSIZE;

			if (VOP_IOCTL(cdev_vp, DKIOCINFO, (intptr_t)&dki,
			    FKIOCTL, kcred, NULL, NULL) == 0) {
				dumpcfg.buf.iosize = dki.dki_maxtransfer * blk_size;
				dumpbuf_resize();
			}
			/*
			 * If we are working with a zvol then dumpify it
			 * if it's not being used as swap.
			 */
			if (strcmp(dki.dki_dname, ZVOL_DRIVER) == 0) {
				if (IS_SWAPVP(common_specvp(cvp)))
					error = EBUSY;
				else if ((error = VOP_IOCTL(cdev_vp,
				    DKIOCDUMPINIT, NULL, FKIOCTL, kcred,
				    NULL, NULL)) != 0)
					dumpfini();
			}

			(void) VOP_CLOSE(cdev_vp, FREAD | FWRITE, 1, 0,
			    kcred, NULL);
		}

		VN_RELE(cdev_vp);
	}

	cmn_err(CE_CONT, "?dump on %s size %llu MB\n", name, dumpvp_size >> 20);

	dump_update_clevel();

	return (error);
}

void
dumpfini(void)
{
	vattr_t vattr;
	boolean_t is_zfs = B_FALSE;
	vnode_t *cdev_vp;
	ASSERT(MUTEX_HELD(&dump_lock));

	kmem_free(dumppath, strlen(dumppath) + 1);

	/*
	 * Determine if we are using zvols for our dump device
	 */
	vattr.va_mask = AT_RDEV;
	if (VOP_GETATTR(dumpvp, &vattr, 0, kcred, NULL) == 0) {
		is_zfs = (getmajor(vattr.va_rdev) ==
		    ddi_name_to_major(ZFS_DRIVER)) ? B_TRUE : B_FALSE;
	}

	/*
	 * If we have a zvol dump device then we call into zfs so
	 * that it may have a chance to cleanup.
	 */
	if (is_zfs &&
	    (cdev_vp = makespecvp(VTOS(dumpvp)->s_dev, VCHR)) != NULL) {
		if (VOP_OPEN(&cdev_vp, FREAD | FWRITE, kcred, NULL) == 0) {
			(void) VOP_IOCTL(cdev_vp, DKIOCDUMPFINI, NULL, FKIOCTL,
			    kcred, NULL, NULL);
			(void) VOP_CLOSE(cdev_vp, FREAD | FWRITE, 1, 0,
			    kcred, NULL);
		}
		VN_RELE(cdev_vp);
	}

	(void) VOP_CLOSE(dumpvp, FREAD | FWRITE, 1, (offset_t)0, kcred, NULL);

	VN_RELE(dumpvp);

	dumpvp = NULL;
	dumpvp_size = 0;
	dumppath = NULL;
}

static offset_t
dumpvp_flush(void)
{
	size_t size = P2ROUNDUP(dumpcfg.buf.cur - dumpcfg.buf.start, PAGESIZE);
	hrtime_t iotime;
	int err;

	if (dumpcfg.buf.vp_off + size > dumpcfg.buf.vp_limit) {
		dump_ioerr = ENOSPC;
		dumpcfg.buf.vp_off = dumpcfg.buf.vp_limit;
	} else if (size != 0) {
		iotime = gethrtime();
		dumpcfg.iowait += iotime - dumpcfg.iowaitts;
		if (panicstr)
			err = VOP_DUMP(dumpvp, dumpcfg.buf.start,
			    lbtodb(dumpcfg.buf.vp_off), btod(size), NULL);
		else
			err = vn_rdwr(UIO_WRITE, dumpcfg.buf.cdev_vp != NULL ?
			    dumpcfg.buf.cdev_vp : dumpvp, dumpcfg.buf.start, size,
			    dumpcfg.buf.vp_off, UIO_SYSSPACE, 0, dumpcfg.buf.vp_limit,
			    kcred, 0);
		if (err && dump_ioerr == 0)
			dump_ioerr = err;
		dumpcfg.iowaitts = gethrtime();
		dumpcfg.iotime += dumpcfg.iowaitts - iotime;
		dumpcfg.nwrite += size;
		dumpcfg.buf.vp_off += size;
	}
	dumpcfg.buf.cur = dumpcfg.buf.start;
	dump_timeleft = dump_timeout;
	return (dumpcfg.buf.vp_off);
}

/* maximize write speed by keeping seek offset aligned with size */
void
dumpvp_write(const void *va, size_t size)
{
	size_t len, off, sz;

	while (size != 0) {
		len = MIN(size, dumpcfg.buf.end - dumpcfg.buf.cur);
		if (len == 0) {
			off = P2PHASE(dumpcfg.buf.vp_off, dumpcfg.buf.size);
			if (off == 0 || !ISP2(dumpcfg.buf.size)) {
				(void) dumpvp_flush();
			} else {
				sz = dumpcfg.buf.size - off;
				dumpcfg.buf.cur = dumpcfg.buf.start + sz;
				(void) dumpvp_flush();
				ovbcopy(dumpcfg.buf.start + sz, dumpcfg.buf.start, off);
				dumpcfg.buf.cur += off;
			}
		} else {
			bcopy(va, dumpcfg.buf.cur, len);
			va = (char *)va + len;
			dumpcfg.buf.cur += len;
			size -= len;
		}
	}
}

/*ARGSUSED*/
static void
dumpvp_ksyms_write(const void *src, void *dst, size_t size)
{
	dumpvp_write(src, size);
}

/*
 * Mark 'pfn' in the bitmap and dump its translation table entry.
 */
void
dump_addpage(struct as *as, void *va, pfn_t pfn)
{
	mem_vtop_t mem_vtop;
	pgcnt_t bitnum;

	if ((bitnum = dump_pfn_to_bitnum(pfn)) != (pgcnt_t)-1) {
		if (!BT_TEST(dumpcfg.bitmap, bitnum)) {
			dumphdr->dump_npages++;
			BT_SET(dumpcfg.bitmap, bitnum);
		}
		dumphdr->dump_nvtop++;
		mem_vtop.m_as = as;
		mem_vtop.m_va = va;
		mem_vtop.m_pfn = pfn;
		dumpvp_write(&mem_vtop, sizeof (mem_vtop_t));
	}
	dump_timeleft = dump_timeout;
}

/*
 * Mark 'pfn' in the bitmap
 */
void
dump_page(pfn_t pfn)
{
	pgcnt_t bitnum;

	if ((bitnum = dump_pfn_to_bitnum(pfn)) != (pgcnt_t)-1) {
		if (!BT_TEST(dumpcfg.bitmap, bitnum)) {
			dumphdr->dump_npages++;
			BT_SET(dumpcfg.bitmap, bitnum);
		}
	}
	dump_timeleft = dump_timeout;
}

/*
 * Dump the <as, va, pfn> information for a given address space.
 * SEGOP_DUMP() will call dump_addpage() for each page in the segment.
 */
static void
dump_as(struct as *as)
{
	struct seg *seg;

	AS_LOCK_ENTER(as, RW_READER);
	for (seg = AS_SEGFIRST(as); seg; seg = AS_SEGNEXT(as, seg)) {
		if (seg->s_as != as)
			break;
		if (seg->s_ops == NULL)
			continue;
		SEGOP_DUMP(seg);
	}
	AS_LOCK_EXIT(as);

	if (seg != NULL)
		cmn_err(CE_WARN, "invalid segment %p in address space %p",
		    (void *)seg, (void *)as);
}

static int
dump_process(pid_t pid)
{
	proc_t *p = sprlock(pid);

	if (p == NULL)
		return (-1);
	if (p->p_as != &kas) {
		mutex_exit(&p->p_lock);
		dump_as(p->p_as);
		mutex_enter(&p->p_lock);
	}

	sprunlock(p);

	return (0);
}

/*
 * The following functions (dump_summary(), dump_ereports(), and
 * dump_messages()), write data to an uncompressed area within the
 * crashdump. The layout of these is
 *
 * +------------------------------------------------------------+
 * |     compressed pages       | summary | ereports | messages |
 * +------------------------------------------------------------+
 *
 * With the advent of saving a compressed crash dump by default, we
 * need to save a little more data to describe the failure mode in
 * an uncompressed buffer available before savecore uncompresses
 * the dump. Initially this is a copy of the stack trace. Additional
 * summary information should be added here.
 */

void
dump_summary(void)
{
	u_offset_t dumpvp_start;
	summary_dump_t sd;

	if (dumpvp == NULL || dumphdr == NULL)
		return;

	dumpcfg.buf.cur = dumpcfg.buf.start;

	dumpcfg.buf.vp_limit = dumpvp_size - (DUMP_OFFSET + DUMP_LOGSIZE +
	    DUMP_ERPTSIZE);
	dumpvp_start = dumpcfg.buf.vp_limit - DUMP_SUMMARYSIZE;
	dumpcfg.buf.vp_off = dumpvp_start;

	sd.sd_magic = SUMMARY_MAGIC;
	sd.sd_ssum = checksum32(dump_stack_scratch, STACK_BUF_SIZE);
	dumpvp_write(&sd, sizeof (sd));
	dumpvp_write(dump_stack_scratch, STACK_BUF_SIZE);

	sd.sd_magic = 0; /* indicate end of summary */
	dumpvp_write(&sd, sizeof (sd));
	(void) dumpvp_flush();
}

void
dump_ereports(void)
{
	u_offset_t dumpvp_start;
	erpt_dump_t ed;

	if (dumpvp == NULL || dumphdr == NULL)
		return;

	dumpcfg.buf.cur = dumpcfg.buf.start;
	dumpcfg.buf.vp_limit = dumpvp_size - (DUMP_OFFSET + DUMP_LOGSIZE);
	dumpvp_start = dumpcfg.buf.vp_limit - DUMP_ERPTSIZE;
	dumpcfg.buf.vp_off = dumpvp_start;

	fm_ereport_dump();
	if (panicstr)
		errorq_dump();

	bzero(&ed, sizeof (ed)); /* indicate end of ereports */
	dumpvp_write(&ed, sizeof (ed));
	(void) dumpvp_flush();

	if (!panicstr) {
		(void) VOP_PUTPAGE(dumpvp, dumpvp_start,
		    (size_t)(dumpcfg.buf.vp_off - dumpvp_start),
		    B_INVAL | B_FORCE, kcred, NULL);
	}
}

void
dump_messages(void)
{
	log_dump_t ld;
	mblk_t *mctl, *mdata;
	queue_t *q, *qlast;
	u_offset_t dumpvp_start;

	if (dumpvp == NULL || dumphdr == NULL || log_consq == NULL)
		return;

	dumpcfg.buf.cur = dumpcfg.buf.start;
	dumpcfg.buf.vp_limit = dumpvp_size - DUMP_OFFSET;
	dumpvp_start = dumpcfg.buf.vp_limit - DUMP_LOGSIZE;
	dumpcfg.buf.vp_off = dumpvp_start;

	qlast = NULL;
	do {
		for (q = log_consq; q->q_next != qlast; q = q->q_next)
			continue;
		for (mctl = q->q_first; mctl != NULL; mctl = mctl->b_next) {
			dump_timeleft = dump_timeout;
			mdata = mctl->b_cont;
			ld.ld_magic = LOG_MAGIC;
			ld.ld_msgsize = MBLKL(mctl->b_cont);
			ld.ld_csum = checksum32(mctl->b_rptr, MBLKL(mctl));
			ld.ld_msum = checksum32(mdata->b_rptr, MBLKL(mdata));
			dumpvp_write(&ld, sizeof (ld));
			dumpvp_write(mctl->b_rptr, MBLKL(mctl));
			dumpvp_write(mdata->b_rptr, MBLKL(mdata));
		}
	} while ((qlast = q) != log_consq);

	ld.ld_magic = 0;		/* indicate end of messages */
	dumpvp_write(&ld, sizeof (ld));
	(void) dumpvp_flush();
	if (!panicstr) {
		(void) VOP_PUTPAGE(dumpvp, dumpvp_start,
		    (size_t)(dumpcfg.buf.vp_off - dumpvp_start),
		    B_INVAL | B_FORCE, kcred, NULL);
	}
}

/*
 * Copy pages, trapping ECC errors. Also, for robustness, trap data
 * access in case something goes wrong in the hat layer and the
 * mapping is broken.
 */
static void
dump_pagecopy(void *src, void *dst)
{
	long *wsrc = (long *)src;
	long *wdst = (long *)dst;
	const ulong_t ncopies = PAGESIZE / sizeof (long);
	volatile int w = 0;
	volatile int ueoff = -1;
	on_trap_data_t otd;

	if (on_trap(&otd, OT_DATA_EC | OT_DATA_ACCESS)) {
		if (ueoff == -1)
			ueoff = w * sizeof (long);
		/* report "bad ECC" or "bad address" */
#ifdef _LP64
		if (otd.ot_trap & OT_DATA_EC)
			wdst[w++] = 0x00badecc00badecc;
		else
			wdst[w++] = 0x00badadd00badadd;
#else
		if (otd.ot_trap & OT_DATA_EC)
			wdst[w++] = 0x00badecc;
		else
			wdst[w++] = 0x00badadd;
#endif
	}
	while (w < ncopies) {
		wdst[w] = wsrc[w];
		w++;
	}
	no_trap();
}

size_t
dumpsys_metrics(char *buf, size_t size)
{
	dumpcfg_t *cfg = &dumpcfg;
	int compress_ratio;
	int sec, iorate;
	char *e = buf + size;
	char *p = buf;

	sec = cfg->elapsed / (1000 * 1000 * 1000ULL);
	if (sec < 1)
		sec = 1;

	if (cfg->iotime < 1)
		cfg->iotime = 1;
	iorate = (cfg->nwrite * 100000ULL) / cfg->iotime;

	compress_ratio = 100LL * cfg->npages / btopr(cfg->nwrite + 1);

#define	P(...) (p += p < e ? snprintf(p, e - p, __VA_ARGS__) : 0)

	P("Master cpu_seqid,%d\n", CPU->cpu_seqid);
	P("Master cpu_id,%d\n", CPU->cpu_id);
	P("dump_flags,0x%x\n", dumphdr->dump_flags);
	P("dump_ioerr,%d\n", dump_ioerr);

	P("Compression type,serial lzjb\n");
	P("Compression ratio,%d.%02d\n", compress_ratio / 100, compress_ratio %
	    100);

	P("Dump I/O rate MBS,%d.%02d\n", iorate / 100, iorate % 100);
	P("..total bytes,%lld\n", (u_longlong_t)cfg->nwrite);
	P("..total nsec,%lld\n", (u_longlong_t)cfg->iotime);
	P("dumpbuf.iosize,%ld\n", dumpcfg.buf.iosize);
	P("dumpbuf.size,%ld\n", dumpcfg.buf.size);

	P("Dump pages/sec,%llu\n", (u_longlong_t)cfg->npages / sec);
	P("Dump pages,%llu\n", (u_longlong_t)cfg->npages);
	P("Dump time,%d\n", sec);

	if (cfg->pages_mapped > 0)
		P("per-cent map utilization,%d\n", (int)((100 * cfg->pages_used)
		    / cfg->pages_mapped));

	P("\nPer-page metrics:\n");
	if (cfg->npages > 0) {
#define	PERPAGE(x) \
		P("%s nsec/page,%d\n", #x, (int)(cfg->perpage.x / cfg->npages));
		PERPAGES;
#undef PERPAGE

		P("I/O wait nsec/page,%llu\n", (u_longlong_t)(cfg->iowait /
		    cfg->npages));
	}
#undef P
	if (p < e)
		bzero(p, e - p);
	return (p - buf);
}

/*
 * Dump the system.
 */
void
dumpsys(void)
{
	dumpcfg_t *cfg = &dumpcfg;
	uint_t percent_done;		/* dump progress reported */
	hrtime_t start;			/* start time */
	pfn_t pfn;
	pgcnt_t bitnum;
	proc_t *p;
	pid_t npids, pidx;
	char *content;
	char *buf;
	size_t size;
	dumpmlw_t mlw;
	dumpcsize_t datatag;
	dumpdatahdr_t datahdr;

	if (dumpvp == NULL || dumphdr == NULL) {
		uprintf("skipping system dump - no dump device configured\n");
		return;
	}
	dumpcfg.buf.cur = dumpcfg.buf.start;

	/* clear the sync variables */
	cfg->npages = 0;
	cfg->pages_mapped = 0;
	cfg->pages_used = 0;
	cfg->nwrite = 0;
	cfg->elapsed = 0;
	cfg->iotime = 0;
	cfg->iowait = 0;
	cfg->iowaitts = 0;

	/*
	 * Calculate the starting block for dump.  If we're dumping on a
	 * swap device, start 1/5 of the way in; otherwise, start at the
	 * beginning.  And never use the first page -- it may be a disk label.
	 */
	if (dumpvp->v_flag & VISSWAP)
		dumphdr->dump_start = P2ROUNDUP(dumpvp_size / 5, DUMP_OFFSET);
	else
		dumphdr->dump_start = DUMP_OFFSET;

	dumphdr->dump_flags = DF_VALID | DF_COMPLETE | DF_LIVE | DF_COMPRESSED;
	dumphdr->dump_crashtime = gethrestime_sec();
	dumphdr->dump_npages = 0;
	dumphdr->dump_nvtop = 0;
	bzero(dumpcfg.bitmap, BT_SIZEOFMAP(dumpcfg.bitmapsize));
	dump_timeleft = dump_timeout;

	if (panicstr) {
		dumphdr->dump_flags &= ~DF_LIVE;
		(void) VOP_DUMPCTL(dumpvp, DUMP_FREE, NULL, NULL);
		(void) VOP_DUMPCTL(dumpvp, DUMP_ALLOC, NULL, NULL);
		(void) vsnprintf(dumphdr->dump_panicstring, DUMP_PANICSIZE,
		    panicstr, panicargs);
	}

	if (dump_conflags & DUMP_ALL)
		content = "all";
	else if (dump_conflags & DUMP_CURPROC)
		content = "kernel + curproc";
	else
		content = "kernel";
	uprintf("dumping to %s, offset %lld, content: %s\n", dumppath,
	    dumphdr->dump_start, content);

	/* Make sure nodename is current */
	bcopy(utsname.nodename, dumphdr->dump_utsname.nodename, SYS_NMLN);

	/*
	 * If this is a live dump, try to open a VCHR vnode for better
	 * performance. We must take care to flush the buffer cache
	 * first.
	 */
	if (!panicstr) {
		vnode_t *cdev_vp, *cmn_cdev_vp;

		ASSERT(dumpcfg.buf.cdev_vp == NULL);
		cdev_vp = makespecvp(VTOS(dumpvp)->s_dev, VCHR);
		if (cdev_vp != NULL) {
			cmn_cdev_vp = common_specvp(cdev_vp);
			if (VOP_OPEN(&cmn_cdev_vp, FREAD | FWRITE, kcred, NULL)
			    == 0) {
				if (vn_has_cached_data(dumpvp))
					(void) pvn_vplist_dirty(dumpvp, 0, NULL,
					    B_INVAL | B_TRUNC, kcred);
				dumpcfg.buf.cdev_vp = cmn_cdev_vp;
			} else {
				VN_RELE(cdev_vp);
			}
		}
	}

	/*
	 * Store a hires timestamp so we can look it up during debugging.
	 */
	lbolt_debug_entry();

	/*
	 * Leave room for the message and ereport save areas and terminal dump
	 * header.
	 */
	dumpcfg.buf.vp_limit = dumpvp_size - DUMP_LOGSIZE - DUMP_OFFSET -
	    DUMP_ERPTSIZE;

	/*
	 * Write out the symbol table.  It's no longer compressed,
	 * so its 'size' and 'csize' are equal.
	 */
	dumpcfg.buf.vp_off = dumphdr->dump_ksyms = dumphdr->dump_start + PAGESIZE;
	dumphdr->dump_ksyms_size = dumphdr->dump_ksyms_csize =
	    ksyms_snapshot(dumpvp_ksyms_write, NULL, LONG_MAX);

	/*
	 * Write out the translation map.
	 */
	dumphdr->dump_map = dumpvp_flush();
	dump_as(&kas);
	dumphdr->dump_nvtop += dump_plat_addr();

	/*
	 * call into hat, which may have unmapped pages that also need to
	 * be in the dump
	 */
	hat_dump();

	if (dump_conflags & DUMP_ALL) {
		mutex_enter(&pidlock);

		for (npids = 0, p = practive; p != NULL; p = p->p_next)
			dumpcfg.pids[npids++] = p->p_pid;

		mutex_exit(&pidlock);

		for (pidx = 0; pidx < npids; pidx++)
			(void) dump_process(dumpcfg.pids[pidx]);

		dump_init_memlist_walker(&mlw);
		for (bitnum = 0; bitnum < dumpcfg.bitmapsize; bitnum++) {
			dump_timeleft = dump_timeout;
			pfn = dump_bitnum_to_pfn(bitnum, &mlw);
			/*
			 * Some hypervisors do not have all pages available to
			 * be accessed by the guest OS.  Check for page
			 * accessibility.
			 */
			if (plat_hold_page(pfn, PLAT_HOLD_NO_LOCK, NULL) !=
			    PLAT_HOLD_OK)
				continue;
			BT_SET(dumpcfg.bitmap, bitnum);
		}
		dumphdr->dump_npages = dumpcfg.bitmapsize;
		dumphdr->dump_flags |= DF_ALL;

	} else if (dump_conflags & DUMP_CURPROC) {
		/*
		 * Determine which pid is to be dumped.  If we're panicking, we
		 * dump the process associated with panic_thread (if any).  If
		 * this is a live dump, we dump the process associated with
		 * curthread.
		 */
		npids = 0;
		if (panicstr) {
			if (panic_thread != NULL &&
			    panic_thread->t_procp != NULL &&
			    panic_thread->t_procp != &p0) {
				dumpcfg.pids[npids++] =
				    panic_thread->t_procp->p_pid;
			}
		} else {
			dumpcfg.pids[npids++] = curthread->t_procp->p_pid;
		}

		if (npids && dump_process(dumpcfg.pids[0]) == 0)
			dumphdr->dump_flags |= DF_CURPROC;
		else
			dumphdr->dump_flags |= DF_KERNEL;

	} else {
		dumphdr->dump_flags |= DF_KERNEL;
	}

	dumphdr->dump_hashmask = (1 << highbit(dumphdr->dump_nvtop - 1)) - 1;

	/*
	 * Write out the pfn table.
	 */
	dumphdr->dump_pfn = dumpvp_flush();
	dump_init_memlist_walker(&mlw);
	for (bitnum = 0; bitnum < dumpcfg.bitmapsize; bitnum++) {
		dump_timeleft = dump_timeout;
		if (!BT_TEST(dumpcfg.bitmap, bitnum))
			continue;
		pfn = dump_bitnum_to_pfn(bitnum, &mlw);
		ASSERT(pfn != PFN_INVALID);
		dumpvp_write(&pfn, sizeof (pfn_t));
	}
	dump_plat_pfn();

	/*
	 * Write out all the pages.
	 * Map pages, copy them handling UEs, compress, and write them out.
	 */
	dumphdr->dump_data = dumpvp_flush();

	ASSERT(dumpcfg.page);
	bzero(&dumpcfg.perpage, sizeof (dumpcfg.perpage));

	start = gethrtime();
	cfg->iowaitts = start;

	if (panicstr)
		kmem_dump_begin();

	percent_done = 0;

	dump_init_memlist_walker(&mlw);
	for (bitnum = 0; bitnum < dumpcfg.bitmapsize; bitnum++) {
		dumpcsize_t csize;

		dump_timeleft = dump_timeout;
		HRSTART(cfg->perpage, bitmap);
		if (!BT_TEST(dumpcfg.bitmap, bitnum)) {
			HRSTOP(cfg->perpage, bitmap);
			continue;
		}
		HRSTOP(cfg->perpage, bitmap);

		pfn = dump_bitnum_to_pfn(bitnum, &mlw);
		ASSERT(pfn != PFN_INVALID);

		HRSTART(cfg->perpage, map);
		hat_devload(kas.a_hat, dumpcfg.cmap, PAGESIZE, pfn, PROT_READ,
			    HAT_LOAD_NOCONSIST);
		HRSTOP(cfg->perpage, map);

		dump_pagecopy(dumpcfg.cmap, dumpcfg.page);

		HRSTART(cfg->perpage, unmap);
		hat_unload(kas.a_hat, dumpcfg.cmap, PAGESIZE, HAT_UNLOAD);
		HRSTOP(cfg->perpage, unmap);

		HRSTART(dumpcfg.perpage, compress);
		csize = compress(dumpcfg.page, dumpcfg.lzbuf, PAGESIZE);
		HRSTOP(dumpcfg.perpage, compress);

		HRSTART(dumpcfg.perpage, write);
		dumpvp_write(&csize, sizeof (csize));
		dumpvp_write(dumpcfg.lzbuf, csize);
		HRSTOP(dumpcfg.perpage, write);

		if (dump_ioerr) {
			dumphdr->dump_flags &= ~DF_COMPLETE;
			dumphdr->dump_npages = cfg->npages;
			break;
		}
		if (++cfg->npages * 100LL / dumphdr->dump_npages > percent_done) {
			int sec;

			sec = (gethrtime() - start) / 1000 / 1000 / 1000;
			uprintf("^\r%2d:%02d %3d%% done", sec / 60, sec % 60,
				++percent_done);
			if (!panicstr)
				delay(1);	/* let the output be sent */
		}
	}

	cfg->elapsed = gethrtime() - start;
	if (cfg->elapsed < 1)
		cfg->elapsed = 1;

	/* record actual pages dumped */
	dumphdr->dump_npages = cfg->npages;

	/* platform-specific data */
	dumphdr->dump_npages += dump_plat_data(dumpcfg.page);

	/* note any errors by clearing DF_COMPLETE */
	if (dump_ioerr || cfg->npages < dumphdr->dump_npages)
		dumphdr->dump_flags &= ~DF_COMPLETE;

	/* end of stream blocks */
	datatag = 0;
	dumpvp_write(&datatag, sizeof (datatag));

	bzero(&datahdr, sizeof (datahdr));

	/* buffer for metrics */
	buf = dumpcfg.page;
	size = MIN(PAGESIZE, DUMP_OFFSET - sizeof (dumphdr_t) -
	    sizeof (dumpdatahdr_t));

	/* finish the kmem intercepts, collect kmem verbose info */
	if (panicstr) {
		datahdr.dump_metrics = kmem_dump_finish(buf, size);
		buf += datahdr.dump_metrics;
		size -= datahdr.dump_metrics;
	}

	/* record in the header whether this is a fault-management panic */
	if (panicstr)
		dumphdr->dump_fm_panic = is_fm_panic();

	/* compression info in data header */
	datahdr.dump_datahdr_magic = DUMP_DATAHDR_MAGIC;
	datahdr.dump_datahdr_version = DUMP_DATAHDR_VERSION;
	datahdr.dump_maxcsize = PAGESIZE;
	datahdr.dump_maxrange = 1;
	datahdr.dump_nstreams = 1;
	datahdr.dump_clevel = 0;

	if (dump_metrics_on)
		datahdr.dump_metrics += dumpsys_metrics(buf, size);

	datahdr.dump_data_csize = dumpvp_flush() - dumphdr->dump_data;

	/*
	 * Write out the initial and terminal dump headers.
	 */
	dumpcfg.buf.vp_off = dumphdr->dump_start;
	dumpvp_write(dumphdr, sizeof (dumphdr_t));
	(void) dumpvp_flush();

	dumpcfg.buf.vp_limit = dumpvp_size;
	dumpcfg.buf.vp_off = dumpcfg.buf.vp_limit - DUMP_OFFSET;
	dumpvp_write(dumphdr, sizeof (dumphdr_t));
	dumpvp_write(&datahdr, sizeof (dumpdatahdr_t));
	dumpvp_write(dumpcfg.page, datahdr.dump_metrics);

	(void) dumpvp_flush();

	uprintf("\r%3d%% done: %llu pages dumped, ",
	    percent_done, (u_longlong_t)cfg->npages);

	if (dump_ioerr == 0) {
		uprintf("dump succeeded\n");
	} else {
		uprintf("dump failed: error %d\n", dump_ioerr);
#ifdef DEBUG
		if (panicstr)
			debug_enter("dump failed");
#endif
	}

	/*
	 * Write out all undelivered messages.  This has to be the *last*
	 * thing we do because the dump process itself emits messages.
	 */
	if (panicstr) {
		dump_summary();
		dump_ereports();
		dump_messages();
	}

	delay(2 * hz);	/* let people see the 'done' message */
	dump_timeleft = 0;
	dump_ioerr = 0;

	/* restore settings after live dump completes */
	if (!panicstr) {
		/* release any VCHR open of the dump device */
		if (dumpcfg.buf.cdev_vp != NULL) {
			(void) VOP_CLOSE(dumpcfg.buf.cdev_vp, FREAD | FWRITE, 1, 0,
			    kcred, NULL);
			VN_RELE(dumpcfg.buf.cdev_vp);
			dumpcfg.buf.cdev_vp = NULL;
		}
	}
}

/*
 * This function is called whenever the memory size, as represented
 * by the phys_install list, changes.
 */
void
dump_resize()
{
	mutex_enter(&dump_lock);
	dumphdr_init();
	dumpbuf_resize();
	dump_update_clevel();
	mutex_exit(&dump_lock);
}

/*
 * This function allows for dynamic resizing of a dump area. It assumes that
 * the underlying device has update its appropriate size(9P).
 */
int
dumpvp_resize()
{
	int error;
	vattr_t vattr;

	mutex_enter(&dump_lock);
	vattr.va_mask = AT_SIZE;
	if ((error = VOP_GETATTR(dumpvp, &vattr, 0, kcred, NULL)) != 0) {
		mutex_exit(&dump_lock);
		return (error);
	}

	if (error == 0 && vattr.va_size < 2 * DUMP_LOGSIZE + DUMP_ERPTSIZE) {
		mutex_exit(&dump_lock);
		return (ENOSPC);
	}

	dumpvp_size = vattr.va_size & -DUMP_OFFSET;
	mutex_exit(&dump_lock);
	return (0);
}

int
dump_set_uuid(const char *uuidstr)
{
	const char *ptr;
	int i;

	if (uuidstr == NULL || strnlen(uuidstr, 36 + 1) != 36)
		return (EINVAL);

	/* uuid_parse is not common code so check manually */
	for (i = 0, ptr = uuidstr; i < 36; i++, ptr++) {
		switch (i) {
		case 8:
		case 13:
		case 18:
		case 23:
			if (*ptr != '-')
				return (EINVAL);
			break;

		default:
			if (!isxdigit(*ptr))
				return (EINVAL);
			break;
		}
	}

	if (dump_osimage_uuid[0] != '\0')
		return (EALREADY);

	(void) strncpy(dump_osimage_uuid, uuidstr, 36 + 1);

	cmn_err(CE_CONT, "?This Solaris instance has UUID %s\n",
	    dump_osimage_uuid);

	return (0);
}

const char *
dump_get_uuid(void)
{
	return (dump_osimage_uuid[0] != '\0' ? dump_osimage_uuid : "");
}
