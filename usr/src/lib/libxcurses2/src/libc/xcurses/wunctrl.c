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
 * Copyright (c) 1995-1998 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/* LINTLIBRARY */

/*
 * wunctrl.c
 *
 * XCurses Library
 *
 * Copyright 1990, 1995 by Mortice Kern Systems Inc.  All rights reserved.
 *
 */

#if M_RCSID
static char rcsID[] = "$Header: /rd/src/libc/xcurses/rcs/wunctrl.c 1.1 "
"1995/05/16 15:15:37 ant Exp $";
#endif

#include <private.h>

static const wchar_t	*carat[] = {
	L"^?",
	L"^@",
	L"^A",
	L"^B",
	L"^C",
	L"^D",
	L"^E",
	L"^F",
	L"^G",
	L"^H",
	L"^I",
	L"^J",
	L"^K",
	L"^L",
	L"^M",
	L"^N",
	L"^O",
	L"^P",
	L"^Q",
	L"^R",
	L"^S",
	L"^T",
	L"^U",
	L"^V",
	L"^W",
	L"^X",
	L"^Y",
	L"^Z",
	L"^[",
	L"^\\",
	L"^]",
	L"^^",
	L"^_"
};

wchar_t *
wunctrl(cchar_t *cc)
{
	int	i;
	wint_t	wc;
	static wchar_t	wcs[_M_CCHAR_MAX + 1];

	if (cc->_n <= 0)
		return (NULL);

	/* Map wide character to a wide string. */
	wc = cc->_wc[0];
	if (iswcntrl(wc)) {
		if (wc == 127)
			return ((wchar_t *)carat[0]);
		if (0 <= wc && wc <= 32)
			return ((wchar_t *)carat[wc+1]);
		return (NULL);
	}

	for (i = 0; i < cc->_n; ++i)
		wcs[i] = cc->_wc[i];
	wcs[i] = L'\0';

	return (wcs);
}
