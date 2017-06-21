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
 * Copyright 2010 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 * Copyright 2017 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 */

#include <sys/types.h>
#include <sys/atomic.h>
#include <sys/kmem.h>
#include <sys/mutex.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/cmn_err.h>
#include <sys/debug.h>

#include <sys/fem.h>
#include <sys/vfs.h>
#include <sys/vfs_dispatch.h>
#include <sys/vnode.h>
#include <sys/vnode_dispatch.h>
#include <sys/vfs_opreg.h>

#define	NNODES_DEFAULT	8	/* Default number of nodes in a fem_list */
/*
 * fl_ntob(n) - Fem_list: number of nodes to bytes
 * Given the number of nodes in a fem_list return the size, in bytes,
 * of the fem_list structure.
 */
#define	fl_ntob(n)	(sizeof (struct fem_list) + \
			 (n) * sizeof (struct fem_node))

typedef enum {
	FEMTYPE_NULL,	/* Uninitialized */
	FEMTYPE_VNODE,
	FEMTYPE_VFS,
	FEMTYPE_NTYPES
} femtype_t;

#define	FEM_GUARD(_t) femtype[(_t)].guard

static struct fem_type_info {
	struct fem_node		head;
	struct fem_node		guard;
	femop_t			*errf;
}	femtype[FEMTYPE_NTYPES];


int fem_err();
int fsem_err();

static struct fem fem_guard_ops = {
	.name			= "fem-guard",
	.femop_open		= fem_err,
	.femop_close		= fem_err,
	.femop_read		= fem_err,
	.femop_write		= fem_err,
	.femop_ioctl		= fem_err,
	.femop_setfl		= fem_err,
	.femop_getattr		= fem_err,
	.femop_setattr		= fem_err,
	.femop_access		= fem_err,
	.femop_lookup		= fem_err,
	.femop_create		= fem_err,
	.femop_remove		= fem_err,
	.femop_link		= fem_err,
	.femop_rename		= fem_err,
	.femop_mkdir		= fem_err,
	.femop_rmdir		= fem_err,
	.femop_readdir		= fem_err,
	.femop_symlink		= fem_err,
	.femop_readlink		= fem_err,
	.femop_fsync		= fem_err,
	.femop_inactive		= (void (*)()) fem_err,
	.femop_fid		= fem_err,
	.femop_rwlock		= fem_err,
	.femop_rwunlock		= (void (*)()) fem_err,
	.femop_seek		= fem_err,
	.femop_cmp		= fem_err,
	.femop_frlock		= fem_err,
	.femop_space		= fem_err,
	.femop_realvp		= fem_err,
	.femop_getpage		= fem_err,
	.femop_putpage		= fem_err,
	.femop_map		= (void *) fem_err,
	.femop_addmap		= (void *) fem_err,
	.femop_delmap		= fem_err,
	.femop_poll		= (void *) fem_err,
	.femop_dump		= fem_err,
	.femop_pathconf		= fem_err,
	.femop_pageio		= fem_err,
	.femop_dumpctl		= fem_err,
	.femop_dispose		= (void *) fem_err,
	.femop_setsecattr	= fem_err,
	.femop_getsecattr	= fem_err,
	.femop_shrlock		= fem_err,
	.femop_vnevent		= fem_err,
	.femop_reqzcbuf		= fem_err,
	.femop_retzcbuf		= fem_err,
};

static struct fsem fsem_guard_ops = {
	.name			= "fsem-guard",
	.fsemop_mount		= fsem_err,
	.fsemop_unmount		= fsem_err,
	.fsemop_root		= fsem_err,
	.fsemop_statvfs		= fsem_err,
	.fsemop_sync		= (void *) fsem_err,
	.fsemop_vget		= fsem_err,
	.fsemop_mountroot	= fsem_err,
	.fsemop_freevfs		= (void *) fsem_err,
	.fsemop_vnstate		= fsem_err,
};


/*
 * vsop_find, vfsop_find -
 *
 * These macros descend the stack until they find either a basic
 * vnode/vfs operation [ indicated by a null fn_available ] or a
 * stacked item where this method is non-null [_vsop].
 */

#define	vsop_find(ap, _vsop) \
	_op_find((ap), offsetof(struct fem, _vsop))

#define	vfsop_find(ap, _fsop) \
	_op_find((ap), offsetof(struct fsem, _fsop))

static inline void *
_op_find(femarg_t *ap, size_t offs1)
{
	for (;;) {
		struct fem_node	*fnod = ap->fa_fnode;
		void *fp;

		if (fnod->fn_available == NULL)
			return NULL;

		fp = *(void **)((char *)fnod->fn_op.anon + offs1);
		if (fp != NULL)
			return fp;

		ap->fa_fnode--;
	}
}

/*
 * fem_get, fem_release - manage reference counts on the stack.
 *
 * The list of monitors can be updated while operations are in
 * progress on the object.
 *
 * The reference count facilitates this by counting the number of
 * current accessors, and deconstructing the list when it is exhausted.
 *
 * fem_lock() is required to:
 *	look at femh_list
 *	update what femh_list points to
 *	update femh_list
 *	increase femh_list->feml_refc.
 *
 * the feml_refc can decrement without holding the lock;
 * when feml_refc becomes zero, the list is destroyed.
 *
 */

static struct fem_list *
fem_lock(struct fem_head *fp)
{
	struct fem_list	*sp = NULL;

	ASSERT(fp != NULL);
	mutex_enter(&fp->femh_lock);
	sp = fp->femh_list;
	return (sp);
}

static void
fem_unlock(struct fem_head *fp)
{
	ASSERT(fp != NULL);
	mutex_exit(&fp->femh_lock);
}

/*
 * Addref can only be called while its head->lock is held.
 */

static void
fem_addref(struct fem_list *sp)
{
	atomic_inc_32(&sp->feml_refc);
}

static uint32_t
fem_delref(struct fem_list *sp)
{
	return (atomic_dec_32_nv(&sp->feml_refc));
}

static struct fem_list *
fem_get(struct fem_head *fp)
{
	struct fem_list *sp = NULL;

	if (fp != NULL) {
		if ((sp = fem_lock(fp)) != NULL) {
			fem_addref(sp);
		}
		fem_unlock(fp);
	}
	return (sp);
}

static void
fem_release(struct fem_list *sp)
{
	int	i;

	if (sp == NULL)
		return;

	ASSERT(sp->feml_refc != 0);
	if (fem_delref(sp) == 0) {
		/*
		 * Before freeing the list, we need to release the
		 * caller-provided data.
		 */
		for (i = sp->feml_tos; i > 0; i--) {
			struct fem_node *fnp = &sp->feml_nodes[i];

			if (fnp->fn_av_rele)
				(*(fnp->fn_av_rele))(fnp->fn_available);
		}
		kmem_free(sp, fl_ntob(sp->feml_ssize));
	}
}


/*
 * These are the 'head' operations which perform the interposition.
 *
 * This set must be 1:1, onto with the (vnodeops, vfsos).
 *
 * If there is a desire to globally disable interposition for a particular
 * method, the corresponding 'head' routine should unearth the base method
 * and invoke it directly rather than bypassing the function.
 *
 * All the functions are virtually the same, save for names, types & args.
 *  1. get a reference to the monitor stack for this object.
 *  2. store the top of stack into the femarg structure.
 *  3. store the basic object (vnode *, vnode **, vfs *) in the femarg struc.
 *  4. invoke the "top" method for this object.
 *  5. release the reference to the monitor stack.
 *
 */

int
vhead_open(vnode_t **vpp, int mode, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get((*vpp)->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vpp = vpp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_open);
	}

	if (func != NULL)
		ret = func(&farg, mode, cr, ct);
	else
		ret = fop_open_dispatch(vpp, mode, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_close(vnode_t *vp, int flag, int count, offset_t offset, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, int, offset_t, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_close);
	}

	if (func != NULL)
		ret = func(&farg, flag, count, offset, cr, ct);
	else
		ret = fop_close_dispatch(vp, flag, count, offset, cr, ct,
					 false);

	fem_release(femsp);

	return ret;
}

