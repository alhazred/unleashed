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
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 *
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _AIO_H
#define	_AIO_H

#include <sys/feature_tests.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/siginfo.h>
#include <sys/aiocb.h>
#include <time.h>

#ifdef	__cplusplus
extern "C" {
#endif

#if	(_POSIX_C_SOURCE - 0 > 0) && (_POSIX_C_SOURCE - 0 <= 2)
#error	"POSIX Asynchronous I/O is not supported in POSIX.1-1990"
#endif

/*
 * function prototypes
 */
extern int	aio_read(aiocb_t *);
extern int	aio_write(aiocb_t *);
extern int	lio_listio(int,
		    aiocb_t *_RESTRICT_KYWD const *_RESTRICT_KYWD,
		    int, struct sigevent *_RESTRICT_KYWD);
extern int	aio_error(const aiocb_t *);
extern ssize_t	aio_return(aiocb_t *);
extern int	aio_cancel(int, aiocb_t *);
extern int	aio_suspend(const aiocb_t *const[], int,
		    const struct timespec *);
extern int	aio_fsync(int, aiocb_t *);
extern int	aio_waitn(aiocb_t *[], uint_t, uint_t *,
		    const struct timespec *);

#ifdef	__cplusplus
}
#endif

#endif	/* _AIO_H */
