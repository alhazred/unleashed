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
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 * Copyright (c) 1989, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved	*/

/*
 * User-visible pieces of the ANSI C standard I/O package.
 */

#ifndef _STDIO_H
#define	_STDIO_H

#include <sys/feature_tests.h>

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#include <iso/stdio_iso.h>

/*
 * If feature test macros are set that enable interfaces that use types
 * defined in <sys/types.h>, get those types by doing the include.
 *
 * Note that in asking for the interfaces associated with this feature test
 * macro one also asks for definitions of the POSIX types.
 */

/*
 * Allow global visibility for symbols defined in
 * C++ "std" namespace in <iso/stdio_iso.h>.
 */
#if __cplusplus >= 199711L
using std::FILE;
using std::size_t;
using std::fpos_t;
using std::remove;
using std::rename;
using std::tmpfile;
using std::tmpnam;
using std::fclose;
using std::fflush;
using std::fopen;
using std::freopen;
using std::setbuf;
using std::setvbuf;
using std::fprintf;
using std::fscanf;
using std::printf;
using std::scanf;
using std::sprintf;
using std::sscanf;
using std::vfprintf;
using std::vprintf;
using std::vsprintf;
using std::fgetc;
using std::fgets;
using std::fputc;
using std::fputs;
using std::getc;
using std::getchar;
using std::gets;
using std::putc;
using std::putchar;
using std::puts;
using std::ungetc;
using std::fread;
using std::fwrite;
using std::fgetpos;
using std::fseek;
using std::fsetpos;
using std::ftell;
using std::rewind;
using std::clearerr;
using std::feof;
using std::ferror;
using std::perror;
#ifndef	_LP64
using std::__filbuf;
using std::__flsbuf;
#endif	/* _LP64 */
#endif	/*  __cplusplus >= 199711L */

/*
 * This header needs to be included here because it relies on the global
 * visibility of FILE and size_t in the C++ environment.
 */
#include <iso/stdio_c99.h>

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef	_OFF_T
#define	_OFF_T
#if defined(_LP64)
typedef long		off_t;
#else
typedef __longlong_t	off_t;
#endif
typedef off_t		off64_t;
#endif /* _OFF_T */

#ifdef _LP64
typedef fpos_t		fpos64_t;
#else
typedef __longlong_t	fpos64_t;
#endif

/*
 * XPG4 requires that va_list be defined in <stdio.h> "as described in
 * <stdarg.h>".  ANSI-C and POSIX require that the namespace of <stdio.h>
 * not be polluted with this name.
 */
#if defined(_XPG4) && !defined(_VA_LIST)
#define	_VA_LIST
typedef	__va_list va_list;
#endif	/* defined(_XPG4 && !defined(_VA_LIST) */

#if defined(__EXTENSIONS__) || !defined(_STRICT_STDC) || \
		defined(__XOPEN_OR_POSIX)

#define	L_ctermid	9

/* Marked LEGACY in SUSv2 and removed in SUSv3 */
#if !defined(_XPG6) || defined(__EXTENSIONS__)
#define	L_cuserid	9
#endif

#endif /* defined(__EXTENSIONS__) || !defined(_STRICT_STDC) ... */

#if defined(__EXTENSIONS__) || \
	(!defined(_STRICT_STDC) && !defined(_POSIX_C_SOURCE)) || \
	defined(_XOPEN_SOURCE)

#define	P_tmpdir	"/var/tmp/"
#endif /* defined(__EXTENSIONS__) || (!defined(_STRICT_STDC) ... */

#ifndef _STDIO_ALLOCATE
extern unsigned char	 _sibuf[], _sobuf[];
#endif

#ifndef _LP64
extern unsigned char	*_bufendtab[];
extern FILE		*_lastbuf;
#endif

#ifndef	_SSIZE_T
#define	_SSIZE_T
#if defined(_LP64) || defined(_I32LPx)
typedef long	ssize_t;	/* size of something in bytes or -1 */
#else
typedef int	ssize_t;	/* (historical version) */
#endif
#endif	/* !_SSIZE_T */

extern char	*tmpnam_r(char *);

#if defined(__EXTENSIONS__) || \
	(!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX))
extern int fcloseall(void);
extern void setbuffer(FILE *, char *, size_t);
extern int setlinebuf(FILE *);
/* PRINTFLIKE2 */
extern int asprintf(char **, const char *, ...);
/* PRINTFLIKE2 */
extern int vasprintf(char **, const char *, __va_list);
#endif

#if defined(__EXTENSIONS__) || \
	(!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX))
	/* || defined(_XPG7) */
extern ssize_t getdelim(char **_RESTRICT_KYWD, size_t *_RESTRICT_KYWD,
	int, FILE *_RESTRICT_KYWD);
extern ssize_t getline(char **_RESTRICT_KYWD, size_t *_RESTRICT_KYWD,
	FILE *_RESTRICT_KYWD);
#endif	/* __EXTENSIONS__ ... */

/*
 * The following are known to POSIX and XOPEN, but not to ANSI-C.
 */
#if defined(__EXTENSIONS__) || \
	!defined(_STRICT_STDC) || defined(__XOPEN_OR_POSIX)

extern FILE	*fdopen(int, const char *);
extern char	*ctermid(char *);
extern int	fileno(FILE *);

#endif	/* defined(__EXTENSIONS__) || !defined(_STRICT_STDC) ... */

/*
 * The following are known to POSIX.1c, but not to ANSI-C or XOPEN.
 */
extern void	flockfile(FILE *);
extern int	ftrylockfile(FILE *);
extern void	funlockfile(FILE *);
extern int	getc_unlocked(FILE *);
extern int	getchar_unlocked(void);
extern int	putc_unlocked(int, FILE *);
extern int	putchar_unlocked(int);

/*
 * The following are known to XOPEN, but not to ANSI-C or POSIX.
 */
#if defined(__EXTENSIONS__) || !defined(_STRICT_STDC) || \
	defined(_XOPEN_SOURCE)
extern FILE	*popen(const char *, const char *);
extern char	*tempnam(const char *, const char *);
extern int	pclose(FILE *);
#if !defined(_XOPEN_SOURCE)
extern int	getsubopt(char **, char *const *, char **);
#endif /* !defined(_XOPEN_SOURCE) */

/* Marked LEGACY in SUSv2 and removed in SUSv3 */
#if !defined(_XPG6) || defined(__EXTENSIONS__)
extern char	*cuserid(char *);
extern int	getopt(int, char *const *, const char *);
extern char	*optarg;
extern int	optind, opterr, optopt;
extern int	getw(FILE *);
extern int	putw(int, FILE *);
#endif /* !defined(_XPG6) || defined(__EXTENSIONS__) */

#endif	/* defined(__EXTENSIONS__) || !defined(_STRICT_STDC) ... */

extern int	fseeko(FILE *, off_t, int);
extern off_t	ftello(FILE *);

/* FIXME: source compat: these syms are aliases to non-64 vers in libc */
int fseeko64(FILE *, off_t, int);
off_t ftello64(FILE *);

#define	getchar_unlocked()	getc_unlocked(stdin)
#define	putchar_unlocked(x)	putc_unlocked((x), stdout)

#ifdef	__cplusplus
}
#endif

#endif	/* _STDIO_H */
