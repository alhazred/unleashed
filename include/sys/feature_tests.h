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
 * Copyright 2013, 2014 Garrett D'Amore <garrett@damore.org>
 * Copyright 2016 Joyent, Inc.
 *
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_FEATURE_TESTS_H
#define	_SYS_FEATURE_TESTS_H

#include <sys/ccompile.h>
#include <sys/isa_defs.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Values of _POSIX_C_SOURCE
 *
 *		undefined   not a POSIX compilation
 *		1	    POSIX.1-1990 compilation
 *		2	    POSIX.2-1992 compilation
 *		199309L	    POSIX.1b-1993 compilation (Real Time)
 *		199506L	    POSIX.1c-1995 compilation (POSIX Threads)
 *		200112L	    POSIX.1-2001 compilation (Austin Group Revision)
 *		200809L     POSIX.1-2008 compilation
 */
#if defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE)
#define	_POSIX_C_SOURCE 1
#endif

/*
 * The feature test macros __XOPEN_OR_POSIX, _STRICT_STDC, _STRICT_SYMBOLS,
 * and _STDC_C99 are Sun implementation specific macros created in order to
 * compress common standards specified feature test macros for easier reading.
 * These macros should not be used by the application developer as
 * unexpected results may occur. Instead, the user should reference
 * standards(5) for correct usage of the standards feature test macros.
 *
 * __XOPEN_OR_POSIX     Used in cases where a symbol is defined by both
 *                      X/Open or POSIX or in the negative, when neither
 *                      X/Open or POSIX defines a symbol.
 *
 * _STRICT_STDC         __STDC__ is specified by the C Standards and defined
 *                      by the compiler. For Sun compilers the value of
 *                      __STDC__ is either 1, 0, or not defined based on the
 *                      compilation mode (see cc(1)). When the value of
 *                      __STDC__ is 1 and in the absence of any other feature
 *                      test macros, the namespace available to the application
 *                      is limited to only those symbols defined by the C
 *                      Standard. _STRICT_STDC provides a more readable means
 *                      of identifying symbols defined by the standard, or in
 *                      the negative, symbols that are extensions to the C
 *                      Standard. See additional comments for GNU C differences.
 *
 * _STDC_C99            __STDC_VERSION__ is specified by the C standards and
 *                      defined by the compiler and indicates the version of
 *                      the C standard. A value of 199901L indicates a
 *                      compiler that complies with ISO/IEC 9899:1999, other-
 *                      wise known as the C99 standard.
 *
 * _STDC_C11		Like _STDC_C99 except that the value of __STDC_VERSION__
 *                      is 201112L indicating a compiler that compiles with
 *                      ISO/IEC 9899:2011, otherwise known as the C11 standard.
 *
 * _STRICT_SYMBOLS	Used in cases where symbol visibility is restricted
 *                      by the standards, and the user has not explicitly
 *                      relaxed the strictness via __EXTENSIONS__.
 */

#if defined(_XOPEN_SOURCE) || defined(_POSIX_C_SOURCE)
#define	__XOPEN_OR_POSIX
#endif

/*
 * ISO/IEC 9899:1990 and it's revisions, ISO/IEC 9899:1999 and ISO/IEC
 * 99899:2011 specify the following predefined macro name:
 *
 * __STDC__	The integer constant 1, intended to indicate a conforming
 *		implementation.
 *
 * Furthermore, a strictly conforming program shall use only those features
 * of the language and library specified in these standards. A conforming
 * implementation shall accept any strictly conforming program.
 *
 * The GCC project interpretation is that __STDC__ should always be defined
 * to 1 for compilation modes that accept ANSI C syntax regardless of whether
 * or not extensions to the C standard are used. Violations of conforming
 * behavior are conditionally flagged as warnings via the use of the
 * -pedantic option. In addition to defining __STDC__ to 1, the GNU C
 * compiler also defines __STRICT_ANSI__ as a means of specifying strictly
 * conforming environments using the -ansi or -std=<standard> options.
 * Clang (LLVM) works similarly.
 *
 * In the absence of any other compiler options, GCC and Clang set the value of
 * __STDC__ as follows when using the following options:
 *
 *					__STDC__	__STRICT_ANSI__
 * cc (default)				1		undefined
 * cc -ansi, -std={c89, c99,...)	1		1
 * cc -traditional (K&R)		undefined	undefined
 */

