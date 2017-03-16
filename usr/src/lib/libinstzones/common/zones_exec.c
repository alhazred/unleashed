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



/*
 * Module:	zones_exec.c
 * Group:	libinstzones
 * Description:	Provide "zones" execution interface for install
 *		consolidation code
 *
 * Public Methods:
 *
 * z_ExecCmdArray - Execute a Unix command and return results and status
 * _zexec - run a command with arguments on a specified zone
 * _zexec_init_template - used by _zexec to establish contracts
 * _z_zone_exec - Execute a Unix command in a specified zone and return results
 * z_ExecCmdList - Execute a Unix command and return results and status
 */

/*
 * System includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/param.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <stropts.h>
#include <libintl.h>
#include <locale.h>
#include <libcontract.h>
#include <sys/contract/process.h>
#include <sys/ctfs.h>
#include <assert.h>

/*
 * local includes
 */

#include "instzones_lib.h"
#include "zones_strings.h"

/*
 * Private structures
 */

/*
 * Library Function Prototypes
 */

/*
 * Local Function Prototypes
 */

/*
 * global internal (private) declarations
 */

/*
 * *****************************************************************************
 * global external (public) functions
 * *****************************************************************************
 */

/*
 * Name:	z_ExecCmdArray
 * Synopsis:	Execute Unix command and return results
 * Description:	Execute a Unix command and return results and status
 * Arguments:
 *		r_status - [RO, *RW] - (int *)
 *			Return (exit) status from Unix command:
 *			== -1 : child terminated with a signal
 *			!= -1 : lower 8-bit value child passed to exit()
 *		r_results - [RO, *RW] - (char **)
 *			Any output generated by the Unix command to stdout
 *			and to stderr
 *			== NULL if no output generated
 *		a_inputFile - [RO, *RO] - (char *)
 *			Pointer to character string representing file to be
 *			used as "standard input" for the command.
 *			== NULL to use "/dev/null" as standard input
 *		a_cmd - [RO, *RO] - (char *)
 *			Pointer to character string representing the full path
 *			of the Unix command to execute
 *		char **a_args - [RO, *RO] - (char **)
 *			List of character strings representing the arguments
 *			to be passed to the Unix command. The list must be
 *			terminated with an element that is NULL
 * Returns:	int
 *			== 0 - Command executed
 *				Look at r_status for results of Unix command
 *			!= 0 - problems executing command
 *				r_status and r_results have no meaning;
 *				r_status will be -1
 *				r_results will be NULL
 * NOTE:    	Any results returned is placed in new storage for the
 *		calling method. The caller must use 'free' to dispose
 *		of the storage once the results are no longer needed.
 * NOTE:	If 0 is returned, 'r_status' must be queried to
 *		determine the results of the Unix command.
 * NOTE:	The system "errno" value from immediately after waitpid() call
 *		is preserved for the calling method to use to determine
 *		the system reason why the operation failed.
 */

