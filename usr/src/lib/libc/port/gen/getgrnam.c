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
/*	  All Rights Reserved  	*/

#pragma weak _getgrnam	= getgrnam
#pragma weak _getgrgid	= getgrgid

#include "lint.h"
#include <sys/types.h>
#include <grp.h>
#include <nss_dbdefs.h>
#include <stdio.h>
#include "tsd.h"

static struct group _gr_group;
static char _gr_buf[1024];

struct group *
getgrgid(gid_t gid)
{
	struct group *result;
	int ret;

	ret = getgrgid_r(gid, &_gr_group, _gr_buf, sizeof(_gr_buf), &result);
	if (ret != 0)
		errno = ret;
	return result;
}

struct group *
getgrnam(const char *nam)
{
	struct group *result;
	int ret;

	ret = getgrnam_r(nam, &_gr_group, _gr_buf, sizeof(_gr_buf), &result);
	if (ret != 0)
		errno = ret;
	return result;
}

void _nss_initf_group(nss_db_params_t *);
void _nss_XbyY_fgets(FILE *, nss_XbyY_args_t *);
static DEFINE_NSS_DB_ROOT(db_root);
static DEFINE_NSS_GETENT(context);
int str2group(const char *, int, void *, char *, int);

void
setgrent(void)
{
	nss_setent(&db_root, _nss_initf_group, &context);
}

void
endgrent(void)
{
	nss_endent(&db_root, _nss_initf_group, &context);
	nss_delete(&db_root);
}

struct group *
getgrent(void)
{
	nss_XbyY_args_t arg;
	char		*nam;

	NSS_XbyY_INIT(&arg, &_gr_group, _gr_buf, sizeof(_gr_buf), str2group);
	(void) nss_getent(&db_root, _nss_initf_group, &context, &arg);

	return NSS_XbyY_FINI(&arg);
}

struct group *
fgetgrent(FILE *f)
{
	nss_XbyY_args_t	arg;

	NSS_XbyY_INIT(&arg, &_gr_group, _gr_buf, sizeof(_gr_buf), str2group);
	_nss_XbyY_fgets(f, &arg);
	return NSS_XbyY_FINI(&arg);
}
