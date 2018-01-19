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
 * Copyright (c) 2004, 2010, Oracle and/or its affiliates. All rights reserved.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <strings.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stropts.h>
#include <time.h>
#include <sys/param.h>
#include <sys/vfstab.h>
#include <dirent.h>
#include "libdevinfo.h"
#include "device_info.h"
#include <regex.h>

#define	isnewline(ch)	((ch) == '\n' || (ch) == '\r' || (ch) == '\f')
#define	isnamechar(ch)  (isalpha(ch) || isdigit(ch) || (ch) == '_' ||\
	(ch) == '-')
#define	MAX_TOKEN_SIZE	1024
#define	BUFSIZE		1024
#define	STRVAL(s)	((s) ? (s) : "NULL")

#define	SCSI_VHCI_CONF		"/kernel/drv/scsi_vhci.conf"
#define	QLC_CONF		"/kernel/drv/qlc.conf"
#define	FP_CONF			"/kernel/drv/fp.conf"
#define	DRIVER_CLASSES		"/etc/driver_classes"
#define	FP_AT			"fp@"
#define	VHCI_CTL_NODE		"/devices/scsi_vhci:devctl"
#define	SLASH_DEVICES		"/devices"
#define	SLASH_DEVICES_SLASH	"/devices/"
#define	SLASH_FP_AT		"/fp@"
#define	SLASH_SCSI_VHCI		"/scsi_vhci"
#define	META_DEV		"/dev/md/dsk/"
#define	SLASH_DEV_SLASH		"/dev/"

/*
 * Macros to produce a quoted string containing the value of a
 * preprocessor macro. For example, if SIZE is defined to be 256,
 * VAL2STR(SIZE) is "256". This is used to construct format
 * strings for scanf-family functions below.
 */
#define	QUOTE(x)	#x
#define	VAL2STR(x)	QUOTE(x)

typedef enum {
	CLIENT_TYPE_UNKNOWN,
	CLIENT_TYPE_PHCI,
	CLIENT_TYPE_VHCI
} client_type_t;

typedef enum {
	T_EQUALS,
	T_AMPERSAND,
	T_BIT_OR,
	T_STAR,
	T_POUND,
	T_COLON,
	T_SEMICOLON,
	T_COMMA,
	T_SLASH,
	T_WHITE_SPACE,
	T_NEWLINE,
	T_EOF,
	T_STRING,
	T_HEXVAL,
	T_DECVAL,
	T_NAME
} token_t;

typedef enum {
	begin, parent, drvname, drvclass, prop,
	parent_equals, name_equals, drvclass_equals,
	parent_equals_string, name_equals_string,
	drvclass_equals_string,
	prop_equals, prop_equals_string, prop_equals_integer,
	prop_equals_string_comma, prop_equals_integer_comma
} conf_state_t;

/* structure to hold entries with mpxio-disable property in driver.conf file */
struct conf_entry {
	char *name;
	char *parent;
	char *class;
	char *unit_address;
	int port;
	int mpxio_disable;
	struct conf_entry *next;
};

struct conf_file {
	char *filename;
	FILE *fp;
	int linenum;
};

static char *tok_err = "Unexpected token '%s'\n";


/* #define	DEBUG */

#ifdef DEBUG

int devfsmap_debug = 0;
/* /var/run is not mounted at install time. Therefore use /tmp */
char *devfsmap_logfile = "/tmp/devfsmap.log";
static FILE *logfp;
#define	logdmsg(args)	log_debug_msg args
static void vlog_debug_msg(char *, va_list);
static void log_debug_msg(char *, ...);
#ifdef __sparc
static void log_confent_list(char *, struct conf_entry *, int);
static void log_pathlist(char **);
#endif /* __sparc */

#else /* DEBUG */
#define	logdmsg(args)	/* nothing */
#endif /* DEBUG */


/*
 * Leave NEWLINE as the next character.
 */
static void
find_eol(FILE *fp)
{
	int ch;

	while ((ch = getc(fp)) != EOF) {
		if (isnewline(ch)) {
			(void) ungetc(ch, fp);
			break;
		}
	}
}

