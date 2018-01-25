/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
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
 * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*
 * Copyright (c) 2013 RackTop Systems.
 */

/*LINTLIBRARY*/

#include	<sys/types.h>
#include	<stdio.h>
#include	<string.h>
#include	<userdefs.h>
#include	<user_attr.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<stddef.h>
#include	<time.h>
#include	<unistd.h>
#include	"userdisp.h"
#include	"funcs.h"
#include	"messages.h"

/* Print out a NL when the line gets too long */
#define	PRINTNL()	\
	if (outcount > 40) { \
		outcount = 0; \
		(void) fprintf(fptr, "\n"); \
	}

#define	SKIPWS(ptr)	while (*ptr && (*ptr == ' ' || *ptr == '\t')) ptr++

static char *dup_to_nl(char *);

static struct userdefs defaults = {
	DEFRID, DEFGROUP, DEFGNAME, DEFPARENT, DEFSKL,
	DEFSHL, DEFINACT, DEFEXPIRE, DEFAUTH, DEFPROF,
	DEFROLE, DEFPROJ, DEFPROJNAME, DEFLIMPRIV,
	DEFDFLTPRIV, DEFLOCK_AFTER_RETRIES
};

#define	INT	0
#define	STR	1
#define	PROJID	2

#define	DEFOFF(field)		offsetof(struct userdefs, field)
#define	FIELD(up, pe, type)	(*(type *)((char *)(up) + (pe)->off))

typedef struct parsent {
	const char *name;	/* deffoo= */
	const size_t nmsz;	/* length of def= string (excluding \0) */
	const int type;		/* type of entry */
	const ptrdiff_t off;	/* offset in userdefs structure */
	const char *uakey;	/* user_attr key, if defined */
} parsent_t;

static const parsent_t tab[] = {
	{ GIDSTR,	sizeof (GIDSTR) - 1,	INT,	DEFOFF(defgroup) },
	{ GNAMSTR,	sizeof (GNAMSTR) - 1,	STR,	DEFOFF(defgname) },
	{ PARSTR,	sizeof (PARSTR) - 1,	STR,	DEFOFF(defparent) },
	{ SKLSTR,	sizeof (SKLSTR) - 1,	STR,	DEFOFF(defskel) },
	{ SHELLSTR,	sizeof (SHELLSTR) - 1,	STR,	DEFOFF(defshell) },
	{ INACTSTR,	sizeof (INACTSTR) - 1,	INT,	DEFOFF(definact) },
	{ EXPIRESTR,	sizeof (EXPIRESTR) - 1,	STR,	DEFOFF(defexpire) },
	{ AUTHSTR,	sizeof (AUTHSTR) - 1,	STR,	DEFOFF(defauth),
		USERATTR_AUTHS_KW },
	{ ROLESTR,	sizeof (ROLESTR) - 1,	STR,	DEFOFF(defrole),
		USERATTR_ROLES_KW },
	{ PROFSTR,	sizeof (PROFSTR) - 1,	STR,	DEFOFF(defprof),
		USERATTR_PROFILES_KW },
	{ PROJSTR,	sizeof (PROJSTR) - 1,	PROJID,	DEFOFF(defproj) },
	{ PROJNMSTR,	sizeof (PROJNMSTR) - 1,	STR,	DEFOFF(defprojname) },
	{ LIMPRSTR,	sizeof (LIMPRSTR) - 1,	STR,	DEFOFF(deflimpriv),
		USERATTR_LIMPRIV_KW },
	{ DFLTPRSTR,	sizeof (DFLTPRSTR) - 1,	STR,	DEFOFF(defdfltpriv),
		USERATTR_DFLTPRIV_KW },
	{ LOCK_AFTER_RETRIESSTR,	sizeof (LOCK_AFTER_RETRIESSTR) - 1,
		STR,	DEFOFF(deflock_after_retries),
		USERATTR_LOCK_AFTER_RETRIES_KW },
};

#define	NDEF	(sizeof (tab) / sizeof (parsent_t))

