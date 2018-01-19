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
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 * Copyright 2015 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 */


/*
 * Ramdisk device driver.
 *
 * There are three types of ramdisk:
 *
 * (1) 'virtual' OBP-created ramdisks created with "address" and "size"
 *     properties describing the virtual address range used by the ramdisk
 *     (used on SPARC during boot),
 * (2) 'physical' OBP-created ramdisks created with an "existing" property
 *     describing a scather-gather list physical address ranges used by the
 *     ramdisk (used on x86 for ramdisk-based root),
 * (3) 'pseudo' ramdisks created at runtime backed by an explicit list of
 *     pages.
 *
 * Unlike virtual and physical ramdisks, pseudo ramdisks have no
 * corresponding OBP device node.  The ramdisk(7D) driver is capable of
 * dealing with all three, including with the creation and deletion of
 * 'pseudo' ramdisks.
 *
 * Every ramdisk has a single 'state' structure which maintains data for
 * that ramdisk, and is assigned a single minor number.  The bottom 10-bits
 * of the minor number index the state structures; the top 8-bits give a
 * OBP-disk number, i.e. they are zero for 'pseudo' ramdisks.
 *
 * Each OBP-created ramdisk (i.e., virtual or physical) has its own node in
 * the device tree with an "address" and "size" (for virtual ramdisks) or
 * "existing" (for physical ramdisks) properties which describe the memory
 * assigned to the ramdisk.  All 'pseudo' ramdisks share a common devinfo
 * structure.
 *
 * A single character device node is used by ramdiskadm(1M) to communicate
 * with the ramdisk driver, with minor number 0:
 *
 *	/dev/ramdiskctl -> /devices/pseudo/ramdisk@0:ctl
 *
 * For consistent access, block and raw device nodes are created for *every*
 * ramdisk.  For 'pseudo' ramdisks:
 *
 *	/dev/ramdisk/<diskname>  -> /devices/pseudo/ramdisk@0:<diskname>
 *	/dev/rramdisk/<diskname> -> /devices/pseudo/ramdisk@0:<diskname>,raw
 *
 * For OBP-created ramdisks:
 *
 *	/dev/ramdisk/<diskname>  -> /devices/ramdisk-<diskname>:a
 *	/dev/ramdisk/<diskname>  -> /devices/ramdisk-<diskname>:a,raw
 *
 * This allows the transition from the standalone to the kernel to proceed
 * when booting from a ramdisk, and for the installation to correctly identify
 * the root device.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/errno.h>
#include <sys/uio.h>
#include <sys/buf.h>
#include <sys/modctl.h>
#include <sys/open.h>
#include <sys/kmem.h>
#include <sys/poll.h>
#include <sys/conf.h>
#include <sys/cmn_err.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>
#include <sys/ramdisk.h>
#include <vm/seg_kmem.h>
#include <vm/seg_kpm.h>

struct rd_map {
	caddr_t vaddr;		/* window base address */
	size_t size;		/* size of the virtual window */
	offset_t base;		/* device offset */
	pfn_t pfn;		/* physical "address" */
	boolean_t mapped;	/* is the rest of this struct valid? */
	boolean_t kpm;		/* mapped using kpm */
};

struct rd_ops {
	int (*alloc)(rd_devstate_t *);
	void (*dealloc)(rd_devstate_t *);
	void (*map)(rd_devstate_t *, off_t, struct rd_map *);
	void (*unmap)(rd_devstate_t *, struct rd_map *);
};

/*
 * Flag to disable the use of real ramdisks (in the OBP - on Sparc) when
 * the associated memory is no longer available - set in the bootops section.
 */

/*
 * An opaque handle where information about our set of ramdisk devices lives.
 */
static void	*rd_statep;

/*
 * Pointer to devinfo for the 'pseudo' ramdisks.  Real OBP-created ramdisks
 * get their own individual devinfo.
 */
static dev_info_t *rd_dip = NULL;

/*
 * Global state lock.
 */
static kmutex_t	rd_lock;

/*
 * Maximum number of ramdisks supported by this driver.
 */
static uint32_t	rd_max_disks = RD_DFLT_DISKS;

/*
 * Percentage of physical memory which can be assigned to pseudo ramdisks,
 * what that equates to in pages, and how many pages are currently assigned.
 */
static uint_t	rd_percent_physmem = RD_DEFAULT_PERCENT_PHYSMEM;
static pgcnt_t	rd_max_physmem;
static pgcnt_t	rd_tot_physmem;

static uint_t	rd_maxphys = RD_DEFAULT_MAXPHYS;

/*
 * Is the driver busy, i.e. are there any pseudo ramdisk devices in existence?
 */
static int
rd_is_busy(void)
{
	minor_t	minor;
	rd_devstate_t	*rsp;

	ASSERT(mutex_owned(&rd_lock));
	for (minor = 1; minor <= rd_max_disks; ++minor) {
		if ((rsp = ddi_get_soft_state(rd_statep, minor)) != NULL &&
		    rsp->rd_dip == rd_dip) {
			return (EBUSY);
		}
	}
	return (0);
}

/*
 * Find the first free minor number; returns zero if there isn't one.
 */
static minor_t
rd_find_free_minor(void)
{
	minor_t	minor;

	ASSERT(mutex_owned(&rd_lock));
	for (minor = 1; minor <= rd_max_disks; ++minor) {
		if (ddi_get_soft_state(rd_statep, minor) == NULL) {
			return (minor);
		}
	}
	return (0);
}

/*
 * Locate the rd_devstate for the named ramdisk; returns NULL if not found.
 * Each ramdisk is identified uniquely by name, i.e. an OBP-created ramdisk
 * cannot have the same name as a pseudo ramdisk.
 */
static rd_devstate_t *
rd_find_named_disk(char *name)
{
	minor_t		minor;
	rd_devstate_t	*rsp;

	ASSERT(mutex_owned(&rd_lock));
	for (minor = 1; minor <= rd_max_disks; ++minor) {
		if ((rsp = ddi_get_soft_state(rd_statep, minor)) != NULL &&
		    strcmp(rsp->rd_name, name) == 0) {
			return (rsp);
		}
	}
	return (NULL);
}