/* ignore parsing errors */
/*ARGSUSED*/
static void
file_err(struct conf_file *filep, char *fmt, ...)
{
#ifdef DEBUG
	va_list ap;

	va_start(ap, fmt);
	log_debug_msg("WARNING: %s line # %d: ",
	    filep->filename, filep->linenum);
	vlog_debug_msg(fmt, ap);
	va_end(ap);
#endif /* DEBUG */
}

/* return the next token from the given driver.conf file, or -1 on error */
static token_t
lex(struct conf_file *filep, char *val, size_t size)
{
	char	*cp;
	int	ch, oval, badquote;
	size_t	remain;
	token_t token;
	FILE	*fp = filep->fp;

	if (size < 2)
		return (-1);

	cp = val;
	while ((ch = getc(fp)) == ' ' || ch == '\t')
		;

	remain = size - 1;
	*cp++ = (char)ch;
	switch (ch) {
	case '=':
		token = T_EQUALS;
		break;
	case '&':
		token = T_AMPERSAND;
		break;
	case '|':
		token = T_BIT_OR;
		break;
	case '*':
		token = T_STAR;
		break;
	case '#':
		token = T_POUND;
		break;
	case ':':
		token = T_COLON;
		break;
	case ';':
		token = T_SEMICOLON;
		break;
	case ',':
		token = T_COMMA;
		break;
	case '/':
		token = T_SLASH;
		break;
	case ' ':
	case '\t':
	case '\f':
		while ((ch = getc(fp)) == ' ' ||
		    ch == '\t' || ch == '\f') {
			if (--remain == 0) {
				*cp = '\0';
				return (-1);
			}
			*cp++ = (char)ch;
		}
		(void) ungetc(ch, fp);
		token = T_WHITE_SPACE;
		break;
	case '\n':
	case '\r':
		token = T_NEWLINE;
		break;
	case '"':
		remain++;
		cp--;
		badquote = 0;
		while (!badquote && (ch  = getc(fp)) != '"') {
			switch (ch) {
			case '\n':
			case EOF:
				file_err(filep, "Missing \"\n");
				remain = size - 1;
				cp = val;
				*cp++ = '\n';
				badquote = 1;
				/* since we consumed the newline/EOF */
				(void) ungetc(ch, fp);
				break;

			case '\\':
				if (--remain == 0) {
					*cp = '\0';
					return (-1);
				}
				ch = (char)getc(fp);
				if (!isdigit(ch)) {
					/* escape the character */
					*cp++ = (char)ch;
					break;
				}
				oval = 0;
				while (ch >= '0' && ch <= '7') {
					ch -= '0';
					oval = (oval << 3) + ch;
					ch = (char)getc(fp);
				}
				(void) ungetc(ch, fp);
				/* check for character overflow? */
				if (oval > 127) {
					file_err(filep,
					    "Character "
					    "overflow detected.\n");
				}
				*cp++ = (char)oval;
				break;
			default:
				if (--remain == 0) {
					*cp = '\0';
					return (-1);
				}
				*cp++ = (char)ch;
				break;
			}
		}
		token = T_STRING;
		break;

	case EOF:
		token = T_EOF;
		break;

	default:
		/*
		 * detect a lone '-' (including at the end of a line), and
		 * identify it as a 'name'
		 */
		if (ch == '-') {
			if (--remain == 0) {
				*cp = '\0';
				return (-1);
			}
			*cp++ = (char)(ch = getc(fp));
			if (ch == ' ' || ch == '\t' || ch == '\n') {
				(void) ungetc(ch, fp);
				remain++;
				cp--;
				token = T_NAME;
				break;
			}
		} else if (ch == '~' || ch == '-') {
			if (--remain == 0) {
				*cp = '\0';
				return (-1);
			}
			*cp++ = (char)(ch = getc(fp));
		}


		if (isdigit(ch)) {
			if (ch == '0') {
				if ((ch = getc(fp)) == 'x') {
					if (--remain == 0) {
						*cp = '\0';
						return (-1);
					}
					*cp++ = (char)ch;
					ch = getc(fp);
					while (isxdigit(ch)) {
						if (--remain == 0) {
							*cp = '\0';
							return (-1);
						}
						*cp++ = (char)ch;
						ch = getc(fp);
					}
					(void) ungetc(ch, fp);
					token = T_HEXVAL;
				} else {
					goto digit;
				}
			} else {
				ch = getc(fp);
digit:
				while (isdigit(ch)) {
					if (--remain == 0) {
						*cp = '\0';
						return (-1);
					}
					*cp++ = (char)ch;
					ch = getc(fp);
				}
				(void) ungetc(ch, fp);
				token = T_DECVAL;
			}
		} else if (isalpha(ch) || ch == '\\') {
			if (ch != '\\') {
				ch = getc(fp);
			} else {
				/*
				 * if the character was a backslash,
				 * back up so we can overwrite it with
				 * the next (i.e. escaped) character.
				 */
				remain++;
				cp--;
			}
			while (isnamechar(ch) || ch == '\\') {
				if (ch == '\\')
					ch = getc(fp);
				if (--remain == 0) {
					*cp = '\0';
					return (-1);
				}
				*cp++ = (char)ch;
				ch = getc(fp);
			}
			(void) ungetc(ch, fp);
			token = T_NAME;
		} else {
			return (-1);
		}
		break;
	}

	*cp = '\0';

	return (token);
}