int
z_ExecCmdArray(int *r_status, char **r_results,
    char *a_inputFile, char *a_cmd, char **a_args)
{
	char		*buffer;
	int		bufferIndex;
	int		bufferSize;
	int		ipipe[2] = {0, 0};
	int		lerrno;
	int		status;
	int		stdinfile = -1;
	pid_t		pid;
	pid_t		resultPid;

	/* entry assertions */

	assert(r_status != NULL);
	assert(a_cmd != NULL);
	assert(*a_cmd != '\0');
	assert(a_args != NULL);

	/* reset return results buffer pointer */

	if (r_results != (char **)NULL) {
		*r_results = NULL;
	}

	*r_status = -1;

	/*
	 * See if command exists
	 */

	if (access(a_cmd, F_OK|X_OK) != 0) {
		return (-1);
	}

	/*
	 * See if input file exists
	 */

	if (a_inputFile != NULL) {
		stdinfile = open(a_inputFile, O_RDONLY);
	} else {
		stdinfile = open("/dev/null", O_RDONLY); /* stdin = /dev/null */
	}

	if (stdinfile < 0) {
		return (-1);
	}

	/*
	 * Create a pipe to be used to capture the command output
	 */

	if (pipe(ipipe) != 0) {
		(void) close(stdinfile);
		return (-1);
	}


	bufferSize = PIPE_BUFFER_INCREMENT;
	bufferIndex = 0;
	buffer = calloc(1, bufferSize);
	if (buffer == NULL) {
		(void) close(stdinfile);
		return (-1);
	}

	/* flush standard i/o before creating new process */

	(void) fflush(stderr);
	(void) fflush(stdout);

	/*
	 * create new process to execute command in;
	 * vfork() is being used to avoid duplicating the parents
	 * memory space - this means that the child process may
	 * not modify any of the parents memory including the
	 * standard i/o descriptors - all the child can do is
	 * adjust interrupts and open files as a prelude to a
	 * call to exec().
	 */

	pid = vfork();

	if (pid == 0) {
		/*
		 * This is the forked (child) process ======================
		 */

		int	i;

		/* reset any signals to default */

		for (i = 0; i < NSIG; i++) {
			(void) sigset(i, SIG_DFL);
		}

		/* assign stdin, stdout, stderr as appropriate */

		(void) dup2(stdinfile, STDIN_FILENO);
		(void) close(ipipe[0]);		/* close out pipe reader side */
		(void) dup2(ipipe[1], STDOUT_FILENO);
		(void) dup2(ipipe[1], STDERR_FILENO);

		/* Close all open files except standard i/o */

		closefrom(3);

		/* execute target executable */

		(void) execvp(a_cmd, a_args);
		perror(a_cmd);	/* Emit error msg - ends up in callers buffer */
		_exit(0x00FE);
	} else if (pid == -1) {
		_z_program_error(ERR_FORK, strerror(errno));
		*r_status = -1;
		return (-1);
	}

	/*
	 * This is the forking (parent) process ====================
	 */

	(void) close(stdinfile);
	(void) close(ipipe[1]);		/* Close write side of pipe */

	/*
	 * Spin reading data from the child into the buffer - when the read eofs
	 * the child has exited
	 */

	for (;;) {
		ssize_t	bytesRead;

		/* read as much child data as there is available buffer space */

		bytesRead = read(ipipe[0], buffer + bufferIndex,
		    bufferSize - bufferIndex);

		/* break out of read loop if end-of-file encountered */

		if (bytesRead == 0) {
			break;
		}

		/* if error, continue if recoverable, else break out of loop */

		if (bytesRead == -1) {
			/* try again: EAGAIN - insufficient resources */

			if (errno == EAGAIN) {
				continue;
			}

			/* try again: EINTR - interrupted system call */

			if (errno == EINTR) {
				continue;
			}

			/* break out of loop - error not recoverable */
			break;
		}

		/* at least 1 byte read: expand buffer if at end */

		bufferIndex += bytesRead;
		if (bufferIndex >= bufferSize) {
			buffer = realloc(buffer,
			    bufferSize += PIPE_BUFFER_INCREMENT);
			(void) memset(buffer + bufferIndex, 0,
			    bufferSize - bufferIndex);
		}
	}

	(void) close(ipipe[0]);		/* Close read side of pipe */

	/* Get subprocess exit status */

	for (;;) {
		resultPid = waitpid(pid, &status, 0L);
		lerrno = (resultPid == -1 ? errno : 0);

		/* break loop if child process status reaped */

		if (resultPid != -1) {
			break;
		}

		/* break loop if not interrupted out of waitpid */

		if (errno != EINTR) {
			break;
		}
	}

	/*
	 * If the child process terminated due to a call to exit(), then
	 * set results equal to the 8-bit exit status of the child process;
	 * otherwise, set the exit status to "-1" indicating that the child
	 * exited via a signal.
	 */

	*r_status = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

	/* return appropriate output */

	if (!*buffer) {
		/* No contents in output buffer - discard */
		free(buffer);
	} else if (r_results == (char **)NULL) {
		/* Not requested to return results - discard */
		free(buffer);
	} else {
		/* have output and request to return: pass to calling method */
		*r_results = buffer;
	}

	errno = lerrno;
	return (resultPid == -1 ? -1 : 0);
}

/*
 * Name:	_zexec
 * Description:	run a command with arguments on a specified zone
 * Arguments:	a_zoneName - pointer to string representing the name of the zone
 *			to execute the specified command in
 *		a_path - pointer to string representing the full path *in the
 *			non-global zone named by a_zoneName* of the Unix command
 *			to be executed
 *		a_argv[] - Pointer to array of character strings representing
 *			the arguments to be passed to the Unix command. The list
 *			must be termianted with an element that is NULL
 *		NOTE: a_argv[0] is the "command name" passed to the command
 * Returns:	int
 *			This function must be treated like a call to exec()
 *			If the exec() is successful, the thread of control is
 *			NOT returned, and the process will exit when completed.
 *			If this function returns, it means the exec() could not
 *			be done, or another fatal error occurred.
 */

