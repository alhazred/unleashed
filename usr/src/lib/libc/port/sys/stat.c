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

#pragma weak fstat_new = fstat
#pragma weak fstatat_new = fstatat
#pragma weak stat_new = stat
#pragma weak lstat_new = lstat

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
