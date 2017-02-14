/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
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
/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _PWD_H
#define	_PWD_H

#include <sys/feature_tests.h>

#include <sys/types.h>

#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)
#include <stdio.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

struct passwd {
	char	*pw_name;
	char	*pw_passwd;
	uid_t	pw_uid;
	gid_t	pw_gid;
	char	*pw_age;
	char	*pw_comment;
	char	*pw_gecos;
	char	*pw_dir;
	char	*pw_shell;
};

#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)
struct comment {
	char	*c_dept;
	char	*c_name;
	char	*c_acct;
	char	*c_bin;
};
#endif /* !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__) */

extern struct passwd *getpwuid(uid_t);		/* MT-unsafe */
extern struct passwd *getpwnam(const char *);	/* MT-unsafe */

#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)
extern int putpwent(const struct passwd *, FILE *);
#endif /* !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__) */

#if !defined(__XOPEN_OR_POSIX) || defined(_XPG4_2) || \
	defined(__EXTENSIONS__)
extern void endpwent(void);
extern struct passwd *getpwent(void);		/* MT-unsafe */
extern void setpwent(void);
#endif /* !defined(__XOPEN_OR_POSIX) || defined(_XPG4_2) ... */

extern int getpwuid_r(uid_t, struct passwd *, char *, size_t,
    struct passwd **);
extern int getpwnam_r(const char *, struct passwd *, char *,
    size_t, struct passwd **);

#ifdef	__cplusplus
}
#endif

#endif /* _PWD_H */