int
vhead_read(vnode_t *vp, uio_t *uiop, int ioflag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, uio_t *, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_read);
	}

	if (func != NULL)
		ret = func(&farg, uiop, ioflag, cr, ct);
	else
		ret = fop_read_dispatch(vp, uiop, ioflag, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_write(vnode_t *vp, uio_t *uiop, int ioflag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, uio_t *, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_write);
	}

	if (func != NULL)
		ret = func(&farg, uiop, ioflag, cr, ct);
	else
		ret = fop_write_dispatch(vp, uiop, ioflag, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_ioctl(vnode_t *vp, int cmd, intptr_t arg, int flag, cred_t *cr,
	int *rvalp, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, intptr_t, int, cred_t *, int *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_ioctl);
	}

	if (func != NULL)
		ret = func(&farg, cmd, arg, flag, cr, rvalp, ct);
	else
		ret = fop_ioctl_dispatch(vp, cmd, arg, flag, cr, rvalp, ct,
					 false);

	fem_release(femsp);

	return ret;
}

int
vhead_setfl(vnode_t *vp, int oflags, int nflags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_setfl);
	}

	if (func != NULL)
		ret = func(&farg, oflags, nflags, cr, ct);
	else
		ret = fop_setfl_dispatch(vp, oflags, nflags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_getattr(vnode_t *vp, vattr_t *vap, int flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vattr_t *, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_getattr);
	}

	if (func != NULL)
		ret = func(&farg, vap, flags, cr, ct);
	else
		ret = fop_getattr_dispatch(vp, vap, flags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_setattr(vnode_t *vp, vattr_t *vap, int flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vattr_t *, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_setattr);
	}

	if (func != NULL)
		ret = func(&farg, vap, flags, cr, ct);
	else
		ret = fop_setattr_dispatch(vp, vap, flags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_access(vnode_t *vp, int mode, int flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_access);
	}

	if (func != NULL)
		ret = func(&farg, mode, flags, cr, ct);
	else
		ret = fop_access_dispatch(vp, mode, flags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_lookup(vnode_t *dvp, char *nm, vnode_t **vpp, pathname_t *pnp,
	int flags, vnode_t *rdir, cred_t *cr, caller_context_t *ct,
	int *direntflags, pathname_t *realpnp)
{
	int (*func)(femarg_t *, char *, vnode_t **, pathname_t *, int,
		    vnode_t *, cred_t *, caller_context_t *, int *,
		    pathname_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(dvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = dvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_lookup);
	}

	if (func != NULL)
		ret = func(&farg, nm, vpp, pnp, flags, rdir, cr, ct,
			   direntflags, realpnp);
	else
		ret = fop_lookup_dispatch(dvp, nm, vpp, pnp, flags, rdir,
					  cr, ct, direntflags, realpnp, false);

	fem_release(femsp);

	return ret;
}

int
vhead_create(vnode_t *dvp, char *name, vattr_t *vap, vcexcl_t excl,
	int mode, vnode_t **vpp, cred_t *cr, int flag, caller_context_t *ct,
	vsecattr_t *vsecp)
{
	int (*func)(femarg_t *, char *, vattr_t *, vcexcl_t, int, vnode_t **,
		    cred_t *, int, caller_context_t *, vsecattr_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(dvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = dvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_create);
	}

	if (func != NULL)
		ret = func(&farg, name, vap, excl, mode, vpp, cr, flag, ct,
			   vsecp);
	else
		ret = fop_create_dispatch(dvp, name, vap, excl, mode, vpp,
					  cr, flag, ct, vsecp, false);

	fem_release(femsp);

	return ret;
}

int
vhead_remove(vnode_t *dvp, char *nm, cred_t *cr, caller_context_t *ct,
	int flags)
{
	int (*func)(femarg_t *, char *, cred_t *, caller_context_t *, int);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(dvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = dvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_remove);
	}

	if (func != NULL)
		ret = func(&farg, nm, cr, ct, flags);
	else
		ret = fop_remove_dispatch(dvp, nm, cr, ct, flags, false);

	fem_release(femsp);

	return ret;
}

int
vhead_link(vnode_t *tdvp, vnode_t *svp, char *tnm, cred_t *cr,
	caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, vnode_t *, char *, cred_t *,
		    caller_context_t *, int);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(tdvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = tdvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_link);
	}

	if (func != NULL)
		ret = func(&farg, svp, tnm, cr, ct, flags);
	else
		ret = fop_link_dispatch(tdvp, svp, tnm, cr, ct, flags, false);

	fem_release(femsp);

	return ret;
}

int
vhead_rename(vnode_t *sdvp, char *snm, vnode_t *tdvp, char *tnm,
	cred_t *cr, caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, char *, vnode_t *, char *, cred_t *,
		    caller_context_t *,int);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(sdvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = sdvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_rename);
	}

	if (func != NULL)
		ret = func(&farg, snm, tdvp, tnm, cr, ct, flags);
	else
		ret = fop_rename_dispatch(sdvp, snm, tdvp, tnm, cr, ct,
					  flags, false);

	fem_release(femsp);

	return ret;
}

int
vhead_mkdir(vnode_t *dvp, char *dirname, vattr_t *vap, vnode_t **vpp,
	cred_t *cr, caller_context_t *ct, int flags, vsecattr_t *vsecp)
{
	int (*func)(femarg_t *, char *, vattr_t *, vnode_t **, cred_t *,
		    caller_context_t *, int, vsecattr_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(dvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = dvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_mkdir);
	}

	if (func != NULL)
		ret = func(&farg, dirname, vap, vpp, cr, ct, flags, vsecp);
	else
		ret = fop_mkdir_dispatch(dvp, dirname, vap, vpp, cr, ct, flags,
					 vsecp, false);

	fem_release(femsp);

	return ret;
}

int
vhead_rmdir(vnode_t *dvp, char *nm, vnode_t *cdir, cred_t *cr,
	caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, char *, vnode_t *, cred_t *, caller_context_t *,
		    int);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(dvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = dvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_rmdir);
	}

	if (func != NULL)
		ret = func(&farg, nm, cdir, cr, ct, flags);
	else
		ret = fop_rmdir_dispatch(dvp, nm, cdir, cr, ct, flags, false);

	fem_release(femsp);

	return ret;
}

int
vhead_readdir(vnode_t *vp, uio_t *uiop, cred_t *cr, int *eofp,
	caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, uio_t *, cred_t *, int *, caller_context_t *,
		    int);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_readdir);
	}

	if (func != NULL)
		ret = func(&farg, uiop, cr, eofp, ct, flags);
	else
		ret = fop_readdir_dispatch(vp, uiop, cr, eofp, ct, flags,
					   false);

	fem_release(femsp);

	return ret;
}

int
vhead_symlink(vnode_t *dvp, char *linkname, vattr_t *vap, char *target,
	cred_t *cr, caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, char *, vattr_t *, char *, cred_t *,
		    caller_context_t *, int);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(dvp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = dvp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_symlink);
	}

	if (func != NULL)
		ret = func(&farg, linkname, vap, target, cr, ct, flags);
	else
		ret = fop_symlink_dispatch(dvp, linkname, vap, target, cr, ct,
					   flags, false);

	fem_release(femsp);

	return ret;
}

int
vhead_readlink(vnode_t *vp, uio_t *uiop, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, uio_t *, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_readlink);
	}

	if (func != NULL)
		ret = func(&farg, uiop, cr, ct);
	else
		ret = fop_readlink_dispatch(vp, uiop, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_fsync(vnode_t *vp, int syncflag, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_fsync);
	}

	if (func != NULL)
		ret = func(&farg, syncflag, cr, ct);
	else
		ret = fop_fsync_dispatch(vp, syncflag, cr, ct, false);

	fem_release(femsp);

	return ret;
}

void
vhead_inactive(vnode_t *vp, cred_t *cr, caller_context_t *ct)
{
	void (*func)(femarg_t *, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_inactive);
	}

	if (func != NULL)
		func(&farg, cr, ct);
	else
		fop_inactive_dispatch(vp, cr, ct, false);

	fem_release(femsp);
}

int
vhead_fid(vnode_t *vp, fid_t *fidp, caller_context_t *ct)
{
	int (*func)(femarg_t *, fid_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_fid);
	}

	if (func != NULL)
		ret = func(&farg, fidp, ct);
	else
		ret = fop_fid_dispatch(vp, fidp, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_rwlock(vnode_t *vp, int write_lock, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_rwlock);
	}

	if (func != NULL)
		ret = func(&farg, write_lock, ct);
	else
		ret = fop_rwlock_dispatch(vp, write_lock, ct, false);

	fem_release(femsp);

	return ret;
}