#if (__STDC__ - 0 == 1 && defined(__STRICT_ANSI__)) || \
	defined(_ASM)
#define	_STRICT_STDC
#else
#undef	_STRICT_STDC
#endif

/*
 * Compiler complies with ISO/IEC 9899:1999 or ISO/IEC 9989:2011
 */

#if __STDC_VERSION__ - 0 >= 201112L || __cplusplus - 0 >= 201703
#define	_STDC_C11
#define __ISO_C_VISIBLE		2011
#elif __STDC_VERSION__ - 0 >= 199901L || __cplusplus - 0 >= 201103
#define	_STDC_C99
#define __ISO_C_VISIBLE		1999
#endif

/*
 * Use strict symbol visibility.
 */
#if (defined(_STRICT_STDC) || defined(__XOPEN_OR_POSIX)) && \
	!defined(__EXTENSIONS__)
#define	_STRICT_SYMBOLS
#endif

/*
 * Large file interfaces:
 *
 *	_LARGEFILE_SOURCE
 *		1		large file-related additions to POSIX
 *				interfaces requested (fseeko, etc.)
 *	_LARGEFILE64_SOURCE
 *		1		transitional large-file-related interfaces
 *				requested (seek64, stat64, etc.)
 *
 * The corresponding announcement macros are respectively:
 *	_LFS_LARGEFILE
 *	_LFS64_LARGEFILE
 * (These are set in <unistd.h>.)
 *
 * Requesting _LARGEFILE64_SOURCE implies requesting _LARGEFILE_SOURCE as
 * well.
 *
 * The large file interfaces are made visible regardless of the initial values
 * of the feature test macros under certain circumstances:
 *    -	If no explicit standards-conforming environment is requested (neither
 *	of _POSIX_SOURCE nor _XOPEN_SOURCE is defined and the value of
 *	__STDC__ does not imply standards conformance).
 *    -	Extended system interfaces are explicitly requested (__EXTENSIONS__
 *	is defined).
 *    -	Access to in-kernel interfaces is requested (_KERNEL or _KMEMUSER is
 *	defined).  (Note that this dependency is an artifact of the current
 *	kernel implementation and may change in future releases.)
 */
#if	(!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX)) || \
		defined(_KERNEL) || defined(_KMEMUSER) || \
		defined(__EXTENSIONS__)
#undef	_LARGEFILE64_SOURCE
#define	_LARGEFILE64_SOURCE	1
#endif
#if	_LARGEFILE64_SOURCE - 0 == 1
#undef	_LARGEFILE_SOURCE
#define	_LARGEFILE_SOURCE	1
#endif

/*
 * Large file compilation environment control:
 *
 * The setting of _FILE_OFFSET_BITS controls the size of various file-related
 * types and governs the mapping between file-related source function symbol
 * names and the corresponding binary entry points.
 *
 * In the 32-bit environment, the default value is 32; if not set, set it to
 * the default here, to simplify tests in other headers.
 *
 * In the 64-bit compilation environment, the only value allowed is 64.
 */
#if defined(_LP64)
#ifndef _FILE_OFFSET_BITS
#define	_FILE_OFFSET_BITS	64
#endif
#if	_FILE_OFFSET_BITS - 0 != 64
#error	"invalid _FILE_OFFSET_BITS value specified"
#endif
#else	/* _LP64 */
#ifndef	_FILE_OFFSET_BITS
#define	_FILE_OFFSET_BITS	32
#endif
#if	_FILE_OFFSET_BITS - 0 != 32 && _FILE_OFFSET_BITS - 0 != 64
#error	"invalid _FILE_OFFSET_BITS value specified"
#endif
#endif	/* _LP64 */