/*
 * Locate the rd_devstate for the OBP-created ramdisk whose devinfo is
 * referenced by 'dip'; returns NULL if not found (shouldn't happen).
 */
static rd_devstate_t *
rd_find_dip_state(dev_info_t *dip)
{
	minor_t		minor;
	rd_devstate_t	*rsp;

	ASSERT(mutex_owned(&rd_lock));
	for (minor = 1; minor <= rd_max_disks; ++minor) {
		if ((rsp = ddi_get_soft_state(rd_statep, minor)) != NULL &&
		    rsp->rd_dip == dip) {
			return (rsp);
		}
	}
	return (NULL);
}

/*
 * Is the ramdisk open?
 */
static int
rd_is_open(rd_devstate_t *rsp)
{
	ASSERT(mutex_owned(&rd_lock));
	return (rsp->rd_chr_open || rsp->rd_blk_open || rsp->rd_lyr_open_cnt);
}

/*
 * Mark the ramdisk open.
 */
static int
rd_opened(rd_devstate_t *rsp, int otyp)
{
	ASSERT(mutex_owned(&rd_lock));
	switch (otyp) {
	case OTYP_CHR:
		rsp->rd_chr_open = 1;
		break;
	case OTYP_BLK:
		rsp->rd_blk_open = 1;
		break;
	case OTYP_LYR:
		rsp->rd_lyr_open_cnt++;
		break;
	default:
		return (-1);
	}
	return (0);
}

/*
 * Mark the ramdisk closed.
 */
static void
rd_closed(rd_devstate_t *rsp, int otyp)
{
	ASSERT(mutex_owned(&rd_lock));
	switch (otyp) {
	case OTYP_CHR:
		rsp->rd_chr_open = 0;
		break;
	case OTYP_BLK:
		rsp->rd_blk_open = 0;
		break;
	case OTYP_LYR:
		rsp->rd_lyr_open_cnt--;
		break;
	default:
		break;
	}
}

static void
rd_init_tuneables(void)
{
	char	*prop, *p;

	/*
	 * Ensure sanity of 'rd_max_disks', which may be tuned in ramdisk.conf.
	 */
	if (ddi_prop_lookup_string(DDI_DEV_T_ANY, rd_dip, 0,
	    "max_disks", &prop) == DDI_PROP_SUCCESS) {
		p = prop;
		rd_max_disks = (uint32_t)stoi(&p);
		ddi_prop_free(prop);
	}
	if (rd_max_disks >= RD_MAX_DISKS) {
		cmn_err(CE_WARN, "ramdisk: rd_max_disks (%u) too big;"
		    " using default (%u).", rd_max_disks, RD_MAX_DISKS - 1);

		rd_max_disks = RD_MAX_DISKS - 1;
	}

	/*
	 * Ensure sanity of 'rd_percent_physmem', which may be tuned
	 * in ramdisk.conf.
	 */
	if (ddi_prop_lookup_string(DDI_DEV_T_ANY, rd_dip, 0,
	    "percent_physmem", &prop) == DDI_PROP_SUCCESS) {
		p = prop;
		rd_percent_physmem = (uint_t)stoi(&p);
		ddi_prop_free(prop);
	}
	if (rd_percent_physmem >= 100) {
		cmn_err(CE_WARN, "ramdisk: rd_percent_physmem (%u) >= 100;"
		    " using default (%u%%).", rd_percent_physmem,
		    RD_DEFAULT_PERCENT_PHYSMEM);

		rd_percent_physmem = RD_DEFAULT_PERCENT_PHYSMEM;
	}

	/*
	 * Since availrmem_initial is a long, this won't overflow.
	 */
	rd_max_physmem = (availrmem_initial * rd_percent_physmem) / 100;
}

/*
 * Allocate enough physical pages to hold "npages" pages.  Returns an
 * array of page_t * pointers that can later be mapped in or out via
 * rd_{un}map_window() but is otherwise opaque, or NULL on failure.
 */
static page_t **
rd_phys_alloc(pgcnt_t npages)
{
	page_t		*pp, **ppa;
	spgcnt_t	i;
	size_t		ppalen;
	struct seg	kseg;
	caddr_t		addr;		/* For coloring */

	if (rd_tot_physmem + npages > rd_max_physmem)
		return (NULL);

	if (!page_resv(npages, KM_NOSLEEP))
		return (NULL);

	if (!page_create_wait(npages, 0)) {
		page_unresv(npages);
		return (NULL);
	}

	ppalen = npages * sizeof (struct page_t *);
	ppa = kmem_zalloc(ppalen, KM_NOSLEEP);
	if (ppa == NULL) {
		page_create_putback(npages);
		page_unresv(npages);
		return (NULL);
	}

	kseg.s_as = &kas;
	for (i = 0, addr = NULL; i < npages; ++i, addr += PAGESIZE) {
		pp = page_get_freelist(&kvp.v_object, 0, &kseg, addr, PAGESIZE,
				       0, NULL);
		if (pp == NULL) {
			pp = page_get_cachelist(&kvp.v_object, 0, &kseg, addr,
						0, NULL);
			if (pp == NULL)
				goto out;
			if (!PP_ISAGED(pp))
				page_hashout(pp, false);
		}

		PP_CLRFREE(pp);
		PP_CLRAGED(pp);
		ppa[i] = pp;
	}

	for (i = 0; i < npages; i++)
		page_downgrade(ppa[i]);
	rd_tot_physmem += npages;

	return (ppa);

out:
	ASSERT(i < npages);
	page_create_putback(npages - i);
	while (--i >= 0)
		page_free(ppa[i], 0);
	kmem_free(ppa, ppalen);
	page_unresv(npages);

	return (NULL);
}

/*
 * Free physical pages previously allocated via rd_phys_alloc(); note that
 * this function may block as it has to wait until it can exclusively lock
 * all the pages first.
 */
