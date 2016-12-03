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
 * clearok.c
 *
 * XCurses Library
 *
 * Copyright 1990, 1995 by Mortice Kern Systems Inc.  All rights reserved.
 *
 */

#if M_RCSID
static char rcsID[] = "$Header: /rd/src/libc/xcurses/rcs/clearok.c 1.3 "
"1995/06/19 16:12:07 ant Exp $";
#endif

#include <private.h>

int
clearok(WINDOW *w, bool bf)
{
	w->_flags &= ~W_CLEAR_WINDOW;
	if (bf)
		w->_flags |= W_CLEAR_WINDOW;

	return (OK);
}

void
immedok(WINDOW *w, bool bf)
{
	w->_flags &= ~W_FLUSH;
	if (bf)
		w->_flags |= W_FLUSH;
}

int
leaveok(WINDOW *w, bool bf)
{
	w->_flags &= ~W_LEAVE_CURSOR;
	if (bf)
		w->_flags |= W_LEAVE_CURSOR;

	return (OK);
}

int
notimeout(WINDOW *w, bool bf)
{
	w->_flags &= ~W_USE_TIMEOUT;
	if (!bf)
		w->_flags |= W_USE_TIMEOUT;

	return (OK);
}

int
scrollok(WINDOW *w, bool bf)
{
	w->_flags &= ~W_CAN_SCROLL;
	if (bf)
		w->_flags |= W_CAN_SCROLL;

	return (OK);
}