static int
devlink_callback(di_devlink_t devlink, void *argp)
{
	const char *link;

	if ((link = di_devlink_path(devlink)) != NULL)
		(void) strlcpy((char *)argp, link, MAXPATHLEN);

	return (DI_WALK_CONTINUE);
}

/*
 * Get the /dev name in the install environment corresponding to physpath.
 *
 * physpath	/devices path in the install environment without the /devices
 * 		prefix.
 * buf		caller supplied buffer where the /dev name is placed on return
 * bufsz	length of the buffer
 *
 * Returns	strlen of the /dev name on success, -1 on failure.
 */
static int
get_install_devlink(char *physpath, char *buf, size_t bufsz)
{
	di_devlink_handle_t devlink_hdl;
	char devname[MAXPATHLEN];
	int tries = 0;
	int sleeptime = 2; /* number of seconds to sleep between retries */
	int maxtries = 10; /* maximum number of tries */

	logdmsg(("get_install_devlink: physpath = %s\n", physpath));

	/*
	 * devlink_db sync happens after MINOR_FINI_TIMEOUT_DEFAULT secs
	 * after dev link creation. So wait for minimum that amout of time.
	 */

retry:
	(void) sleep(sleeptime);

	if ((devlink_hdl = di_devlink_init(NULL, 0)) == NULL) {
		logdmsg(("get_install_devlink: di_devlink_init() failed: %s\n",
		    strerror(errno)));
		return (-1);
	}

	devname[0] = '\0';
	if (di_devlink_walk(devlink_hdl, NULL, physpath, DI_PRIMARY_LINK,
	    devname, devlink_callback) == 0) {
		if (devname[0] == '\0' && tries < maxtries) {
			tries++;
			(void) di_devlink_fini(&devlink_hdl);
			goto retry;
		} else if (devname[0] == '\0') {
			logdmsg(("get_install_devlink: di_devlink_walk"
			    " failed: %s\n", strerror(errno)));
			(void) di_devlink_fini(&devlink_hdl);
			return (-1);
		}
	} else {
		logdmsg(("get_install_devlink: di_devlink_walk failed: %s\n",
		    strerror(errno)));
		(void) di_devlink_fini(&devlink_hdl);
		return (-1);
	}

	(void) di_devlink_fini(&devlink_hdl);

	logdmsg(("get_install_devlink: devlink = %s\n", devname));
	return (strlcpy(buf, devname, bufsz));
}

