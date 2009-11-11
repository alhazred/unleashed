/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */


/*
 * Copyright (C) 2008  Internet Systems Consortium, Inc. ("ISC")
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: platform.h.in,v 1.3 2008/01/23 02:15:56 tbox Exp $ */

/*! \file */

#ifndef ISC_PLATFORM_H
#define ISC_PLATFORM_H

/*
 * Define if the OS does not define struct timespec.
 */
#undef ISC_PLATFORM_NEEDTIMESPEC
#ifdef ISC_PLATFORM_NEEDTIMESPEC
#include <time.h>               /* For time_t */
struct timespec {
	time_t  tv_sec;         /* seconds */
	long    tv_nsec;        /* nanoseconds */
};
#endif

#endif
