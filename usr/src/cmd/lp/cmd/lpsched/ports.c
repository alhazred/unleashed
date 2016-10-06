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
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#include "termio.h"
#include "unistd.h"

#include "lpsched.h"

#include <sys/ioccom.h>
#include <sys/ecppsys.h>

static void		sigalrm(int);
static int		push_module(int, char *, char *);

static int		SigAlrm;

/*
 * open_direct() - OPEN A PORT TO A DIRECTLY CONNECTED PRINTER
 */

int
open_direct(char *ptype, PRINTER *pp)
{
	short bufsz = -1, cps = -1;
	int open_mode, fd;
	register unsigned int oldalarm, newalarm = 0;
	char *device;

	struct ecpp_transfer_parms ecpp_params;	/* for ECPP port checking */
	char **modules = NULL;

	struct flock		lck;
	struct stat		buf;

	register void		(*oldsig)() = signal(SIGALRM, sigalrm);


	/*
	 * Set an alarm to wake us from trying to open the port.
	 * We'll try at least 60 seconds, or more if the printer
	 * has a huge buffer that, in the worst case, would take
	 * a long time to drain.
	 */
	tidbit(ptype, "bufsz", &bufsz);
	tidbit(ptype, "cps", &cps);
	if (bufsz > 0 && cps > 0)
		newalarm = (((long)bufsz * 1100) / cps) / 1000;
	if (newalarm < 60)
		newalarm = 60;
	oldalarm = alarm(newalarm);

	device = pp->device;
	if (is_printer_uri(device) == 0) {
		/*
		 * if it's a device uri and the endpoint contains a valid
		 * path, that path should be opened/locked by lpsched for
		 * the backend.  If not, the uri isn't associated with a
		 * local device, so use /dev/null.
		 */
		device = strstr(device, "://");
		if (device != NULL)
			device = strchr(device + 3, '/');

		if ((device == NULL) || (access(device, F_OK) < 0))
			device = "/dev/null";
	}

	/*
	 * The following open must be interruptable.
	 * O_APPEND is set in case the ``port'' is a file.
	 * O_RDWR is set in case the interface program wants
	 * to get input from the printer. Don't fail, though,
	 * just because we can't get read access.
	 */

	open_mode = O_WRONLY;
	if (access(device, R_OK) == 0)
		open_mode = O_RDWR;
	open_mode |= O_APPEND;

	SigAlrm = 0;

	while ((fd = open(device, open_mode, 0)) == -1) {
		if (errno != EINTR)
			return (EXEC_EXIT_NPORT);
		else if (SigAlrm)
			return (EXEC_EXIT_TMOUT);
	}

	alarm(oldalarm);
	signal(SIGALRM, oldsig);

	/*
	 * Lock the file in case two "printers" are defined on the
	 * same port.  Don't lock /dev/null.
	 */

	lck.l_type = F_WRLCK;
	lck.l_whence = 0;
	lck.l_start = 0L;
	lck.l_len = 0L;

	if (strcmp(device, "/dev/null") && Fcntl(fd, F_SETLKW, &lck) < 0) {
		execlog("lock error: %s\n", pp->device);
		return (EXEC_EXIT_NPORT);
	}

	/*
	 * We should get the correct channel number (1), but just
	 * in case....
	 */
	if (fd != 1) {
		dup2(fd, 1);
		Close(fd);
	}

	/*
	 * Handle streams modules:
	 */
	if (fstat(1, &buf))
		buf.st_mode = 0;

	/*
	 * for some unknown reason, lpsched appears to pop the streams
	 * modules off the device and push back some "default" ones,
	 * unless a specific set were specified with the printer configuration.
	 * This behaviour causes problems with the ECPP port, so if we have
	 * an ECPP port, and nobody specified a set of modules to use, we
	 * should leave it alone.  Normally, we would not bother to play with
	 * the streams modules, but it is possible that someone has come
	 * to rely on this behaviour for other devices.
	 */
	if ((pp->modules != NULL) && (pp->modules[0] != NULL) &&
	    (strcmp(pp->modules[0], "default") != 0))
		modules = pp->modules;

	if ((modules == NULL) && (ioctl(1, ECPPIOC_GETPARMS, &ecpp_params) < 0))
		modules = getlist(DEFMODULES, LP_WS, LP_SEP);

	/* if "nopush" is supplied, leave the modules alone */
	if ((modules != NULL) && (modules[0] != NULL) &&
	    (strcasecmp(modules[0], "nopush") == 0))
		modules = NULL;

	/*
	 * If we have a stream and a list of modules to use, then pop the old
	 * modules and push the new ones.
	 */
	if ((modules != NULL) && !S_ISFIFO(buf.st_mode) && isastream(1)) {
		/*
		 * First, pop all current modules off, unless
		 * instructed not to.
		 */
		while (ioctl(1, I_POP, 0) == 0)
			;

		/*
		 * Now push either the administrator specified modules
		 * or the standard modules, unless instructed to push
		 * nothing.
		 */

		if ((modules[1] == NULL) &&
		    (strcasecmp(modules[0], "none") == 0))
			return (0);

		while (*modules)
			if (push_module(1, device, *modules++) == -1)
				return (EXEC_EXIT_NPUSH);
	}

	return (0);
}

/*
 * sigalrm()
 */
static void
sigalrm(int ignore)
{
	signal(SIGALRM, SIG_IGN);
	SigAlrm = 1;
}


/*
 * push_module()
 */

static int
push_module(int fd, char *device, char *module)
{
	int ret	= ioctl(fd, I_PUSH, module);

	if (ret == -1)
		note("push (%s) on %s failed (%s)\n", module, device, PERROR);
	return (ret);
}