void
vhead_rwunlock(vnode_t *vp, int write_lock, caller_context_t *ct)
{
	void (*func)(femarg_t *, int, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_rwunlock);
	}

	if (func != NULL)
		func(&farg, write_lock, ct);
	else
		fop_rwunlock_dispatch(vp, write_lock, ct, false);

	fem_release(femsp);
}

int
vhead_seek(vnode_t *vp, offset_t ooff, offset_t *noffp, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, offset_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_seek);
	}

	if (func != NULL)
		ret = func(&farg, ooff, noffp, ct);
	else
		ret = fop_seek_dispatch(vp, ooff, noffp, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_cmp(vnode_t *vp1, vnode_t *vp2, caller_context_t *ct)
{
	int (*func)(femarg_t *, vnode_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp1->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp1;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_cmp);
	}

	if (func != NULL)
		ret = func(&farg, vp2, ct);
	else
		ret = fop_cmp_dispatch(vp1, vp2, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_frlock(vnode_t *vp, int cmd, struct flock64 *bfp, int flag,
	offset_t offset, struct flk_callback *flk_cbp, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, struct flock64 *, int, offset_t,
		    struct flk_callback *, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_frlock);
	}

	if (func != NULL)
		ret = func(&farg, cmd, bfp, flag, offset, flk_cbp, cr, ct);
	else
		ret = fop_frlock_dispatch(vp, cmd, bfp, flag, offset,
					  flk_cbp, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_space(vnode_t *vp, int cmd, struct flock64 *bfp, int flag,
	offset_t offset, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, struct flock64 *, int, offset_t,
		    cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_space);
	}

	if (func != NULL)
		ret = func(&farg, cmd, bfp, flag, offset, cr, ct);
	else
		ret = fop_space_dispatch(vp, cmd, bfp, flag, offset, cr, ct,
					 false);

	fem_release(femsp);

	return ret;
}

int
vhead_realvp(vnode_t *vp, vnode_t **vpp, caller_context_t *ct)
{
	int (*func)(femarg_t *, vnode_t **, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_realvp);
	}

	if (func != NULL)
		ret = func(&farg, vpp, ct);
	else
		ret = fop_realvp_dispatch(vp, vpp, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_getpage(vnode_t *vp, offset_t off, size_t len, uint_t *protp,
	struct page **plarr, size_t plsz, struct seg *seg, caddr_t addr,
	enum seg_rw rw, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, size_t, uint_t *, struct page **,
		    size_t, struct seg *, caddr_t, enum seg_rw, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_getpage);
	}

	if (func != NULL)
		ret = func(&farg, off, len, protp, plarr, plsz, seg, addr, rw,
			   cr, ct);
	else
		ret = fop_getpage_dispatch(vp, off, len, protp, plarr, plsz,
					   seg, addr, rw, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_putpage(vnode_t *vp, offset_t off, size_t len, int flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, size_t, int, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_putpage);
	}

	if (func != NULL)
		ret = func(&farg, off, len, flags, cr, ct);
	else
		ret = fop_putpage_dispatch(vp, off, len, flags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_map(vnode_t *vp, offset_t off, struct as *as, caddr_t *addrp,
	size_t len, uchar_t prot, uchar_t maxprot, uint_t flags,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, struct as *, caddr_t *, size_t,
		    uchar_t, uchar_t, uint_t, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_map);
	}

	if (func != NULL)
		ret = func(&farg, off, as, addrp, len, prot, maxprot, flags,
			   cr, ct);
	else
		ret = fop_map_dispatch(vp, off, as, addrp, len, prot,
				       maxprot, flags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_addmap(vnode_t *vp, offset_t off, struct as *as, caddr_t addr,
	size_t len, uchar_t prot, uchar_t maxprot, uint_t flags,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, struct as *, caddr_t, size_t, uchar_t,
		    uchar_t, uint_t, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_addmap);
	}

	if (func != NULL)
		ret = func(&farg, off, as, addr, len, prot, maxprot, flags,
			   cr, ct);
	else
		ret = fop_addmap_dispatch(vp, off, as, addr, len, prot,
					  maxprot, flags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_delmap(vnode_t *vp, offset_t off, struct as *as, caddr_t addr,
	size_t len, uint_t prot, uint_t maxprot, uint_t flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, struct as *, caddr_t, size_t, uint_t,
		    uint_t, uint_t, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_delmap);
	}

	if (func != NULL)
		ret = func(&farg, off, as, addr, len, prot, maxprot, flags,
			   cr, ct);
	else
		ret = fop_delmap_dispatch(vp, off, as, addr, len, prot,
					  maxprot, flags, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_poll(vnode_t *vp, short events, int anyyet, short *reventsp,
	struct pollhead **phpp, caller_context_t *ct)
{
	int (*func)(femarg_t *, short, int, short *, struct pollhead **,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_poll);
	}

	if (func != NULL)
		ret = func(&farg, events, anyyet, reventsp, phpp, ct);
	else
		ret = fop_poll_dispatch(vp, events, anyyet, reventsp, phpp,
					ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_dump(vnode_t *vp, caddr_t addr, offset_t lbdn, offset_t dblks,
    caller_context_t *ct)
{
	int (*func)(femarg_t *, caddr_t, offset_t, offset_t,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_dump);
	}

	if (func != NULL)
		ret = func(&farg, addr, lbdn, dblks, ct);
	else
		ret = fop_dump_dispatch(vp, addr, lbdn, dblks, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_pathconf(vnode_t *vp, int cmd, ulong_t *valp, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, ulong_t *, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_pathconf);
	}

	if (func != NULL)
		ret = func(&farg, cmd, valp, cr, ct);
	else
		ret = fop_pathconf_dispatch(vp, cmd, valp, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_pageio(vnode_t *vp, struct page *pp, uoff_t io_off,
	size_t io_len, int flags, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, struct page *, uoff_t, size_t, int, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_pageio);
	}

	if (func != NULL)
		ret = func(&farg, pp, io_off, io_len, flags, cr, ct);
	else
		ret = fop_pageio_dispatch(vp, pp, io_off, io_len, flags, cr,
					  ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_dumpctl(vnode_t *vp, int action, offset_t *blkp, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, offset_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_dumpctl);
	}

	if (func != NULL)
		ret = func(&farg, action, blkp, ct);
	else
		ret = fop_dumpctl_dispatch(vp, action, blkp, ct, false);

	fem_release(femsp);

	return ret;
}

void
vhead_dispose(vnode_t *vp, struct page *pp, int flag, int dn, cred_t *cr,
	caller_context_t *ct)
{
	void (*func)(femarg_t *, struct page *, int, int, cred_t *,
		     caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_dispose);
	}

	if (func != NULL)
		func(&farg, pp, flag, dn, cr, ct);
	else
		fop_dispose_dispatch(vp, pp, flag, dn, cr, ct, false);

	fem_release(femsp);
}

int
vhead_setsecattr(vnode_t *vp, vsecattr_t *vsap, int flag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vsecattr_t *, int, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_setsecattr);
	}

	if (func != NULL)
		ret = func(&farg, vsap, flag, cr, ct);
	else
		ret = fop_setsecattr_dispatch(vp, vsap, flag, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_getsecattr(vnode_t *vp, vsecattr_t *vsap, int flag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vsecattr_t *, int, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_getsecattr);
	}

	if (func != NULL)
		ret = func(&farg, vsap, flag, cr, ct);
	else
		ret = fop_getsecattr_dispatch(vp, vsap, flag, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_shrlock(vnode_t *vp, int cmd, struct shrlock *shr, int flag,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, struct shrlock *, int, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_shrlock);
	}

	if (func != NULL)
		ret = func(&farg, cmd, shr, flag, cr, ct);
	else
		ret = fop_shrlock_dispatch(vp, cmd, shr, flag, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_vnevent(vnode_t *vp, vnevent_t vnevent, vnode_t *dvp, char *cname,
    caller_context_t *ct)
{
	int (*func)(femarg_t *, vnevent_t, vnode_t *, char *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_vnevent);
	}

	if (func != NULL)
		ret = func(&farg, vnevent, dvp, cname, ct);
	else
		ret = fop_vnevent_dispatch(vp, vnevent, dvp, cname, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_reqzcbuf(vnode_t *vp, enum uio_rw ioflag, xuio_t *xuiop, cred_t *cr,
    caller_context_t *ct)
{
	int (*func)(femarg_t *, enum uio_rw, xuio_t *, cred_t *,
		    caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_reqzcbuf);
	}

	if (func != NULL)
		ret = func(&farg, ioflag, xuiop, cr, ct);
	else
		ret = fop_reqzcbuf_dispatch(vp, ioflag, xuiop, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
vhead_retzcbuf(vnode_t *vp, xuio_t *xuiop, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, xuio_t *, cred_t *, caller_context_t *);
	struct fem_list	*femsp;
	femarg_t farg;
	int ret;

	if ((femsp = fem_get(vp->v_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vp = vp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vsop_find(&farg, femop_retzcbuf);
	}

	if (func != NULL)
		ret = func(&farg, xuiop, cr, ct);
	else
		ret = fop_retzcbuf_dispatch(vp, xuiop, cr, ct, false);

	fem_release(femsp);

	return ret;
}

int
fshead_mount(vfs_t *vfsp, vnode_t *mvp, struct mounta *uap, cred_t *cr)
{
	int (*func)(fsemarg_t *, vnode_t *, struct mounta *, cred_t *);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_mount);
	}

	if (func != NULL)
		ret = func(&farg, mvp, uap, cr);
	else
		ret = fsop_mount_dispatch(vfsp, mvp, uap, cr, false);

	fem_release(femsp);

	return ret;
}

int
fshead_unmount(vfs_t *vfsp, int flag, cred_t *cr)
{
	int (*func)(fsemarg_t *, int, cred_t *);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_unmount);
	}

	if (func != NULL)
		ret = func(&farg, flag, cr);
	else
		ret = fsop_unmount_dispatch(vfsp, flag, cr, false);

	fem_release(femsp);

	return ret;
}

