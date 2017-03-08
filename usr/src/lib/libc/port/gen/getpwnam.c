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
 */

/*	Copyright (c) 1988 AT&T	*/
/*	All Rights Reserved  	*/

#pragma weak _getpwnam = getpwnam
#pragma weak _getpwuid = getpwuid

#include "lint.h"
#include <sys/types.h>
#include <pwd.h>
#include <nss_dbdefs.h>
#include <stdio.h>
#include "tsd.h"

void _nss_initf_passwd(nss_db_params_t *p);
void _nss_XbyY_fgets(FILE *, nss_XbyY_args_t *);
int str2passwd(const char *, int, void *, char *, int);

static struct passwd _pw_passwd;
static char _pw_buf[NSS_BUFLEN_PASSWD];
static DEFINE_NSS_DB_ROOT(db_root);
static DEFINE_NSS_GETENT(context);

struct passwd *
getpwuid(uid_t uid)
{
	struct passwd *result;
	int ret;
	if ((ret = getpwuid_r(uid, &_pw_passwd, _pw_buf, sizeof (_pw_buf),
	    &result)) != 0)
		errno = ret;
	return (result);
}

struct passwd *
getpwnam(const char *nam)
{
	struct passwd *result;
	int ret;
	if ((ret = getpwnam_r(nam, &_pw_passwd, _pw_buf, sizeof (_pw_buf),
	    &result)) != 0)
		errno = ret;
	return (result);
}

struct passwd *
getpwent(void)
{
	nss_XbyY_args_t arg;
	NSS_XbyY_INIT(&arg, &_pw_passwd, _pw_buf, sizeof(_pw_buf), str2passwd);
	(void) nss_getent(&db_root, _nss_initf_passwd, &context, &arg);
	return (NSS_XbyY_FINI(&arg));
}

struct passwd *
fgetpwent(FILE *f)
{
	nss_XbyY_args_t arg;
	NSS_XbyY_INIT(&arg, &_pw_passwd, _pw_buf, sizeof(_pw_buf), str2passwd);
	(void) _nss_XbyY_fgets(f, &arg);
	return (NSS_XbyY_FINI(&arg));
}
