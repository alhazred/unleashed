/*
 * Copyright 2017 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _SYS_VNODE_DISPATCH_H
#define _SYS_VNODE_DISPATCH_H

#include <sys/vnode.h>
#include <sys/stdbool.h>
#include <sys/fem.h>

/*
 * Ideally these static inlines could be just inline statements in the
 * corresponding fop_*() functions.  Unfortunately, fem gets in the way and
 * we have to make these to centralize the damage.
 *
 * Let's use the close vnode op as an example.
 *
 * fop_close() calls the fop_close_dispatch() static inline defined in this
 * header.  fop_close_dispatch() does one of two things:
 *
 * (1) checks if ->vop_close is NULL, and if it is returns not-implemented
 *     errno (i.e., ENOSYS)
 * (2) calls ->vop_close
 */

#define FOP_DISPATCH(fname, vopname, vheadname, args, callargs)		\
static inline int fname args						\
{									\
	if (vnode->v_op->vopname == NULL)				\
		return ENOSYS;						\
	return vnode->v_op->vopname callargs;				\
}

/* open takes a struct vnode **, so it is hand-coded */
static inline int fop_open_dispatch(struct vnode **vnode, int mode, cred_t *cr,
    caller_context_t *ct)
{
	if ((*vnode)->v_op->vop_open == NULL)
		return ENOSYS;

	return (*vnode)->v_op->vop_open(vnode, mode, cr, ct);
}

FOP_DISPATCH(fop_close_dispatch, vop_close, vhead_close,
    (struct vnode *vnode, int flag, int count, offset_t offset, cred_t *cr,
     caller_context_t *ct),
    (vnode, flag, count, offset, cr, ct))
FOP_DISPATCH(fop_read_dispatch, vop_read, vhead_read,
    (struct vnode *vnode, uio_t *uio, int ioflag, cred_t *cr,
     caller_context_t *ct),
    (vnode, uio, ioflag, cr, ct))
FOP_DISPATCH(fop_write_dispatch, vop_write, vhead_write,
    (struct vnode *vnode, uio_t *uio, int ioflag, cred_t *cr,
     caller_context_t *ct),
    (vnode, uio, ioflag, cr, ct))
FOP_DISPATCH(fop_ioctl_dispatch, vop_ioctl, vhead_ioctl,
    (struct vnode *vnode, int cmd, intptr_t arg, int flag, cred_t *cr,
     int *rvalp, caller_context_t *ct),
    (vnode, cmd, arg, flag, cr, rvalp, ct))
FOP_DISPATCH(fop_setfl_dispatch, vop_setfl, vhead_setfl,
    (struct vnode *vnode, int oflags, int nflags, cred_t *cr,
     caller_context_t *ct),
    (vnode, oflags, nflags, cr, ct))
FOP_DISPATCH(fop_getattr_dispatch, vop_getattr, vhead_getattr,
    (struct vnode *vnode, vattr_t *vap, int flags, cred_t *cr,
     caller_context_t *ct),
    (vnode, vap, flags, cr, ct))
FOP_DISPATCH(fop_setattr_dispatch, vop_setattr, vhead_setattr,
    (struct vnode *vnode, vattr_t *vap, int flags, cred_t *cr,
     caller_context_t *ct),
    (vnode, vap, flags, cr, ct))
FOP_DISPATCH(fop_access_dispatch, vop_access, vhead_access,
    (struct vnode *vnode, int mode, int flags, cred_t *cr,
     caller_context_t *ct),
    (vnode, mode, flags, cr, ct))
FOP_DISPATCH(fop_lookup_dispatch, vop_lookup, vhead_lookup,
    (struct vnode *vnode, char *nm, struct vnode **vpp, pathname_t *pnp,
     int flags, struct vnode *rdir, cred_t *cr, caller_context_t *ct,
     int *direntflags, pathname_t *realpnp),
    (vnode, nm, vpp, pnp, flags, rdir, cr, ct, direntflags, realpnp))
FOP_DISPATCH(fop_create_dispatch, vop_create, vhead_create,
    (struct vnode *vnode, char *name, vattr_t *vap, vcexcl_t excl,
     int mode, struct vnode **vpp, cred_t *cr, int flag, caller_context_t *ct,
     vsecattr_t *vsecattr),
    (vnode, name, vap, excl, mode, vpp, cr, flag, ct, vsecattr))
FOP_DISPATCH(fop_remove_dispatch, vop_remove, vhead_remove,
    (struct vnode *vnode, char *nm, cred_t *cr, caller_context_t *ct,
     int flags),
    (vnode, nm, cr, ct, flags))
FOP_DISPATCH(fop_link_dispatch, vop_link, vhead_link,
    (struct vnode *vnode, struct vnode *svp, char *tnm, cred_t *cr,
     caller_context_t *ct, int flags),
    (vnode, svp, tnm, cr, ct, flags))
