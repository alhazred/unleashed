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
 * Copyright (c) 1994, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/*	Copyright (c) 1983, 1984, 1985, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Portions of this source code were derived from Berkeley 4.3 BSD
 * under license from the Regents of the University of California.
 */

#include <sys/param.h>
#include <sys/isa_defs.h>
#include <sys/types.h>
#include <sys/inttypes.h>
#include <sys/sysmacros.h>
#include <sys/cred.h>
#include <sys/dirent.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/vnode.h>
#include <sys/file.h>
#include <sys/mode.h>
#include <sys/uio.h>
#include <sys/filio.h>
#include <sys/debug.h>
#include <sys/kmem.h>
#include <sys/cmn_err.h>

int
getdents64(int fd, void *buf, size_t count)
{
	vnode_t *vp;
	file_t *fp;
	struct uio auio;
	struct iovec aiov;
	register int error;
	int sink;

	if (count < sizeof (struct dirent64))
		return (set_errno(EINVAL));

	/*
	 * Don't let the user overcommit kernel resources.
	 */
	if (count > MAXGETDENTS_SIZE)
		count = MAXGETDENTS_SIZE;

	if ((fp = getf(fd)) == NULL)
		return (set_errno(EBADF));
	vp = fp->f_vnode;
	if (vp->v_type != VDIR) {
		releasef(fd);
		return (set_errno(ENOTDIR));
	}
	if (!(fp->f_flag & FREAD)) {
		releasef(fd);
		return (set_errno(EBADF));
	}
	aiov.iov_base = buf;
	aiov.iov_len = count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_loffset = fp->f_offset;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_resid = count;
	auio.uio_fmode = 0;
	auio.uio_extflg = UIO_COPY_CACHED;
	(void) fop_rwlock(vp, V_WRITELOCK_FALSE, NULL);
	error = fop_readdir(vp, &auio, fp->f_cred, &sink, NULL, 0);
	fop_rwunlock(vp, V_WRITELOCK_FALSE, NULL);
	if (error) {
		releasef(fd);
		return (set_errno(error));
	}
	count = count - auio.uio_resid;
	fp->f_offset = auio.uio_loffset;
	releasef(fd);
	return (count);
}