/*
 * Get the /dev name in the target environment corresponding to physpath.
 *
 * rootdir	root directory of the target environment
 * physpath	/devices path in the target environment without the /devices
 * 		prefix.
 * buf		caller supplied buffer where the /dev name is placed on return
 * bufsz	length of the buffer
 *
 * Returns	strlen of the /dev name on success, -1 on failure.
 */
static int
get_target_devlink(char *rootdir, char *physpath, char *buf, size_t bufsz)
{
	char *p;
	int linksize;
	DIR *dirp;
	struct dirent *direntry;
	char dirpath[MAXPATHLEN];
	char devname[MAXPATHLEN];
	char physdev[MAXPATHLEN];

	logdmsg(("get_target_devlink: rootdir = %s, physpath = %s\n",
	    rootdir, physpath));

	if ((p = strrchr(physpath, '/')) == NULL)
		return (-1);

	if (strstr(p, ",raw") != NULL) {
		(void) snprintf(dirpath, MAXPATHLEN, "%s/dev/rdsk", rootdir);
	} else {
		(void) snprintf(dirpath, MAXPATHLEN, "%s/dev/dsk", rootdir);
	}

	if ((dirp = opendir(dirpath)) == NULL)
		return (-1);

	while ((direntry = readdir(dirp)) != NULL) {
		if (strcmp(direntry->d_name, ".") == 0 ||
		    strcmp(direntry->d_name, "..") == 0)
			continue;

		(void) snprintf(devname, MAXPATHLEN, "%s/%s",
		    dirpath, direntry->d_name);

		if ((linksize = readlink(devname, physdev, MAXPATHLEN)) > 0 &&
		    linksize < (MAXPATHLEN - 1)) {
			physdev[linksize] = '\0';
			if ((p = strstr(physdev, SLASH_DEVICES_SLASH)) !=
			    NULL && strcmp(p + sizeof (SLASH_DEVICES) - 1,
			    physpath) == 0) {
				(void) closedir(dirp);
				logdmsg(("get_target_devlink: devlink = %s\n",
				    devname + strlen(rootdir)));
				return (strlcpy(buf, devname + strlen(rootdir),
				    bufsz));
			}
		}
	}

	(void) closedir(dirp);
	return (-1);
}

/*
 * Convert device name to physpath.
 *
 * rootdir	root directory
 * devname	a /dev name or /devices name under rootdir
 * physpath	caller supplied buffer where the /devices path will be placed
 *		on return (without the /devices prefix).
 * physpathlen	length of the physpath buffer
 *
 * Returns 0 on success, -1 on failure.
 */
static int
devname2physpath(char *rootdir, char *devname, char *physpath, int physpathlen)
{
	int linksize;
	char *p;
	char devlink[MAXPATHLEN];
	char tmpphyspath[MAXPATHLEN];

	logdmsg(("devname2physpath: rootdir = %s, devname = %s\n",
	    rootdir, devname));

	if (strncmp(devname, SLASH_DEVICES_SLASH,
	    sizeof (SLASH_DEVICES_SLASH) - 1) != 0) {
		if (*rootdir == '\0')
			linksize = readlink(devname, tmpphyspath, MAXPATHLEN);
		else {
			(void) snprintf(devlink, MAXPATHLEN, "%s%s",
			    rootdir, devname);
			linksize = readlink(devlink, tmpphyspath, MAXPATHLEN);
		}
		if (linksize > 0 && linksize < (MAXPATHLEN - 1)) {
			tmpphyspath[linksize] = '\0';
			if ((p = strstr(tmpphyspath, SLASH_DEVICES_SLASH))
			    == NULL)
				return (-1);
		} else
			return (-1);
	} else
		p = devname;

	(void) strlcpy(physpath, p + sizeof (SLASH_DEVICES) - 1, physpathlen);
	logdmsg(("devname2physpath: physpath = %s\n", physpath));
	return (0);
}

/*
 * Map a device name (devname) from the target environment to the
 * install environment.
 *
 * rootdir	root directory of the target environment
 * devname	/dev or /devices name under the target environment
 * buf		caller supplied buffer where the mapped /dev name is placed
 *		on return
 * bufsz	length of the buffer
 *
 * Returns	strlen of the mapped /dev name on success, -1 on failure.
 */
