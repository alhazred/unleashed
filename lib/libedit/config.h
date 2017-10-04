/*	$OpenBSD: config.h,v 1.3 2016/03/20 22:09:24 schwarze Exp $	*/
/* config.h.  Generated automatically by configure.  */
/* #undef SUNOS */

/* #undef HAVE_SYS_CDEFS_H */
#define HAVE_TERMCAP_H 1
#define HAVE_CURSES_H 1

/* Define to 1 if you have the `getline' function. */
#define HAVE_GETLINE 1

/* #undef HAVE_NCURSES_H */
/* #undef HAVE_TERM_H */
#define HAVE_VIS_H 1
#define HAVE_ISSETUGID 1

#define HAVE_STRLCAT 1
#define HAVE_STRLCPY 1
#define HAVE_FGETLN 1
#define HAVE_STRVIS 1
#define HAVE_STRUNVIS 1

/* #undef HAVE_STRUCT_DIRENT_D_NAMLEN */

#ifndef __STDC_ISO_10646__
#define __STDC_ISO_10646__
#endif
#ifndef __dso_hidden
#define __dso_hidden __attribute__((__visibility__("hidden")))
#endif

#include "sys.h"
