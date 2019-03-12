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
 * seekdir -- C library extension routine
 */

#include	<sys/feature_tests.h>

#include "lint.h"
#include "libc.h"
#include <mtlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

void
seekdir(DIR *dirp, long loc)
{
	private_DIR	*pdirp = (private_DIR *)dirp;
	dirent_t	*dp;
	off_t		off = 0;

	lmutex_lock(&pdirp->dd_lock);
	if (lseek(dirp->dd_fd, 0, SEEK_CUR) != 0) {
		dp = (dirent_t *)(uintptr_t)&dirp->dd_buf[dirp->dd_loc];
		off = dp->d_off;
	}
	if (off != loc) {
		dirp->dd_loc = 0;
		(void) lseek(dirp->dd_fd, loc, SEEK_SET);
		dirp->dd_size = 0;

		/*
		 * Save seek offset in d_off field, in case telldir
		 * follows seekdir with no intervening call to readdir
		 */
		((dirent_t *)(uintptr_t)&dirp->dd_buf[0])->d_off = loc;
	}
	lmutex_unlock(&pdirp->dd_lock);
}
