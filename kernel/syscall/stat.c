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
 * Copyright (c) 1994, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Portions of this source code were derived from Berkeley 4.3 BSD
 * under license from the Regents of the University of California.
 */

/*
 * Get file attribute information through a file name or a file descriptor.
 */

#include <sys/param.h>
#include <sys/isa_defs.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/cred.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/pathname.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/vnode.h>
#include <sys/mode.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/debug.h>
#include <sys/cmn_err.h>
#include <sys/fs_subr.h>

/*
 * Get the vp to be stated and the cred to be used for the call
 * to fop_getattr
 */

static int
cstatat_getvp(int fd, char *name, int follow, vnode_t **vp, cred_t **cred)
{
	vnode_t *startvp;
	file_t *fp;
	int error;
	cred_t *cr;
	int estale_retry = 0;

	*vp = NULL;

	/*
	 * Only return EFAULT for fstatat when fd == AT_FDCWD && name == NULL
	 */

	if (fd == AT_FDCWD) {
		startvp = NULL;
		cr = CRED();
		crhold(cr);
	} else {
		char startchar;

		if (copyin(name, &startchar, sizeof (char)))
			return EFAULT;
		if (startchar != '/') {
			if ((fp = getf(fd)) == NULL) {
				return EBADF;
			}
			startvp = fp->f_vnode;
			cr = fp->f_cred;
			crhold(cr);
			VN_HOLD(startvp);
			releasef(fd);
		} else {
			startvp = NULL;
			cr = CRED();
			crhold(cr);
		}
	}
	*cred = cr;

lookup:
	if (error = lookupnameat(name, UIO_USERSPACE, follow, NULLVPP,
	    vp, startvp)) {
		if ((error == ESTALE) &&
		    fs_need_estale_retry(estale_retry++))
			goto lookup;
		if (startvp != NULL)
			VN_RELE(startvp);
		crfree(cr);
		return error;
	}
	if (startvp != NULL)
		VN_RELE(startvp);

	return 0;
}

static int
cstat(vnode_t *vp, struct stat *sb, int flag, cred_t *cr)
{
	struct vfssw *vswp;
	vattr_t vattr;
	int error;

	vattr.va_mask = VATTR_STAT | VATTR_NBLOCKS | VATTR_BLKSIZE | VATTR_SIZE;
	if ((error = fop_getattr(vp, &vattr, flag, cr, NULL)) != 0)
		return error;
	if (vattr.va_size > MAXOFF_T || vattr.va_nblocks > LONG_MAX ||
	    vattr.va_nodeid > ULONG_MAX)
		return EOVERFLOW;

	bzero(sb, sizeof (struct stat));
	sb->st_dev = vattr.va_fsid;
	sb->st_ino = (ino_t)vattr.va_nodeid;
	sb->st_mode = VTTOIF(vattr.va_type) | vattr.va_mode;
	sb->st_nlink = vattr.va_nlink;
	sb->st_uid = vattr.va_uid;
	sb->st_gid = vattr.va_gid;
	sb->st_rdev = vattr.va_rdev;
	sb->st_size = (off_t)vattr.va_size;
	sb->st_atim = vattr.va_atime;
	sb->st_mtim = vattr.va_mtime;
	sb->st_ctim = vattr.va_ctime;
	sb->st_blksize = vattr.va_blksize;
	sb->st_blocks = (blkcnt_t)vattr.va_nblocks;
	if (vp->v_vfsp != NULL) {
		vswp = &vfssw[vp->v_vfsp->vfs_fstype];
		if (vswp->vsw_name && *vswp->vsw_name)
			(void) strcpy(sb->st_fstype, vswp->vsw_name);
	}
	return 0;
}

static int
cstatat(int fd, char *name, struct stat *sb, int follow, int flags)
{
	vnode_t *vp;
	int error;
	cred_t *cred;
	int link_follow;
	int estale_retry = 0;

	link_follow = (follow == AT_SYMLINK_NOFOLLOW) ? NO_FOLLOW : FOLLOW;
lookup:
	if (error = cstatat_getvp(fd, name, link_follow, &vp, &cred))
		return error;
	error = cstat(vp, sb, flags, cred);
	crfree(cred);
	VN_RELE(vp);
	if (error != 0) {
		if (error == ESTALE &&
		    fs_need_estale_retry(estale_retry++))
			goto lookup;
		return error;
	}
	return 0;
}

static int
fstatat_nocopy(int fd, char *name, struct stat *sb, int flags)
{
	int ret;

	if (name == NULL) {
		file_t *fp;

		if (fd == AT_FDCWD)
			return EFAULT;
		if ((fp = getf(fd)) == NULL)
			return EBADF;
		ret = cstat(fp->f_vnode, sb, 0, fp->f_cred);
		releasef(fd);
	} else {
		int followflag = (flags & AT_SYMLINK_NOFOLLOW);
		int csflags = (flags & _AT_TRIGGER ? ATTR_TRIGGER : 0);

		if (followflag == 0)
			csflags |= ATTR_REAL;	/* flag for procfs lookups */
		ret = cstatat(fd, name, sb, followflag, csflags);
	}

	return ret;
}

int
fstatat(int fd, char *name, struct stat *usb, int flags)
{
	int ret;
	struct stat sb;

	ret = fstatat_nocopy(fd, name, &sb, flags);
	if (ret != 0)
		return set_errno(ret);
	if (copyout(&sb, usb, sizeof (sb)))
		return EFAULT;
	return 0;
}

#ifdef _SYSCALL32_IMPL
int
fstatat_user32(int fd, char *name, struct stat32 *usb, int flags)
{
	int ret;
	struct stat nsb;
	struct stat32 sb;

	ret = fstatat_nocopy(fd, name, &nsb, flags);
	if (ret != 0)
		return set_errno(ret);

	if (TIMESPEC_OVERFLOW(&(sb.st_atim)) ||
	    TIMESPEC_OVERFLOW(&(sb.st_mtim)) ||
	    TIMESPEC_OVERFLOW(&(sb.st_ctim)))
		return set_errno(EOVERFLOW);

	bzero(&sb, sizeof(struct stat32));
	if (!cmpldev(&(sb.st_dev), nsb.st_dev) ||
	    !cmpldev(&(sb.st_rdev), nsb.st_rdev))
		return set_errno(EOVERFLOW);
	sb.st_ino = nsb.st_ino;
	sb.st_mode = nsb.st_mode;
	sb.st_nlink = nsb.st_nlink;
	sb.st_uid = nsb.st_uid;
	sb.st_gid = nsb.st_gid;
	sb.st_size = nsb.st_size;
	TIMESPEC_TO_TIMESPEC32(&(sb.st_atim), &(nsb.st_atim));
	TIMESPEC_TO_TIMESPEC32(&(sb.st_mtim), &(nsb.st_mtim));
	TIMESPEC_TO_TIMESPEC32(&(sb.st_ctim), &(nsb.st_ctim));
	sb.st_blksize = nsb.st_blksize;
	sb.st_blocks = nsb.st_blocks;
	strlcpy(sb.st_fstype, nsb.st_fstype, sizeof(sb.st_fstype));

	if (copyout(&sb, usb, sizeof (sb)))
		return EFAULT;
	return 0;
}
#endif
