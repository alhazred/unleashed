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
 * innstr.c
 *
 * XCurses Library
 *
 * Copyright 1990, 1995 by Mortice Kern Systems Inc.  All rights reserved.
 *
 */

#if M_RCSID
static char rcsID[] =
"$Header: /team/ps/sun_xcurses/archive/local_changes/xcurses/src/lib/"
"libxcurses/src/libc/xcurses/rcs/innstr.c 1.2 1998/04/30 20:30:22 "
"cbates Exp $";
#endif

#include <private.h>

#undef innstr

int
innstr(char *s, int n)
{
	int code;

	code = winnstr(stdscr, s, n);

	return (code);
}

#undef mvinnstr

int
mvinnstr(int y, int x, char *s, int n)
{
	int code;

	if ((code = wmove(stdscr, y, x)) == OK)
		code = winnstr(stdscr, s, n);

	return (code);
}

#undef mvwinnstr

int
mvwinnstr(WINDOW *w, int y, int x, char *s, int n)
{
	int code;

	if ((code = wmove(w, y, x)) == OK)
		code = winnstr(w, s, n);

	return (code);
}

#undef instr

int
instr(char *s)
{
	int code;

	code = winnstr(stdscr, s, -1);

	return ((code == ERR) ? ERR : OK);
}

#undef mvinstr

int
mvinstr(int y, int x, char *s)
{
	int code;

	if ((code = wmove(stdscr, y, x)) == OK)
		code = winnstr(stdscr, s, -1);

	return ((code == ERR) ? ERR : OK);
}

#undef mvwinstr

int
mvwinstr(WINDOW *w, int y, int x, char *s)
{
	int code;

	if ((code = wmove(w, y, x)) == OK)
		code = winnstr(w, s, -1);

	return ((code == ERR) ? ERR : OK);
}

#undef winstr

int
winstr(WINDOW *w, char *s)
{
	int	code;

	code = winnstr(w, s, -1);

	return ((code == ERR) ? ERR : OK);
}
