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
 * Copyright 2010 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 * Copyright 2015 Nexenta Systems, Inc. All rights reserved.
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <sys/cpuvar.h>
#include <sys/vfs.h>
#include <sys/vnode.h>
#include <sys/pathname.h>
#include <sys/callb.h>
#include <sys/fs/ufs_inode.h>
#include <vm/anon.h>
#include <sys/fs/swapnode.h>	/* for swapfs_minfree */
#include <sys/kmem.h>
#include <sys/cpr.h>
#include <sys/conf.h>
#include <sys/machclock.h>

/*
 * CPR miscellaneous support routines
 */
#define	cpr_open(path, mode,  vpp)	(vn_open(path, UIO_SYSSPACE, \
		mode, 0600, vpp, CRCREAT, 0))
#define	cpr_rdwr(rw, vp, basep, cnt)	(vn_rdwr(rw, vp,  (caddr_t)(basep), \
		cnt, 0LL, UIO_SYSSPACE, 0, (rlim64_t)MAXOFF_T, CRED(), \
		NULL))

extern void clkset(time_t);
extern cpu_t *i_cpr_bootcpu(void);
extern caddr_t i_cpr_map_setup(void);
extern void i_cpr_free_memory_resources(void);

extern kmutex_t cpr_slock;
extern size_t cpr_buf_size;
extern char *cpr_buf;
extern size_t cpr_pagedata_size;
extern char *cpr_pagedata;
extern int cpr_bufs_allocated;
extern int cpr_bitmaps_allocated;


int cpr_is_ufs(struct vfs *);
int cpr_is_zfs(struct vfs *);

char cpr_default_path[] = CPR_DEFAULT;

#define	COMPRESS_PERCENT 40	/* approx compression ratio in percent */
#define	SIZE_RATE	115	/* increase size by 15% */
#define	INTEGRAL	100	/* for integer math */


/*
 * cmn_err() followed by a 1/4 second delay; this gives the
 * logging service a chance to flush messages and helps avoid
 * intermixing output from prom_printf().
 */
/*PRINTFLIKE2*/
void
cpr_err(int ce, const char *fmt, ...)
{
	va_list adx;

	va_start(adx, fmt);
	vcmn_err(ce, fmt, adx);
	va_end(adx);
	drv_usecwait(MICROSEC >> 2);
}


int
cpr_init(int fcn)
{
	/*
	 * Allow only one suspend/resume process.
	 */
	if (mutex_tryenter(&cpr_slock) == 0)
		return (EBUSY);

	CPR->c_flags = 0;
	CPR->c_substate = 0;
	CPR->c_cprboot_magic = 0;
	CPR->c_alloc_cnt = 0;

	CPR->c_fcn = fcn;
	if (fcn == AD_CPR_REUSABLE)
		CPR->c_flags |= C_REUSABLE;
	else
		CPR->c_flags |= C_SUSPENDING;
	if (fcn == AD_SUSPEND_TO_RAM || fcn == DEV_SUSPEND_TO_RAM) {
		return (0);
	}

	return (0);
}

/*
 * This routine releases any resources used during the checkpoint.
 */
void
cpr_done(void)
{
	cpr_stat_cleanup();
	i_cpr_bitmap_cleanup();

	/*
	 * Free pages used by cpr buffers.
	 */
	if (cpr_buf) {
		kmem_free(cpr_buf, cpr_buf_size);
		cpr_buf = NULL;
	}
	if (cpr_pagedata) {
		kmem_free(cpr_pagedata, cpr_pagedata_size);
		cpr_pagedata = NULL;
	}

	i_cpr_free_memory_resources();
	mutex_exit(&cpr_slock);
	cpr_err(CE_CONT, "System has been resumed.\n");
}



/*
 * clock/time related routines
 */
static time_t   cpr_time_stamp;


void
cpr_tod_get(cpr_time_t *ctp)
{
	timestruc_t ts;

	mutex_enter(&tod_lock);
	ts = TODOP_GET(tod_ops);
	mutex_exit(&tod_lock);
	ctp->tv_sec = (time32_t)ts.tv_sec;
	ctp->tv_nsec = (int32_t)ts.tv_nsec;
}

void
cpr_tod_status_set(int tod_flag)
{
	mutex_enter(&tod_lock);
	tod_status_set(tod_flag);
	mutex_exit(&tod_lock);
}

void
cpr_save_time(void)
{
	cpr_time_stamp = gethrestime_sec();
}

/*
 * correct time based on saved time stamp or hardware clock
 */
void
cpr_restore_time(void)
{
	clkset(cpr_time_stamp);
}