int
fshead_root(vfs_t *vfsp, vnode_t **vpp)
{
	int (*func)(fsemarg_t *, vnode_t **);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_root);
	}

	if (func != NULL)
		ret = func(&farg, vpp);
	else
		ret = fsop_root_dispatch(vfsp, vpp, false);

	fem_release(femsp);

	return ret;
}

int
fshead_statvfs(vfs_t *vfsp, statvfs64_t *sp)
{
	int (*func)(fsemarg_t *, statvfs64_t *);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_statvfs);
	}

	if (func != NULL)
		ret = func(&farg, sp);
	else
		ret = fsop_statfs_dispatch(vfsp, sp, false);

	fem_release(femsp);

	return ret;
}

int
fshead_sync(vfs_t *vfsp, short flag, cred_t *cr)
{
	int (*func)(fsemarg_t *, short, cred_t *);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_sync);
	}

	if (func != NULL)
		ret = func(&farg, flag, cr);
	else
		ret = fsop_sync_dispatch(vfsp, flag, cr, false);

	fem_release(femsp);

	return ret;
}

int
fshead_vget(vfs_t *vfsp, vnode_t **vpp, fid_t *fidp)
{
	int (*func)(fsemarg_t *, vnode_t **, fid_t *);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_vget);
	}

	if (func != NULL)
		ret = func(&farg, vpp, fidp);
	else
		ret = fsop_vget_dispatch(vfsp, vpp, fidp, false);

	fem_release(femsp);

	return ret;
}

int
fshead_mountroot(vfs_t *vfsp, enum whymountroot reason)
{
	int (*func)(fsemarg_t *, enum whymountroot);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_mountroot);
	}

	if (func != NULL)
		ret = func(&farg, reason);
	else
		ret = fsop_mountroot_dispatch(vfsp, reason, false);

	fem_release(femsp);

	return ret;
}

void
fshead_freevfs(vfs_t *vfsp)
{
	void (*func)(fsemarg_t *);
	struct fem_list	*femsp;
	fsemarg_t farg;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_freevfs);
	}

	if (func != NULL)
		func(&farg);
	else
		fsop_freefs_dispatch(vfsp, false);

	fem_release(femsp);
}

int
fshead_vnstate(vfs_t *vfsp, vnode_t *vp, vntrans_t nstate)
{
	int (*func)(fsemarg_t *, vnode_t *, vntrans_t);
	struct fem_list	*femsp;
	fsemarg_t farg;
	int ret;

	ASSERT(vfsp->vfs_implp);

	if ((femsp = fem_get(vfsp->vfs_femhead)) == NULL) {
		func = NULL;
	} else {
		farg.fa_vnode.vfsp = vfsp;
		farg.fa_fnode = femsp->feml_nodes + femsp->feml_tos;
		func = vfsop_find(&farg, fsemop_vnstate);
	}

	if (func != NULL)
		ret = func(&farg, vp, nstate);
	else
		ret = fsop_vnstate_dispatch(vfsp, vp, nstate, false);

	fem_release(femsp);

	return ret;
}


/*
 * This set of routines transfer control to the next stacked monitor.
 *
 * Each routine is identical except for naming, types and arguments.
 *
 * The basic steps are:
 * 1.  Decrease the stack pointer by one.
 * 2.  If the current item is a base operation (vnode, vfs), goto 5.
 * 3.  If the current item does not have a corresponding operation, goto 1
 * 4.  Return by invoking the current item with the argument handle.
 * 5.  Return by invoking the base operation with the base object.
 *
 * for each classification, there needs to be at least one "next" operation
 * for each "head"operation.
 *
 */

int
vnext_open(femarg_t *vf, int mode, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, cred_t *, caller_context_t *);
	struct vnode **vnode = vf->fa_vnode.vpp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_open);

	if (func != NULL)
		return func(vf, mode, cr, ct);

	return fop_open_dispatch(vnode, mode, cr, ct, false);
}

int
vnext_close(femarg_t *vf, int flag, int count, offset_t offset, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, int, offset_t, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_close);

	if (func != NULL)
		return func(vf, flag, count, offset, cr, ct);

	return fop_close_dispatch(vnode, flag, count, offset, cr, ct, false);
}

int
vnext_read(femarg_t *vf, uio_t *uiop, int ioflag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, uio_t *, int, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_read);

	if (func != NULL)
		return func(vf, uiop, ioflag, cr, ct);

	return fop_read_dispatch(vnode, uiop, ioflag, cr, ct, false);
}

int
vnext_write(femarg_t *vf, uio_t *uiop, int ioflag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, uio_t *, int, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_write);

	if (func != NULL)
		return func(vf, uiop, ioflag, cr, ct);

	return fop_write_dispatch(vnode, uiop, ioflag, cr, ct, false);
}

int
vnext_ioctl(femarg_t *vf, int cmd, intptr_t arg, int flag, cred_t *cr,
	int *rvalp, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, intptr_t, int, cred_t *, int *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_ioctl);

	if (func != NULL)
		return func(vf, cmd, arg, flag, cr, rvalp, ct);

	return fop_ioctl_dispatch(vnode, cmd, arg, flag, cr, rvalp, ct, false);
}

int
vnext_setfl(femarg_t *vf, int oflags, int nflags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, int, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_setfl);

	if (func != NULL)
		return func(vf, oflags, nflags, cr, ct);

	return fop_setfl_dispatch(vnode, oflags, nflags, cr, ct, false);
}

int
vnext_getattr(femarg_t *vf, vattr_t *vap, int flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vattr_t *, int, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_getattr);

	if (func != NULL)
		return func(vf, vap, flags, cr, ct);

	return fop_getattr_dispatch(vnode, vap, flags, cr, ct, false);
}

int
vnext_setattr(femarg_t *vf, vattr_t *vap, int flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vattr_t *, int, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_setattr);

	if (func != NULL)
		return func(vf, vap, flags, cr, ct);

	return fop_setattr_dispatch(vnode, vap, flags, cr, ct, false);
}

int
vnext_access(femarg_t *vf, int mode, int flags, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, int, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_access);

	if (func != NULL)
		return func(vf, mode, flags, cr, ct);

	return fop_access_dispatch(vnode, mode, flags, cr, ct, false);
}

int
vnext_lookup(femarg_t *vf, char *nm, vnode_t **vpp, pathname_t *pnp,
	int flags, vnode_t *rdir, cred_t *cr, caller_context_t *ct,
	int *direntflags, pathname_t *realpnp)
{
	int (*func)(femarg_t *, char *, vnode_t **, pathname_t *, int,
		    vnode_t *, cred_t *, caller_context_t *, int *,
		    pathname_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_lookup);

	if (func != NULL)
		return func(vf, nm, vpp, pnp, flags, rdir, cr, ct,
			    direntflags, realpnp);

	return fop_lookup_dispatch(vnode, nm, vpp, pnp, flags, rdir, cr, ct,
				   direntflags, realpnp, false);
}

