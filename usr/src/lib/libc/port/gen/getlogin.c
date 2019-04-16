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
 * Copyright (c) 2013 Gary Mills
 *
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

#include "lint.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "utmpx.h"
#include <unistd.h>
#include <errno.h>
#include <thread.h>
#include <synch.h>
#include <mtlib.h>
#include "tsd.h"

/*
 * Use the full length of a login name.
 * The utmpx interface provides for a 32 character login name.
 */
#define	NMAX	(sizeof (((struct utmpx *)0)->ut_user))

/*
 * Common function
 */
static char *
getl_r_common(char *answer, size_t namelen, size_t maxlen)
{
	int		uf;
	off64_t		me;
	struct futmpx	ubuf;
	size_t		ulen;

	if ((me = (off64_t)ttyslot()) < 0)
		return (NULL);
	if ((uf = open(UTMPX_FILE, 0)) < 0)
		return (NULL);
	(void) lseek64(uf, me * sizeof (ubuf), SEEK_SET);
	if (read(uf, &ubuf, sizeof (ubuf)) != sizeof (ubuf)) {
		(void) close(uf);
		return (NULL);
	}
	(void) close(uf);
	if (ubuf.ut_user[0] == '\0')
		return (NULL);

	/* Insufficient buffer size */
	ulen = strnlen(ubuf.ut_user, maxlen);
	if (namelen <= ulen) {
		errno = ERANGE;
		return (NULL);
	}

	/*
	 * While the interface to getlogin_r says that a user should supply a
	 * buffer with at least LOGIN_NAME_MAX bytes, we shouldn't assume they
	 * have, especially since we've been supplied with its actual size.
	 * Doing otherwise is just asking us to corrupt memory (and has in the
	 * past).
	 */
	(void) strncpy(answer, ubuf.ut_user, ulen);
	answer[ulen] = '\0';
	return (answer);
}

/*
 * Compat aliases. Can be removed in Unleashed 1.3, or before.
 */
#pragma weak __posix_getlogin_r = getlogin_r
#pragma weak __posix_getloginx_r = getlogin_r
#pragma weak getloginx_r = getlogin_r
#pragma weak getloginx = getlogin

int
getlogin_r(char *name, size_t namelen)
{
	name = getl_r_common(name, namelen, NMAX);
	if (!name && errno == 0)
		return EINVAL;
	return errno;
}

char *
getlogin(void)
{
	char *answer = tsdalloc(_T_LOGIN, LOGIN_NAME_MAX, NULL);

	if (answer == NULL)
		return (NULL);
	return (getl_r_common(answer, LOGIN_NAME_MAX, NMAX));
}
