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
 * Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/fs_subr.h>

#include <sys/kmem.h>
#include <sys/modctl.h>
#include <sys/objfs.h>
#include <sys/objfs_impl.h>
#include <sys/vfs_opreg.h>
#include <sys/stat.h>

static const struct vnodeops objfs_ops_odir;

static gfs_dirent_t objfs_odir_entries[] = {
	{ "object", objfs_create_data, 0 },
	{ NULL }
};

/* ARGSUSED */
static ino64_t
objfs_odir_do_inode(vnode_t *vp, int index)
{
	objfs_odirnode_t *odir = vp->v_data;

	return (OBJFS_INO_DATA(odir->objfs_odir_modctl->mod_id));
}

vnode_t *
objfs_create_odirnode(vnode_t *pvp, struct modctl *mp)
{
	vnode_t *vp = gfs_dir_create(sizeof (objfs_odirnode_t), pvp,
	    &objfs_ops_odir, objfs_odir_entries, objfs_odir_do_inode,
	    OBJFS_NAME_MAX, NULL, NULL);
	objfs_odirnode_t *onode = vp->v_data;

	onode->objfs_odir_modctl = mp;

	return (vp);
}

/* ARGSUSED */
static int
objfs_odir_getattr(vnode_t *vp, vattr_t *vap, int flags, cred_t *cr,
	caller_context_t *ct)
{
	timestruc_t now;

	vap->va_type = VDIR;
	vap->va_mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP |
	    S_IROTH | S_IXOTH;
	vap->va_nodeid = gfs_file_inode(vp);
	vap->va_nlink = vap->va_size = 2;
	gethrestime(&now);
	vap->va_atime = vap->va_ctime = vap->va_mtime = now;
	return (objfs_common_getattr(vp, vap));
}

static const struct vnodeops objfs_ops_odir = {
	.vnop_name = "objfs object directory",
	.vop_open = objfs_dir_open,
	.vop_close = objfs_common_close,
	.vop_ioctl = fs_inval,
	.vop_getattr = objfs_odir_getattr,
	.vop_access = objfs_dir_access,
	.vop_readdir = gfs_vop_readdir,
	.vop_lookup = gfs_vop_lookup,
	.vop_seek = fs_seek,
	.vop_inactive = gfs_vop_inactive,
};