static void
rd_phys_free(page_t **ppa, pgcnt_t npages)
{
	pgcnt_t	i;
	size_t	ppalen = npages * sizeof (struct page_t *);

	for (i = 0; i < npages; ++i) {
		if (! page_tryupgrade(ppa[i])) {
			page_unlock(ppa[i]);
			while (! page_lock(ppa[i], SE_EXCL, NULL, P_RECLAIM))
				;
		}
		page_free(ppa[i], 0);
	}

	kmem_free(ppa, ppalen);

	page_unresv(npages);
	rd_tot_physmem -= npages;
}

/*
 * Remove a window mapping (if present).
 */
static void
rdop_unmap_physical(rd_devstate_t *rsp, struct rd_map *map)
{
	if (map->kpm) {
		hat_kpm_mapout_pfn(map->pfn);
	} else {
		hat_unload(kas.a_hat, map->vaddr, map->size, HAT_UNLOAD_UNLOCK);
		vmem_free(heap_arena, map->vaddr, map->size);
	}
}

static void
rdop_unmap_pseudo(rd_devstate_t *rsp, struct rd_map *map)
{
	if (map->kpm) {
		pgcnt_t offpgs = btop(map->base);

		hat_kpm_mapout(rsp->rd_ppa[offpgs], NULL, map->vaddr);
	} else {
		hat_unload(kas.a_hat, map->vaddr, map->size, HAT_UNLOAD_UNLOCK);
		vmem_free(heap_arena, map->vaddr, map->size);
	}
}

static void
rdop_unmap(rd_devstate_t *rsp, struct rd_map *map)
{
	if (!map->mapped)
		return;

	if (rsp->rd_ops->unmap)
		rsp->rd_ops->unmap(rsp, map);

	map->mapped = B_FALSE;
}

/*
 * Map a portion of the ramdisk into the virtual window.
 */
static void
rdop_map_virtual(rd_devstate_t *rsp, off_t offset, struct rd_map *map)
{
	/*
	 * The whole range is already mapped.
	 */
	map->base   = 0;
	map->size   = rsp->rd_size;
	map->vaddr  = rsp->rd_obp_virt;
	map->mapped = B_TRUE;
}

static void
rdop_map_physical(rd_devstate_t *rsp, off_t offset, struct rd_map *map)
{
	boolean_t used_kpm = B_TRUE;
	pgcnt_t offpgs = btop(offset);
	caddr_t vaddr = NULL;
	uint_t i;

	map->base = ptob(offpgs);
	map->size = PAGESIZE;

	/*
	 * Physical ramdisk: locate the physical range which contains this
	 * offset.
	 */
	for (i = 0; i < rsp->rd_nexisting; ++i) {
		if (offset < rsp->rd_existing[i].size)
			break;

		offset -= rsp->rd_existing[i].size;
	}
	ASSERT3U(i, <, rsp->rd_nexisting);

	map->pfn = btop(rsp->rd_existing[i].phys + offset);

	/*
	 * Load the mapping.
	 */
	if (kpm_enable)
		vaddr = hat_kpm_mapin_pfn(map->pfn);

	if (!vaddr) {
		used_kpm = B_FALSE;
		vaddr = vmem_alloc(heap_arena, map->size, VM_SLEEP);

		/* XXX: support more than 1 page map */
		hat_devload(kas.a_hat, vaddr, map->size, map->pfn,
		    PROT_READ | PROT_WRITE,
		    HAT_LOAD_NOCONSIST | HAT_LOAD_LOCK);
	}

	map->vaddr = vaddr;
	map->kpm = used_kpm;
	map->mapped = B_TRUE;
}

static void
rdop_map_pseudo(rd_devstate_t *rsp, off_t offset, struct rd_map *map)
{
	boolean_t used_kpm = B_TRUE;
	pgcnt_t	offpgs = btop(offset);
	caddr_t	vaddr = NULL;

	map->base = ptob(offpgs);
	map->size = PAGESIZE;

	/*
	 * Load the mapping.
	 */
	if (kpm_enable)
		vaddr = hat_kpm_mapin(rsp->rd_ppa[offpgs], NULL);

	if (!vaddr) {
		used_kpm = B_FALSE;
		vaddr = vmem_alloc(heap_arena, map->size, VM_SLEEP);

		/* XXX: support more than 1 page map */
		hat_memload(kas.a_hat, vaddr, rsp->rd_ppa[offpgs],
		    PROT_READ | PROT_WRITE | HAT_NOSYNC, HAT_LOAD_LOCK);
	}

	map->vaddr = vaddr;
	map->kpm = used_kpm;
	map->mapped = B_TRUE;
}

static void
rdop_map(rd_devstate_t *rsp, off_t offset, struct rd_map *map)
{
	if (map->mapped) {
		/*
		 * Already mapped; is offset within our window?
		 */
		if (offset >= map->base && offset < map->base + map->size)
			return;

		/*
		 * No, we need to re-map; toss the old mapping.
		 */
		rdop_unmap(rsp, map);
	}

	ASSERT3U(map->mapped, ==, B_FALSE);

	rsp->rd_ops->map(rsp, offset, map);
}

static int
rdop_alloc_obp(rd_devstate_t *rsp)
{
	/*
	 * For OBP-created ramdisks the device nodes are:
	 *
	 *	/devices/ramdisk-<diskname>:a
	 *	/devices/ramdisk-<diskname>:a,raw
	 */
	if (ddi_create_minor_node(rsp->rd_dip, "a", S_IFBLK, rsp->rd_minor,
	    DDI_PSEUDO, 0) == DDI_FAILURE)
		return (1);

	if (ddi_create_minor_node(rsp->rd_dip, "a,raw", S_IFCHR, rsp->rd_minor,
	    DDI_PSEUDO, 0) == DDI_FAILURE)
		return (1);

	return (0);
}

