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
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_UTSNAME_H
#define	_SYS_UTSNAME_H

#include <sys/feature_tests.h>

#ifdef _KERNEL
#include <sys/stdbool.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#define	_SYS_NMLN	257	/* 4.0 size of utsname elements	*/
				/* Must be at least 257 to	*/
				/* support Internet hostnames.	*/

#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)
#ifndef	SYS_NMLN
#define	SYS_NMLN	_SYS_NMLN
#endif
#endif /* !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__) */

struct utsname {
	char	sysname[_SYS_NMLN];
	char	nodename[_SYS_NMLN];
	char	release[_SYS_NMLN];
	char	version[_SYS_NMLN];
	char	machine[_SYS_NMLN];
};

#if !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__)
extern struct utsname utsname;
#endif /* !defined(__XOPEN_OR_POSIX) || defined(__EXTENSIONS__) */

#if !defined(_KERNEL)
extern int uname(struct utsname *);
#else	/* !(_KERNEL) */
/*
 * Routine to retrieve the nodename as seen in the current process's zone.
 */
extern char *uts_nodename(void);

/*
 * Routines to access parts of the utsname.
 */
extern const struct utsname *utsname_get(bool legacy);
extern void utsname_set_machine(const char *machine);

#endif	/* !(_KERNEL) */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_UTSNAME_H */
