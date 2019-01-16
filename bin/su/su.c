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
 * Copyright 2012 Milan Jurik. All rights reserved.
 * Copyright 2014 Nexenta Systems, Inc.
 */
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T */
/*	  All Rights Reserved	*/

/*	Copyright (c) 1987, 1988 Microsoft Corporation	*/
/*	  All Rights Reserved	*/

/*
 *	su [-] [name [arg ...]] change userid, `-' changes environment.
 *	If CONSOLE is defined, all successful attempts to su to uid 0
 *	are also logged there.
 */

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <crypt.h>
#include <deflt.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <locale.h>
#include <priv.h>
#include <pwd.h>
#include <shadow.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <user_attr.h>

#include <security/pam_appl.h>

#define	PATH	"/usr/bin:/usr/sbin:/sbin:/usr/gnu/bin"		/* path for users other than root */
#define	SUPATH	"/usr/sbin:/sbin:/usr/bin"			/* path for root */
#define	ELIM 128
#define	DEF_ATTEMPTS	3		/* attempts to change password */

#define	PW_FALSE	1		/* no password change */
#define	PW_TRUE		2		/* successful password change */
#define	PW_FAILED	3		/* failed password change */

/*
 * Intervals to sleep after failed su
 */
#define	SLEEPTIME	4

#define	DEFAULT_LOGIN "/etc/default/login"
#define	DEFFILE "/etc/default/su"


char *Path = PATH;
char *Supath = SUPATH;

/*
 * Variables to be propagated to "su -" environment
 */
static char *inherit[] = {
	"TZ", "TERM", "LANG", "LC_CTYPE",
	"LC_NUMERIC", "LC_TIME", "LC_COLLATE",
	"LC_MONETARY", "LC_MESSAGES", "LC_ALL", NULL };
static char *newenv[sizeof(inherit)/sizeof(inherit[0])];

enum messagemode { USAGE, ERR, WARN };
static void message(enum messagemode, char *, ...);

static char *alloc_vsprintf(const char *, va_list);
static char *tail(char *);

static void validate(char *, int *);
static int legalenvvar(char *);
static int su_conv(int, struct pam_message **, struct pam_response **, void *);
static void freeresponse(int, struct pam_response **response);
static struct pam_conv pam_conv = {su_conv, NULL};

static pam_handle_t	*pamh = NULL;	/* Authentication handle */
struct	passwd pwd;
char	pwdbuf[1024];			/* buffer for getpwnam_r() */
char	shell[] = "/bin/sh";		/* default shell */
char	safe_shell[] = "/bin/sh";	/* "fallback" shell */
char	su[PATH_MAX] = "su";		/* arg0 for exec of shprog */
char	logname[20] = "LOGNAME=";
char	*term;
char	*tzdef = NULL;
extern	char **environ;
char *ttyn;
char *username;					/* the invoker */
char	*myname;
int pam_flags = 0;