static int
rdop_alloc_pseudo(rd_devstate_t *rsp)
{
	char namebuf[RD_NAME_LEN + 5];

	rsp->rd_npages = btopr(rsp->rd_size);
	rsp->rd_ppa = rd_phys_alloc(rsp->rd_npages);
	if (rsp->rd_ppa == NULL)
		return (1);

	/*
	 * For non-OBP ramdisks the device nodes are:
	 *
	 *	/devices/pseudo/ramdisk@0:<diskname>
	 *	/devices/pseudo/ramdisk@0:<diskname>,raw
	 */
	(void) snprintf(namebuf, sizeof (namebuf), "%s", rsp->rd_name);
	if (ddi_create_minor_node(rsp->rd_dip, namebuf, S_IFBLK, rsp->rd_minor,
	    DDI_PSEUDO, 0) == DDI_FAILURE)
		return (1);

	(void) snprintf(namebuf, sizeof (namebuf), "%s,raw", rsp->rd_name);
	if (ddi_create_minor_node(rsp->rd_dip, namebuf, S_IFCHR, rsp->rd_minor,
	    DDI_PSEUDO, 0) == DDI_FAILURE)
		return (1);

	return (0);
}

static int
rdop_alloc(rd_devstate_t *rsp)
{
	return (rsp->rd_ops->alloc(rsp));
}

static void
rdop_dealloc_obp(rd_devstate_t *rsp)
{
	ddi_remove_minor_node(rsp->rd_dip, "a");
	ddi_remove_minor_node(rsp->rd_dip, "a,raw");
}

static void
rdop_dealloc_pseudo(rd_devstate_t *rsp)
{
	char namebuf[RD_NAME_LEN + 5];

	(void) snprintf(namebuf, sizeof (namebuf), "%s", rsp->rd_name);
	ddi_remove_minor_node(rsp->rd_dip, namebuf);
	(void) snprintf(namebuf, sizeof (namebuf), "%s,raw", rsp->rd_name);
	ddi_remove_minor_node(rsp->rd_dip, namebuf);
}

static void
rdop_dealloc(rd_devstate_t *rsp)
{
	rsp->rd_ops->dealloc(rsp);
}

static const struct rd_ops rd_virtual_ops = {
	.alloc		= rdop_alloc_obp,
	.dealloc	= rdop_dealloc_obp,
	.map		= rdop_map_virtual,
};

static const struct rd_ops rd_physical_ops = {
	.alloc		= rdop_alloc_obp,
	.dealloc	= rdop_dealloc_obp,
	.map		= rdop_map_physical,
	.unmap		= rdop_unmap_physical,
};

static const struct rd_ops rd_pseudo_ops = {
	.alloc		= rdop_alloc_pseudo,
	.dealloc	= rdop_dealloc_pseudo,
	.map		= rdop_map_pseudo,
	.unmap		= rdop_unmap_pseudo,
};

/*
 * Fakes up a disk geometry, and one big partition, based on the size
 * of the file. This is needed because we allow newfs'ing the device,
 * and newfs will do several disk ioctls to figure out the geometry and
 * partition information. It uses that information to determine the parameters
 * to pass to mkfs. Geometry is pretty much irrelevant these days, but we
 * have to support it.
 *
 * Stolen from lofi.c - should maybe split out common code sometime.
 */
static void
rd_fake_disk_geometry(rd_devstate_t *rsp)
{
	/* dk_geom - see dkio(7I) */
	/*
	 * dkg_ncyl _could_ be set to one here (one big cylinder with gobs
	 * of sectors), but that breaks programs like fdisk which want to
	 * partition a disk by cylinder. With one cylinder, you can't create
	 * an fdisk partition and put pcfs on it for testing (hard to pick
	 * a number between one and one).
	 *
	 * The cheezy floppy test is an attempt to not have too few cylinders
	 * for a small file, or so many on a big file that you waste space
	 * for backup superblocks or cylinder group structures.
	 */
	if (rsp->rd_size < (2 * 1024 * 1024)) /* floppy? */
		rsp->rd_dkg.dkg_ncyl = rsp->rd_size / (100 * 1024);
	else
		rsp->rd_dkg.dkg_ncyl = rsp->rd_size / (300 * 1024);
	/* in case file file is < 100k */
	if (rsp->rd_dkg.dkg_ncyl == 0)
		rsp->rd_dkg.dkg_ncyl = 1;
	rsp->rd_dkg.dkg_acyl = 0;
	rsp->rd_dkg.dkg_bcyl = 0;
	rsp->rd_dkg.dkg_nhead = 1;
	rsp->rd_dkg.dkg_obs1 = 0;
	rsp->rd_dkg.dkg_intrlv = 0;
	rsp->rd_dkg.dkg_obs2 = 0;
	rsp->rd_dkg.dkg_obs3 = 0;
	rsp->rd_dkg.dkg_apc = 0;
	rsp->rd_dkg.dkg_rpm = 7200;
	rsp->rd_dkg.dkg_pcyl = rsp->rd_dkg.dkg_ncyl + rsp->rd_dkg.dkg_acyl;
	rsp->rd_dkg.dkg_nsect = rsp->rd_size /
	    (DEV_BSIZE * rsp->rd_dkg.dkg_ncyl);
	rsp->rd_dkg.dkg_write_reinstruct = 0;
	rsp->rd_dkg.dkg_read_reinstruct = 0;

	/* vtoc - see dkio(7I) */
	bzero(&rsp->rd_vtoc, sizeof (struct vtoc));
	rsp->rd_vtoc.v_sanity = VTOC_SANE;
	rsp->rd_vtoc.v_version = V_VERSION;
	bcopy(RD_DRIVER_NAME, rsp->rd_vtoc.v_volume, 7);
	rsp->rd_vtoc.v_sectorsz = DEV_BSIZE;
	rsp->rd_vtoc.v_nparts = 1;
	rsp->rd_vtoc.v_part[0].p_tag = V_UNASSIGNED;
	rsp->rd_vtoc.v_part[0].p_flag = V_UNMNT;
	rsp->rd_vtoc.v_part[0].p_start = (daddr_t)0;
	/*
	 * The partition size cannot just be the number of sectors, because
	 * that might not end on a cylinder boundary. And if that's the case,
	 * newfs/mkfs will print a scary warning. So just figure the size
	 * based on the number of cylinders and sectors/cylinder.
	 */
	rsp->rd_vtoc.v_part[0].p_size = rsp->rd_dkg.dkg_pcyl *
	    rsp->rd_dkg.dkg_nsect * rsp->rd_dkg.dkg_nhead;

	/* dk_cinfo - see dkio(7I) */
	bzero(&rsp->rd_ci, sizeof (struct dk_cinfo));
	(void) strcpy(rsp->rd_ci.dki_cname, RD_DRIVER_NAME);
	rsp->rd_ci.dki_ctype = DKC_MD;
	rsp->rd_ci.dki_flags = 0;
	rsp->rd_ci.dki_cnum = 0;
	rsp->rd_ci.dki_addr = 0;
	rsp->rd_ci.dki_space = 0;
	rsp->rd_ci.dki_prio = 0;
	rsp->rd_ci.dki_vec = 0;
	(void) strcpy(rsp->rd_ci.dki_dname, RD_DRIVER_NAME);
	rsp->rd_ci.dki_unit = 0;
	rsp->rd_ci.dki_slave = 0;
	rsp->rd_ci.dki_partition = 0;
	/*
	 * newfs uses this to set maxcontig. Must not be < 16, or it
	 * will be 0 when newfs multiplies it by DEV_BSIZE and divides
	 * it by the block size. Then tunefs doesn't work because
	 * maxcontig is 0.
	 */
	rsp->rd_ci.dki_maxtransfer = 16;
}

