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

	.file	"pwrite.s"

#include <sys/asm_linkage.h>

	ANSI_PRAGMA_WEAK2(__pwrite64,__pwrite,function)

#include "SYS.h"

/* C library -- pwrite						*/
/* ssize_t __pwrite(int, const void *, size_t, off_t);		*/

	SYSCALL2_RESTART_RVAL1(__pwrite,pwrite)
	RET
	SET_SIZE(__pwrite)
