/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright (c) 2015, Joyent, Inc.  All rights reserved.
 */

	.file	"preadv.s"

#include <sys/asm_linkage.h>

	ANSI_PRAGMA_WEAK2(__preadv64,__preadv,function)

/* C library -- preadv							*/
/* ssize_t __preadv(int, const struct iovec *, int, off_t, off_t);	*/

#include "SYS.h"

	SYSCALL2_RESTART_RVAL1(__preadv,preadv)
	RET
	SET_SIZE(__preadv)
