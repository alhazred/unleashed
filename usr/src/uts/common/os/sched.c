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
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved	*/

#include <sys/param.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/cpuvar.h>
#include <sys/var.h>
#include <sys/tuneable.h>
#include <sys/cmn_err.h>
#include <sys/buf.h>
#include <sys/disp.h>
#include <sys/vmsystm.h>
#include <sys/vmparam.h>
#include <sys/class.h>
#include <sys/vtrace.h>
#include <sys/modctl.h>
#include <sys/debug.h>
#include <sys/tnf_probe.h>
#include <sys/procfs.h>

#include <vm/seg.h>
#include <vm/seg_kp.h>
#include <vm/as.h>
#include <vm/rm.h>
#include <vm/seg_kmem.h>
#include <sys/callb.h>

pgcnt_t	avefree;	/* 5 sec moving average of free memory */
pgcnt_t	avefree30;	/* 30 sec moving average of free memory */

/*
 * Memory scheduler.
 */
void
sched()
{
	kthread_id_t	t;
	callb_cpr_t	cprinfo;
	kmutex_t	swap_cpr_lock;

	mutex_init(&swap_cpr_lock, NULL, MUTEX_DEFAULT, NULL);
	CALLB_CPR_INIT(&cprinfo, &swap_cpr_lock, callb_generic_cpr, "sched");

	for (;;) {
		if (avenrun[0] >= 2 * FSCALE &&
		    (MAX(avefree, avefree30) < desfree) &&
		    (pginrate + pgoutrate > maxpgio || avefree < minfree)) {
			/*
			 * Unload all unloadable modules, free all other memory
			 * resources we can find, then look for a thread to
			 * hardswap.
			 */
			modreap();
			segkp_cache_free();
		}

		t = curthread;
		thread_lock(t);
		t->t_schedflag |= (TS_ALLSTART & ~TS_CSTART);
		t->t_whystop = PR_SUSPENDED;
		t->t_whatstop = SUSPEND_NORMAL;
		(void) new_mstate(t, LMS_SLEEP);
		mutex_enter(&swap_cpr_lock);
		CALLB_CPR_SAFE_BEGIN(&cprinfo);
		mutex_exit(&swap_cpr_lock);
		thread_stop(t);		/* change to stop state and drop lock */
		swtch();
		mutex_enter(&swap_cpr_lock);
		CALLB_CPR_SAFE_END(&cprinfo, &swap_cpr_lock);
		mutex_exit(&swap_cpr_lock);
	}
}