FOP_DISPATCH(fop_rename_dispatch, vop_rename, vhead_rename,
    (struct vnode *vnode, char *snm, struct vnode *tdvp, char *tnm,
     cred_t *cr, caller_context_t *ct, int flags),
    (vnode, snm, tdvp, tnm, cr, ct, flags))
FOP_DISPATCH(fop_mkdir_dispatch, vop_mkdir, vhead_mkdir,
    (struct vnode *vnode, char *dirname, vattr_t *vap, struct vnode **vpp,
     cred_t *cr, caller_context_t *ct, int flags, vsecattr_t *vsecp),
    (vnode, dirname, vap, vpp, cr, ct, flags, vsecp))
FOP_DISPATCH(fop_rmdir_dispatch, vop_rmdir, vhead_rmdir,
    (struct vnode *vnode, char *nm, struct vnode *cdir, cred_t *cr,
     caller_context_t *ct, int flags),
    (vnode, nm, cdir, cr, ct, flags))
FOP_DISPATCH(fop_readdir_dispatch, vop_readdir, vhead_readdir,
    (struct vnode *vnode, uio_t *uiop, cred_t *cr, int *eofp,
     caller_context_t *ct, int flags),
    (vnode, uiop, cr, eofp, ct, flags))
FOP_DISPATCH(fop_symlink_dispatch, vop_symlink, vhead_symlink,
    (struct vnode *vnode, char *linkname, vattr_t *vap, char *target,
     cred_t *cr, caller_context_t *ct, int flags),
    (vnode, linkname, vap, target, cr, ct, flags))
FOP_DISPATCH(fop_readlink_dispatch, vop_readlink, vhead_readlink,
    (struct vnode *vnode, uio_t *uiop, cred_t *cr, caller_context_t *ct),
    (vnode, uiop, cr, ct))
FOP_DISPATCH(fop_fsync_dispatch, vop_fsync, vhead_fsync,
    (struct vnode *vnode, int syncflag, cred_t *cr, caller_context_t *ct),
    (vnode, syncflag, cr, ct))

/* returns void, so it is hand-coded */
static inline void fop_inactive_dispatch(struct vnode *vnode, cred_t *cr,
    caller_context_t *ct)
{
	if (vnode->v_op->vop_inactive != NULL)
		vnode->v_op->vop_inactive(vnode, cr, ct);
}

FOP_DISPATCH(fop_fid_dispatch, vop_fid, vhead_fid,
    (struct vnode *vnode, fid_t *fidp, caller_context_t *ct),
    (vnode, fidp, ct))
FOP_DISPATCH(fop_rwlock_dispatch, vop_rwlock, vhead_rwlock,
    (struct vnode *vnode, int write_lock, caller_context_t *ct),
    (vnode, write_lock, ct))

/* returns void, so it is hand-coded */
static inline void fop_rwunlock_dispatch(struct vnode *vnode, int write_lock,
    caller_context_t *ct)
{
	if (vnode->v_op->vop_rwunlock != NULL)
		vnode->v_op->vop_rwunlock(vnode, write_lock, ct);
}

FOP_DISPATCH(fop_seek_dispatch, vop_seek, vhead_seek,
    (struct vnode *vnode, offset_t off, offset_t *noff, caller_context_t *ct),
    (vnode, off, noff, ct))
FOP_DISPATCH(fop_cmp_dispatch, vop_cmp, vhead_cmp,
    (struct vnode *vnode, struct vnode *vp2, caller_context_t *ct),
    (vnode, vp2, ct))
FOP_DISPATCH(fop_frlock_dispatch, vop_frlock, vhead_frlock,
    (struct vnode *vnode, int cmd, struct flock64 *bfp, int flag,
     offset_t offset, struct flk_callback *flk_cbp, cred_t *cr,
     caller_context_t *ct),
    (vnode, cmd, bfp, flag, offset, flk_cbp, cr, ct))
FOP_DISPATCH(fop_space_dispatch, vop_space, vhead_space,
    (struct vnode *vnode, int cmd, flock64_t *bfp, int flag, offset_t offset,
     cred_t *cr, caller_context_t *ct),
    (vnode, cmd, bfp, flag, offset, cr, ct))
FOP_DISPATCH(fop_realvp_dispatch, vop_realvp, vhead_realvp,
    (struct vnode *vnode, struct vnode **vpp, caller_context_t *ct),
    (vnode, vpp, ct))
FOP_DISPATCH(fop_getpage_dispatch, vop_getpage, vhead_getpage,
    (struct vnode *vnode, offset_t off, size_t len, uint_t *protp,
     struct page **plarr, size_t plsz, struct seg *seg, caddr_t addr,
     enum seg_rw rw, cred_t *cr, caller_context_t *ct),
    (vnode, off, len, protp, plarr, plsz, seg, addr, rw, cr, ct))