/*
 * Deallocate resources (virtual and physical, device nodes, structures)
 * from a ramdisk.
 */
static void
rd_dealloc_resources(rd_devstate_t *rsp)
{
	dev_info_t	*dip = rsp->rd_dip;
	dev_t		fulldev;

	mutex_destroy(&rsp->rd_device_lock);

	if (rsp->rd_existing)
		ddi_prop_free(rsp->rd_existing);

	if (rsp->rd_ppa != NULL)
		rd_phys_free(rsp->rd_ppa, rsp->rd_npages);

	/*
	 * Remove the block and raw device nodes.
	 */
	rdop_dealloc(rsp);

	/*
	 * Remove the "Size" and "Nblocks" properties.
	 */
	fulldev = makedevice(ddi_driver_major(dip), rsp->rd_minor);
	(void) ddi_prop_remove(fulldev, dip, SIZE_PROP_NAME);
	(void) ddi_prop_remove(fulldev, dip, NBLOCKS_PROP_NAME);

	if (rsp->rd_kstat) {
		kstat_delete(rsp->rd_kstat);
		mutex_destroy(&rsp->rd_kstat_lock);
	}

	ddi_soft_state_free(rd_statep, rsp->rd_minor);
}

/*
 * Allocate resources (virtual and physical memory, device nodes, structures)
 * for a ramdisk.
 */
static rd_devstate_t *
rd_alloc_resources(char *name, const struct rd_ops *ops, uint_t addr,
    size_t size, dev_info_t *dip)
{
	minor_t		minor;
	rd_devstate_t	*rsp;
	dev_t		fulldev;
	int64_t		Nblocks_prop_val;
	int64_t		Size_prop_val;

	minor = rd_find_free_minor();
	if (ddi_soft_state_zalloc(rd_statep, minor) == DDI_FAILURE) {
		return (NULL);
	}
	rsp = ddi_get_soft_state(rd_statep, minor);

	(void) strcpy(rsp->rd_name, name);
	rsp->rd_dip = dip;
	rsp->rd_minor = minor;
	rsp->rd_size = size;
	rsp->rd_ops = ops;
	rsp->rd_obp_virt = (caddr_t)(ulong_t)addr;

	mutex_init(&rsp->rd_device_lock, NULL, MUTEX_DRIVER, NULL);

	/*
	 * Allocate physical memory for non-OBP ramdisks.
	 * Create pseudo block and raw device nodes.
	 */
	if (rdop_alloc(rsp))
		goto create_failed;

	/*
	 * Create the "Size" and "Nblocks" properties.
	 */
	fulldev = makedevice(ddi_driver_major(dip), minor);
	Size_prop_val = size;
	if ((ddi_prop_update_int64(fulldev, dip,
	    SIZE_PROP_NAME, Size_prop_val)) != DDI_PROP_SUCCESS) {
		goto create_failed;
	}
	Nblocks_prop_val = size / DEV_BSIZE;
	if ((ddi_prop_update_int64(fulldev, dip,
	    NBLOCKS_PROP_NAME, Nblocks_prop_val)) != DDI_PROP_SUCCESS) {
		goto create_failed;
	}

	/*
	 * Allocate kstat stuff.
	 */
	rsp->rd_kstat = kstat_create(RD_DRIVER_NAME, minor, NULL,
	    "disk", KSTAT_TYPE_IO, 1, 0);
	if (rsp->rd_kstat) {
		mutex_init(&rsp->rd_kstat_lock, NULL,
		    MUTEX_DRIVER, NULL);
		rsp->rd_kstat->ks_lock = &rsp->rd_kstat_lock;
		kstat_install(rsp->rd_kstat);
	}

	rd_fake_disk_geometry(rsp);

	return (rsp);

create_failed:
	/*
	 * Cleanup.
	 */
	rd_dealloc_resources(rsp);

	return (NULL);
}

/*
 * Undo what we did in rd_attach, freeing resources and removing things which
 * we installed.  The system framework guarantees we are not active with this
 * devinfo node in any other entry points at this time.
 */
static int
rd_common_detach(dev_info_t *dip)
{
	if (dip == rd_dip) {
		/*
		 * Pseudo node: can't detach if any pseudo ramdisks exist.
		 */
		if (rd_is_busy()) {
			return (DDI_FAILURE);
		}
		ddi_soft_state_free(rd_statep, RD_CTL_MINOR);
		rd_dip = NULL;
	} else {
		/*
		 * A non-pseudo ramdisk; find the state and free resources.
		 */
		rd_devstate_t	*rsp;

		if ((rsp = rd_find_dip_state(dip)) != NULL)
			rd_dealloc_resources(rsp);
	}

	ddi_remove_minor_node(dip, NULL);

	return (DDI_SUCCESS);
}