int
main(int argc, char **argv)
{
	char *nptr;
	char *pshell;
	int eflag = 0;
	int envidx = 0;
	uid_t uid;
	gid_t gid;
	char *dir, *shprog, *name;
	char *ptr;
	char *prog = argv[0];
	int sleeptime = SLEEPTIME;
	char **pam_env = 0;
	int flags = 0;
	int retcode;
	int idx = 0;
	int pw_change = PW_FALSE;
	struct passwd *result;

	(void) setlocale(LC_ALL, "");
#if !defined(TEXT_DOMAIN)	/* Should be defined by cc -D */
#define	TEXT_DOMAIN "SYS_TEST"	/* Use this only if it wasn't */
#endif
	(void) textdomain(TEXT_DOMAIN);

	myname = tail(argv[0]);

	if (argc > 1 && *argv[1] == '-') {
		/* Explicitly check for just `-' (no trailing chars) */
		if (strlen(argv[1]) == 1) {
			eflag++;	/* set eflag if `-' is specified */
			argv++;
			argc--;
		} else {
			message(USAGE,
			    gettext("Usage: %s [-] [ username [ arg ... ] ]"),
			    prog);
			exit(1);
		}
	}

	/*
	 * Determine specified userid, get their password file entry,
	 * and set variables to values in password file entry fields.
	 */
	if (argc > 1) {
		/*
		 * Usernames can't start with a `-', so we check for that to
		 * catch bad usage (like "su - -c ls").
		 */
		if (*argv[1] == '-') {
			message(USAGE,
			    gettext("Usage: %s [-] [ username [ arg ... ] ]"),
			    prog);
			exit(1);
		} else
			nptr = argv[1];	/* use valid command-line username */
	} else
		nptr = "root";		/* use default "root" username */

	openlog("su", LOG_CONS, LOG_AUTH);

	if (defopen(DEFAULT_LOGIN) == 0) {
		if ((ptr = defread("SLEEPTIME=")) != NULL) {
			const char *errstr;
			sleeptime = strtonum(ptr, 0, 60, &errstr);
			if (sleeptime == 0 && errstr != NULL) {
				syslog(LOG_ERR, "SLEEPTIME '%s' is %s; "
				    "ignored.", ptr, errstr);
				sleeptime = SLEEPTIME;
			}
		}

		if ((ptr = defread("PASSREQ=")) != NULL &&
		    strcasecmp("YES", ptr) == 0)
			pam_flags |= PAM_DISALLOW_NULL_AUTHTOK;
		if ((Path = defread("PATH=")) != NULL)
			Path = strdup(Path);
		if ((Supath = defread("SUPATH=")) != NULL)
			Supath = strdup(Supath);
		if ((tzdef = defread("TIMEZONE=")) != NULL)
			tzdef = strdup(tzdef);

		(void) defopen(NULL);
	}
	if ((ttyn = ttyname(0)) == NULL)
		if ((ttyn = ttyname(1)) == NULL)
			if ((ttyn = ttyname(2)) == NULL)
				ttyn = "/dev/???";
	if ((username = cuserid(NULL)) == NULL)
		username = "(null)";

	if (pam_start("su", nptr, &pam_conv, &pamh) != PAM_SUCCESS)
		exit(1);
	if (pam_set_item(pamh, PAM_TTY, ttyn) != PAM_SUCCESS)
		exit(1);
	getpwuid_r(getuid(), &pwd, pwdbuf, sizeof (pwdbuf), &result);
	if (!result ||
	    pam_set_item(pamh, PAM_AUSER, pwd.pw_name) != PAM_SUCCESS)
		exit(1);

	/*
	 * Ignore SIGQUIT and SIGINT
	 */
	(void) signal(SIGQUIT, SIG_IGN);
	(void) signal(SIGINT, SIG_IGN);

	/* call pam authenticate() to authenticate the user through PAM */
	getpwnam_r(nptr, &pwd, pwdbuf, sizeof (pwdbuf), &result);
	if (!result)
		retcode = PAM_USER_UNKNOWN;
	else if ((flags = (getuid() != 0)) != 0) {
		retcode = pam_authenticate(pamh, pam_flags);
	} else /* root user does not need to authenticate */
		retcode = PAM_SUCCESS;

	if (retcode != PAM_SUCCESS) {
		/*
		 * 1st step: log the error.
		 * 2nd step: sleep.
		 * 3rd step: print out message to user.
		 */
		switch (retcode) {
		case PAM_USER_UNKNOWN:
			closelog();
			(void) sleep(sleeptime);
			message(ERR, gettext("Unknown id: %s"), nptr);
			break;

		case PAM_AUTH_ERR:
			syslog(LOG_CRIT, "'su %s' failed for %s on %s",
			    pwd.pw_name, username, ttyn);
			closelog();
			(void) sleep(sleeptime);
			message(ERR, gettext("Sorry"));
			break;

		case PAM_CONV_ERR:
		default:
			syslog(LOG_CRIT, "'su %s' failed for %s on %s",
			    pwd.pw_name, username, ttyn);
			closelog();
			(void) sleep(sleeptime);
			message(ERR, gettext("Sorry"));
			break;
		}

		(void) signal(SIGQUIT, SIG_DFL);
		(void) signal(SIGINT, SIG_DFL);
		exit(1);
	}
	if (flags)
		validate(username, &pw_change);
	if (pam_setcred(pamh, PAM_REINITIALIZE_CRED) != PAM_SUCCESS) {
		message(ERR, gettext("unable to set credentials"));
		exit(2);
	}
	syslog(pwd.pw_uid == 0 ? LOG_NOTICE : LOG_INFO,
	    "'su %s' succeeded for %s on %s", pwd.pw_name, username, ttyn);
	closelog();
	(void) signal(SIGQUIT, SIG_DFL);
	(void) signal(SIGINT, SIG_DFL);

	uid = pwd.pw_uid;
	gid = pwd.pw_gid;
	dir = strdup(pwd.pw_dir);
	shprog = strdup(pwd.pw_shell);
	name = strdup(pwd.pw_name);

	/* set user and group ids to specified user */

	/* set the real (and effective) GID */
	if (setgid(gid) == -1) {
		message(ERR, gettext("Invalid GID"));
		exit(2);
	}
	/* Initialize the supplementary group access list. */
	if (!nptr)
		exit(2);
	if (initgroups(nptr, gid) == -1) {
		exit(2);
	}
	/* set the real (and effective) UID */
	if (setuid(uid) == -1) {
		message(ERR, gettext("Invalid UID"));
		exit(2);
	}

	/*
	 * If new user's shell field is neither NULL nor equal to /usr/bin/sh,
	 * set:
	 *
	 *	pshell = their shell
	 *	su = [-]last component of shell's pathname
	 *
	 * Otherwise, set the shell to /usr/bin/sh and set argv[0] to '[-]su'.
	 */
	if (shprog[0] != '\0' && strcmp(shell, shprog) != 0) {
		char *p;

		pshell = shprog;
		(void) strcpy(su, eflag ? "-" : "");

		if ((p = strrchr(pshell, '/')) != NULL)
			(void) strlcat(su, p + 1, sizeof (su));
		else
			(void) strlcat(su, pshell, sizeof (su));
	} else {
		pshell = shell;
		(void) strcpy(su, eflag ? "-su" : "su");
	}

	/*
	 * set environment variables for new user;
	 * arg0 for exec of shprog must now contain `-'
	 * so that environment of new user is given
	 */
	if (eflag) {
		int j;

		/*
		 * Fetch the relevant locale/TZ environment variables from
		 * the inherited environment.
		 */
		for (j = 0; inherit[j] != NULL; j++) {
			char *value;
			if ((value = getenv(inherit[j])) != NULL) {
				/*
				 * Skip over values beginning with '/' for
				 * security.
				 */
				if (value[0] == '/')  continue;

				if ((newenv[j] = strdup(value)) == NULL)
					err(4, "strdup");
			}
		}

		if (clearenv() != 0)
			err(1, "clearenv");
		if (tzdef && setenv("TZ", tzdef, 1) != 0)
			err(1, "setenv");
		/* if TZ is in newenv, it will override tznam */
		for (j = 0; inherit[j] != NULL; j++) {
			if(newenv[j]) {
				if (setenv(inherit[j], newenv[j], 1) != 0)
					err(1, "setenv");
				free(newenv[j]);
			}
		}

		if (strlen(dir) == 0) {
			(void) strcpy(dir, "/");
			message(WARN, gettext("No directory! Using HOME=/"));
		}
		if (chdir(dir) < 0)
			errx(1, "No directory!");

		if (setenv("HOME", dir, 1) != 0)
			err(1, "setenv");
		if (setenv("LOGNAME", name, 1) != 0)
			err(1, "setenv");
		if (setenv("SHELL", pshell, 1) != 0)
			err(1, "setenv");
		if (setenv("PATH", (uid == 0 ? Supath : Path), 1) != 0)
			err(1, "setenv");

		/*
		 * set the PAM environment variables -
		 * check for legal environment variables
		 */
		if ((pam_env = pam_getenvlist(pamh)) != 0) {
			while (pam_env[idx] != 0) {
				if (envidx + 2 < ELIM &&
				    legalenvvar(pam_env[idx])) {
					if (putenv(pam_env[idx]) != 0)
						err(1, "putenv");
				}
				idx++;
			}
		}
	} else {
		char **pp = environ, **qq, *p;

		while ((p = *pp) != NULL) {
			if (*p == 'L' && p[1] == 'D' && p[2] == '_') {
				for (qq = pp; (*qq = qq[1]) != NULL; qq++)
					;
				/* pp is not advanced */
			} else {
				pp++;
			}
		}
	}

	if (pamh)
		(void) pam_end(pamh, PAM_SUCCESS);

	/*
	 * if new user is root:
	 *	if eflag not set, change environment to that of root.
	 */
	if (uid == 0 && !eflag) {
		if (setenv("PATH", Supath, 1) != 0)
			err(1, "setenv");
	}

	/*
	 * Default for SIGCPU and SIGXFSZ.  Shells inherit
	 * signal disposition from parent.  And the
	 * shells should have default dispositions for these
	 * signals.
	 */
	(void) signal(SIGXCPU, SIG_DFL);
	(void) signal(SIGXFSZ, SIG_DFL);

	/*
	 * if additional arguments, exec shell program with array
	 * of pointers to arguments:
	 *	-> if shell = default, then su = [-]su
	 *	-> if shell != default, then su = [-]last component of
	 *						shell's pathname
	 *
	 * if no additional arguments, exec shell with arg0 of su
	 * where:
	 *	-> if shell = default, then su = [-]su
	 *	-> if shell != default, then su = [-]last component of
	 *						shell's pathname
	 */
	if (argc > 2) {
		argv[1] = su;
		(void) execv(pshell, &argv[1]);
	} else
		(void) execl(pshell, su, NULL);


	/*
	 * Try to clean up after an administrator who has made a mistake
	 * configuring root's shell; if root's shell is other than /bin/sh,
	 * try exec'ing /bin/sh instead.
	 */
	if ((uid == 0) && (strcmp(name, "root") == 0) &&
	    (strcmp(safe_shell, pshell) != 0)) {
		message(WARN,
		    gettext("No shell %s.  Trying fallback shell %s."),
		    pshell, safe_shell);

		if (eflag) {
			(void) strcpy(su, "-sh");
			(void) setenv("SHELL", safe_shell, 1);
		} else {
			(void) strcpy(su, "sh");
		}

		if (argc > 2) {
			argv[1] = su;
			(void) execv(safe_shell, &argv[1]);
		} else {
			(void) execl(safe_shell, su, NULL);
		}
		message(ERR, gettext("Couldn't exec fallback shell %s: %s"),
		    safe_shell, strerror(errno));
	} else {
		message(ERR, gettext("No shell"));
	}
	return (3);
}