/*
 * Use of _XOPEN_SOURCE
 *
 * The following X/Open specifications are supported:
 *
 * X/Open Portability Guide, Issue 3 (XPG3)
 * X/Open CAE Specification, Issue 4 (XPG4)
 * X/Open CAE Specification, Issue 4, Version 2 (XPG4v2)
 * X/Open CAE Specification, Issue 5 (XPG5)
 * Open Group Technical Standard, Issue 6 (XPG6), also referred to as
 *    IEEE Std. 1003.1-2001 and ISO/IEC 9945:2002.
 * Open Group Technical Standard, Issue 7 (XPG7), also referred to as
 *    IEEE Std. 1003.1-2008 and ISO/IEC 9945:2009.
 *
 * XPG4v2 is also referred to as UNIX 95 (SUS or SUSv1).
 * XPG5 is also referred to as UNIX 98 or the Single Unix Specification,
 *     Version 2 (SUSv2)
 * XPG6 is the result of a merge of the X/Open and POSIX specifications
 *     and as such is also referred to as IEEE Std. 1003.1-2001 in
 *     addition to UNIX 03 and SUSv3.
 * XPG7 is also referred to as UNIX 08 and SUSv4.
 *
 * When writing a conforming X/Open application, as per the specification
 * requirements, the appropriate feature test macros must be defined at
 * compile time. These are as follows. For more info, see standards(5).
 *
 * Feature Test Macro				     Specification
 * ------------------------------------------------  -------------
 * _XOPEN_SOURCE                                         XPG3
 * _XOPEN_SOURCE && _XOPEN_VERSION = 4                   XPG4
 * _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED = 1           XPG4v2
 * _XOPEN_SOURCE = 500                                   XPG5
 * _XOPEN_SOURCE = 600  (or POSIX_C_SOURCE=200112L)      XPG6
 * _XOPEN_SOURCE = 700  (or POSIX_C_SOURCE=200809L)      XPG7
 */

#ifndef _XOPEN_SOURCE
# define __XPG_VISIBLE		700
#else
# if (_XOPEN_SOURCE - 0 >= 700)
#  define __XPG_VISIBLE		700
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE	200809L
# elif (_XOPEN_SOURCE - 0 >= 600)
#  define __XPG_VISIBLE		600
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE	200112L
# elif (_XOPEN_SOURCE - 0 >= 500)
#  define __XPG_VISIBLE		500
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE	199506L
# elif (_XOPEN_SOURCE_EXTENDED - 0 == 1)
#  define __XPG_VISIBLE		420
# elif (_XOPEN_VERSION - 0 >= 4)
#  define __XPG_VISIBLE		400
# else
#  define __XPG_VISIBLE		300
# endif
#endif

/*
 * XXX for compatibility with headers inherited from illumos.
 * this is a bit of a hack; a lot of headers check "#if !_XPG4_2" when exposing
 * things needed in-tree, so we work around by not setting these for the
 * kernel, for now.
 */
#ifndef _KERNEL
#if (__XPG_VISIBLE >= 700)
# define _XPG7
#endif
#if (__XPG_VISIBLE >= 600)
# define _XPG6
#endif
#if (__XPG_VISIBLE >= 500)
# define _XPG5
#endif
#if (__XPG_VISIBLE >= 420)
# define _XPG4_2
#endif
#if (__XPG_VISIBLE >= 400)
# define _XPG4
#endif
#if (__XPG_VISIBLE >= 300)
# define _XPG3
#endif
#endif

/*
 * _XOPEN_VERSION is defined by the X/Open specifications and is not
 * normally defined by the application, except in the case of an XPG4
 * application.  On the implementation side, _XOPEN_VERSION defined with
 * the value of 3 indicates an XPG3 application. _XOPEN_VERSION defined
 * with the value of 4 indicates an XPG4 or XPG4v2 (UNIX 95) application.
 * _XOPEN_VERSION  defined with a value of 500 indicates an XPG5 (UNIX 98)
 * application and with a value of 600 indicates an XPG6 (UNIX 03)
 * application and with a value of 700 indicates an XPG7 (UNIX 08).
 * The appropriate version is determined by the use of the
 * feature test macros described earlier.  The value of _XOPEN_VERSION
 * defaults to 3 otherwise indicating support for XPG3 applications.
 */