int
_zexec(const char *a_zoneName, const char *a_path, char *a_argv[])
{
	zoneid_t zoneid;
	zone_state_t st;
	char **new_env = { NULL };
	priv_set_t *privset;

	/* entry assertions */

	assert(a_zoneName != NULL);
	assert(*a_zoneName != '\0');
	assert(a_path != NULL);
	assert(*a_path != '\0');

	/* establish locale settings */

	(void) setlocale(LC_ALL, "");
	(void) textdomain(TEXT_DOMAIN);

	/* can only be invoked from within the global zone */

	if (getzoneid() != GLOBAL_ZONEID) {
		_z_program_error(ERR_ZEXEC_NOT_IN_GZ, a_zoneName);
		return (-1);
	}

	if (strcmp(a_zoneName, GLOBAL_ZONENAME) == 0) {
		_z_program_error(ERR_ZEXEC_GZUSED, a_zoneName);
		return (-1);
	}

	/* get the state of the specified zone */

	if (zone_get_state((char *)a_zoneName, &st) != Z_OK) {
		_z_program_error(ERR_ZEXEC_BADZONE, a_zoneName);
		return (-1);
	}

	if (st < ZONE_STATE_INSTALLED) {
		_z_program_error(ERR_ZEXEC_BADSTATE, a_zoneName,
		    zone_state_str(st));
		return (-1);
	}

	if (st != ZONE_STATE_RUNNING && st != ZONE_STATE_MOUNTED) {
		_z_program_error(ERR_ZEXEC_NOTRUNNING, a_zoneName,
		    zone_state_str(st));
		return (-1);
	}

	/*
	 * In both console and non-console cases, we require all privs.
	 * In the console case, because we may need to startup zoneadmd.
	 * In the non-console case in order to do zone_enter(2), zonept()
	 * and other tasks.
	 *
	 * Future work: this solution is temporary.  Ultimately, we need to
	 * move to a flexible system which allows the global admin to
	 * designate that a particular user can zlogin (and probably zlogin
	 * -C) to a particular zone.  This all-root business we have now is
	 * quite sketchy.
	 */

	if ((privset = priv_allocset()) == NULL) {
		_z_program_error(ERR_ZEXEC_PRIV_ALLOCSET, a_zoneName,
		    strerror(errno));
		return (-1);
	}

	if (getppriv(PRIV_EFFECTIVE, privset) != 0) {
		_z_program_error(ERR_ZEXEC_GETPPRIV, a_zoneName,
		    strerror(errno));
		priv_freeset(privset);
		return (-1);
	}

	if (priv_isfullset(privset) == B_FALSE) {
		_z_program_error(ERR_ZEXEC_PRIVS, a_zoneName);
		priv_freeset(privset);
		return (-1);
	}
	priv_freeset(privset);

	if ((zoneid = getzoneidbyname(a_zoneName)) == -1) {
		_z_program_error(ERR_ZEXEC_NOZONEID, a_zoneName,
		    strerror(errno));
		return (-1);
	}

	if ((new_env = _zexec_prep_env()) == NULL) {
		_z_program_error(ERR_ZEXEC_ASSEMBLE, a_zoneName);
		return (-1);
	}

	/*
	 * In case any of stdin, stdout or stderr are streams,
	 * anchor them to prevent malicious I_POPs.
	 *
	 * Future work: use pipes to entirely eliminate FD leakage
	 * into the zone.
	 */

	(void) ioctl(STDIN_FILENO, I_ANCHOR);
	(void) ioctl(STDOUT_FILENO, I_ANCHOR);
	(void) ioctl(STDERR_FILENO, I_ANCHOR);

	if (zone_enter(zoneid) == -1) {
		int	lerrno = errno;

		_z_program_error(ERR_ZEXEC_ZONEENTER, a_zoneName,
		    strerror(errno));

		if (lerrno == EFAULT) {
			_z_program_error(ERR_ZEXEC_EFAULT, a_zoneName);
		}

		free(new_env);

		return (-1);
	}

	(void) execve(a_path, &a_argv[0], new_env);

	_z_program_error(ERR_ZEXEC_EXECFAILURE, a_zoneName, strerror(errno));

	return (-1);
}

