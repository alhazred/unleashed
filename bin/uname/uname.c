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
 * Copyright (c) 1988, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

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
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/systeminfo.h>

static void usage(void);

/* ARGSUSED */
int
main(int argc, char *argv[], char *envp[])
{
	char *nodename;
	char *optstring = "asnrpvmiX";
	int sflg = 0, nflg = 0, rflg = 0, vflg = 0, mflg = 0;
	int pflg = 0, iflg = 0;
	int errflg = 0, optlet;
	int Xflg = 0;
	struct utsname  unstr, *un;
	char fmt_string[] = " %.*s";
	char *fs = &fmt_string[1];
	char procbuf[SYS_NMLN];

	(void) umask(~(S_IRWXU|S_IRGRP|S_IROTH) & S_IAMB);
	un = &unstr;
	(void) uname(un);

	(void) setlocale(LC_ALL, "");

	while ((optlet = getopt(argc, argv, optstring)) != EOF)
		switch (optlet) {
		case 'a':
			sflg++; nflg++; rflg++; vflg++; mflg++;
			pflg++;
			iflg++;
			break;
		case 's':
			sflg++;
			break;
		case 'n':
			nflg++;
			break;
		case 'r':
			rflg++;
			break;
		case 'v':
			vflg++;
			break;
		case 'm':
			mflg++;
			break;
		case 'p':
			pflg++;
			break;
		case 'i':
			iflg++;
			break;
		case 'X':
			Xflg++;
			break;

		case '?':
			errflg++;
		}

	if (errflg || (optind != argc))
		usage();

	/*
	 * "uname -s" is the default
	 */
	if (!(sflg || nflg || rflg || vflg || mflg || pflg || iflg || Xflg))
		sflg++;
	if (sflg) {
		(void) fprintf(stdout, fs, sizeof (un->sysname),
		    un->sysname);
		fs = fmt_string;
	}
	if (nflg) {
		(void) fprintf(stdout, fs, sizeof (un->nodename), un->nodename);
		fs = fmt_string;
	}
	if (rflg) {
		(void) fprintf(stdout, fs, sizeof (un->release), un->release);
		fs = fmt_string;
	}
	if (vflg) {
		(void) fprintf(stdout, fs, sizeof (un->version), un->version);
		fs = fmt_string;
	}
	if (mflg) {
		(void) fprintf(stdout, fs, sizeof (un->machine), un->machine);
		fs = fmt_string;
	}
	if (pflg) {
		if (sysinfo(SI_ARCHITECTURE, procbuf, sizeof (procbuf)) == -1) {
			err(1, "sysinfo");
		}
		(void) fprintf(stdout, fs, strlen(procbuf), procbuf);
		fs = fmt_string;
	}
	if (iflg) {
		if (sysinfo(SI_PLATFORM, procbuf, sizeof (procbuf)) == -1) {
			err(1, "sysinfo");
		}
		(void) fprintf(stdout, fs, strlen(procbuf), procbuf);
		fs = fmt_string;
	}
	if (sflg || nflg || rflg || vflg || mflg || pflg || iflg)
		(void) putchar('\n');
	if (Xflg) {
		int	val;

		(void) fprintf(stdout, "System = %.*s\n", sizeof (un->sysname),
		    un->sysname);
		(void) fprintf(stdout, "Node = %.*s\n", sizeof (un->nodename),
		    un->nodename);
		(void) fprintf(stdout, "Release = %.*s\n", sizeof (un->release),
		    un->release);
		(void) fprintf(stdout, "KernelID = %.*s\n",
		    sizeof (un->version), un->version);
		(void) fprintf(stdout, "Machine = %.*s\n", sizeof (un->machine),
		    un->machine);

		val = sysconf(_SC_NPROCESSORS_CONF);
		(void) fprintf(stdout, "NumCPU = %d\n", val);
	}
	return (0);
}

static void
usage(void)
{
	(void) fprintf(stderr, "usage:	uname [-snrvmapiX]\n");
	exit(1);
}
