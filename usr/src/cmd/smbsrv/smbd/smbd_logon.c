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
 * Copyright (c) 2007, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2017 Nexenta Systems, Inc.  All rights reserved.
 */

#include <sys/types.h>
#include <errno.h>
#include <synch.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <syslog.h>
#include <fcntl.h>
#include <pwd.h>
#include <nss_dbdefs.h>
#include <sys/idmap.h>
#include "smbd.h"

/*
 * Invoked at user logon due to SmbSessionSetupX.
 */
smb_token_t *
smbd_user_auth_logon(smb_logon_t *user_info)
{
	smb_token_t *token = NULL;
	smb_logon_t tmp_user;
	char *p;
	char *buf = NULL;

	if (user_info->lg_username == NULL ||
	    user_info->lg_domain == NULL ||
	    user_info->lg_workstation == NULL) {
		return (NULL);
	}

	tmp_user = *user_info;
	if (tmp_user.lg_username[0] == '\0') {
		tmp_user.lg_flags |= SMB_ATF_ANON;
		tmp_user.lg_e_username = "anonymous";
	} else {
		tmp_user.lg_e_username = tmp_user.lg_username;
	}

	/* Handle user@domain format. */
	if (tmp_user.lg_domain[0] == '\0' &&
	    (p = strchr(tmp_user.lg_e_username, '@')) != NULL) {
		buf = strdup(tmp_user.lg_e_username);
		if (buf == NULL)
			goto errout;
		p = buf + (p - tmp_user.lg_e_username);
		*p = '\0';
		tmp_user.lg_e_domain = p + 1;
		tmp_user.lg_e_username = buf;
	} else {
		tmp_user.lg_e_domain = tmp_user.lg_domain;
	}

	token = smb_logon(&tmp_user);

	return (token);

errout:
	free(buf);
	smb_token_destroy(token);
	return NULL;
}