int
devfs_target2install(const char *rootdir, const char *devname, char *buf,
    size_t bufsz)
{
	char physpath[MAXPATHLEN];

	logdmsg(("devfs_target2install: rootdir = %s, devname = %s\n",
	    STRVAL(rootdir), STRVAL(devname)));

	if (rootdir == NULL || devname == NULL || buf == NULL || bufsz == 0)
		return (-1);

	if (strcmp(rootdir, "/") == 0)
		rootdir = "";

	if (devname2physpath((char *)rootdir, (char *)devname, physpath,
	    MAXPATHLEN) != 0)
		return (-1);


	return (get_install_devlink(physpath, buf, bufsz));
}

/*
 * Map a device name (devname) from the install environment to the target
 * environment.
 *
 * rootdir	root directory of the target environment
 * devname	/dev or /devices name under the install environment
 * buf		caller supplied buffer where the mapped /dev name is placed
 *		on return
 * bufsz	length of the buffer
 *
 * Returns	strlen of the mapped /dev name on success, -1 on failure.
 */
int
devfs_install2target(const char *rootdir, const char *devname, char *buf,
    size_t bufsz)
{
	char physpath[MAXPATHLEN];

	logdmsg(("devfs_install2target: rootdir = %s, devname = %s\n",
	    STRVAL(rootdir), STRVAL(devname)));

	if (rootdir == NULL || devname == NULL || buf == NULL || bufsz == 0)
		return (-1);

	if (strcmp(rootdir, "/") == 0)
		rootdir = "";

	if (devname2physpath("", (char *)devname, physpath, MAXPATHLEN) != 0)
		return (-1);


	return (get_target_devlink((char *)rootdir, physpath, buf, bufsz));
}

/*
 * A parser for /etc/path_to_inst.
 * The user-supplied callback is called once for each entry in the file.
 * Returns 0 on success, ENOMEM/ENOENT/EINVAL on error.
 * Callback may return DI_WALK_TERMINATE to terminate the walk,
 * otherwise DI_WALK_CONTINUE.
 */
int
devfs_parse_binding_file(const char *binding_file,
	int (*callback)(void *, const char *, int,
	    const char *), void *cb_arg)
{
	token_t token;
	struct conf_file file;
	char tokval[MAX_TOKEN_SIZE];
	enum { STATE_RESET, STATE_DEVPATH, STATE_INSTVAL } state;
	char *devpath;
	char *bindname;
	int instval = 0;
	int rv;

	if ((devpath = calloc(1, MAXPATHLEN)) == NULL)
		return (ENOMEM);
	if ((bindname = calloc(1, MAX_TOKEN_SIZE)) == NULL) {
		free(devpath);
		return (ENOMEM);
	}

	if ((file.fp = fopen(binding_file, "r")) == NULL) {
		free(devpath);
		free(bindname);
		return (errno);
	}

	file.filename = (char *)binding_file;
	file.linenum = 1;

	state = STATE_RESET;
	while ((token = lex(&file, tokval, MAX_TOKEN_SIZE)) != T_EOF) {
		switch (token) {
		case T_POUND:
			/*
			 * Skip comments.
			 */
			find_eol(file.fp);
			break;
		case T_NAME:
		case T_STRING:
			switch (state) {
			case STATE_RESET:
				if (strlcpy(devpath, tokval,
				    MAXPATHLEN) >= MAXPATHLEN)
					goto err;
				state = STATE_DEVPATH;
				break;
			case STATE_INSTVAL:
				if (strlcpy(bindname, tokval,
				    MAX_TOKEN_SIZE) >= MAX_TOKEN_SIZE)
					goto err;
				rv = callback(cb_arg,
				    devpath, instval, bindname);
				if (rv == DI_WALK_TERMINATE)
					goto done;
				if (rv != DI_WALK_CONTINUE)
					goto err;
				state = STATE_RESET;
				break;
			default:
				file_err(&file, tok_err, tokval);
				state = STATE_RESET;
				break;
			}
			break;
		case T_DECVAL:
		case T_HEXVAL:
			switch (state) {
			case STATE_DEVPATH:
				instval = (int)strtol(tokval, NULL, 0);
				state = STATE_INSTVAL;
				break;
			default:
				file_err(&file, tok_err, tokval);
				state = STATE_RESET;
				break;
			}
			break;
		case T_NEWLINE:
			file.linenum++;
			state = STATE_RESET;
			break;
		default:
			file_err(&file, tok_err, tokval);
			state = STATE_RESET;
			break;
		}
	}

done:
	(void) fclose(file.fp);
	free(devpath);
	free(bindname);
	return (0);

err:
	(void) fclose(file.fp);
	free(devpath);
	free(bindname);
	return (EINVAL);
}

