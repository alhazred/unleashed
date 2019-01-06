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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * University Copyright- Copyright (c) 1982, 1986, 1988
 * The Regents of the University of California
 * All Rights Reserved
 *
 * University Acknowledgment- Portions of this document are derived from
 * software developed by the University of California, Berkeley, and its
 * contributors.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * readdir -- C library extension routine
 */

#include	<sys/feature_tests.h>

#pragma weak readdir64 = readdir

#include "lint.h"
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include "libc.h"

dirent_t *
readdir(DIR *dirp)
{
	dirent_t *dp;	/* -> directory data */
	int saveloc = 0;

	if (dirp->dd_size != 0) {
		dp = (dirent_t *)(uintptr_t)&dirp->dd_buf[dirp->dd_loc];
		saveloc = dirp->dd_loc;		/* save for possible EOF */
		dirp->dd_loc += (int)dp->d_reclen;
	}
	if (dirp->dd_loc >= dirp->dd_size)
		dirp->dd_loc = dirp->dd_size = 0;

	if (dirp->dd_size == 0 && 	/* refill buffer */
	    (dirp->dd_size = getdents(dirp->dd_fd,
	    (dirent_t *)(uintptr_t)dirp->dd_buf, DIRBUF)) <= 0) {
		if (dirp->dd_size == 0)		/* This means EOF */
			dirp->dd_loc = saveloc;	/* so save for telldir */
		return (NULL);		/* error or EOF */
	}

	return ((dirent_t *)(uintptr_t)&dirp->dd_buf[dirp->dd_loc]);
}
