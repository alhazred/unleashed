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
 * Copyright (c) 2016 by Delphix. All rights reserved.
 */

#include "lint.h"
#include <sys/types.h>
#include <pwd.h>
#include <nss_dbdefs.h>
#include <stdio.h>
#include <synch.h>
#include <sys/param.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

/*
 * Compatibility aliases; Solaris/illumos default to the POSIX.1c Draft 6
 * versions of these functions and use __posix_*_r for the standard versions.
 * Our unadorned symbols are the standard versions, but we allow previously
 * compiled binaries that are using the POSIX version to work with these
 * aliases.
 */
#pragma weak __posix_getpwuid_r = getpwuid_r
#pragma weak __posix_getpwnam_r = getpwnam_r

int str2passwd(const char *, int, void *, char *, int);

static DEFINE_NSS_DB_ROOT(db_root);
static DEFINE_NSS_GETENT(context);

void
_nss_initf_passwd(nss_db_params_t *p)
{
	p->name	= NSS_DBNAM_PASSWD;
	p->default_config = NSS_DEFCONF_PASSWD;
}

#include <getxby_door.h>

int
getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize,
    struct passwd **result)
{
	nss_XbyY_args_t arg;

	if (!name) {
		errno = ERANGE;
		return (errno);
	}
	NSS_XbyY_INIT(&arg, pwd, buffer, bufsize, str2passwd);
	arg.key.name = name;
	(void) nss_search(&db_root, _nss_initf_passwd, NSS_DBOP_PASSWD_BYNAME,
	    &arg);
	*result = ((struct passwd *)NSS_XbyY_FINI(&arg));
	if (!*result)
		return (errno);
	return (0);
}


int
getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize,
    struct passwd **result)
{
	nss_XbyY_args_t arg;

	NSS_XbyY_INIT(&arg, pwd, buffer, bufsize, str2passwd);
	arg.key.uid = uid;
	(void) nss_search(&db_root, _nss_initf_passwd, NSS_DBOP_PASSWD_BYUID,
	    &arg);
	*result = ((struct passwd *)NSS_XbyY_FINI(&arg));
	if (!*result)
		return (errno);
	return (0);
}

struct passwd *
_uncached_getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
	int buflen)
{
	nss_XbyY_args_t arg;

	NSS_XbyY_INIT(&arg, pwd, buffer, buflen, str2passwd);
	arg.key.name = name;
	(void) nss_search(&db_root, _nss_initf_passwd, NSS_DBOP_PASSWD_BYNAME,
	    &arg);
	return ((struct passwd *)NSS_XbyY_FINI(&arg));
}

struct passwd *
_uncached_getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer,
	int buflen)
{
	nss_XbyY_args_t arg;

	NSS_XbyY_INIT(&arg, pwd, buffer, buflen, str2passwd);
	arg.key.uid = uid;
	(void) nss_search(&db_root, _nss_initf_passwd, NSS_DBOP_PASSWD_BYUID,
	    &arg);
	return ((struct passwd *)NSS_XbyY_FINI(&arg));
}

void
setpwent(void)
{
	nss_setent(&db_root, _nss_initf_passwd, &context);
}

void
endpwent(void)
{
	nss_endent(&db_root, _nss_initf_passwd, &context);
	nss_delete(&db_root);
}

static char *
gettok(char **nextpp)
{
	char	*p = *nextpp;
	char	*q = p;
	char	c;

	if (p == 0)
		return (0);

	while ((c = *q) != '\0' && c != ':')
		q++;

	if (c == '\0')
		*nextpp = 0;
	else {
		*q++ = '\0';
		*nextpp = q;
	}
	return (p);
}

/*
 * Return values: 0 = success, 1 = parse error, 2 = erange ...
 * The structure pointer passed in is a structure in the caller's space
 * wherein the field pointers would be set to areas in the buffer if
 * need be. instring and buffer should be separate areas.
 */
int
str2passwd(const char *instr, int lenstr, void *ent, char *buffer, int buflen)
{
	struct passwd	*passwd	= (struct passwd *)ent;
	char		*p, *next;
	ulong_t		tmp;

	if (lenstr + 1 > buflen)
		return (NSS_STR_PARSE_ERANGE);

	/*
	 * We copy the input string into the output buffer and
	 * operate on it in place.
	 */
	if (instr != buffer) {
		/* Overlapping buffer copies are OK */
		(void) memmove(buffer, instr, lenstr);
		buffer[lenstr] = '\0';
	}

	/* quick exit do not entry fill if not needed */
	if (ent == NULL)
		return (NSS_STR_PARSE_SUCCESS);

	next = buffer;

	passwd->pw_name = p = gettok(&next);		/* username */
	if (*p == '\0') {
		/* Empty username;  not allowed */
		return (NSS_STR_PARSE_PARSE);
	}

	passwd->pw_passwd = p = gettok(&next);		/* password */
	if (p == 0)
		return (NSS_STR_PARSE_PARSE);
	for (; *p != '\0';  p++) {			/* age */
		if (*p == ',') {
			*p++ = '\0';
			break;
		}
	}
	passwd->pw_age = p;

	p = next;					/* uid */
	if (p == 0 || *p == '\0')
		return (NSS_STR_PARSE_PARSE);

	/*
	 * strtoul returns unsigned long which is
	 * 8 bytes on a 64-bit system. We don't want
	 * to assign it directly to passwd->pw_uid
	 * which is 4 bytes or else we will end up
	 * truncating the value.
	 */
	errno = 0;
	tmp = strtoul(p, &next, 10);
	if (next == p || errno != 0) {
		/* uid field should be nonempty */
		/* also check errno from strtoul */
		return (NSS_STR_PARSE_PARSE);
	}
	/*
	 * The old code (in 2.0 through 2.5) would check
	 * for the uid being negative, or being greater
	 * than 60001 (the rfs limit).  If it met either of
	 * these conditions, the uid was translated to 60001.
	 *
	 * Now we just check for -1 (UINT32_MAX); anything else
	 * is administrative policy
	 */
	if (tmp >= UINT32_MAX)
		passwd->pw_uid = UID_NOBODY;
	else
		passwd->pw_uid = (uid_t)tmp;

	if (*next++ != ':')
		return (NSS_STR_PARSE_PARSE);
	p = next;					/* gid */
	if (p == 0 || *p == '\0')
		return (NSS_STR_PARSE_PARSE);

	errno = 0;
	tmp = strtoul(p, &next, 10);
	if (next == p || errno != 0) {
		/* gid field should be nonempty */
		/* also check errno from strtoul */
		return (NSS_STR_PARSE_PARSE);
	}
	/*
	 * gid should not be -1; anything else
	 * is administrative policy.
	 */
	if (tmp >= UINT32_MAX)
		passwd->pw_gid = GID_NOBODY;
	else
		passwd->pw_gid = (gid_t)tmp;

	if (*next++ != ':')
		return (NSS_STR_PARSE_PARSE);

	passwd->pw_gecos = passwd->pw_comment = p = gettok(&next);
	if (p == 0)
		return (NSS_STR_PARSE_PARSE);

	passwd->pw_dir = p = gettok(&next);
	if (p == 0)
		return (NSS_STR_PARSE_PARSE);

	passwd->pw_shell = p = gettok(&next);
	if (p == 0)
		return (NSS_STR_PARSE_PARSE);

	/* Better not be any more fields... */
	if (next == 0) {
		/* Successfully parsed and stored */
		return (NSS_STR_PARSE_SUCCESS);
	}
	return (NSS_STR_PARSE_PARSE);
}
