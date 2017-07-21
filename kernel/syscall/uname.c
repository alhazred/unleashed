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
/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved	*/


#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/utsname.h>
#include <sys/debug.h>
#include <sys/proc.h>
#include <sys/user.h>

int
uname(struct utsname *buf)
{
	const char *name_to_use = uts_nodename();
	const struct utsname *info;

	info = utsname_get((PTOU(curproc)->u_flags & U_FLAG_LEGACYUNAME) != 0);

	if (copyout(info->sysname, buf->sysname, strlen(info->sysname) + 1)) {
		return (set_errno(EFAULT));
	}
	if (copyout(name_to_use, buf->nodename, strlen(name_to_use)+1)) {
		return (set_errno(EFAULT));
	}
	if (copyout(info->release, buf->release, strlen(info->release) + 1)) {
		return (set_errno(EFAULT));
	}
	if (copyout(info->version, buf->version, strlen(info->version) + 1)) {
		return (set_errno(EFAULT));
	}
	if (copyout(info->machine, buf->machine, strlen(info->machine) + 1)) {
		return (set_errno(EFAULT));
	}
	return (0);
}
