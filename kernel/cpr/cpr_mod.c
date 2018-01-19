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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 * Copyright (c) 2011 Bayard G. Bell. All rights reserved.
 * Copyright 2012 Milan Jurik. All rights reserved.
 */

/*
 * System call to checkpoint and resume the currently running kernel
 */
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/modctl.h>
#include <sys/syscall.h>
#include <sys/cred.h>
#include <sys/uadmin.h>
#include <sys/cmn_err.h>
#include <sys/systm.h>
#include <sys/cpr.h>
#include <sys/swap.h>
#include <sys/vfs.h>
#include <sys/autoconf.h>
#include <sys/machsystm.h>

extern int i_cpr_is_supported(int sleeptype);
extern int cpr_is_ufs(struct vfs *);
extern int cpr_is_zfs(struct vfs *);
extern int cpr_check_spec_statefile(void);
extern int cpr_reusable_mount_check(void);
extern int i_cpr_reusable_supported(void);
extern int i_cpr_reusefini(void);
extern struct mod_ops mod_miscops;

extern int cpr_init(int);
extern void cpr_done(void);
extern void i_cpr_stop_other_cpus(void);
extern int i_cpr_power_down(int);


static struct modlmisc modlmisc = {
	&mod_miscops, "checkpoint resume"
};

static struct modlinkage modlinkage = {
	MODREV_1, (void *)&modlmisc, NULL
};

int cpr_reusable_mode;

kmutex_t	cpr_slock;	/* cpr serial lock */
cpr_t		cpr_state;
int		cpr_debug;
int		cpr_test_mode; /* true if called via uadmin testmode */
int		cpr_test_point = LOOP_BACK_NONE;	/* cpr test point */
int		cpr_mp_enable = 0;	/* set to 1 to enable MP suspend */
major_t		cpr_device = 0;		/* major number for S3 on one device */

/*
 * All the loadable module related code follows
 */
int
_init(void)
{
	register int e;

	if ((e = mod_install(&modlinkage)) == 0) {
		mutex_init(&cpr_slock, NULL, MUTEX_DEFAULT, NULL);
	}
	return (e);
}

int
_fini(void)
{
	register int e;

	if ((e = mod_remove(&modlinkage)) == 0) {
		mutex_destroy(&cpr_slock);
	}
	return (e);
}

int
_info(struct modinfo *modinfop)
{
	return (mod_info(&modlinkage, modinfop));
}

static
int
atoi(char *p)
{
	int	i;

	i = (*p++ - '0');

	while (*p != '\0')
		i = 10 * i + (*p++ - '0');

	return (i);
}