/*
 * Walk the minor nodes of all children below the specified device
 * by calling the provided callback with the path to each minor.
 */
static int
devfs_walk_children_minors(const char *device_path, struct stat *st,
    int (*callback)(void *, const char *), void *cb_arg, int *terminate)
{
	DIR *dir;
	struct dirent *dp;
	char *minor_path = NULL;
	int need_close = 0;
	int rv;

	if ((minor_path = calloc(1, MAXPATHLEN)) == NULL)
		return (ENOMEM);

	if ((dir = opendir(device_path)) == NULL) {
		rv = ENOENT;
		goto err;
	}
	need_close = 1;

	while ((dp = readdir(dir)) != NULL) {
		if ((strcmp(dp->d_name, ".") == 0) ||
		    (strcmp(dp->d_name, "..") == 0))
			continue;
		(void) snprintf(minor_path, MAXPATHLEN,
		    "%s/%s", device_path, dp->d_name);
		if (stat(minor_path, st) == -1)
			continue;
		if (S_ISDIR(st->st_mode)) {
			rv = devfs_walk_children_minors(
			    (const char *)minor_path, st,
			    callback, cb_arg, terminate);
			if (rv != 0)
				goto err;
			if (*terminate)
				break;
		} else {
			rv = callback(cb_arg, minor_path);
			if (rv == DI_WALK_TERMINATE) {
				*terminate = 1;
				break;
			}
			if (rv != DI_WALK_CONTINUE) {
				rv = EINVAL;
				goto err;
			}
		}
	}

	rv = 0;
err:
	if (need_close)
		(void) closedir(dir);
	free(minor_path);
	return (rv);
}

/*
 * Return the path to each minor node for a device by
 * calling the provided callback.
 */
static int
devfs_walk_device_minors(const char *device_path, struct stat *st,
    int (*callback)(void *, const char *), void *cb_arg, int *terminate)
{
	char *minor_path;
	char *devpath;
	char *expr;
	regex_t regex;
	int need_regfree = 0;
	int need_close = 0;
	DIR *dir;
	struct dirent *dp;
	int rv;
	char *p;

	minor_path = calloc(1, MAXPATHLEN);
	devpath = calloc(1, MAXPATHLEN);
	expr = calloc(1, MAXNAMELEN);
	if (devpath == NULL || expr == NULL || minor_path == NULL) {
		rv = ENOMEM;
		goto err;
	}

	rv = EINVAL;
	if (strlcpy(devpath, device_path, MAXPATHLEN) >= MAXPATHLEN)
		goto err;
	if ((p = strrchr(devpath, '/')) == NULL)
		goto err;
	*p++ = 0;
	if (strlen(p) == 0)
		goto err;
	if (snprintf(expr, MAXNAMELEN, "%s:.*", p) >= MAXNAMELEN)
		goto err;
	if (regcomp(&regex, expr, REG_EXTENDED) != 0)
		goto err;
	need_regfree = 1;

	if ((dir = opendir(devpath)) == NULL) {
		rv = ENOENT;
		goto err;
	}
	need_close = 1;

	while ((dp = readdir(dir)) != NULL) {
		if ((strcmp(dp->d_name, ".") == 0) ||
		    (strcmp(dp->d_name, "..") == 0))
			continue;
		(void) snprintf(minor_path, MAXPATHLEN,
		    "%s/%s", devpath, dp->d_name);
		if (stat(minor_path, st) == -1)
			continue;
		if ((S_ISBLK(st->st_mode) || S_ISCHR(st->st_mode)) &&
		    regexec(&regex, dp->d_name, 0, NULL, 0) == 0) {
			rv = callback(cb_arg, minor_path);
			if (rv == DI_WALK_TERMINATE) {
				*terminate = 1;
				break;
			}
			if (rv != DI_WALK_CONTINUE) {
				rv = EINVAL;
				goto err;
			}
		}
	}

	rv = 0;
err:
	if (need_close)
		(void) closedir(dir);
	if (need_regfree)
		regfree(&regex);
	free(devpath);
	free(minor_path);
	free(expr);
	return (rv);
}

