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
 * winchn.c
 *
 * XCurses Library
 *
 * Copyright 1990, 1995 by Mortice Kern Systems Inc.  All rights reserved.
 *
 */

#if M_RCSID
static char rcsID[] = "$Header: /rd/src/libc/xcurses/rcs/winchn.c 1.1 "
"1995/06/13 20:54:33 ant Exp $";
#endif

#include <private.h>

int
winchnstr(WINDOW *w, chtype *chs, int n)
{
	int	x, eol;
	cchar_t	*cp;

	eol = (n < 0 || w->_maxx < w->_curx + n) ? w->_maxx : w->_curx + n;

	for (cp = w->_line[w->_cury], x = w->_curx; x < eol; ++x, ++chs) {
		if ((*chs = __m_cc_chtype(&cp[x])) == (chtype) ERR)
			return (ERR);
	}

	/*
	 * For an unbounded buffer or a buffer with room remaining,
	 * null terminate the buffer.
	 */
	if (n < 0 || eol < w->_curx + n)
		*chs = 0;

	return (OK);
}