int
vnext_create(femarg_t *vf, char *name, vattr_t *vap, vcexcl_t excl,
	int mode, vnode_t **vpp, cred_t *cr, int flag, caller_context_t *ct,
	vsecattr_t *vsecp)
{
	int (*func)(femarg_t *, char *, vattr_t *, vcexcl_t, int, vnode_t **,
		    cred_t *, int, caller_context_t *, vsecattr_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_create);

	if (func != NULL)
		return func(vf, name, vap, excl, mode, vpp, cr, flag, ct,
			    vsecp);

	return fop_create_dispatch(vnode, name, vap, excl, mode, vpp, cr, flag,
				   ct, vsecp, false);
}

int
vnext_remove(femarg_t *vf, char *nm, cred_t *cr, caller_context_t *ct,
	int flags)
{
	int (*func)(femarg_t *, char *, cred_t *, caller_context_t *, int);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_remove);

	if (func != NULL)
		return func(vf, nm, cr, ct, flags);

	return fop_remove_dispatch(vnode, nm, cr, ct, flags, false);
}

int
vnext_link(femarg_t *vf, vnode_t *svp, char *tnm, cred_t *cr,
	caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, vnode_t *, char *, cred_t *,
		    caller_context_t *, int);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_link);

	if (func != NULL)
		return func(vf, svp, tnm, cr, ct, flags);

	return fop_link_dispatch(vnode, svp, tnm, cr, ct, flags, false);
}

int
vnext_rename(femarg_t *vf, char *snm, vnode_t *tdvp, char *tnm, cred_t *cr,
	caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, char *, vnode_t *, char *, cred_t *,
		    caller_context_t *,int);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_rename);

	if (func != NULL)
		return func(vf, snm, tdvp, tnm, cr, ct, flags);

	return fop_rename_dispatch(vnode, snm, tdvp, tnm, cr, ct, flags, false);
}

int
vnext_mkdir(femarg_t *vf, char *dirname, vattr_t *vap, vnode_t **vpp,
	cred_t *cr, caller_context_t *ct, int flags, vsecattr_t *vsecp)
{
	int (*func)(femarg_t *, char *, vattr_t *, vnode_t **, cred_t *,
		    caller_context_t *, int, vsecattr_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_mkdir);

	if (func != NULL)
		return func(vf, dirname, vap, vpp, cr, ct, flags, vsecp);

	return fop_mkdir_dispatch(vnode, dirname, vap, vpp, cr, ct, flags,
				  vsecp, false);
}

int
vnext_rmdir(femarg_t *vf, char *nm, vnode_t *cdir, cred_t *cr,
	caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, char *, vnode_t *, cred_t *, caller_context_t *,
		    int);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_rmdir);

	if (func != NULL)
		return func(vf, nm, cdir, cr, ct, flags);

	return fop_rmdir_dispatch(vnode, nm, cdir, cr, ct, flags, false);
}

int
vnext_readdir(femarg_t *vf, uio_t *uiop, cred_t *cr, int *eofp,
	caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, uio_t *, cred_t *, int *, caller_context_t *,
		    int);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_readdir);

	if (func != NULL)
		return func(vf, uiop, cr, eofp, ct, flags);

	return fop_readdir_dispatch(vnode, uiop, cr, eofp, ct, flags, false);
}

int
vnext_symlink(femarg_t *vf, char *linkname, vattr_t *vap, char *target,
	cred_t *cr, caller_context_t *ct, int flags)
{
	int (*func)(femarg_t *, char *, vattr_t *, char *, cred_t *,
		    caller_context_t *, int);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_symlink);

	if (func != NULL)
		return func(vf, linkname, vap, target, cr, ct, flags);

	return fop_symlink_dispatch(vnode, linkname, vap, target, cr, ct,
				    flags, false);
}

int
vnext_readlink(femarg_t *vf, uio_t *uiop, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, uio_t *, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_readlink);

	if (func != NULL)
		return func(vf, uiop, cr, ct);

	return fop_readlink_dispatch(vnode, uiop, cr, ct, false);
}

int
vnext_fsync(femarg_t *vf, int syncflag, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_fsync);

	if (func != NULL)
		return func(vf, syncflag, cr, ct);

	return fop_fsync_dispatch(vnode, syncflag, cr, ct, false);
}

void
vnext_inactive(femarg_t *vf, cred_t *cr, caller_context_t *ct)
{
	void (*func)(femarg_t *, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_inactive);

	if (func != NULL)
		func(vf, cr, ct);
	else
		fop_inactive_dispatch(vnode, cr, ct, false);
}

int
vnext_fid(femarg_t *vf, fid_t *fidp, caller_context_t *ct)
{
	int (*func)(femarg_t *, fid_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_fid);

	if (func != NULL)
		return func(vf, fidp, ct);

	return fop_fid_dispatch(vnode, fidp, ct, false);
}

int
vnext_rwlock(femarg_t *vf, int write_lock, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_rwlock);

	if (func != NULL)
		return func(vf, write_lock, ct);

	return fop_rwlock_dispatch(vnode, write_lock, ct, false);
}

void
vnext_rwunlock(femarg_t *vf, int write_lock, caller_context_t *ct)
{
	void (*func)(femarg_t *, int, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_rwunlock);

	if (func != NULL)
		func(vf, write_lock, ct);
	else
		fop_rwunlock_dispatch(vnode, write_lock, ct, false);
}

int
vnext_seek(femarg_t *vf, offset_t ooff, offset_t *noffp, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, offset_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_seek);

	if (func != NULL)
		return func(vf, ooff, noffp, ct);

	return fop_seek_dispatch(vnode, ooff, noffp, ct, false);
}

int
vnext_cmp(femarg_t *vf, vnode_t *vp2, caller_context_t *ct)
{
	int (*func)(femarg_t *, vnode_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_cmp);

	if (func != NULL)
		return func(vf, vp2, ct);

	return fop_cmp_dispatch(vnode, vp2, ct, false);
}

int
vnext_frlock(femarg_t *vf, int cmd, struct flock64 *bfp, int flag,
	offset_t offset, struct flk_callback *flk_cbp, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, struct flock64 *, int, offset_t,
		    struct flk_callback *, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_frlock);

	if (func != NULL)
		return func(vf, cmd, bfp, flag, offset, flk_cbp, cr, ct);

	return fop_frlock_dispatch(vnode, cmd, bfp, flag, offset, flk_cbp, cr,
				   ct, false);
}

int
vnext_space(femarg_t *vf, int cmd, struct flock64 *bfp, int flag,
	offset_t offset, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, struct flock64 *, int, offset_t,
		    cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_space);

	if (func != NULL)
		return func(vf, cmd, bfp, flag, offset, cr, ct);

	return fop_space_dispatch(vnode, cmd, bfp, flag, offset, cr, ct, false);
}

int
vnext_realvp(femarg_t *vf, vnode_t **vpp, caller_context_t *ct)
{
	int (*func)(femarg_t *, vnode_t **, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_realvp);

	if (func != NULL)
		return func(vf, vpp, ct);

	return fop_realvp_dispatch(vnode, vpp, ct, false);
}

int
vnext_getpage(femarg_t *vf, offset_t off, size_t len, uint_t *protp,
	struct page **plarr, size_t plsz, struct seg *seg, caddr_t addr,
	enum seg_rw rw, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, size_t, uint_t *, struct page **,
		    size_t, struct seg *, caddr_t, enum seg_rw, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_getpage);

	if (func != NULL)
		return func(vf, off, len, protp, plarr, plsz, seg, addr, rw,
			    cr, ct);

	return fop_getpage_dispatch(vnode, off, len, protp, plarr, plsz, seg, addr,
				    rw, cr, ct, false);
}

int
vnext_putpage(femarg_t *vf, offset_t off, size_t len, int flags,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, size_t, int, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_putpage);

	if (func != NULL)
		return func(vf, off, len, flags, cr, ct);

	return fop_putpage_dispatch(vnode, off, len, flags, cr, ct, false);
}

int
vnext_map(femarg_t *vf, offset_t off, struct as *as, caddr_t *addrp,
	size_t len, uchar_t prot, uchar_t maxprot, uint_t flags,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, struct as *, caddr_t *, size_t,
		    uchar_t, uchar_t, uint_t, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_map);

	if (func != NULL)
		return func(vf, off, as, addrp, len, prot, maxprot, flags,
			    cr, ct);

	return fop_map_dispatch(vnode, off, as, addrp, len, prot, maxprot, flags,
				cr, ct, false);
}