FOP_DISPATCH(fop_putpage_dispatch, vop_putpage, vhead_putpage,
    (struct vnode *vnode, offset_t off, size_t len, int flags, cred_t *cr,
     caller_context_t *ct),
    (vnode, off, len, flags, cr, ct))
FOP_DISPATCH(fop_map_dispatch, vop_map, vhead_map,
    (struct vnode *vnode, offset_t off, struct as *as, caddr_t *addr,
     size_t len, uchar_t prot, uchar_t maxprot, uint_t flags, cred_t *cr,
     caller_context_t *ct),
    (vnode, off, as, addr, len, prot, maxprot, flags, cr, ct))
FOP_DISPATCH(fop_addmap_dispatch, vop_addmap, vhead_addmap,
    (struct vnode *vnode, offset_t off, struct as *as, caddr_t addr,
     size_t len, uchar_t prot, uchar_t maxprot, uint_t flags, cred_t *cr,
     caller_context_t *ct),
    (vnode, off, as, addr, len, prot, maxprot, flags, cr, ct))
FOP_DISPATCH(fop_delmap_dispatch, vop_delmap, vhead_delmap,
    (struct vnode *vnode, offset_t off, struct as *as, caddr_t addr, size_t len,
     uint_t prot, uint_t maxprot, uint_t flags, cred_t *cr,
     caller_context_t *ct),
    (vnode, off, as, addr, len, prot, maxprot, flags, cr, ct))
FOP_DISPATCH(fop_poll_dispatch, vop_poll, vhead_poll,
    (struct vnode *vnode, short events, int anyyet, short *reventsp,
     struct pollhead **phpp, caller_context_t *ct),
    (vnode, events, anyyet, reventsp, phpp, ct))
FOP_DISPATCH(fop_dump_dispatch, vop_dump, vhead_dump,
    (struct vnode *vnode, caddr_t addr, offset_t lbdn, offset_t dblks,
     caller_context_t *ct),
    (vnode, addr, lbdn, dblks, ct))
FOP_DISPATCH(fop_pathconf_dispatch, vop_pathconf, vhead_pathconf,
    (struct vnode *vnode, int cmd, ulong_t *valp, cred_t *cr,
     caller_context_t *ct),
    (vnode, cmd, valp, cr, ct))
FOP_DISPATCH(fop_pageio_dispatch, vop_pageio, vhead_pageio,
    (struct vnode *vnode, struct page *page, uoff_t io_off, size_t io_len,
     int flags, cred_t *cr, caller_context_t *ct),
    (vnode, page, io_off, io_len, flags, cr, ct))
FOP_DISPATCH(fop_dumpctl_dispatch, vop_dumpctl, vhead_dumpctl,
    (struct vnode *vnode, int action, offset_t *blkp,
     caller_context_t *ct),
    (vnode, action, blkp, ct))

/* returns void, so it is hand-coded */
static inline void fop_dispose_dispatch(struct vnode *vnode, struct page *pp,
    int flag, int dn, cred_t *cr, caller_context_t *ct)
{
	if (vnode->v_op->vop_dispose != NULL)
		vnode->v_op->vop_dispose(vnode, pp, flag, dn, cr, ct);
}

FOP_DISPATCH(fop_setsecattr_dispatch, vop_setsecattr, vhead_setsecattr,
    (struct vnode *vnode, vsecattr_t *vsap, int flag, cred_t *cr,
     caller_context_t *ct),
    (vnode, vsap, flag, cr, ct))
FOP_DISPATCH(fop_getsecattr_dispatch, vop_getsecattr, vhead_getsecattr,
    (struct vnode *vnode, vsecattr_t *vsap, int flag, cred_t *cr,
     caller_context_t *ct),
    (vnode, vsap, flag, cr, ct))
FOP_DISPATCH(fop_shrlock_dispatch, vop_shrlock, vhead_shrlock,
    (struct vnode *vnode, int cmd, struct shrlock *shr, int flag,
     cred_t *cr, caller_context_t *ct),
    (vnode, cmd, shr, flag, cr, ct))
FOP_DISPATCH(fop_vnevent_dispatch, vop_vnevent, vhead_vnevent,
    (struct vnode *vnode, vnevent_t vnevent, struct vnode *dvp, char *fnm,
     caller_context_t *ct),
    (vnode, vnevent, dvp, fnm, ct))
FOP_DISPATCH(fop_reqzcbuf_dispatch, vop_reqzcbuf, vhead_reqzcbuf,
    (struct vnode *vnode, enum uio_rw ioflag, xuio_t *uio, cred_t *cr,
     caller_context_t *ct),
    (vnode, ioflag, uio, cr, ct))
FOP_DISPATCH(fop_retzcbuf_dispatch, vop_retzcbuf, vhead_retzcbuf,
    (struct vnode *vnode, xuio_t *uio, cred_t *cr, caller_context_t *ct),
    (vnode, uio, cr, ct))

#undef FOP_DISPATCH

#endif
