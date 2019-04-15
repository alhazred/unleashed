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
 */

#define _LIBC_STAT_C

#include "lint.h"
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#pragma weak fstat64 = fstat
#pragma weak fstatat64 = fstatat
#pragma weak stat64 = stat
#pragma weak lstat64 = lstat

#ifdef _LP64
int
fstatat(int fd, const char *name, struct stat *sb, int flags)
{
	return (syscall(SYS_fstatat, fd, name, sb, flags));
}
#else
#include <string.h>
int
fstatat_new(int fd, const char *name, struct stat *sb, int flags)
{
	return (syscall(SYS_fstatat, fd, name, sb, flags));
}
struct oldstat32 {
	dev_t		st_dev;
	long		st_pad1[3];
	ino_t		st_ino;
	mode_t		st_mode;
	nlink_t		st_nlink;
	uid_t		st_uid;
	gid_t		st_gid;
	dev_t		st_rdev;
	long		st_pad2[2];
	off_t		st_size;
	timestruc_t	st_atim;
	timestruc_t	st_mtim;
	timestruc_t	st_ctim;
	blksize_t	st_blksize;
	blkcnt_t	st_blocks;
	char		st_fstype[_ST_FSTYPSZ];
	long		st_pad4[8];
};
int
fstatat(int fd, const char *name, struct stat *oldsb, int flags)
{
	struct stat sb = { 0 };
	struct oldstat32 *old = (struct oldstat32 *)oldsb;
	int ret;
	ret = fstatat_new(fd, name, &sb, flags);
	if (ret == 0) {
		old->st_dev = sb.st_dev;
		old->st_ino = sb.st_ino;
		old->st_mode = sb.st_mode;
		old->st_nlink = sb.st_nlink;
		old->st_uid = sb.st_uid;
		old->st_gid = sb.st_gid;
		old->st_rdev = sb.st_rdev;
		old->st_size = sb.st_size;
		old->st_atim = sb.st_atim;
		old->st_mtim = sb.st_mtim;
		old->st_ctim = sb.st_ctim;
		old->st_blksize = sb.st_blksize;
		old->st_blocks = sb.st_blocks;
		strlcpy(old->st_fstype, sb.st_fstype, _ST_FSTYPSZ);
	}
	return ret;
}
int
stat_new(const char *name, struct stat *sb)
{
	return (fstatat_new(AT_FDCWD, name, sb, 0));
}

int
lstat_new(const char *name, struct stat *sb)
{
	return (fstatat_new(AT_FDCWD, name, sb, AT_SYMLINK_NOFOLLOW));
}

int
fstat_new(int fd, struct stat *sb)
{
	return (fstatat_new(fd, NULL, sb, 0));
}
#endif

int
stat(const char *name, struct stat *sb)
{
	return (fstatat(AT_FDCWD, name, sb, 0));
}

int
lstat(const char *name, struct stat *sb)
{
	return (fstatat(AT_FDCWD, name, sb, AT_SYMLINK_NOFOLLOW));
}

int
fstat(int fd, struct stat *sb)
{
	return (fstatat(fd, NULL, sb, 0));
}
