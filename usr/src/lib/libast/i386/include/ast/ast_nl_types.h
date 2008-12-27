/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1985-2008 AT&T Intellectual Property          *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                    by AT&T Intellectual Property                     *
*                                                                      *
*                A copy of the License is available at                 *
*            http://www.opensource.org/licenses/cpl1.0.txt             *
*         (with md5 checksum 059e8cd6165cb4c31e351f2b69388fd9)         *
*                                                                      *
*              Information and Software Systems Research               *
*                            AT&T Research                             *
*                           Florham Park NJ                            *
*                                                                      *
*                 Glenn Fowler <gsf@research.att.com>                  *
*                  David Korn <dgk@research.att.com>                   *
*                   Phong Vo <kpv@research.att.com>                    *
*                                                                      *
***********************************************************************/

/* : : generated by proto : : */
/* : : generated from /home/gisburn/ksh93/ast_ksh_20081104/build_i386_32bit/src/lib/libast/features/nl_types by iffe version 2008-01-31 : : */
                  
#ifndef _def_nl_types_ast
#if !defined(__PROTO__)
#  if defined(__STDC__) || defined(__cplusplus) || defined(_proto) || defined(c_plusplus)
#    if defined(__cplusplus)
#      define __LINKAGE__	"C"
#    else
#      define __LINKAGE__
#    endif
#    define __STDARG__
#    define __PROTO__(x)	x
#    define __OTORP__(x)
#    define __PARAM__(n,o)	n
#    if !defined(__STDC__) && !defined(__cplusplus)
#      if !defined(c_plusplus)
#      	define const
#      endif
#      define signed
#      define void		int
#      define volatile
#      define __V_		char
#    else
#      define __V_		void
#    endif
#  else
#    define __PROTO__(x)	()
#    define __OTORP__(x)	x
#    define __PARAM__(n,o)	o
#    define __LINKAGE__
#    define __V_		char
#    define const
#    define signed
#    define void		int
#    define volatile
#  endif
#  define __MANGLE__	__LINKAGE__
#  if defined(__cplusplus) || defined(c_plusplus)
#    define __VARARG__	...
#  else
#    define __VARARG__
#  endif
#  if defined(__STDARG__)
#    define __VA_START__(p,a)	va_start(p,a)
#  else
#    define __VA_START__(p,a)	va_start(p)
#  endif
#  if !defined(__INLINE__)
#    if defined(__cplusplus)
#      define __INLINE__	extern __MANGLE__ inline
#    else
#      if defined(_WIN32) && !defined(__GNUC__)
#      	define __INLINE__	__inline
#      endif
#    endif
#  endif
#endif
#if !defined(__LINKAGE__)
#define __LINKAGE__		/* 2004-08-11 transition */
#endif

#define _def_nl_types_ast	1
#define _sys_types	1	/* #include <sys/types.h> ok */
#define _lib_catopen	1	/* catopen() in default lib(s) */
#define _lib_nl_langinfo	1	/* nl_langinfo() in default lib(s) */
#define _hdr_nl_types	1	/* #include <nl_types.h> ok */
#define _hdr_langinfo	1	/* #include <langinfo.h> ok */
#define _nxt_nl_types <../include/nl_types.h>	/* include path for the native <nl_types.h> */
#define _nxt_nl_types_str "../include/nl_types.h"	/* include string for the native <nl_types.h> */
#include <limits.h>
#include <../include/nl_types.h>	/* the native nl_types.h */

#undef	NL_SETMAX
#define NL_SETMAX	1023
#undef	NL_MSGMAX
#define NL_MSGMAX	32767
#undef	nl_catd
#define	nl_catd		_ast_nl_catd
#undef	catopen
#define catopen		_ast_catopen
#undef	catgets
#define	catgets		_ast_catgets
#undef	catclose
#define catclose	_ast_catclose

typedef __V_* nl_catd;

#if _BLD_ast && defined(__EXPORT__)
#undef __MANGLE__
#define __MANGLE__ __LINKAGE__		__EXPORT__
#endif

extern __MANGLE__ nl_catd		catopen __PROTO__((const char*, int));
extern __MANGLE__ char*		catgets __PROTO__((nl_catd, int, int, const char*));
extern __MANGLE__ int		catclose __PROTO__((nl_catd));

#undef __MANGLE__
#define __MANGLE__ __LINKAGE__

#endif
