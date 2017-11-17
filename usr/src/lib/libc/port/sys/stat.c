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

#include "lint.h"
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#if !defined(_LP64) && _FILE_OFFSET_BITS == 64

int
fstatat64(int fd, const char *name, struct stat64 *sb, int flags)
{
	return (syscall(SYS_fstatat64, fd, name, sb, flags));
}

int
stat64(const char *name, struct stat64 *sb)
{
	return (fstatat64(AT_FDCWD, name, sb, 0));
}

int
lstat64(const char *name, struct stat64 *sb)
{
	return (fstatat64(AT_FDCWD, name, sb, AT_SYMLINK_NOFOLLOW));
}

int
fstat64(int fd, struct stat64 *sb)
{
	return (fstatat64(fd, NULL, sb, 0));
}

#else	/* !defined(_LP64) && _FILE_OFFSET_BITS == 64 */

int
fstatat(int fd, const char *name, struct stat *sb, int flags)
{
	return (syscall(SYS_fstatat, fd, name, sb, flags));
}

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

#endif	/* !defined(_LP64) && _FILE_OFFSET_BITS == 64 */