int
vnext_addmap(femarg_t *vf, offset_t off, struct as *as, caddr_t addr,
	size_t len, uchar_t prot, uchar_t maxprot, uint_t flags,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, struct as *, caddr_t, size_t, uchar_t,
		    uchar_t, uint_t, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_addmap);

	if (func != NULL)
		return func(vf, off, as, addr, len, prot, maxprot, flags,
			    cr, ct);

	return fop_addmap_dispatch(vnode, off, as, addr, len, prot, maxprot, flags,
				   cr, ct, false);
}

int
vnext_delmap(femarg_t *vf, offset_t off, struct as *as, caddr_t addr,
	size_t len, uint_t prot, uint_t maxprot, uint_t flags,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, offset_t, struct as *, caddr_t, size_t, uint_t,
		    uint_t, uint_t, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_delmap);

	if (func != NULL)
		return func(vf, off, as, addr, len, prot, maxprot, flags,
			    cr, ct);

	return fop_delmap_dispatch(vnode, off, as, addr, len, prot, maxprot, flags,
				   cr, ct, false);
}

int
vnext_poll(femarg_t *vf, short events, int anyyet, short *reventsp,
	struct pollhead **phpp, caller_context_t *ct)
{
	int (*func)(femarg_t *, short, int, short *, struct pollhead **,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_poll);

	if (func != NULL)
		return func(vf, events, anyyet, reventsp, phpp, ct);

	return fop_poll_dispatch(vnode, events, anyyet, reventsp, phpp, ct, false);
}

int
vnext_dump(femarg_t *vf, caddr_t addr, offset_t lbdn, offset_t dblks,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, caddr_t, offset_t, offset_t,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_dump);

	if (func != NULL)
		return func(vf, addr, lbdn, dblks, ct);

	return fop_dump_dispatch(vnode, addr, lbdn, dblks, ct, false);
}

int
vnext_pathconf(femarg_t *vf, int cmd, ulong_t *valp, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, int, ulong_t *, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_pathconf);

	if (func != NULL)
		return func(vf, cmd, valp, cr, ct);

	return fop_pathconf_dispatch(vnode, cmd, valp, cr, ct, false);
}

int
vnext_pageio(femarg_t *vf, struct page *pp, uoff_t io_off,
	size_t io_len, int flags, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, struct page *, uoff_t, size_t, int, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_pageio);

	if (func != NULL)
		return func(vf, pp, io_off, io_len, flags, cr, ct);

	return fop_pageio_dispatch(vnode, pp, io_off, io_len, flags, cr, ct,
				   false);
}

int
vnext_dumpctl(femarg_t *vf, int action, offset_t *blkp, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, offset_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_dumpctl);

	if (func != NULL)
		return func(vf, action, blkp, ct);

	return fop_dumpctl_dispatch(vnode, action, blkp, ct, false);
}

void
vnext_dispose(femarg_t *vf, struct page *pp, int flag, int dn, cred_t *cr,
	caller_context_t *ct)
{
	void (*func)(femarg_t *, struct page *, int, int, cred_t *,
		     caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_dispose);

	if (func != NULL)
		func(vf, pp, flag, dn, cr, ct);
	else
		fop_dispose_dispatch(vnode, pp, flag, dn, cr, ct, false);
}

int
vnext_setsecattr(femarg_t *vf, vsecattr_t *vsap, int flag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vsecattr_t *, int, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_setsecattr);

	if (func != NULL)
		return func(vf, vsap, flag, cr, ct);

	return fop_setsecattr_dispatch(vnode, vsap, flag, cr, ct, false);
}

int
vnext_getsecattr(femarg_t *vf, vsecattr_t *vsap, int flag, cred_t *cr,
	caller_context_t *ct)
{
	int (*func)(femarg_t *, vsecattr_t *, int, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_getsecattr);

	if (func != NULL)
		return func(vf, vsap, flag, cr, ct);

	return fop_getsecattr_dispatch(vnode, vsap, flag, cr, ct, false);
}

int
vnext_shrlock(femarg_t *vf, int cmd, struct shrlock *shr, int flag,
	cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, int, struct shrlock *, int, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_shrlock);

	if (func != NULL)
		return func(vf, cmd, shr, flag, cr, ct);

	return fop_shrlock_dispatch(vnode, cmd, shr, flag, cr, ct, false);
}

int
vnext_vnevent(femarg_t *vf, vnevent_t vnevent, vnode_t *dvp, char *cname,
    caller_context_t *ct)
{
	int (*func)(femarg_t *, vnevent_t, vnode_t *, char *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_vnevent);

	if (func != NULL)
		return func(vf, vnevent, dvp, cname, ct);

	return fop_vnevent_dispatch(vnode, vnevent, dvp, cname, ct, false);
}

int
vnext_reqzcbuf(femarg_t *vf, enum uio_rw ioflag, xuio_t *xuiop, cred_t *cr,
    caller_context_t *ct)
{
	int (*func)(femarg_t *, enum uio_rw, xuio_t *, cred_t *,
		    caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_reqzcbuf);

	if (func != NULL)
		return func(vf, ioflag, xuiop, cr, ct);

	return fop_reqzcbuf_dispatch(vnode, ioflag, xuiop, cr, ct, false);
}

int
vnext_retzcbuf(femarg_t *vf, xuio_t *xuiop, cred_t *cr, caller_context_t *ct)
{
	int (*func)(femarg_t *, xuio_t *, cred_t *, caller_context_t *);
	struct vnode *vnode = vf->fa_vnode.vp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vsop_find(vf, femop_retzcbuf);

	if (func != NULL)
		return func(vf, xuiop, cr, ct);

	return fop_retzcbuf_dispatch(vnode, xuiop, cr, ct, false);
}

int
vfsnext_mount(fsemarg_t *vf, vnode_t *mvp, struct mounta *uap, cred_t *cr)
{
	int (*func)(fsemarg_t *, vnode_t *, struct mounta *, cred_t *);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_mount);

	if (func != NULL)
		return func(vf, mvp, uap, cr);

	return fsop_mount_dispatch(vfs, mvp, uap, cr, false);
}

int
vfsnext_unmount(fsemarg_t *vf, int flag, cred_t *cr)
{
	int (*func)(fsemarg_t *, int, cred_t *);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_unmount);

	if (func != NULL)
		return func(vf, flag, cr);

	return fsop_unmount_dispatch(vfs, flag, cr, false);
}

int
vfsnext_root(fsemarg_t *vf, vnode_t **vpp)
{
	int (*func)(fsemarg_t *, vnode_t **);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_root);

	if (func != NULL)
		return func(vf, vpp);

	return fsop_root_dispatch(vfs, vpp, false);
}

int
vfsnext_statvfs(fsemarg_t *vf, statvfs64_t *sp)
{
	int (*func)(fsemarg_t *, statvfs64_t *);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_statvfs);

	if (func != NULL)
		return func(vf, sp);

	return fsop_statfs_dispatch(vfs, sp, false);
}

int
vfsnext_sync(fsemarg_t *vf, short flag, cred_t *cr)
{
	int (*func)(fsemarg_t *, short, cred_t *);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_sync);

	if (func != NULL)
		return func(vf, flag, cr);

	return fsop_sync_dispatch(vfs, flag, cr, false);
}

int
vfsnext_vget(fsemarg_t *vf, vnode_t **vpp, fid_t *fidp)
{
	int (*func)(fsemarg_t *, vnode_t **, fid_t *);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_vget);

	if (func != NULL)
		return func(vf, vpp, fidp);

	return fsop_vget_dispatch(vfs, vpp, fidp, false);
}

int
vfsnext_mountroot(fsemarg_t *vf, enum whymountroot reason)
{
	int (*func)(fsemarg_t *, enum whymountroot);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_mountroot);

	if (func != NULL)
		return func(vf, reason);

	return fsop_mountroot_dispatch(vfs, reason, false);
}

void
vfsnext_freevfs(fsemarg_t *vf)
{
	void (*func)(fsemarg_t *);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_freevfs);

	if (func != NULL)
		func(vf);
	else
		fsop_freefs_dispatch(vfs, false);
}

