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

#ifndef _SDEV_VNOPS_H
#define _SDEV_VNOPS_H

extern int sdev_open(struct vnode **vpp, int flag, struct cred *cred,
		     caller_context_t *ct);
extern int sdev_close(struct vnode *vp, int flag, int count, offset_t offset,
		      struct cred *cred, caller_context_t *ct);
extern int sdev_read(struct vnode *vp, struct uio *uio, int ioflag,
		     struct cred *cred, struct caller_context *ct);
extern int sdev_write(struct vnode *vp, struct uio *uio, int ioflag,
		      struct cred *cred, struct caller_context *ct);
extern int sdev_ioctl(struct vnode *vp, int cmd, intptr_t arg, int flag,
		      struct cred *cred, int *rvalp,  caller_context_t *ct);
extern int sdev_getattr(struct vnode *vp, struct vattr *vap, int flags,
			struct cred *cr, caller_context_t *ct);
extern int sdev_setattr(struct vnode *vp, struct vattr *vap, int flags,
			struct cred *cred, caller_context_t *ctp);
extern int sdev_getsecattr(struct vnode *vp, struct vsecattr *vsap, int flags,
			   struct cred *cr, caller_context_t *ct);
extern int sdev_setsecattr(struct vnode *vp, struct vsecattr *vsap, int flags,
			   struct cred *cr, caller_context_t *ct);
extern int sdev_access(struct vnode *vp, int mode, int flags, struct cred *cr,
		       caller_context_t *ct);
extern int sdev_lookup(struct vnode *dvp, char *nm, struct vnode **vpp,
		       struct pathname *pnp, int flags, struct vnode *rdir,
		       struct cred *cred, caller_context_t *ct,
		       int *direntflags, pathname_t *realpnp);
extern int sdev_create(struct vnode *dvp, char *nm, struct vattr *vap,
		       vcexcl_t excl, int mode, struct vnode **vpp,
		       struct cred *cred, int flag, caller_context_t *ct,
		       vsecattr_t *vsecp);
extern int sdev_remove(struct vnode *dvp, char *nm, struct cred *cred,
		       caller_context_t *ct, int flags);
extern int sdev_rename(struct vnode *odvp, char *onm, struct vnode *ndvp,
		       char *nnm, struct cred *cred, caller_context_t *ct,
		       int flags);
extern int sdev_symlink(struct vnode *dvp, char *lnm, struct vattr *tva,
			char *tnm, struct cred *cred, caller_context_t *ct,
			int flags);
extern int sdev_mkdir(struct vnode *dvp, char *nm, struct vattr *va,
		      struct vnode **vpp, struct cred *cred,
		      caller_context_t *ct, int flags, vsecattr_t *vsecp);
extern int sdev_rmdir(struct vnode *dvp, char *nm, struct vnode *cdir,
		      struct cred *cred, caller_context_t *ct, int flags);
extern int sdev_readlink(struct vnode *vp, struct uio *uiop, struct cred *cred,
			 caller_context_t *ct);
extern int sdev_readdir(struct vnode *dvp, struct uio *uiop, struct cred *cred,
			int *eofp, caller_context_t *ct, int flags);
extern void sdev_inactive(struct vnode *vp, struct cred *cred,
			  caller_context_t *ct);
extern int sdev_fid(struct vnode *vp, struct fid *fidp, caller_context_t *ct);
extern int sdev_rwlock(struct vnode *vp, int write_flag, caller_context_t *ctp);
extern void sdev_rwunlock(struct vnode *vp, int write_flag,
			  caller_context_t *ctp);
extern int sdev_seek(struct vnode *vp, offset_t ooff, offset_t *noffp,
		     caller_context_t *ct);
extern int sdev_frlock(struct vnode *vp, int cmd, struct flock64 *bfp, int flag,
		       offset_t offset, struct flk_callback *flk_cbp,
		       struct cred *cr, caller_context_t *ct);
extern int sdev_pathconf(vnode_t *vp, int cmd, ulong_t *valp, cred_t *cr,
			 caller_context_t *ct);

#endif