int
cpr(int fcn, void *mdep)
{

	register int rc = 0;
	int cpr_sleeptype;

	/*
	 * First, reject commands that we don't (yet) support on this arch.
	 * This is easier to understand broken out like this than grotting
	 * through the second switch below.
	 */

	switch (fcn) {
#if defined(__x86)
	case AD_CHECK_SUSPEND_TO_DISK:
	case AD_SUSPEND_TO_DISK:
	case AD_CPR_REUSEINIT:
	case AD_CPR_NOCOMPRESS:
	case AD_CPR_FORCE:
	case AD_CPR_REUSABLE:
	case AD_CPR_REUSEFINI:
	case AD_CPR_TESTZ:
	case AD_CPR_TESTNOZ:
	case AD_CPR_TESTHALT:
	case AD_CPR_PRINT:
		return (ENOTSUP);
	/* The DEV_* values need to be removed after sys-syspend is fixed */
	case DEV_CHECK_SUSPEND_TO_RAM:
	case DEV_SUSPEND_TO_RAM:
	case AD_CPR_SUSP_DEVICES:
	case AD_CHECK_SUSPEND_TO_RAM:
	case AD_SUSPEND_TO_RAM:
	case AD_LOOPBACK_SUSPEND_TO_RAM_PASS:
	case AD_LOOPBACK_SUSPEND_TO_RAM_FAIL:
	case AD_FORCE_SUSPEND_TO_RAM:
	case AD_DEVICE_SUSPEND_TO_RAM:
		cpr_sleeptype = CPR_TORAM;
		break;
#endif
	}

	switch (fcn) {


	case AD_CPR_DEBUG0:
		cpr_debug = 0;
		return (0);

	case AD_CPR_DEBUG1:
	case AD_CPR_DEBUG2:
	case AD_CPR_DEBUG3:
	case AD_CPR_DEBUG4:
	case AD_CPR_DEBUG5:
	case AD_CPR_DEBUG7:
	case AD_CPR_DEBUG8:
		cpr_debug |= CPR_DEBUG_BIT(fcn);
		return (0);

	case AD_CPR_DEBUG9:
		cpr_debug |= CPR_DEBUG6;
		return (0);

	/* The DEV_* values need to be removed after sys-syspend is fixed */
	case DEV_CHECK_SUSPEND_TO_RAM:
	case DEV_SUSPEND_TO_RAM:
	case AD_CHECK_SUSPEND_TO_RAM:
	case AD_SUSPEND_TO_RAM:
		cpr_test_point = LOOP_BACK_NONE;
		break;

	case AD_LOOPBACK_SUSPEND_TO_RAM_PASS:
		cpr_test_point = LOOP_BACK_PASS;
		break;

	case AD_LOOPBACK_SUSPEND_TO_RAM_FAIL:
		cpr_test_point = LOOP_BACK_FAIL;
		break;

	case AD_FORCE_SUSPEND_TO_RAM:
		cpr_test_point = FORCE_SUSPEND_TO_RAM;
		break;

	case AD_DEVICE_SUSPEND_TO_RAM:
		if (mdep == NULL) {
			/* Didn't pass enough arguments */
			return (EINVAL);
		}
		cpr_test_point = DEVICE_SUSPEND_TO_RAM;
		cpr_device = (major_t)atoi((char *)mdep);
		break;

	case AD_CPR_SUSP_DEVICES:
		cpr_test_point = FORCE_SUSPEND_TO_RAM;
		if (cpr_suspend_devices(ddi_root_node()) != DDI_SUCCESS)
			cmn_err(CE_WARN,
			    "Some devices did not suspend "
			    "and may be unusable");
		(void) cpr_resume_devices(ddi_root_node(), 0);
		return (0);

	default:
		return (ENOTSUP);
	}

	if (!i_cpr_is_supported(cpr_sleeptype))
		return (ENOTSUP);


	if (fcn == AD_CHECK_SUSPEND_TO_RAM ||
	    fcn == DEV_CHECK_SUSPEND_TO_RAM) {
		ASSERT(i_cpr_is_supported(cpr_sleeptype));
		return (0);
	}


	/*
	 * acquire cpr serial lock and init cpr state structure.
	 */
	if (rc = cpr_init(fcn))
		return (rc);


	/*
	 * Call the main cpr routine. If we are successful, we will be coming
	 * down from the resume side, otherwise we are still in suspend.
	 */
	cpr_err(CE_CONT, "System is being suspended");
	if (rc = cpr_main(cpr_sleeptype)) {
		CPR->c_flags |= C_ERROR;
		PMD(PMD_SX, ("cpr: Suspend operation failed.\n"))
		cpr_err(CE_NOTE, "Suspend operation failed.");
	} else if (CPR->c_flags & C_SUSPENDING) {

		/*
		 * In the suspend to RAM case, by the time we get
		 * control back we're already resumed
		 */
		if (cpr_sleeptype == CPR_TORAM) {
			PMD(PMD_SX, ("cpr: cpr CPR_TORAM done\n"))
			cpr_done();
			return (rc);
		}

	}
	PMD(PMD_SX, ("cpr: cpr done\n"))
	cpr_done();
	return (rc);
}