/*
 * Name:	_zexec_init_template
 * Description:	used by _zexec to establish contracts
 */

int
_zexec_init_template(void)
{
	int fd;
	int err = 0;

	fd = open64(CTFS_ROOT "/process/template", O_RDWR);
	if (fd == -1) {
		return (-1);
	}

	/*
	 * zlogin doesn't do anything with the contract.
	 * Deliver no events, don't inherit, and allow it to be orphaned.
	 */
	err |= ct_tmpl_set_critical(fd, 0);
	err |= ct_tmpl_set_informative(fd, 0);
	err |= ct_pr_tmpl_set_fatal(fd, CT_PR_EV_HWERR);
	err |= ct_pr_tmpl_set_param(fd, CT_PR_PGRPONLY | CT_PR_REGENT);
	if (err || ct_tmpl_activate(fd)) {
		(void) close(fd);
		return (-1);
	}

	return (fd);
}

/*
 * Helper routine for _zexec_prep_env below.
 */
char *
_zexec_add_env(char *name, char *value)
{
	size_t sz = strlen(name) + strlen(value) + 1;
	char *str;

	if ((str = malloc(sz)) == NULL)
		return (NULL);

	(void) snprintf(str, sz, "%s%s", name, value);
	return (str);
}

/*
 * Prepare envp array for exec'd process.
 */
char **
_zexec_prep_env()
{
	int e = 0, size = 1;
	char **new_env, *estr;
	char *term = getenv("TERM");

	size++;	/* for $PATH */
	if (term != NULL)
		size++;

	/*
	 * In failsafe mode we set $HOME
	 */
	size++;

	/*
	 * In failsafe mode we set $SHELL, since login won't be around to do it.
	 */
	size++;

	if ((new_env = malloc(sizeof (char *) * size)) == NULL)
		return (NULL);

	if ((estr = _zexec_add_env("PATH=", ZONE_DEF_PATH)) == NULL) {
		free(new_env);
		return (NULL);
	}
	new_env[e++] = estr;

	if (term != NULL) {
		if ((estr = _zexec_add_env("TERM=", term)) == NULL) {
			free(new_env);
			return (NULL);
		}
		new_env[e++] = estr;
	}

	if ((estr = _zexec_add_env("HOME=", "/")) == NULL) {
		free(new_env);
		return (NULL);
	}
	new_env[e++] = estr;

	if ((estr = _zexec_add_env("SHELL=", ZONE_FAILSAFESHELL)) == NULL) {
		free(new_env);
		return (NULL);
	}
	new_env[e++] = estr;

	new_env[e++] = NULL;

	return (new_env);
}

/*
 * Name:	_z_zone_exec
 * Description:	Execute a Unix command in a specified zone and return results
 * Arguments:
 *		r_status - [RO, *RW] - (int *)
 *			Return (exit) status from Unix command:
 *			== -1 : child terminated with a signal
 *			!= -1 : lower 8-bit value child passed to exit()
 *		r_results - [RO, *RW] - (char **)
 *			Any output generated by the Unix command to stdout
 *			and to stderr
 *			== NULL if no output generated
 *		a_inputFile - [RO, *RO] - (char *)
 *			Pointer to character string representing file to be
 *			used as "standard input" for the command.
 *			== NULL to use "/dev/null" as standard input
 *		a_path - [RO, *RO] - (char *)
 *			Pointer to character string representing the full path
 *			*in the non-global zone named by a_zoneName*of the Unix
 *			command to be executed
 *		char **a_args - [RO, *RO] - (char **)
 *			List of character strings representing the arguments
 *			to be passed to the Unix command.
 *			NOTE: The list must be terminated with an element that
 *			----- is NULL
 *			NOTE: a_argv[0] is the "command name" passed to the
 *			----- command executed in the specified non-global zone
 *		a_zoneName - pointer to string representing the name of the zone
 *			to execute the specified command in
 *		a_fds - Pointer to array of integers representing file
 *			descriptors to remain open during the call - all
 *			file descriptors above STDERR_FILENO not in this
 *			list will be closed.
 * Returns:	int
 *			== 0 - Command executed
 *				Look at r_status for results of Unix command
 *			!= 0 - problems executing command
 *				r_status and r_results have no meaning;
 *				r_status will be -1
 *				r_results will be NULL
 *			The return (exit) code from the specified Unix command
 *			Special return codes:
 *			-1 : failure to exec process
 *			-2 : could not create contract for greenline
 *			-3 : fork() failed
 *			-4 : could not open stdin source file
 *			-5 : error from 'waitpid' other than EINTR
 *			-6 : zones are not supported
 *			-7 : interrupt received
 * NOTE:	All file descriptores other than 0, 1 and 2 are closed except
 *		for those file descriptors listed in the a_fds array.
 */

