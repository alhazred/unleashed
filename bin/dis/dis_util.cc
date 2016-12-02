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
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <cxxabi.h>

#include "dis_util.h"

extern int g_error;	/* global process exit status, set when warn() is called */

/*
 * Fatal error.  Print out the error with a leading "dis: ", and then exit the
 * program.
 */
void
die(const char *fmt, ...)
{
	va_list ap;

	(void) fprintf(stderr, "dis: fatal: ");

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);

	(void) fprintf(stderr, "\n");

	exit(1);
}

/*
 * Non-fatal error.  Print out the error with a leading "dis: ", set the global
 * error flag, and return.
 */
void
warn(const char *fmt, ...)
{
	va_list ap;

	(void) fprintf(stderr, "dis: warning: ");

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);

	(void) fprintf(stderr, "\n");

	g_error = 1;
}

/*
 * Convenience wrapper around malloc() to cleanly exit if any allocation fails.
 */
void *
safe_malloc(size_t size)
{
	void *ret;

	if ((ret = calloc(1, size)) == NULL)
		die("Out of memory");

	return (ret);
}


/*
 * Generic interface to demangle C++ names.  Calls __cxa_demangle to do the
 * necessary translation.  If the translation fails, the argument is returned
 * unchanged.  The memory returned is only valid until the next call to
 * demangle().
 *
 * We dlopen() libstdc++.so rather than linking directly against it in case it
 * is not installed on the system.
 */
const char *
dis_demangle(const char *name)
{
	static char *demangled_name;
	static char *(*demangle_func)(
	    const char *, char *, size_t *, int *
	    ) = NULL;
	static size_t size = BUFSIZE;
	static int first_flag = 0;

	/*
	 * If this is the first call, allocate storage
	 * for the buffer.
	 */
	if (first_flag == 0) {
		void *demangle_hand;

		demangle_hand = dlopen("libstdc++.so.6", RTLD_LAZY);
		if (demangle_hand != NULL)
			demangle_func = (char *(*)(const char *, char *,
			    size_t *, int *))dlsym(demangle_hand,
				"__cxa_demangle");

		demangled_name = (char*) safe_malloc(size);
		first_flag = 1;
	}

	/* If we failed to find __cxa_demangle, return the original name. */
	if (!demangle_func)
		return (name);

	/*
	 * __cxa_demangle will realloc our buffer if necessary, but even if
	 * that fails we want to keep the buffer around for the next call.
	 */
	if (!demangle_func(name, demangled_name, &size, NULL))
		return (name);

	return (demangled_name);
}