/*
 * su_conv():
 *	This is the conv (conversation) function called from
 *	a PAM authentication module to print error messages
 *	or garner information from the user.
 */
/*ARGSUSED*/
static int
su_conv(int num_msg, struct pam_message **msg, struct pam_response **response,
    void *appdata_ptr)
{
	struct pam_message	*m;
	struct pam_response	*r;
	char			*temp;
	int			k;
	char			respbuf[PAM_MAX_RESP_SIZE];

	if (num_msg <= 0)
		return (PAM_CONV_ERR);

	*response = (struct pam_response *)calloc(num_msg,
	    sizeof (struct pam_response));
	if (*response == NULL)
		return (PAM_BUF_ERR);

	k = num_msg;
	m = *msg;
	r = *response;
	while (k--) {

		switch (m->msg_style) {

		case PAM_PROMPT_ECHO_OFF:
			errno = 0;
			temp = getpassphrase(m->msg);
			if (errno == EINTR)
				return (PAM_CONV_ERR);
			if (temp != NULL) {
				r->resp = strdup(temp);
				if (r->resp == NULL) {
					freeresponse(num_msg, response);
					return (PAM_BUF_ERR);
				}
			}
			break;

		case PAM_PROMPT_ECHO_ON:
			if (m->msg != NULL) {
				(void) fputs(m->msg, stdout);
			}

			(void) fgets(respbuf, sizeof (respbuf), stdin);
			temp = strchr(respbuf, '\n');
			if (temp != NULL)
				*temp = '\0';

			r->resp = strdup(respbuf);
			if (r->resp == NULL) {
				freeresponse(num_msg, response);
				return (PAM_BUF_ERR);
			}
			break;

		case PAM_ERROR_MSG:
			if (m->msg != NULL) {
				(void) fputs(m->msg, stderr);
				(void) fputs("\n", stderr);
			}
			break;

		case PAM_TEXT_INFO:
			if (m->msg != NULL) {
				(void) fputs(m->msg, stdout);
				(void) fputs("\n", stdout);
			}
			break;

		default:
			break;
		}
		m++;
		r++;
	}
	return (PAM_SUCCESS);
}