static int
rd_attach(dev_info_t *dip, ddi_attach_cmd_t cmd)
{
	char		*name;
	rd_existing_t	*ep = NULL;
	uint_t		obpaddr = 0, nep, i;
	size_t		size = 0;
	rd_devstate_t	*rsp;

	switch (cmd) {

	case DDI_ATTACH:
		mutex_enter(&rd_lock);

		/*
		 * For pseudo ramdisk devinfo set up state 0 and :ctl device;
		 * else it's an OBP-created ramdisk.
		 */
		if (is_pseudo_device(dip)) {
			rd_dip = dip;
			rd_init_tuneables();

			/*
			 * The zeroth minor is reserved for the ramdisk
			 * 'control' device.
			 */
			if (ddi_soft_state_zalloc(rd_statep, RD_CTL_MINOR) ==
			    DDI_FAILURE) {
				goto attach_failed;
			}
			rsp = ddi_get_soft_state(rd_statep, RD_CTL_MINOR);
			rsp->rd_dip = dip;

			if (ddi_create_minor_node(dip, RD_CTL_NODE,
			    S_IFCHR, 0, DDI_PSEUDO, 0) == DDI_FAILURE) {
				goto attach_failed;
			}
		} else {
			const struct rd_ops *ops;


			RD_STRIP_PREFIX(name, ddi_node_name(dip));

			if (strlen(name) > RD_NAME_LEN) {
				cmn_err(CE_CONT,
				    "%s: name too long - ignoring\n", name);
				goto attach_failed;
			}

			/*
			 * An OBP-created ramdisk must have an 'existing'
			 * property; get and check it.
			 */
			if (ddi_prop_lookup_byte_array(DDI_DEV_T_ANY, dip,
			    DDI_PROP_DONTPASS, OBP_EXISTING_PROP_NAME,
			    (uchar_t **)&ep, &nep) == DDI_SUCCESS) {

				if (nep == 0 || (nep % sizeof (*ep)) != 0) {
					cmn_err(CE_CONT,
					    "%s: " OBP_EXISTING_PROP_NAME
					    " illegal size\n", name);
					goto attach_failed;
				}
				nep /= sizeof (*ep);

				/*
				 * Calculate the size of the ramdisk.
				 */
				for (i = 0; i < nep; ++i)
					size += ep[i].size;

				ops = &rd_physical_ops;
			} else if ((obpaddr = ddi_prop_get_int(DDI_DEV_T_ANY,
			    dip, DDI_PROP_DONTPASS, OBP_ADDRESS_PROP_NAME,
			    0)) != 0)  {

				size = ddi_prop_get_int(DDI_DEV_T_ANY, dip,
				    DDI_PROP_DONTPASS, OBP_SIZE_PROP_NAME, 0);

				ops = &rd_virtual_ops;
			} else {
				cmn_err(CE_CONT, "%s: missing OBP properties\n",
				    name);
				goto attach_failed;
			}

			/*
			 * Allocate driver resources for the ramdisk.
			 */
			if ((rsp = rd_alloc_resources(name, ops, obpaddr, size,
			    dip)) == NULL)
				goto attach_failed;

			rsp->rd_existing = ep;
			rsp->rd_nexisting = nep;
		}

		mutex_exit(&rd_lock);

		ddi_report_dev(dip);

		return (DDI_SUCCESS);

	case DDI_RESUME:
		return (DDI_SUCCESS);

	default:
		return (DDI_FAILURE);
	}

attach_failed:
	/*
	 * Use our common detach routine to unallocate any stuff which
	 * was allocated above.
	 */
	(void) rd_common_detach(dip);
	mutex_exit(&rd_lock);

	if (ep != NULL) {
		ddi_prop_free(ep);
	}
	return (DDI_FAILURE);
}

static int
rd_detach(dev_info_t *dip, ddi_detach_cmd_t cmd)
{
	int	e;

	switch (cmd) {

	case DDI_DETACH:
		mutex_enter(&rd_lock);
		e = rd_common_detach(dip);
		mutex_exit(&rd_lock);

		return (e);

	case DDI_SUSPEND:
		return (DDI_SUCCESS);

	default:
		return (DDI_FAILURE);
	}
}

/*ARGSUSED*/
static int
rd_getinfo(dev_info_t *dip, ddi_info_cmd_t infocmd, void *arg, void **result)
{
	rd_devstate_t	*rsp;

	switch (infocmd) {
	case DDI_INFO_DEVT2DEVINFO:
		if ((rsp = ddi_get_soft_state(rd_statep,
		    getminor((dev_t)arg))) != NULL) {
			*result = rsp->rd_dip;
			return (DDI_SUCCESS);
		}
		*result = NULL;
		return (DDI_FAILURE);

	case DDI_INFO_DEVT2INSTANCE:
		if ((rsp = ddi_get_soft_state(rd_statep,
		    getminor((dev_t)arg))) != NULL) {
			*result = (void *)(uintptr_t)
			    ddi_get_instance(rsp->rd_dip);
			return (DDI_SUCCESS);
		}
		*result = NULL;
		return (DDI_FAILURE);

	default:
		return (DDI_FAILURE);
	}
}

