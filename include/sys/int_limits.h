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
 * Copyright 2019 Lauri Tirkkonen <lotheac@iki.fi>
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_INT_LIMITS_H
#define _SYS_INT_LIMITS_H

/*
 * This file, <sys/int_limits.h>, is part of the Sun Microsystems implementation
 * of <inttypes.h> as defined in the ISO C standard, ISO/IEC 9899:1999
 * Programming language - C.
 *
 * Programs/Modules should not directly include this file.  Access to the
 * types defined in this file should be through the inclusion of one of the
 * following files:
 *
 *	<limits.h>		This nested inclusion is disabled for strictly
 *				ANSI-C conforming compilations.  The *_MIN
 *				definitions are not visible to POSIX or XPG
 *				conforming applications (due to what may be
 *				a bug in the specification - this is under
 *				investigation)
 *
 *	<sys/inttypes.h>	Provides the Kernel and Driver appropriate
 *				components of <inttypes.h>.
 *
 *	<inttypes.h>		For use by applications.
 *
 * See these files for more details.
 */

#include <sys/feature_tests.h>

/*
 * Limits
 *
 * The following define the limits for the types defined in <sys/int_types.h>.
 *
 * INTMAX_MIN (minimum value of the largest supported signed integer type),
 * INTMAX_MAX (maximum value of the largest supported signed integer type),
 * and UINTMAX_MAX (maximum value of the largest supported unsigned integer
 * type) can be set to implementation defined limits.
 *
 * NOTE : A programmer can test to see whether an implementation supports
 * a particular size of integer by testing if the macro that gives the
 * maximum for that datatype is defined. For example, if #ifdef UINT64_MAX
 * tests false, the implementation does not support unsigned 64 bit integers.
 *
 * The type of these macros is intentionally unspecified.
 *
 * The types int8_t, int_least8_t, and int_fast8_t are not defined for ISAs
 * where the ABI specifies "char" as unsigned when the translation mode is
 * not ANSI-C.
 */
#define INT8_MIN		(-0x7f - 1)
#define INT16_MIN		(-0x7fff - 1)
#define INT32_MIN		(-0x7fffffff - 1)
#define INT64_MIN		(-0x7fffffffffffffffLL - 1)

#define INT8_MAX		0x7f
#define INT16_MAX		0x7fff
#define INT32_MAX		0x7fffffff
#define INT64_MAX		0x7fffffffffffffffLL

#define UINT8_MAX		0xff
#define UINT16_MAX		0xffff
#define UINT32_MAX		0xffffffffU
#define UINT64_MAX		0xffffffffffffffffULL

#define INT_LEAST8_MIN		INT8_MIN
#define INT_LEAST16_MIN		INT16_MIN
#define INT_LEAST32_MIN		INT32_MIN
#define INT_LEAST64_MIN		INT64_MIN

#define INT_LEAST8_MAX		INT8_MAX
#define INT_LEAST16_MAX		INT16_MAX
#define INT_LEAST32_MAX		INT32_MAX
#define INT_LEAST64_MAX		INT64_MAX

#define UINT_LEAST8_MAX		UINT8_MAX
#define UINT_LEAST16_MAX	UINT16_MAX
#define UINT_LEAST32_MAX	UINT32_MAX
#define UINT_LEAST64_MAX	UINT64_MAX

#define INT_FAST8_MIN		INT8_MIN
#define INT_FAST16_MIN		INT16_MIN
#define INT_FAST32_MIN		INT32_MIN
#define INT_FAST64_MIN		INT64_MIN

#define INT_FAST8_MAX		INT8_MAX
#define INT_FAST16_MAX		INT16_MAX
#define INT_FAST32_MAX		INT32_MAX
#define INT_FAST64_MAX		INT64_MAX

#define UINT_FAST8_MAX		UINT8_MAX
#define UINT_FAST16_MAX		UINT16_MAX
#define UINT_FAST32_MAX		UINT32_MAX
#define UINT_FAST64_MAX		UINT64_MAX

#define INTMAX_MIN		INT64_MIN
#define INTMAX_MAX		INT64_MAX
#define UINTMAX_MAX		UINT64_MAX

#if defined(_LP64) || defined(_I32LPx)
#define INTPTR_MIN		(-0x7fffffffffffffffL - 1)
#define INTPTR_MAX		0x7fffffffffffffffL
#define UINTPTR_MAX		0xffffffffffffffffUL
#else
#define INTPTR_MIN		(-0x7fffffffL - 1)
#define INTPTR_MAX		0x7fffffffL
#define UINTPTR_MAX		0xffffffffUL
#endif

/*
 * Maximum value of a "size_t".  SIZE_MAX was previously defined
 * in <limits.h>, however, the standards specify it be defined
 * in <stdint.h>. The <stdint.h> headers includes this header as
 * does <limits.h>. The value of SIZE_MAX should not deviate
 * from the value of ULONG_MAX defined <sys/types.h>.
 */
#define SIZE_MAX		UINTPTR_MAX

/* Limits of ptrdiff_t defined in <sys/types.h> */
#define PTRDIFF_MIN		INTPTR_MIN
#define PTRDIFF_MAX		INTPTR_MAX

/* Limits of sig_atomic_t defined in <sys/types.h> */
#ifndef SIG_ATOMIC_MIN
#define SIG_ATOMIC_MIN		INT32_MIN
#endif
#ifndef SIG_ATOMIC_MAX
#define SIG_ATOMIC_MAX		INT32_MAX
#endif

/*
 * Limits of wchar_t. The WCHAR_* macros are also
 * defined in <iso/wchar_iso.h>, but inclusion of that header
 * will break ISO/IEC C namespace.
 */
#ifndef WCHAR_MIN
#define WCHAR_MIN		INT32_MIN
#endif
#ifndef WCHAR_MAX
#define WCHAR_MAX		INT32_MAX
#endif

/* Limits of wint_t */
#ifndef WINT_MIN
#define WINT_MIN		INT32_MIN
#endif
#ifndef WINT_MAX
#define WINT_MAX		INT32_MAX
#endif

#endif /* _SYS_INT_LIMITS_H */