int
_z_zone_exec(int *r_status, char **r_results, char *a_inputFile,
	char *a_path, char *a_argv[], const char *a_zoneName, int *a_fds)
{
	struct sigaction	nact;
	struct sigaction	oact;
	char			*buffer;
	char			*thisZoneName;
	int			bufferIndex;
	int			bufferSize;
	int			exit_no;
	int			ipipe[2] = {0, 0};
	int			lerrno;
	int			n;
	int			status;
	int			stdinfile = -1;
	int			tmpl_fd;
	pid_t			child_pid;
	pid_t			result_pid;
	void			(*funcSighup)();
	void			(*funcSigint)();

	/* entry assertions */

	assert(a_path != NULL);
	assert(*a_path != '\0');
	assert(a_argv != (char **)NULL);
	assert(a_argv[0] != NULL);
	assert(*a_argv[0] != '\0');
	assert(a_zoneName != NULL);

	/*
	 * if requested to execute in current zone name, directly execute
	 */

	thisZoneName = z_get_zonename();
	status = (strcmp(a_zoneName, thisZoneName) == 0);

	/* entry debugging info */

	_z_echoDebug(DBG_ZONE_EXEC_CMD_ENTER, a_path, a_zoneName, thisZoneName);
	(void) free(thisZoneName);
	for (n = 0; a_argv[n]; n++) {
		_z_echoDebug(DBG_ARG, n, a_argv[n]);
	}

	/* if this zone, just exec the command directly */

	if (status != 0) {
		return (z_ExecCmdArray(r_status, r_results, a_inputFile,
		    a_path, a_argv));
	}

	/* reset return results buffer pointer */

	if (r_results != (char **)NULL) {
		*r_results = NULL;
	}

	*r_status = -1;	/* -1 : failure to exec process */

	/* if zones are not implemented, return TRUE */

	if (!z_zones_are_implemented()) {
		return (-6);	/* -6 : zones are not supported */
	}

	if ((tmpl_fd = _zexec_init_template()) == -1) {
		_z_program_error(ERR_CANNOT_CREATE_CONTRACT, strerror(errno));
		return (-2);	/* -2 : cannot create greenline contract */
	}

	/*
	 * See if input file exists
	 */

	if (a_inputFile != NULL) {
		stdinfile = open(a_inputFile, O_RDONLY);
	} else {
		stdinfile = open("/dev/null", O_RDONLY); /* stdin = /dev/null */
	}

	if (stdinfile < 0) {
		return (-4);	/* -4 : could not open stdin source file */
	}

	/*
	 * Create a pipe to be used to capture the command output
	 */

	if (pipe(ipipe) != 0) {
		(void) close(stdinfile);
		return (-1);
	}

	bufferSize = PIPE_BUFFER_INCREMENT;
	bufferIndex = 0;
	buffer = calloc(1, bufferSize);
	if (buffer == NULL) {
		(void) close(stdinfile);
		return (-1);
	}

	/* flush standard i/o before creating new process */

	(void) fflush(stderr);
	(void) fflush(stdout);

	/*
	 * hold SIGINT/SIGHUP signals and reset signal received counter;
	 * after the fork1() the parent and child need to setup their respective
	 * interrupt handling and release the hold on the signals
	 */

	(void) sighold(SIGINT);
	(void) sighold(SIGHUP);

	_z_global_data._z_SigReceived = 0;	/* no signals received */

	/*
	 * fork off a new process to execute command in;
	 * fork1() is used instead of vfork() so the child process can
	 * perform operations that would modify the parent process if
	 * vfork() were used
	 */

	child_pid = fork1();

	if (child_pid < 0) {
		/*
		 * *************************************************************
		 * fork failed!
		 * *************************************************************
		 */

		(void) ct_tmpl_clear(tmpl_fd);
		(void) close(tmpl_fd);
		(void) free(buffer);
		_z_program_error(ERR_FORK, strerror(errno));

		/* release hold on signals */
		(void) sigrelse(SIGHUP);
		(void) sigrelse(SIGINT);

		return (-3);	/* -3 : fork() failed */
	}

	if (child_pid == 0) {
		int	i;

		/*
		 * *************************************************************
		 * This is the forked (child) process
		 * *************************************************************
		 */

		(void) ct_tmpl_clear(tmpl_fd);
		(void) close(tmpl_fd);

		/* reset any signals to default */

		for (i = 0; i < NSIG; i++) {
			(void) sigset(i, SIG_DFL);
		}

		/* assign stdin, stdout, stderr as appropriate */

		(void) dup2(stdinfile, STDIN_FILENO);
		(void) close(ipipe[0]);		/* close out pipe reader side */
		(void) dup2(ipipe[1], STDOUT_FILENO);
		(void) dup2(ipipe[1], STDERR_FILENO);

		/*
		 * close all file descriptors not in the a_fds list
		 */

		(void) fdwalk(&_z_close_file_descriptors, (void *)a_fds);

		/* release all held signals */

		(void) sigrelse(SIGHUP);
		(void) sigrelse(SIGINT);

		/* execute command in the specified non-global zone */

		_exit(_zexec(a_zoneName, a_path, a_argv));
	}

	/*
	 * *********************************************************************
	 * This is the forking (parent) process
	 * *********************************************************************
	 */

	/* register child process i.d. so signal handlers can pass signal on */

	_z_global_data._z_ChildProcessId = child_pid;

	/*
	 * setup signal handlers for SIGINT and SIGHUP and release hold
	 */

	/* hook SIGINT to _z_sig_trap() */

	nact.sa_handler = _z_sig_trap;
	nact.sa_flags = SA_RESTART;
	(void) sigemptyset(&nact.sa_mask);

	if (sigaction(SIGINT, &nact, &oact) < 0) {
		funcSigint = SIG_DFL;
	} else {
		funcSigint = oact.sa_handler;
	}

	/* hook SIGHUP to _z_sig_trap() */

	nact.sa_handler = _z_sig_trap;
	nact.sa_flags = SA_RESTART;
	(void) sigemptyset(&nact.sa_mask);

	if (sigaction(SIGHUP, &nact, &oact) < 0) {
		funcSighup = SIG_DFL;
	} else {
		funcSighup = oact.sa_handler;
	}

	/* release hold on signals */

	(void) sigrelse(SIGHUP);
	(void) sigrelse(SIGINT);

	(void) ct_tmpl_clear(tmpl_fd);
	(void) close(tmpl_fd);

	(void) close(stdinfile);
	(void) close(ipipe[1]);		/* Close write side of pipe */

	/*
	 * Spin reading data from the child into the buffer - when the read eofs
	 * the child has exited
	 */

	for (;;) {
		ssize_t	bytesRead;

		/* read as much child data as there is available buffer space */

		bytesRead = read(ipipe[0], buffer + bufferIndex,
		    bufferSize - bufferIndex);

		/* break out of read loop if end-of-file encountered */

		if (bytesRead == 0) {
			break;
		}

		/* if error, continue if recoverable, else break out of loop */

		if (bytesRead == -1) {
			/* try again: EAGAIN - insufficient resources */

			if (errno == EAGAIN) {
				continue;
			}

			/* try again: EINTR - interrupted system call */

			if (errno == EINTR) {
				continue;
			}

			/* break out of loop - error not recoverable */
			break;
		}

		/* at least 1 byte read: expand buffer if at end */

		bufferIndex += bytesRead;
		if (bufferIndex >= bufferSize) {
			buffer = realloc(buffer,
			    bufferSize += PIPE_BUFFER_INCREMENT);
			(void) memset(buffer + bufferIndex, 0,
			    bufferSize - bufferIndex);
		}
	}

	(void) close(ipipe[0]);		/* Close read side of pipe */

	/*
	 * wait for the process to exit, reap child exit status
	 */

	for (;;) {
		result_pid = waitpid(child_pid, &status, 0L);
		lerrno = (result_pid == -1 ? errno : 0);

		/* break loop if child process status reaped */

		if (result_pid != -1) {
			break;
		}

		/* break loop if not interrupted out of waitpid */

		if (errno != EINTR) {
			break;
		}
	}

	/* reset child process i.d. so signal handlers do not pass signals on */

	_z_global_data._z_ChildProcessId = -1;

	/*
	 * If the child process terminated due to a call to exit(), then
	 * set results equal to the 8-bit exit status of the child process;
	 * otherwise, set the exit status to "-1" indicating that the child
	 * exited via a signal.
	 */

	if (WIFEXITED(status)) {
		*r_status = WEXITSTATUS(status);
		if ((_z_global_data._z_SigReceived != 0) && (*r_status == 0)) {
			*r_status = 1;
		}
	} else {
		*r_status = -1;	/* -1 : failure to exec process */
	}

	/* determine proper exit code */

	if (result_pid == -1) {
		exit_no = -5;	/* -5 : error from 'waitpid' other than EINTR */
	} else if (_z_global_data._z_SigReceived != 0) {
		exit_no = -7;	/* -7 : interrupt received */
	} else {
		exit_no = 0;
	}

	/* return appropriate output */

	if (!*buffer) {
		/* No contents in output buffer - discard */
		free(buffer);
	} else if (r_results == (char **)NULL) {
		/* Not requested to return results - discard */
		free(buffer);
	} else {
		/* have output and request to return: pass to calling method */
		*r_results = buffer;
	}

	/*
	 * reset signal handlers
	 */

	/* reset SIGINT */

	nact.sa_handler = funcSigint;
	nact.sa_flags = SA_RESTART;
	(void) sigemptyset(&nact.sa_mask);

	(void) sigaction(SIGINT, &nact, NULL);

	/* reset SIGHUP */

	nact.sa_handler = funcSighup;
	nact.sa_flags = SA_RESTART;
	(void) sigemptyset(&nact.sa_mask);

	(void) sigaction(SIGHUP, &nact, NULL);

	/*
	 * if signal received during command execution, interrupt
	 * this process now.
	 */

	if (_z_global_data._z_SigReceived != 0) {
		(void) kill(getpid(), SIGINT);
	}

	/* set errno and return */

	errno = lerrno;

	return (exit_no);
}