/*ARGSUSED3*/
static int
rd_open(dev_t *devp, int flag, int otyp, cred_t *credp)
{
	minor_t		minor;
	rd_devstate_t	*rsp;

	mutex_enter(&rd_lock);

	minor = getminor(*devp);
	if (minor == RD_CTL_MINOR) {
		/*
		 * Master control device; must be opened exclusively.
		 */
		if ((flag & FEXCL) != FEXCL || otyp != OTYP_CHR) {
			mutex_exit(&rd_lock);
			return (EINVAL);
		}

		rsp = ddi_get_soft_state(rd_statep, RD_CTL_MINOR);
		if (rsp == NULL) {
			mutex_exit(&rd_lock);
			return (ENXIO);
		}

		if (rd_is_open(rsp)) {
			mutex_exit(&rd_lock);
			return (EBUSY);
		}
		(void) rd_opened(rsp, OTYP_CHR);

		mutex_exit(&rd_lock);

		return (0);
	}

	rsp = ddi_get_soft_state(rd_statep, minor);
	if (rsp == NULL) {
		mutex_exit(&rd_lock);
		return (ENXIO);
	}

	if (rd_opened(rsp, otyp) == -1) {
		mutex_exit(&rd_lock);
		return (EINVAL);
	}

	mutex_exit(&rd_lock);
	return (0);
}

/*ARGSUSED*/
static int
rd_close(dev_t dev, int flag, int otyp, struct cred *credp)
{
	minor_t		minor;
	rd_devstate_t	*rsp;

	mutex_enter(&rd_lock);

	minor = getminor(dev);

	rsp = ddi_get_soft_state(rd_statep, minor);
	if (rsp == NULL) {
		mutex_exit(&rd_lock);
		return (EINVAL);
	}

	rd_closed(rsp, otyp);

	mutex_exit(&rd_lock);

	return (0);
}

static void
rd_minphys(struct buf *bp)
{
	if (bp->b_bcount > rd_maxphys) {
		bp->b_bcount = rd_maxphys;
	}
}

static void
rd_rw(rd_devstate_t *rsp, struct buf *bp, offset_t offset, size_t nbytes)
{
	const int reading = bp->b_flags & B_READ;
	struct rd_map map;
	caddr_t	buf_addr;

	map.mapped = B_FALSE;

	bp_mapin(bp);
	buf_addr = bp->b_un.b_addr;

	while (nbytes > 0) {
		offset_t	off_in_window;
		size_t		rem_in_window, copy_bytes;
		caddr_t		raddr;

		rdop_map(rsp, offset, &map);

		off_in_window = offset - map.base;
		rem_in_window = map.size - off_in_window;

		raddr = map.vaddr + off_in_window;
		copy_bytes = MIN(nbytes, rem_in_window);

		mutex_enter(&rsp->rd_device_lock);
		if (reading) {
			(void) bcopy(raddr, buf_addr, copy_bytes);
		} else {
			(void) bcopy(buf_addr, raddr, copy_bytes);
		}
		mutex_exit(&rsp->rd_device_lock);

		offset   += copy_bytes;
		buf_addr += copy_bytes;
		nbytes   -= copy_bytes;
	}

	rdop_unmap(rsp, &map);
}

/*
 * On Sparc, this function deals with both pseudo ramdisks and OBP ramdisks.
 * In the case where we freed the "bootarchive" ramdisk in bop_free_archive(),
 * we stop allowing access to the OBP ramdisks. To do so, we set the
 * bootops_obp_ramdisk_disabled flag to true, and we check if the operation
 * is for an OBP ramdisk. In this case we indicate an ENXIO error.
 */
static int
rd_strategy(struct buf *bp)
{
	rd_devstate_t	*rsp;
	offset_t	offset;

	rsp = ddi_get_soft_state(rd_statep, getminor(bp->b_edev));
	offset = bp->b_blkno * DEV_BSIZE;

	if (rsp == NULL) {
		bp->b_error = ENXIO;
		bp->b_flags |= B_ERROR;
	} else if (offset >= rsp->rd_size) {
		bp->b_error = EINVAL;
		bp->b_flags |= B_ERROR;
	} else {
		size_t	nbytes;

		if (rsp->rd_kstat) {
			mutex_enter(rsp->rd_kstat->ks_lock);
			kstat_runq_enter(KSTAT_IO_PTR(rsp->rd_kstat));
			mutex_exit(rsp->rd_kstat->ks_lock);
		}

		nbytes = min(bp->b_bcount, rsp->rd_size - offset);

		rd_rw(rsp, bp, offset, nbytes);

		bp->b_resid = bp->b_bcount - nbytes;

		if (rsp->rd_kstat) {
			kstat_io_t *kioptr;

			mutex_enter(rsp->rd_kstat->ks_lock);
			kioptr = KSTAT_IO_PTR(rsp->rd_kstat);
			if (bp->b_flags & B_READ) {
				kioptr->nread += nbytes;
				kioptr->reads++;
			} else {
				kioptr->nwritten += nbytes;
				kioptr->writes++;
			}
			kstat_runq_exit(kioptr);
			mutex_exit(rsp->rd_kstat->ks_lock);
		}
	}

	biodone(bp);
	return (0);
}

/*ARGSUSED*/
static int
rd_read(dev_t dev, struct uio *uiop, cred_t *credp)
{
	rd_devstate_t	*rsp;

	rsp = ddi_get_soft_state(rd_statep, getminor(dev));

	if (uiop->uio_offset >= rsp->rd_size)
		return (EINVAL);

	return (physio(rd_strategy, NULL, dev, B_READ, rd_minphys, uiop));
}

/*ARGSUSED*/
static int
rd_write(dev_t dev, register struct uio *uiop, cred_t *credp)
{
	rd_devstate_t	*rsp;

	rsp = ddi_get_soft_state(rd_statep, getminor(dev));

	if (uiop->uio_offset >= rsp->rd_size)
		return (EINVAL);

	return (physio(rd_strategy, NULL, dev, B_WRITE, rd_minphys, uiop));
}

/*ARGSUSED*/
static int
rd_create_disk(dev_t dev, struct rd_ioctl *urip, int mode, int *rvalp)
{
	struct rd_ioctl	kri;
	size_t		size;
	rd_devstate_t	*rsp;

	if (ddi_copyin(urip, &kri, sizeof (kri), mode) == -1) {
		return (EFAULT);
	}

	kri.ri_name[RD_NAME_LEN] = '\0';

	size = kri.ri_size;
	if (size == 0) {
		return (EINVAL);
	}
	size = ptob(btopr(size));

	mutex_enter(&rd_lock);

	if (rd_find_named_disk(kri.ri_name) != NULL) {
		mutex_exit(&rd_lock);
		return (EEXIST);
	}

	rsp = rd_alloc_resources(kri.ri_name, &rd_pseudo_ops, 0, size, rd_dip);
	if (rsp == NULL) {
		mutex_exit(&rd_lock);
		return (EAGAIN);
	}

	mutex_exit(&rd_lock);

	return (ddi_copyout(&kri, urip, sizeof (kri), mode) == -1 ? EFAULT : 0);
}

