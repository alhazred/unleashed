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
/*
 * Copyright 2010 Nexenta Systems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */


#include "lint.h"
#include "file64.h"
#include <mtlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <thread.h>
#include <synch.h>
#include <values.h>
#include <wchar.h>
#include "print.h"
#include "stdiom.h"
#include <sys/types.h>
#include "libc.h"
#include "mse.h"

int
wprintf(const wchar_t *format, ...)
{
	ssize_t	count;
	rmutex_t	*lk;
	va_list	ap;

	va_start(ap, format);
	FLOCKFILE(lk, stdout);

	if (GET_NO_MODE(stdout))
		_setorientation(stdout, _WC_MODE);

	if (!(stdout->_flag & _IOWRT)) {
		/* if no write flag */
		if (stdout->_flag & _IORW) {
			/* if ok, cause read-write */
			stdout->_flag |= _IOWRT;
		} else {
			/* else error */
			errno = EBADF;
			FUNLOCKFILE(lk);
			return (EOF);
		}
	}

	count = _wndoprnt(format, ap, stdout, 0);
	va_end(ap);
	if (FERROR(stdout) || count == EOF) {
		FUNLOCKFILE(lk);
		return (EOF);
	}
	FUNLOCKFILE(lk);
	/* check for overflow */
	if ((size_t)count > MAXINT) {
		errno = EOVERFLOW;
		return (EOF);
	} else {
		return ((int)count);
	}
}

int
fwprintf(FILE *iop, const wchar_t *format, ...)
{
	ssize_t	count;
	rmutex_t	*lk;
	va_list	ap;

	va_start(ap, format);

	FLOCKFILE(lk, iop);

	if (GET_NO_MODE(iop))
		_setorientation(iop, _WC_MODE);

	if (!(iop->_flag & _IOWRT)) {
		/* if no write flag */
		if (iop->_flag & _IORW) {
			/* if ok, cause read-write */
			iop->_flag |= _IOWRT;
		} else {
			/* else error */
			errno = EBADF;
			FUNLOCKFILE(lk);
			return (EOF);
		}
	}

	count = _wndoprnt(format, ap, iop, 0);
	va_end(ap);
	if (FERROR(iop) || count == EOF) {
		FUNLOCKFILE(lk);
		return (EOF);
	}
	FUNLOCKFILE(lk);
	/* check for overflow */
	if ((size_t)count > MAXINT) {
		errno = EOVERFLOW;
		return (EOF);
	} else {
		return ((int)count);
	}
}

int
swprintf(wchar_t *string, size_t n, const wchar_t *format, ...)
{
	ssize_t	count;
	FILE	siop;
	wchar_t	*wp;
	va_list	ap;

	if (n == 0)
		return (EOF);
	siop._cnt = (ssize_t)n - 1;
	siop._base = siop._ptr = (unsigned char *)string;
	siop._flag = _IOREAD;

	va_start(ap, format);
	count = _wndoprnt(format, ap, &siop, 0);
	va_end(ap);
	wp = (wchar_t *)(uintptr_t)siop._ptr;
	*wp = L'\0';
	if (count == EOF) {
		return (EOF);
	}
	/* check for overflow */
	if ((size_t)count > MAXINT) {
		errno = EOVERFLOW;
		return (EOF);
	} else {
		return ((int)count);
	}
}
