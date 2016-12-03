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
 * Copyright (c) 1995, by Sun Microsystems, Inc.
 * All rights reserved.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * tgetnum.c
 *
 * XCurses Library 
 *
 * Copyright 1990, 1995 by Mortice Kern Systems Inc.  All rights reserved.
 *
 */

#ifdef M_RCSID
static char rcsID[] = "$Header: /rd/src/libc/xcurses/rcs/tgetnum.c 1.2 1995/08/30 19:30:58 danv Exp $";
#endif

#include <private.h>
#include <string.h>

int
tgetnum(cap)
const char *cap;
{
	char **p;
	int value = -2;

#ifdef M_CURSES_TRACE
	__m_trace("tgetnum(%p = \"%.2s\")", cap, cap);
#endif

	for (p = __m_numcodes; *p != (char *) 0; ++p) {
		if (strcmp(*p, cap) == 0) {
			value = cur_term->_num[(int)(p - __m_numcodes)];
			break;
		}
	}

	return __m_return_int("tgetnum", value);
}