/*ARGSUSED*/
static int
rd_delete_disk(dev_t dev, struct rd_ioctl *urip, int mode)
{
	struct rd_ioctl	kri;
	rd_devstate_t	*rsp;

	if (ddi_copyin(urip, &kri, sizeof (kri), mode) == -1) {
		return (EFAULT);
	}

	kri.ri_name[RD_NAME_LEN] = '\0';

	mutex_enter(&rd_lock);

	rsp = rd_find_named_disk(kri.ri_name);
	if (rsp == NULL || rsp->rd_dip != rd_dip) {
		mutex_exit(&rd_lock);
		return (EINVAL);
	}
	if (rd_is_open(rsp)) {
		mutex_exit(&rd_lock);
		return (EBUSY);
	}

	rd_dealloc_resources(rsp);

	mutex_exit(&rd_lock);

	return (0);
}

/*ARGSUSED*/
static int
rd_ioctl(dev_t dev, int cmd, intptr_t arg, int mode, cred_t *credp, int *rvalp)
{
	minor_t		minor;
	int		error;
	enum dkio_state	dkstate;
	rd_devstate_t	*rsp;

	minor = getminor(dev);

	/*
	 * Ramdisk ioctls only apply to the master device.
	 */
	if (minor == RD_CTL_MINOR) {
		struct rd_ioctl *rip = (struct rd_ioctl *)arg;

		/*
		 * The query commands only need read-access - i.e., normal
		 * users are allowed to do those on the controlling device
		 * as long as they can open it read-only.
		 */
		switch (cmd) {
		case RD_CREATE_DISK:
			if ((mode & FWRITE) == 0)
				return (EPERM);
			return (rd_create_disk(dev, rip, mode, rvalp));

		case RD_DELETE_DISK:
			if ((mode & FWRITE) == 0)
				return (EPERM);
			return (rd_delete_disk(dev, rip, mode));

		default:
			return (EINVAL);
		}
	}

	rsp = ddi_get_soft_state(rd_statep, minor);
	if (rsp == NULL) {
		return (ENXIO);
	}

	/*
	 * These are for faking out utilities like newfs.
	 */
	switch (cmd) {
	case DKIOCGVTOC:
		switch (ddi_model_convert_from(mode & FMODELS)) {
		case DDI_MODEL_ILP32: {
			struct vtoc32 vtoc32;

			vtoctovtoc32(rsp->rd_vtoc, vtoc32);
			if (ddi_copyout(&vtoc32, (void *)arg,
			    sizeof (struct vtoc32), mode))
				return (EFAULT);
			}
			break;

		case DDI_MODEL_NONE:
			if (ddi_copyout(&rsp->rd_vtoc, (void *)arg,
			    sizeof (struct vtoc), mode))
				return (EFAULT);
			break;
		}
		return (0);
	case DKIOCINFO:
		error = ddi_copyout(&rsp->rd_ci, (void *)arg,
		    sizeof (struct dk_cinfo), mode);
		if (error)
			return (EFAULT);
		return (0);
	case DKIOCG_VIRTGEOM:
	case DKIOCG_PHYGEOM:
	case DKIOCGGEOM:
		error = ddi_copyout(&rsp->rd_dkg, (void *)arg,
		    sizeof (struct dk_geom), mode);
		if (error)
			return (EFAULT);
		return (0);
	case DKIOCSTATE:
		/* the file is always there */
		dkstate = DKIO_INSERTED;
		error = ddi_copyout(&dkstate, (void *)arg,
		    sizeof (enum dkio_state), mode);
		if (error)
			return (EFAULT);
		return (0);
	default:
		return (ENOTTY);
	}
}


static struct cb_ops rd_cb_ops = {
	rd_open,
	rd_close,
	rd_strategy,
	nodev,
	nodev,		/* dump */
	rd_read,
	rd_write,
	rd_ioctl,
	nodev,		/* devmap */
	nodev,		/* mmap */
	nodev,		/* segmap */
	nochpoll,	/* poll */
	ddi_prop_op,
	NULL,
	D_NEW | D_MP
};

static struct dev_ops rd_ops = {
	DEVO_REV,
	0,
	rd_getinfo,
	nulldev,	/* identify */
	nulldev,	/* probe */
	rd_attach,
	rd_detach,
	nodev,		/* reset */
	&rd_cb_ops,
	NULL,
	NULL,
	ddi_quiesce_not_needed,		/* quiesce */
};


extern struct mod_ops mod_driverops;

static struct modldrv modldrv = {
	&mod_driverops,
	"ramdisk driver",
	&rd_ops
};

static struct modlinkage modlinkage = {
	MODREV_1,
	&modldrv,
	0
};

int
_init(void)
{
	int e;

	if ((e = ddi_soft_state_init(&rd_statep,
	    sizeof (rd_devstate_t), 0)) != 0) {
		return (e);
	}

	mutex_init(&rd_lock, NULL, MUTEX_DRIVER, NULL);

	if ((e = mod_install(&modlinkage)) != 0)  {
		mutex_destroy(&rd_lock);
		ddi_soft_state_fini(&rd_statep);
	}

	return (e);
}

int
_fini(void)
{
	int e;

	if ((e = mod_remove(&modlinkage)) != 0)  {
		return (e);
	}

	ddi_soft_state_fini(&rd_statep);
	mutex_destroy(&rd_lock);

	return (e);
}

int
_info(struct modinfo *modinfop)
{
	return (mod_info(&modlinkage, modinfop));
}
