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
 *
 * Portions Copyright 2008 Chad Mynhier
 *
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 * Copyright 2017 Lauri Tirkkonen <lotheac@iki.fi>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <libproc.h>
#include <wait.h>
#include <err.h>
#include <stdbool.h>

static void
usage()
{
	const char *command = getprogname();
	fprintf(stderr, "usage: %s command [args]\n"
	    "       %s -p pid\n", command, command);
	exit(1);
}

int
main(int argc, char **argv)
{
	char *pidarg = NULL;
	int opt;
	int gret;
	struct ps_prochandle *Pr;

	while ((opt = getopt(argc, argv, "p:")) != EOF) {
		switch (opt) {
		case 'p':
			pidarg = optarg;
			break;
		default:
			usage();
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (!pidarg && argc < 1)
		usage();
	if (pidarg) {
		Pr = proc_arg_grab(pidarg, PR_ARG_PIDS, PGRAB_RDONLY, &gret);
		if (!Pr)
			errx(1, "cannot examine %s: %s", pidarg,
			    Pgrab_error(gret));
		bool islegacy = Pstatus(Pr)->pr_flags & PR_LUNAME;
		printf("%s: %s\n", pidarg, islegacy ? "legacy uname" :
		    "standard uname");
		Prelease(Pr, 0);
		return 0;
	}
	pid_t pid = fork();
	switch (pid) {
	case -1:
		err(1, "fork");
		break;
	case 0:
		Pr = Pgrab(getppid(), 0, &gret);
		if (!Pr)
			errx(1, "cannot grab parent: %s", Pgrab_error(gret));
		if (Psetflags(Pr, PR_LUNAME) != 0)
			err(1, "cannot set legacy flag");
		Prelease(Pr, 0);
		return 0;
	default:
		if (waitpid(pid, &gret, 0) < 0)
			err(1, "waitpid");
		if ((!WIFEXITED(gret)) || (WEXITSTATUS(gret) != 0))
			return (1);
		execvp(argv[0], argv);
		err(1, "execvp");
		break;
	}
	return 1;
}
