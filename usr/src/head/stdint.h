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
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _STDINT_H
#define	_STDINT_H

/*
 * This header was introduced by the ISO C Standard, ISO/IEC
 * 9899:1999 Programming language - C. It is a subset of the
 * <inttypes.h> header.
 */

#include <sys/stdint.h>

#if __EXT1_VISIBLE
/* ISO/IEC 9899:2011 K.3.4.4 */
#ifndef	RSIZE_MAX
#define	RSIZE_MAX (SIZE_MAX >> 1)
#endif
#endif	/* __EXT1_VISIBLE */

#endif	/* _STDINT_H */
