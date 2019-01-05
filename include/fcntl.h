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
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved	*/

#ifndef	_FCNTL_H
#define	_FCNTL_H

#include <sys/feature_tests.h>
#if defined(__EXTENSIONS__) || defined(_XPG4)
#include <sys/stat.h>
#endif
#include <sys/types.h>
#include <sys/fcntl.h>

#ifdef	__cplusplus
extern "C" {
#endif

#if defined(__EXTENSIONS__) || defined(_XPG4)

/* Symbolic constants for the "lseek" routine. */

#ifndef	SEEK_SET
#define	SEEK_SET	0	/* Set file pointer to "offset" */
#endif

#ifndef	SEEK_CUR
#define	SEEK_CUR	1	/* Set file pointer to current plus "offset" */
#endif

#ifndef	SEEK_END
#define	SEEK_END	2	/* Set file pointer to EOF plus "offset" */
#endif
#endif /* defined(__EXTENSIONS__) || defined(_XPG4) */

#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)
#ifndef	SEEK_DATA
#define	SEEK_DATA	3	/* Set file pointer to next data past offset */
#endif

#ifndef	SEEK_HOLE
#define	SEEK_HOLE	4	/* Set file pointer to next hole past offset */
#endif
#endif /* !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__) */

extern int fcntl(int, int, ...);
extern int open(const char *, int, ...);
extern int creat(const char *, mode_t);

#if !defined(__XOPEN_OR_POSIX) || defined(_XPG6) || defined(__EXTENSIONS__)
extern int posix_fadvise(int, off_t, off_t, int);
extern int posix_fallocate(int, off_t, off_t);
#endif /* !defined(__XOPEN_OR_POSIX) || defined(_XPG6) || ... */
#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX) || \
	defined(_ATFILE_SOURCE)
extern int openat(int, const char *, int, ...);
extern int attropen(const char *, const char *, int, ...);
#endif /* defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX) ... */
#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX)
extern int directio(int, int);
#endif

/* FIXME source compat; these libc symbols are aliases to the non-64 ones */
int open64(const char *, int, ...);
int openat64(int, const char *, int, ...);
int creat64(const char *, mode_t);

#ifdef	__cplusplus
}
#endif

#endif	/* _FCNTL_H */
