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
 * Copyright 1998-2002 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_CLADM_H
#define	_SYS_CLADM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/clconf.h>
#include <netinet/in.h>


/*
 * This file defines interfaces which are private to Sun Clustering.
 * Others should not depend on this in any way as it may change or be
 * removed completely.
 */

/*
 * Command definitions for each of the facilities.
 * The type of the data pointer and the direction of the data transfer
 * is listed for each command.
 */

/*
 * Definitions for the flag bits returned by CL_GET_BOOTFLAG.
 */
#define	CLUSTER_CONFIGURED	0x0001	/* system is configured as a cluster */
#define	CLUSTER_BOOTED		0x0002	/* system is booted as a cluster */

#ifdef _KERNEL
#define	CLUSTER_INSTALLING	0x0004	/* cluster is being installed */
#define	CLUSTER_DCS_ENABLED	0x0008	/* cluster device framework enabled */
#endif	/* _KERNEL */

#ifdef _KERNEL
extern int cluster_bootflags;
#endif	/* _KERNEL */

#ifdef	__cplusplus
}
#endif


#endif	/* _SYS_CLADM_H */