static void
freeresponse(int num_msg, struct pam_response **response)
{
	struct pam_response *r;
	int i;

	/* free responses */
	r = *response;
	for (i = 0; i < num_msg; i++, r++) {
		if (r->resp != NULL) {
			/* Zap it in case it's a password */
			(void) memset(r->resp, '\0', strlen(r->resp));
			free(r->resp);
		}
	}
	free(*response);
	*response = NULL;
}

/*
 * validate - Check that the account is valid for switching to.
 */
static void
validate(char *usernam, int *pw_change)
{
	int error;
	int tries;

	if ((error = pam_acct_mgmt(pamh, pam_flags)) != PAM_SUCCESS) {
		if (error == PAM_NEW_AUTHTOK_REQD) {
			tries = 0;
			message(ERR, gettext("Password for user "
			    "'%s' has expired"), pwd.pw_name);
			while ((error = pam_chauthtok(pamh,
			    PAM_CHANGE_EXPIRED_AUTHTOK)) != PAM_SUCCESS) {
				if ((error == PAM_AUTHTOK_ERR ||
				    error == PAM_TRY_AGAIN) &&
				    (tries++ < DEF_ATTEMPTS)) {
					continue;
				}
				message(ERR, gettext("Sorry"));
				syslog(LOG_CRIT, "'su %s' failed for %s on %s",
				    pwd.pw_name, usernam, ttyn);
				closelog();
				exit(1);
			}
			*pw_change = PW_TRUE;
			return;
		} else {
			message(ERR, gettext("Sorry"));
			syslog(LOG_CRIT, "'su %s' failed for %s on %s",
			    pwd.pw_name, usernam, ttyn);
			closelog();
			exit(3);
		}
	}
}

