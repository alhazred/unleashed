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

#include <sys/isa_defs.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/sysmacros.h>
#include "libproc.h"

/*
 * stat() system call -- executed by subject process
 */
int
pr_stat(struct ps_prochandle *Pr, const char *path, struct stat *buf)
{
	sysret_t rval;			/* return value from stat() */
	argdes_t argd[4];		/* arg descriptors for fstatat() */
	argdes_t *adp = &argd[0];	/* first argument */
	int syscall = SYS_fstatat;
	int error;

	if (Pr == NULL)		/* no subject process */
		return (stat(path, buf));

	adp->arg_value = AT_FDCWD;
	adp->arg_object = NULL;
	adp->arg_type = AT_BYVAL;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = 0;
	adp++;			/* move to path argument */

	adp->arg_value = 0;
	adp->arg_object = (void *)path;
	adp->arg_type = AT_BYREF;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = strlen(path) + 1;
	adp++;			/* move to buffer argument */

	adp->arg_value = 0;
	adp->arg_type = AT_BYREF;
	adp->arg_inout = AI_OUTPUT;
	adp->arg_object = buf;
	adp->arg_size = sizeof (*buf);
	adp++;			/* move to flags argument */

	adp->arg_value = 0;
	adp->arg_object = NULL;
	adp->arg_type = AT_BYVAL;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = 0;

	error = Psyscall(Pr, &rval, syscall, 4, &argd[0]);

	if (error) {
		errno = (error > 0)? error : ENOSYS;
		return (-1);
	}
	return (0);
}

/*
 * lstat() system call -- executed by subject process
 */
int
pr_lstat(struct ps_prochandle *Pr, const char *path, struct stat *buf)
{
	sysret_t rval;			/* return value from stat() */
	argdes_t argd[4];		/* arg descriptors for fstatat() */
	argdes_t *adp = &argd[0];	/* first argument */
	int syscall = SYS_fstatat;
	int error;

	if (Pr == NULL)		/* no subject process */
		return (lstat(path, buf));

	adp->arg_value = AT_FDCWD;
	adp->arg_object = NULL;
	adp->arg_type = AT_BYVAL;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = 0;
	adp++;			/* move to path argument */

	adp->arg_value = 0;
	adp->arg_object = (void *)path;
	adp->arg_type = AT_BYREF;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = strlen(path) + 1;
	adp++;			/* move to buffer argument */

	adp->arg_value = 0;
	adp->arg_type = AT_BYREF;
	adp->arg_inout = AI_OUTPUT;
	adp->arg_object = buf;
	adp->arg_size = sizeof (*buf);
	adp++;			/* move to flags argument */

	adp->arg_value = AT_SYMLINK_NOFOLLOW;
	adp->arg_object = NULL;
	adp->arg_type = AT_BYVAL;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = 0;

	error = Psyscall(Pr, &rval, syscall, 4, &argd[0]);

	if (error) {
		errno = (error > 0)? error : ENOSYS;
		return (-1);
	}
	return (0);
}

/*
 * fstat() system call -- executed by subject process
 */
int
pr_fstat(struct ps_prochandle *Pr, int fd, struct stat *buf)
{
	sysret_t rval;			/* return value from stat() */
	argdes_t argd[4];		/* arg descriptors for fstatat() */
	argdes_t *adp = &argd[0];	/* first argument */
	int syscall = SYS_fstatat;
	int error;

	if (Pr == NULL)		/* no subject process */
		return (fstat(fd, buf));

	adp->arg_value = fd;
	adp->arg_object = NULL;
	adp->arg_type = AT_BYVAL;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = 0;
	adp++;			/* move to path argument */

	adp->arg_value = 0;
	adp->arg_object = NULL;
	adp->arg_type = AT_BYVAL;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = 0;
	adp++;			/* move to buffer argument */

	adp->arg_value = 0;
	adp->arg_type = AT_BYREF;
	adp->arg_inout = AI_OUTPUT;
	adp->arg_object = buf;
	adp->arg_size = sizeof (*buf);
	adp++;			/* move to flags argument */

	adp->arg_value = 0;
	adp->arg_object = NULL;
	adp->arg_type = AT_BYVAL;
	adp->arg_inout = AI_INPUT;
	adp->arg_size = 0;

	error = Psyscall(Pr, &rval, syscall, 4, &argd[0]);

	if (error) {
		errno = (error > 0)? error : ENOSYS;
		return (-1);
	}
	return (0);
}