/*
 * Name:	z_ExecCmdList
 * Synopsis:	Execute Unix command and return results
 * Description:	Execute a Unix command and return results and status
 * Arguments:
 *		r_status - [RO, *RW] - (int *)
 *			Return (exit) status from Unix command
 *		r_results - [RO, *RW] - (char **)
 *			Any output generated by the Unix command to stdout
 *			and to stderr
 *			== NULL if no output generated
 *		a_inputFile - [RO, *RO] - (char *)
 *			Pointer to character string representing file to be
 *			used as "standard input" for the command.
 *			== NULL to use "/dev/null" as standard input
 *		a_cmd - [RO, *RO] - (char *)
 *			Pointer to character string representing the full path
 *			of the Unix command to execute
 *		... - [RO] (?)
 *			Zero or more arguments to the Unix command
 *			The argument list must be ended with NULL
 * Returns:	int
 *			== 0 - Command executed
 *				Look at r_status for results of Unix command
 *			!= 0 - problems executing command
 *				r_status and r_results have no meaning
 * NOTE:    	Any results returned is placed in new storage for the
 *		calling method. The caller must use 'free' to dispose
 *		of the storage once the results are no longer needed.
 * NOTE:	If LU_SUCCESS is returned, 'r_status' must be queried to
 *		determine the results of the Unix command.
 */

/*VARARGS*/
int
z_ExecCmdList(int *r_status, char **r_results,
	char *a_inputFile, char *a_cmd, ...)
{
	va_list		ap;		/* references variable argument list */
	char		*array[MAX_EXEC_CMD_ARGS+1];
	int		argno = 0;

	/*
	 * Create argument array for exec system call
	 */

	bzero(array, sizeof (array));

	va_start(ap, a_cmd);	/* Begin variable argument processing */

	for (argno = 0; argno < MAX_EXEC_CMD_ARGS; argno++) {
		array[argno] = va_arg(ap, char *);
		if (array[argno] == NULL) {
			break;
		}
	}

	va_end(ap);
	return (z_ExecCmdArray(r_status, r_results, a_inputFile,
	    a_cmd, array));
}
