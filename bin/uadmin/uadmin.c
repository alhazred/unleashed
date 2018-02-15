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
 * Copyright (c) 2000, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/



#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <libscf_priv.h>

#include <sys/types.h>
#include <sys/uadmin.h>
#include <sys/wait.h>

#define	SMF_RST	"/etc/svc/volatile/resetting"
#define	RETRY_COUNT 15	/* number of 1 sec retries for audit(8) to complete */

static const char *Usage = "Usage: %s cmd fcn [mdep]\n";

int
main(int argc, char *argv[])
{
	int cmd, fcn;
	uintptr_t mdep = (uintptr_t)NULL;
	sigset_t set;

	if (argc < 3 || argc > 4) {
		(void) fprintf(stderr, Usage, argv[0]);
		return (1);
	}

	(void) sigfillset(&set);
	(void) sigprocmask(SIG_BLOCK, &set, NULL);

	cmd = atoi(argv[1]);
	fcn = atoi(argv[2]);
	if (argc == 4) {	/* mdep argument given */
		if (cmd != A_REBOOT && cmd != A_SHUTDOWN && cmd != A_DUMP &&
		    cmd != A_FREEZE) {
			(void) fprintf(stderr, "%s: mdep argument not "
			    "allowed for this cmd value\n", argv[0]);
			(void) fprintf(stderr, Usage, argv[0]);
			return (1);
		} else {
			mdep = (uintptr_t)argv[3];
		}
	}

	switch (fcn) {
	case AD_HALT:
	case AD_POWEROFF:
	case AD_BOOT:
	case AD_IBOOT:
	case AD_SBOOT:
	case AD_SIBOOT:
	case AD_NOSYNC:
		break;
	case AD_FASTREBOOT:
	case AD_FASTREBOOT_DRYRUN:
		mdep = (uintptr_t)NULL;	/* Ignore all arguments */
		break;
	default:
		break;
	}

	if (cmd == A_CONFIG) {
		uint8_t boot_config = 0;
		uint8_t boot_config_ovr = 0;

		switch (fcn) {
		case AD_UPDATE_BOOT_CONFIG:
			scf_get_boot_config(&boot_config);
			boot_config_ovr = boot_config;
			scf_get_boot_config_ovr(&boot_config_ovr);
			boot_config &= boot_config_ovr;
			mdep = (uintptr_t)(&boot_config);
			break;
		}
	}

	if (geteuid() == 0) {
		if (cmd == A_SHUTDOWN || cmd == A_REBOOT)
			(void) creat(SMF_RST, 0777);
	}

	if (uadmin(cmd, fcn, mdep) < 0) {
		perror("uadmin");

		(void) unlink(SMF_RST);

		return (1);
	}

	return (0);
}