/*
 * Perform a walk of all minor nodes for the specified device,
 * and minor nodes below the device.
 */
int
devfs_walk_minor_nodes(const char *device_path,
	int (*callback)(void *, const char *), void *cb_arg)
{
	struct stat stbuf;
	int rv;
	int terminate = 0;

	rv = devfs_walk_device_minors(device_path,
	    &stbuf, callback, cb_arg, &terminate);
	if (rv == 0 && terminate == 0) {
		rv = devfs_walk_children_minors(device_path,
		    &stbuf, callback, cb_arg, &terminate);
	}
	return (rv);
}

#ifdef DEBUG

static void
vlog_debug_msg(char *fmt, va_list ap)
{
	time_t clock;
	struct tm t;

	if (!devfsmap_debug)
		return;

	if (logfp == NULL) {
		if (*devfsmap_logfile != '\0') {
			logfp = fopen(devfsmap_logfile, "a");
			if (logfp)
				(void) fprintf(logfp, "\nNew Log:\n");
		}

		if (logfp == NULL)
			logfp = stdout;
	}

	clock = time(NULL);
	(void) localtime_r(&clock, &t);
	(void) fprintf(logfp, "%02d:%02d:%02d ", t.tm_hour, t.tm_min,
	    t.tm_sec);
	(void) vfprintf(logfp, fmt, ap);
	(void) fflush(logfp);
}

static void
log_debug_msg(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vlog_debug_msg(fmt, ap);
	va_end(ap);
}

#ifdef __sparc

static char *
mpxio_disable_string(int mpxio_disable)
{
	if (mpxio_disable == 0)
		return ("no");
	else if (mpxio_disable == 1)
		return ("yes");
	else
		return ("not specified");
}

static void
log_confent_list(char *filename, struct conf_entry *confent_list,
    int global_mpxio_disable)
{
	struct conf_entry *confent;

	log_debug_msg("log_confent_list: filename = %s:\n", filename);
	if (global_mpxio_disable != -1)
		log_debug_msg("\tdriver global mpxio_disable = \"%s\"\n\n",
		    mpxio_disable_string(global_mpxio_disable));

	for (confent = confent_list; confent != NULL; confent = confent->next) {
		if (confent->name)
			log_debug_msg("\tname = %s\n", confent->name);
		if (confent->parent)
			log_debug_msg("\tparent = %s\n", confent->parent);
		if (confent->class)
			log_debug_msg("\tclass = %s\n", confent->class);
		if (confent->unit_address)
			log_debug_msg("\tunit_address = %s\n",
			    confent->unit_address);
		if (confent->port != -1)
			log_debug_msg("\tport = %d\n", confent->port);
		log_debug_msg("\tmpxio_disable = \"%s\"\n\n",
		    mpxio_disable_string(confent->mpxio_disable));
	}
}

static void
log_pathlist(char **pathlist)
{
	char **p;

	for (p = pathlist; *p != NULL; p++)
		log_debug_msg("\t%s\n", *p);
}

#endif /* __sparc */

#endif /* DEBUG */