static const parsent_t *
scan(char **start_p)
{
	static int ind = NDEF - 1;
	char *cur_p = *start_p;
	int lastind = ind;

	if (!*cur_p || *cur_p == '\n' || *cur_p == '#')
		return (NULL);

	/*
	 * The magic in this loop is remembering the last index when
	 * reentering the function; the entries above are also used to
	 * order the output to the default file.
	 */
	do {
		ind++;
		ind %= NDEF;

		if (strncmp(cur_p, tab[ind].name, tab[ind].nmsz) == 0) {
			*start_p = cur_p + tab[ind].nmsz;
			return (&tab[ind]);
		}
	} while (ind != lastind);

	return (NULL);
}

/*
 * getusrdef - returns default values of values in userdefs.h.
 */

struct userdefs *
getusrdef(char *usertype)
{
	return (&defaults);
}

static char *
dup_to_nl(char *from)
{
	char *res = strdup(from);

	char *p = strchr(res, '\n');
	if (p)
		*p = '\0';

	return (res);
}

void
dispusrdef(FILE *fptr, unsigned flags, char *usertype)
{
	struct userdefs *deflts = getusrdef(usertype);
	int outcount = 0;

	/* Print out values */

	if (flags & D_GROUP) {
		outcount += fprintf(fptr, "group=%s,%ld  ",
			deflts->defgname, deflts->defgroup);
		PRINTNL();
	}

	if (flags & D_PROJ) {
		outcount += fprintf(fptr, "project=%s,%ld  ",
		    deflts->defprojname, deflts->defproj);
		PRINTNL();
	}

	if (flags & D_BASEDIR) {
		outcount += fprintf(fptr, "basedir=%s  ", deflts->defparent);
		PRINTNL();
	}

	if (flags & D_RID) {
		outcount += fprintf(fptr, "rid=%ld  ", deflts->defrid);
		PRINTNL();
	}

	if (flags & D_SKEL) {
		outcount += fprintf(fptr, "skel=%s  ", deflts->defskel);
		PRINTNL();
	}

	if (flags & D_SHELL) {
		outcount += fprintf(fptr, "shell=%s  ", deflts->defshell);
		PRINTNL();
	}

	if (flags & D_INACT) {
		outcount += fprintf(fptr, "inactive=%d  ", deflts->definact);
		PRINTNL();
	}

	if (flags & D_EXPIRE) {
		outcount += fprintf(fptr, "expire=%s  ", deflts->defexpire);
		PRINTNL();
	}

	if (flags & D_AUTH) {
		outcount += fprintf(fptr, "auths=%s  ", deflts->defauth);
		PRINTNL();
	}

	if (flags & D_PROF) {
		outcount += fprintf(fptr, "profiles=%s  ", deflts->defprof);
		PRINTNL();
	}

	if ((flags & D_ROLE) &&
	    (!is_role(usertype))) {
		outcount += fprintf(fptr, "roles=%s  ", deflts->defrole);
		PRINTNL();
	}

	if (flags & D_LPRIV) {
		outcount += fprintf(fptr, "limitpriv=%s  ",
			deflts->deflimpriv);
		PRINTNL();
	}

	if (flags & D_DPRIV) {
		outcount += fprintf(fptr, "defaultpriv=%s  ",
			deflts->defdfltpriv);
		PRINTNL();
	}

	if (flags & D_LOCK) {
		outcount += fprintf(fptr, "lock_after_retries=%s  ",
		    deflts->deflock_after_retries);
	}

	if (outcount > 0)
		(void) fprintf(fptr, "\n");
}

/* Import default keys for ordinary useradd */
void
import_def(struct userdefs *ud)
{
	int i;

	for (i = 0; i < NDEF; i++) {
		if (tab[i].uakey != NULL && tab[i].type == STR) {
			char *val = FIELD(ud, &tab[i], char *);
			if (val == getsetdefval(tab[i].uakey, val))
				nkeys ++;
		}
	}
}
