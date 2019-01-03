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
/*	  All Rights Reserved	*/


/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _GRP_H
#define	_GRP_H

#include <sys/feature_tests.h>

#include <sys/types.h>

#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX)
#include <stdio.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

struct	group {	/* see getgrent(3C) */
	char	*gr_name;
	char	*gr_passwd;
	gid_t	gr_gid;
	char	**gr_mem;
};

extern struct group *getgrgid(gid_t);		/* MT-unsafe */
extern struct group *getgrnam(const char *);	/* MT-unsafe */

#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX)
extern int initgroups(const char *, gid_t);
#endif /* defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX) */

#if defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX) || defined(_XPG4_2)
extern void endgrent(void);
extern void setgrent(void);
extern struct group *getgrent(void);		/* MT-unsafe */
#endif /* defined(__EXTENSIONS__) || !defined(__XOPEN_OR_POSIX)... */

extern int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
extern int getgrnam_r(const char *, struct group *, char *, size_t,
    struct group **);

#ifdef	__cplusplus
}
#endif

#endif	/* _GRP_H */