int
vfsnext_vnstate(fsemarg_t *vf, vnode_t *vp, vntrans_t nstate)
{
	int (*func)(fsemarg_t *, vnode_t *, vntrans_t);
	struct vfs *vfs = vf->fa_vnode.vfsp;

	ASSERT(vf != NULL);
	vf->fa_fnode--;
	func = vfsop_find(vf, fsemop_vnstate);

	if (func != NULL)
		return func(vf, vp, nstate);

	return fsop_vnstate_dispatch(vfs, vp, nstate, false);
}


/*
 * Create a new fem_head and associate with the vnode.
 * To keep the unaugmented vnode access path lock free, we spin
 * update this - create a new one, then try and install it. If
 * we fail to install, release the old one and pretend we succeeded.
 */

static struct fem_head *
new_femhead(struct fem_head **hp)
{
	struct fem_head	*head;

	head = kmem_alloc(sizeof (*head), KM_SLEEP);
	mutex_init(&head->femh_lock, NULL, MUTEX_DEFAULT, NULL);
	head->femh_list = NULL;
	if (atomic_cas_ptr(hp, NULL, head) != NULL) {
		kmem_free(head, sizeof (*head));
		head = *hp;
	}
	return (head);
}

/*
 * Create a fem_list.  The fem_list that gets returned is in a
 * very rudimentary state and MUST NOT be used until it's initialized
 * (usually by femlist_construct() or fem_dup_list()).  The refcount
 * and size is set properly and top-of-stack is set to the "guard" node
 * just to be consistent.
 *
 * If anyone were to accidentally trying to run on this fem_list before
 * it's initialized then the system would likely panic trying to defererence
 * the (NULL) fn_op pointer.
 *
 */
static struct fem_list *
femlist_create(int numnodes)
{
	struct fem_list	*sp;

	sp = kmem_alloc(fl_ntob(numnodes), KM_SLEEP);
	sp->feml_refc  = 1;
	sp->feml_ssize = numnodes;
	sp->feml_nodes[0] = FEM_GUARD(FEMTYPE_NULL);
	sp->feml_tos = 0;
	return (sp);
}

/*
 * Construct a new femlist.
 * The list is constructed with the appropriate type of guard to
 * anchor it, and inserts the original ops.
 */

static struct fem_list *
femlist_construct(int type, int numnodes)
{
	struct fem_list	*sp;

	sp = femlist_create(numnodes);
	sp->feml_nodes[0] = FEM_GUARD(type);
	sp->feml_nodes[1].fn_op.anon = NULL;
	sp->feml_nodes[1].fn_available = NULL;
	sp->feml_nodes[1].fn_av_hold = NULL;
	sp->feml_nodes[1].fn_av_rele = NULL;
	sp->feml_tos = 1;
	return (sp);
}

/*
 * Duplicate a list.  Copy the original list to the clone.
 *
 * NOTE: The caller must have the fem_head for the lists locked.
 * Assuming the appropriate lock is held and the caller has done the
 * math right, the clone list should be big enough to old the original.
 */

static void
fem_dup_list(struct fem_list *orig, struct fem_list *clone)
{
	int		i;

	ASSERT(clone->feml_ssize >= orig->feml_ssize);

	bcopy(orig->feml_nodes, clone->feml_nodes,
	    sizeof (orig->feml_nodes[0]) * orig->feml_ssize);
	clone->feml_tos = orig->feml_tos;
	/*
	 * Now that we've copied the old list (orig) to the new list (clone),
	 * we need to walk the new list and put another hold on fn_available.
	 */
	for (i = clone->feml_tos; i > 0; i--) {
		struct fem_node *fnp = &clone->feml_nodes[i];

		if (fnp->fn_av_hold)
			(*(fnp->fn_av_hold))(fnp->fn_available);
	}
}


static int
fem_push_node(
	struct fem_head **hp,
	int type,
	struct fem_node *nnode,
	femhow_t how)
{
	struct fem_head	*hd;
	struct fem_list	*list;
	int		retry;
	int		error = 0;
	int		i;

	/* Validate the node */
	if ((nnode->fn_op.anon == NULL) || (nnode->fn_available == NULL)) {
		return (EINVAL);
	}

	if ((hd = *hp) == NULL) { /* construct a proto-list */
		hd = new_femhead(hp);
	}
	/*
	 * RULE: once a femhead has been pushed onto a object, it cannot be
	 * removed until the object is destroyed.  It can be deactivated by
	 * placing the original 'object operations' onto the object, which
	 * will ignore the femhead.
	 * The loop will exist when the femh_list has space to push a monitor
	 * onto it.
	 */
	do {
		retry = 1;
		list = fem_lock(hd);

		if (list != NULL) {
			if (list->feml_tos+1 < list->feml_ssize) {
				retry = 0;
			} else {
				struct fem_list	*olist = list;

				fem_addref(olist);
				fem_unlock(hd);
				list = femlist_create(olist->feml_ssize * 2);
				(void) fem_lock(hd);
				if (hd->femh_list == olist) {
					if (list->feml_ssize <=
					    olist->feml_ssize) {
						/*
						 * We have a new list, but it
						 * is too small to hold the
						 * original contents plus the
						 * one to push.  Release the
						 * new list and start over.
						 */
						fem_release(list);
						fem_unlock(hd);
					} else {
						/*
						 * Life is good:  Our new list
						 * is big enough to hold the
						 * original list (olist) + 1.
						 */
						fem_dup_list(olist, list);
						/* orphan this list */
						hd->femh_list = list;
						(void) fem_delref(olist);
						retry = 0;
					}
				} else {
					/* concurrent update, retry */
					fem_release(list);
					fem_unlock(hd);
				}
				/* remove the reference we added above */
				fem_release(olist);
			}
		} else {
			fem_unlock(hd);
			list = femlist_construct(type, NNODES_DEFAULT);
			(void) fem_lock(hd);
			if (hd->femh_list != NULL) {
				/* concurrent update, retry */
				fem_release(list);
				fem_unlock(hd);
			} else {
				hd->femh_list = list;
				retry = 0;
			}
		}
	} while (retry);

	ASSERT(mutex_owner(&hd->femh_lock) == curthread);
	ASSERT(list->feml_tos+1 < list->feml_ssize);

	/*
	 * The presence of "how" will modify the behavior of how/if
	 * nodes are pushed.  If it's FORCE, then we can skip
	 * all the checks and push it on.
	 */
	if (how != FORCE) {
		/* Start at the top and work our way down */
		for (i = list->feml_tos; i > 0; i--) {
			void *fn_av = list->feml_nodes[i].fn_available;
			void *fn_op = list->feml_nodes[i].fn_op.anon;

			/*
			 * OPARGUNIQ means that this node should not
			 * be pushed on if a node with the same op/avail
			 * combination exists.  This situation returns
			 * EBUSY.
			 *
			 * OPUNIQ means that this node should not be
			 * pushed on if a node with the same op exists.
			 * This situation also returns EBUSY.
			 */
			switch (how) {

			case OPUNIQ:
				if (fn_op == nnode->fn_op.anon) {
					error = EBUSY;
				}
				break;

			case OPARGUNIQ:
				if ((fn_op == nnode->fn_op.anon) &&
				    (fn_av == nnode->fn_available)) {
					error = EBUSY;
				}
				break;

			default:
				error = EINVAL;	/* Unexpected value */
				break;
			}

			if (error)
				break;
		}
	}

	if (error == 0) {
		/*
		 * If no errors, slap the node on the list.
		 * Note: The following is a structure copy.
		 */
		list->feml_nodes[++(list->feml_tos)] = *nnode;
	}

	fem_unlock(hd);
	return (error);
}

/*
 * Remove a node by copying the list above it down a notch.
 * If the list is busy, replace it with an idle one and work
 * upon it.
 * A node matches if the opset matches and the datap matches or is
 * null.
 */

static int
remove_node(struct fem_list *sp, void *opset, void *datap)
{
	int	i;
	struct fem_node	*fn;

	for (i = sp->feml_tos; i > 0; i--) {
		fn = sp->feml_nodes+i;
		if (fn->fn_op.anon == opset &&
		    (fn->fn_available == datap || datap == NULL)) {
			break;
		}
	}
	if (i == 0) {
		return (EINVAL);
	}

	/*
	 * At this point we have a node in-hand (*fn) that we are about
	 * to remove by overwriting it and adjusting the stack.  This is
	 * our last chance to do anything with this node so we do the
	 * release on the arg.
	 */
	if (fn->fn_av_rele)
		(*(fn->fn_av_rele))(fn->fn_available);

	while (i++ < sp->feml_tos) {
		sp->feml_nodes[i-1] = sp->feml_nodes[i];
	}
	return (0);
}

