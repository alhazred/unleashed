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

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifndef _C89_INTMAX32
#pragma weak _vscanf = vscanf
#pragma weak _vfscanf = vfscanf
#pragma weak _vsscanf = vsscanf
#endif

#include "lint.h"
#include "file64.h"
#include "mtlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <thread.h>
#include <synch.h>
#include "libc.h"
#include "stdiom.h"
#include "mse.h"
#include <stdio_ext.h>


int
vscanf(const char *fmt, va_list ap)
{
	rmutex_t	*lk;
	int	ret;

	FLOCKFILE(lk, stdin);

	_SET_ORIENTATION_BYTE(stdin);

	ret = __doscan_u(stdin, fmt, ap, 0);

	FUNLOCKFILE(lk);
	return (ret);
}

int
vfscanf(FILE *iop, const char *fmt, va_list ap)
{
	rmutex_t	*lk;
	int	ret;

	FLOCKFILE(lk, iop);

	_SET_ORIENTATION_BYTE(iop);

	ret = __doscan_u(iop, fmt, ap, 0);

	FUNLOCKFILE(lk);
	return (ret);
}

int
vsscanf(const char *str, const char *fmt, va_list ap)
{
	FILE strbuf;

	/*
	 * The dummy FILE * created for sscanf has the _IOWRT
	 * flag set to distinguish it from scanf and fscanf
	 * invocations.
	 */
	strbuf._flag = _IOREAD | _IOWRT;
	strbuf._ptr = strbuf._base = (unsigned char *)str;
	strbuf._cnt = strlen(str);
	SET_FILE(&strbuf, _NFILE);

	/*
	 * Mark the stream so that routines called by __doscan_u()
	 * do not do any locking. In particular this avoids a NULL
	 * lock pointer being used by getc() causing a core dump.
	 * See bugid -  1210179 program SEGV's in sscanf if linked with
	 * the libthread.
	 * This also makes sscanf() quicker since it does not need
	 * to do any locking.
	 */
	if (__fsetlocking(&strbuf, FSETLOCKING_BYCALLER) == -1) {
		return (-1);	/* this should never happen */
	}

	/* as this stream is local to this function, no locking is be done */
	return (__doscan_u(&strbuf, fmt, ap, 0));
}