#ifndef _XOPEN_VERSION
#if	defined(_XPG7)
#define	_XOPEN_VERSION 700
#elif	defined(_XPG6)
#define	_XOPEN_VERSION 600
#elif defined(_XPG5)
#define	_XOPEN_VERSION 500
#elif	defined(_XPG4_2)
#define	_XOPEN_VERSION  4
#else
#define	_XOPEN_VERSION  3
#endif
#endif

/*
 * ANSI C and ISO 9899:1990 say the type long long doesn't exist in strictly
 * conforming environments.  ISO 9899:1999 says it does.
 *
 * The presence of _LONGLONG_TYPE says "long long exists" which is therefore
 * defined in all but strictly conforming environments that disallow it.
 */
#if !defined(_STDC_C99) && defined(_STRICT_STDC) && !defined(__GNUC__)
/*
 * Resist attempts to force the definition of long long in this case.
 */
#if defined(_LONGLONG_TYPE)
#error	"No long long in strictly conforming ANSI C & 1990 ISO C environments"
#endif
#else
#if !defined(_LONGLONG_TYPE)
#define	_LONGLONG_TYPE
#endif
#endif

/*
 * The following macro defines a value for the ISO C99 restrict
 * keyword so that _RESTRICT_KYWD resolves to "restrict" if
 * an ISO C99 compiler is used, "__restrict" for c++ and "" (null string)
 * if any other compiler is used. This allows for the use of single
 * prototype declarations regardless of compiler version.
 */
#if (defined(__STDC__) && defined(_STDC_C99))
#ifdef __cplusplus
#define	_RESTRICT_KYWD	__restrict
#else
/*
 * NOTE: The whitespace between the '#' and 'define' is significant.
 * It foils gcc's fixincludes from defining a redundant 'restrict'.
 */
/* CSTYLED */
# define	_RESTRICT_KYWD	restrict
#endif
#else
#define	_RESTRICT_KYWD
#endif

/*
 * The following macro defines a value for the ISO C11 _Noreturn
 * keyword so that _NORETURN_KYWD resolves to "_Noreturn" if
 * an ISO C11 compiler is used and "" (null string) if any other
 * compiler is used. This allows for the use of single prototype
 * declarations regardless of compiler version.
 */
#if (defined(__STDC__) && defined(_STDC_C11)) && !defined(__cplusplus)
#define	_NORETURN_KYWD	_Noreturn
#else
#define	_NORETURN_KYWD
#endif

/* ISO/IEC 9899:2011 Annex K */
#if defined(__STDC_WANT_LIB_EXT1__)
#if __STDC_WANT_LIB_EXT1__
#define	__EXT1_VISIBLE		1
#else
#define	__EXT1_VISIBLE		0
#endif
#else
#define	__EXT1_VISIBLE		0
#endif /* __STDC_WANT_LIB_EXT1__ */

/*
 * The following macro indicates header support for the ANSI C++
 * standard.  The ISO/IEC designation for this is ISO/IEC FDIS 14882.
 */
#define	_ISO_CPP_14882_1998

/*
 * The following macro indicates header support for the C99 standard,
 * ISO/IEC 9899:1999, Programming Languages - C.
 */
#define	_ISO_C_9899_1999

/*
 * The following macro indicates header support for the C11 standard,
 * ISO/IEC 9899:2011, Programming Languages - C.
 */
#define	_ISO_C_9899_2011

/*
 * The following macro indicates header support for the C11 standard,
 * ISO/IEC 9899:2011 Annex K, Programming Languages - C.
 */
#undef	__STDC_LIB_EXT1__

/*
 * The following macro indicates header support for DTrace. The value is an
 * integer that corresponds to the major version number for DTrace.
 */
#define	_DTRACE_VERSION	1

/*
 * The following macro indicates that we are on illumos, and serves to
 * distinguish us from other SunOS derived systems.
 */
#define	_ILLUMOS	1

/*
 * Default values.
 */
#ifndef _KERNEL
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE			700
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE			200809L
#endif
#ifndef __ISO_C_VISIBLE
#define __ISO_C_VISIBLE			2011
#endif
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_FEATURE_TESTS_H */
