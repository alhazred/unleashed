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
 * Copyright (c) 1990, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T */
/*	  All Rights Reserved   */

/*
 * University Copyright- Copyright (c) 1982, 1986, 1988
 * The Regents of the University of California
 * All Rights Reserved
 *
 * University Acknowledgment- Portions of this document are derived from
 * software developed by the University of California, Berkeley, and its
 * contributors.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/socketvar.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

extern int _so_socket();
extern int _s_netconfig_path();

#pragma weak __xnet_socket = socket

int
socket(int family, int type, int protocol)
{
	int fd;

	/*
	 * Try creating without knowing the device assuming that
	 * the transport provider is registered in /etc/sock2path.d.
	 * If none found fall back to using /etc/netconfig to look
	 * up the name of the transport device name. This provides
	 * backwards compatibility for transport providers that have not
	 * yet been converted to using /etc/sock2path.d.
	 * XXX When all transport providers use /etc/sock2path.d. this
	 * part of the code can be removed.
	 */
	fd = _so_socket(family, type, protocol, NULL);
	if (fd == -1) {
		char *devpath;
		int saved_errno = errno;
		int prototype = 0;

		switch (saved_errno) {
		case EAFNOSUPPORT:
		case EPROTOTYPE:
			break;
		case EPROTONOSUPPORT:
			break;

		default:
			errno = saved_errno;
			return (-1);
		}
		if (_s_netconfig_path(family, type, protocol,
		    &devpath, &prototype) == -1) {
			errno = saved_errno;
			return (-1);
		}
		fd = _so_socket(family, type, protocol, devpath);
		free(devpath);
		if (fd == -1) {
			errno = saved_errno;
			return (-1);
		}
		if (prototype != 0) {
			if (setsockopt(fd, SOL_SOCKET, SO_PROTOTYPE,
			    (caddr_t)&prototype, (int)sizeof (prototype)) < 0) {
				(void) close(fd);
				/*
				 * setsockopt often fails with ENOPROTOOPT
				 * but socket() should fail with
				 * EPROTONOSUPPORT.
				 */
				errno = EPROTONOSUPPORT;
				return (-1);
			}
		}
	}
	return (fd);
}