static char *illegal[] = {
	"SHELL=",
	"HOME=",
	"LOGNAME=",
	"CDPATH=",
	"IFS=",
	"PATH=",
	"TZ=",
	"HZ=",
	"TERM=",
	0
};

/*
 * legalenvvar - can PAM modules insert this environmental variable?
 */

static int
legalenvvar(char *s)
{
	register char **p;

	for (p = illegal; *p; p++)
		if (strncmp(s, *p, strlen(*p)) == 0)
			return (0);

	if (s[0] == 'L' && s[1] == 'D' && s[2] == '_')
		return (0);

	return (1);
}

/*
 * Report an error, either a fatal one, a warning, or a usage message,
 * depending on the mode parameter.
 */
/*ARGSUSED*/
static void
message(enum messagemode mode, char *fmt, ...)
{
	char *s;
	va_list v;

	va_start(v, fmt);
	s = alloc_vsprintf(fmt, v);
	va_end(v);

	if (mode == USAGE) {
		(void) fprintf(stderr, "%s\n", s);
	} else { /* ERR, WARN */
		(void) fprintf(stderr, "%s: %s\n", myname, s);
	}

	free(s);
}

/*
 * Return a pointer to the last path component of a.
 */
static char *
tail(char *a)
{
	char *p;

	p = strrchr(a, '/');
	if (p == NULL)
		p = a;
	else
		p++;	/* step over the '/' */

	return (p);
}

static char *
alloc_vsprintf(const char *fmt, va_list ap1)
{
	va_list ap2;
	int n;
	char buf[1];
	char *s;

	/*
	 * We need to scan the argument list twice.  Save off a copy
	 * of the argument list pointer(s) for the second pass.  Note that
	 * we are responsible for va_end'ing our copy.
	 */
	va_copy(ap2, ap1);

	/*
	 * vsnprintf into a dummy to get a length.  One might
	 * think that passing 0 as the length to snprintf would
	 * do what we want, but it's defined not to.
	 *
	 * Perhaps we should sprintf into a 100 character buffer
	 * or something like that, to avoid two calls to snprintf
	 * in most cases.
	 */
	n = vsnprintf(buf, sizeof (buf), fmt, ap2);
	va_end(ap2);

	/*
	 * Allocate an appropriately-sized buffer.
	 */
	s = malloc(n + 1);
	if (s == NULL) {
		perror("malloc");
		exit(4);
	}

	(void) vsnprintf(s, n+1, fmt, ap1);

	return (s);
}