static int
fem_remove_node(struct fem_head *fh, void *opset, void *datap)
{
	struct fem_list *sp;
	int		error = 0;
	int		retry;

	if (fh == NULL) {
		return (EINVAL);
	}

	do {
		retry = 0;
		if ((sp = fem_lock(fh)) == NULL) {
			fem_unlock(fh);
			error = EINVAL;
		} else if (sp->feml_refc == 1) {
			error = remove_node(sp, opset, datap);
			if (sp->feml_tos == 1) {
				/*
				 * The top-of-stack was decremented by
				 * remove_node().  If it got down to 1,
				 * then the base ops were replaced and we
				 * call fem_release() which will free the
				 * fem_list.
				 */
				fem_release(sp);
				fh->femh_list = NULL;
				/* XXX - Do we need a membar_producer() call? */
			}
			fem_unlock(fh);
		} else {
			/* busy - install a new one without this monitor */
			struct fem_list *nsp;	/* New fem_list being cloned */

			fem_addref(sp);
			fem_unlock(fh);
			nsp = femlist_create(sp->feml_ssize);
			if (fem_lock(fh) == sp) {
				/*
				 * We popped out of the lock, created a
				 * list, then relocked.  If we're in here
				 * then the fem_head points to the same list
				 * it started with.
				 */
				fem_dup_list(sp, nsp);
				error = remove_node(nsp, opset, datap);
				if (error != 0) {
					fem_release(nsp);
				} else if (nsp->feml_tos == 1) {
					/* New list now empty, tear it down */
					fem_release(nsp);
					fh->femh_list = NULL;
				} else {
					fh->femh_list = nsp;
				}
				(void) fem_delref(sp);
			} else {
				/* List changed while locked, try again... */
				fem_release(nsp);
				retry = 1;
			}
			/*
			 * If error is set, then we tried to remove a node
			 * from the list, but failed.  This means that we
			 * will still be using this list so don't release it.
			 */
			if (error == 0)
				fem_release(sp);
			fem_unlock(fh);
		}
	} while (retry);
	return (error);
}


/*
 * perform operation on each element until one returns non zero
 */
static int
fem_walk_list(
	struct fem_list *sp,
	int (*f)(struct fem_node *, void *, void *),
	void *mon,
	void *arg)
{
	int	i;

	ASSERT(sp != NULL);
	for (i = sp->feml_tos; i > 0; i--) {
		if ((*f)(sp->feml_nodes+i, mon, arg) != 0) {
			break;
		}
	}
	return (i);
}

/*
 * companion comparison functions.
 */
static int
fem_compare_mon(struct fem_node *n, void *mon, void *arg)
{
	return ((n->fn_op.anon == mon) && (n->fn_available == arg));
}

/*
 * VNODE interposition.
 */

int
fem_install(
	vnode_t *vp,		/* Vnode on which monitor is being installed */
	fem_t *mon,		/* Monitor operations being installed */
	void *arg,		/* Opaque data used by monitor */
	femhow_t how,		/* Installation control */
	void (*arg_hold)(void *),	/* Hold routine for "arg" */
	void (*arg_rele)(void *))	/* Release routine for "arg" */
{
	int	error;
	struct fem_node	nnode;

	nnode.fn_available = arg;
	nnode.fn_op.fem = mon;
	nnode.fn_av_hold = arg_hold;
	nnode.fn_av_rele = arg_rele;
	/*
	 * If we have a non-NULL hold function, do the hold right away.
	 * The release is done in remove_node().
	 */
	if (arg_hold)
		(*arg_hold)(arg);

	error = fem_push_node(&vp->v_femhead, FEMTYPE_VNODE, &nnode, how);

	/* If there was an error then the monitor wasn't pushed */
	if (error && arg_rele)
		(*arg_rele)(arg);

	return (error);
}

int
fem_is_installed(vnode_t *v, fem_t *mon, void *arg)
{
	int	e;
	struct fem_list	*fl;

	fl = fem_get(v->v_femhead);
	if (fl != NULL) {
		e = fem_walk_list(fl, fem_compare_mon, mon, arg);
		fem_release(fl);
		return (e);
	}
	return (0);
}

int
fem_uninstall(vnode_t *v, fem_t *mon, void *arg)
{
	int	e;
	e = fem_remove_node(v->v_femhead, mon, arg);
	return (e);
}

/*
 * VFS interposition
 *
 * These need to be re-written, but there should be more common bits.
 */

int
fsem_is_installed(struct vfs *v, fsem_t *mon, void *arg)
{
	struct fem_list	*fl;

	if (v->vfs_implp == NULL)
		return (0);

	fl = fem_get(v->vfs_femhead);
	if (fl != NULL) {
		int	e;
		e = fem_walk_list(fl, fem_compare_mon, mon, arg);
		fem_release(fl);
		return (e);
	}
	return (0);
}

int
fsem_install(
	struct vfs *vfsp,	/* VFS on which monitor is being installed */
	fsem_t *mon,		/* Monitor operations being installed */
	void *arg,		/* Opaque data used by monitor */
	femhow_t how,		/* Installation control */
	void (*arg_hold)(void *),	/* Hold routine for "arg" */
	void (*arg_rele)(void *))	/* Release routine for "arg" */
{
	int	error;
	struct fem_node	nnode;

	/* If this vfs hasn't been properly initialized, fail the install */
	if (vfsp->vfs_implp == NULL)
		return (EINVAL);

	nnode.fn_available = arg;
	nnode.fn_op.fsem = mon;
	nnode.fn_av_hold = arg_hold;
	nnode.fn_av_rele = arg_rele;
	/*
	 * If we have a non-NULL hold function, do the hold right away.
	 * The release is done in remove_node().
	 */
	if (arg_hold)
		(*arg_hold)(arg);

	error = fem_push_node(&vfsp->vfs_femhead, FEMTYPE_VFS, &nnode, how);

	/* If there was an error then the monitor wasn't pushed */
	if (error && arg_rele)
		(*arg_rele)(arg);

	return (error);
}

int
fsem_uninstall(struct vfs *v, fsem_t *mon, void *arg)
{
	int	e;

	if (v->vfs_implp == NULL)
		return (EINVAL);

	e = fem_remove_node(v->vfs_femhead, mon, arg);
	return (e);
}

/*
 * Setup FEM.
 */
void
fem_init()
{
	struct fem_type_info   *fi;

	/*
	 * This femtype is only used for fem_list creation so we only
	 * need the "guard" to be initialized so that feml_tos has
	 * some rudimentary meaning.  A fem_list must not be used until
	 * it has been initialized (either via femlist_construct() or
	 * fem_dup_list()).  Anything that tries to use this fem_list
	 * before it's actually initialized would panic the system as
	 * soon as "fn_op" (NULL) is dereferenced.
	 */
	fi = &femtype[FEMTYPE_NULL];
	fi->errf = fem_err;
	fi->guard.fn_available = &fi->guard;
	fi->guard.fn_av_hold = NULL;
	fi->guard.fn_av_rele = NULL;
	fi->guard.fn_op.anon = NULL;

	fi = &femtype[FEMTYPE_VNODE];
	fi->errf = fem_err;
	fi->head.fn_available = NULL;
	fi->head.fn_av_hold = NULL;
	fi->head.fn_av_rele = NULL;
	fi->head.fn_op.fem = NULL;
	fi->guard.fn_available = &fi->guard;
	fi->guard.fn_av_hold = NULL;
	fi->guard.fn_av_rele = NULL;
	fi->guard.fn_op.fem = &fem_guard_ops;

	fi = &femtype[FEMTYPE_VFS];
	fi->errf = fsem_err;
	fi->head.fn_available = NULL;
	fi->head.fn_av_hold = NULL;
	fi->head.fn_av_rele = NULL;
	fi->head.fn_op.fsem = NULL;
	fi->guard.fn_available = &fi->guard;
	fi->guard.fn_av_hold = NULL;
	fi->guard.fn_av_rele = NULL;
	fi->guard.fn_op.fsem = &fsem_guard_ops;
}


int
fem_err()
{
	cmn_err(CE_PANIC, "fem/vnode operations corrupt");
	return (0);
}

int
fsem_err()
{
	cmn_err(CE_PANIC, "fem/vfs operations corrupt");
	return (0);
}
