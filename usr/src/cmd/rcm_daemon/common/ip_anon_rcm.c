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
 * RCM module to prevent plumbed IP addresses from being removed.
 */


#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <thread.h>
#include <synch.h>
#include <assert.h>
#include <errno.h>
#include <libintl.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/time.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip6.h>
#include <inet/ip.h>
#include <inet/ip6.h>
#include <libinetutil.h>

#include "rcm_module.h"

#define	SUNW_IP		"SUNW_ip/"
#define	IP_REG_SIZE	(9 + INET6_ADDRSTRLEN)
#define	IP_ANON_USAGE	gettext("Plumbed IP Address")
#define	IP_SUSPEND_ERR	gettext("Plumbed IP Addresses cannot be suspended")
#define	IP_OFFLINE_ERR	gettext("Invalid operation: IP cannot be offlined")
#define	IP_REMOVE_ERR	gettext("Invalid operation: IP cannot be removed")
#define	IP_REG_FAIL	gettext("Registration Failed")

#define	IP_FLAG_NEW	0x00
#define	IP_FLAG_REG	0x01
#define	IP_FLAG_CL	0x02
#define	IP_FLAG_IGNORE	0x04
#define	IP_FLAG_DELETE	0x08

static int		ip_anon_register(rcm_handle_t *);
static int		ip_anon_unregister(rcm_handle_t *);
static int		ip_anon_getinfo(rcm_handle_t *, char *, id_t, uint_t,
			    char **, char **, nvlist_t *, rcm_info_t **);
static int		ip_anon_suspend(rcm_handle_t *, char *, id_t,
			    timespec_t *, uint_t, char **, rcm_info_t **);
static int		ip_anon_resume(rcm_handle_t *, char *, id_t, uint_t,
			    char **, rcm_info_t **);
static int		ip_anon_offline(rcm_handle_t *, char *, id_t, uint_t,
			    char **, rcm_info_t **);
static int		ip_anon_online(rcm_handle_t *, char *, id_t, uint_t,
			    char **, rcm_info_t **);
static int		ip_anon_remove(rcm_handle_t *, char *, id_t, uint_t,
			    char **, rcm_info_t **);

typedef struct ip_status {
	int			flags;
	char			device[IP_REG_SIZE];
	struct ip_status	*next;
} ip_status_t;

static ip_status_t	*findreg(char *reg);
static ip_status_t	*addreg(char *reg);
static int		deletereg(ip_status_t *entry);

static ip_status_t	*ip_list = NULL;
static mutex_t		ip_list_lock;

static struct rcm_mod_ops ip_anon_ops =
{
	RCM_MOD_OPS_VERSION,
	ip_anon_register,
	ip_anon_unregister,
	ip_anon_getinfo,
	ip_anon_suspend,
	ip_anon_resume,
	ip_anon_offline,
	ip_anon_online,
	ip_anon_remove,
	NULL,
	NULL,
	NULL
};

struct rcm_mod_ops *
rcm_mod_init()
{
	return (&ip_anon_ops);
}

const char *
rcm_mod_info()
{
	return ("RCM IP address module 1.4");
}

int
rcm_mod_fini()
{
	ip_status_t *tlist;

	/* free the registration list */

	(void) mutex_lock(&ip_list_lock);
	while (ip_list != NULL) {
		tlist = ip_list->next;
		free(ip_list);
		ip_list = tlist;
	}
	(void) mutex_unlock(&ip_list_lock);

	(void) mutex_destroy(&ip_list_lock);
	return (RCM_SUCCESS);
}

static int
ip_anon_register(rcm_handle_t *hdl)
{
	struct ifaddrlist *al = NULL, *al6 = NULL;
	char errbuf[ERRBUFSIZE];
	char treg[IP_REG_SIZE], tstr[IP_REG_SIZE];
	int num_ifs, num_ifs6,  i, ret;
	ip_status_t *tlist, *tentry;

	(void) mutex_lock(&ip_list_lock);

	rcm_log_message(RCM_DEBUG, "ip_anon: registration refresh.\n");

	/* obtain a list of all IPv4 and IPv6 addresses in the system */

	rcm_log_message(RCM_DEBUG,
	    "ip_anon: obtaining list of IPv4 addresses.\n");
	num_ifs = ifaddrlist(&al, AF_INET, LIFC_UNDER_IPMP, errbuf);
	if (num_ifs == -1) {
		rcm_log_message(RCM_ERROR,
		    gettext("cannot get IPv4 address list errno=%d (%s)\n"),
		    errno, errbuf);
		(void) mutex_unlock(&ip_list_lock);
		return (RCM_FAILURE);
	}

	rcm_log_message(RCM_DEBUG,
	    "ip_anon: obtaining list of IPv6 addresses.\n");

	num_ifs6 = ifaddrlist(&al6, AF_INET6, LIFC_UNDER_IPMP, errbuf);
	if (num_ifs6 == -1) {
		rcm_log_message(RCM_ERROR,
		    gettext("cannot get IPv6 address list errno=%d (%s)\n"),
		    errno, errbuf);
		free(al);
		(void) mutex_unlock(&ip_list_lock);
		return (RCM_FAILURE);
	}

	/* check the state of outstanding registrations against the list */

	rcm_log_message(RCM_DEBUG,
	    "ip_anon: checking outstanding registrations.\n");

	tlist = ip_list;
	while (tlist != NULL) {
		tlist->flags |= IP_FLAG_DELETE;
		tlist = tlist->next;
	}

	/* IPv4 */

	rcm_log_message(RCM_DEBUG, "ip_anon: checking IPv4 addresses.\n");

	for (i = 0; i < num_ifs; i++) {
		(void) inet_ntop(AF_INET, &al[i].addr.addr, tstr,
		    INET_ADDRSTRLEN);
		(void) strcpy(treg, SUNW_IP);
		(void) strcat(treg, tstr);

		if ((tlist = findreg(treg)) == NULL)
			tlist = addreg(treg);
		else
			tlist->flags &= (~IP_FLAG_DELETE);

		if (tlist == NULL) {
			rcm_log_message(RCM_ERROR,
			    gettext("out of memory\n"));
			free(al);
			free(al6);
			(void) mutex_unlock(&ip_list_lock);
			return (RCM_FAILURE);
		}
	}

	/* IPv6 */

	rcm_log_message(RCM_DEBUG, "ip_anon: checking IPv6 addresses.\n");

	for (i = 0; i < num_ifs6; i++) {
		(void) inet_ntop(AF_INET6, &al6[i].addr.addr, tstr,
		    INET6_ADDRSTRLEN);
		(void) strcpy(treg, SUNW_IP);
		(void) strcat(treg, tstr);

		if ((tlist = findreg(treg)) == NULL)
			tlist = addreg(treg);
		else
			tlist->flags &= (~IP_FLAG_DELETE);

		if (tlist == NULL) {
			rcm_log_message(RCM_ERROR,
			    gettext("out of memory\n"));
			free(al);
			free(al6);
			(void) mutex_unlock(&ip_list_lock);
			return (RCM_FAILURE);
		}
	}

	rcm_log_message(RCM_DEBUG, "ip_anon: updating reg. state.\n");

	/* examine the list of ip address registrations and their state */

	tlist = ip_list;
	while (tlist != NULL) {
		tentry = tlist;
		tlist = tlist->next;

		if (tentry->flags & IP_FLAG_DELETE) {
			if (tentry->flags & IP_FLAG_REG) {
				rcm_log_message(RCM_DEBUG,
				    "ip_anon: unregistering interest in %s\n",
				    tentry->device);
				if (rcm_unregister_interest(hdl,
				    tentry->device, 0) != 0) {
					rcm_log_message(RCM_ERROR,
					    gettext("failed to unregister"));
				}
			}
			(void) deletereg(tentry);
		} else if (!(tentry->flags & IP_FLAG_IGNORE)) {
			/*
			 * If the registration is not a clustered devices and
			 * not already registered, then RCM doesn't
			 * currently know about it.
			 */
			if (!(tentry->flags & IP_FLAG_CL) &&
			    !(tentry->flags & IP_FLAG_REG)) {
				tentry->flags |= IP_FLAG_REG;
				rcm_log_message(RCM_DEBUG,
				    "ip_anon: registering interest in %s\n",
				    tentry->device);
				if (rcm_register_interest(hdl,
				    tentry->device, 0, NULL) !=
				    RCM_SUCCESS) {
					rcm_log_message(RCM_ERROR,
					    IP_REG_FAIL);
					free(al);
					free(al6);
					(void) mutex_unlock(&ip_list_lock);
					return (RCM_FAILURE);
				} else {
					rcm_log_message(RCM_DEBUG,
					    "ip_anon: registered %s\n",
					    tentry->device);
				}
			}

			/*
			 * If the entry is registered and clustered, then
			 * the configuration has been changed and it
			 * should be unregistered.
			 */
			if ((tentry->flags & IP_FLAG_REG) &
			    (tentry->flags & IP_FLAG_CL)) {
				rcm_log_message(RCM_DEBUG,
				    "ip_anon: unregistering in %s\n",
				    tentry->device);
				if (rcm_unregister_interest(hdl,
				    tentry->device, 0) != 0) {
					rcm_log_message(RCM_ERROR,
					    gettext("failed to unregister"));
				}
				tentry->flags &= (~IP_FLAG_REG);
			}
		}
	}

	tlist = ip_list;
	while (tlist != NULL) {
		rcm_log_message(RCM_DEBUG, "ip_anon: %s (%Xh)\n",
		    tlist->device, tlist->flags);
		tlist = tlist->next;
	}
	rcm_log_message(RCM_DEBUG, "ip_anon: registration complete.\n");

	free(al);
	free(al6);
	(void) mutex_unlock(&ip_list_lock);
	return (RCM_SUCCESS);
}

static int
ip_anon_unregister(rcm_handle_t *hdl)
{
	ip_status_t *tlist;

	(void) mutex_lock(&ip_list_lock);

	tlist = ip_list;
	while (tlist != NULL) {
		if ((tlist->flags & IP_FLAG_REG)) {
			if (rcm_unregister_interest(hdl,
			    tlist->device, 0) != 0) {
				rcm_log_message(RCM_ERROR,
				    gettext("failed to unregister"));
			}
			tlist->flags &= (~IP_FLAG_REG);
		}
		tlist = tlist->next;
	}

	(void) mutex_unlock(&ip_list_lock);

	return (RCM_SUCCESS);
}

/*ARGSUSED*/
static int
ip_anon_getinfo(rcm_handle_t *hdl, char *rsrcname, id_t id, uint_t flags,
    char **infostr, char **errstr, nvlist_t *props, rcm_info_t **dependent)
{

	assert(rsrcname != NULL && infostr != NULL);

	if ((*infostr = strdup(IP_ANON_USAGE)) == NULL)
		rcm_log_message(RCM_ERROR, gettext("strdup failure\n"));

	return (RCM_SUCCESS);
}

/*ARGSUSED*/
static int
ip_anon_suspend(rcm_handle_t *hdl, char *rsrcname, id_t id,
    timespec_t *interval, uint_t flags, char **errstr,
    rcm_info_t **dependent)
{
	if ((*errstr = strdup(IP_SUSPEND_ERR)) == NULL)
		rcm_log_message(RCM_ERROR, gettext("strdup failure\n"));

	return (RCM_FAILURE);
}

/*ARGSUSED*/
static int
ip_anon_resume(rcm_handle_t *hdl, char *rsrcname, id_t id, uint_t flags,
    char **errstr, rcm_info_t **dependent)
{
	return (RCM_SUCCESS);
}

/*ARGSUSED*/
static int
ip_anon_offline(rcm_handle_t *hdl, char *rsrcname, id_t id, uint_t flags,
    char **errstr, rcm_info_t **dependent)
{
	if ((*errstr = strdup(IP_OFFLINE_ERR)) == NULL)
		rcm_log_message(RCM_ERROR, gettext("strdup failure\n"));

	return (RCM_FAILURE);
}

/*ARGSUSED*/
static int
ip_anon_online(rcm_handle_t *hdl, char *rsrcname, id_t id, uint_t flags,
    char  **errstr, rcm_info_t **dependent)
{
	return (RCM_SUCCESS);
}

/*ARGSUSED*/
static int
ip_anon_remove(rcm_handle_t *hdl, char *rsrcname, id_t id, uint_t flags,
    char **errstr, rcm_info_t **dependent)
{
	if ((*errstr = strdup(IP_REMOVE_ERR)) == NULL)
		rcm_log_message(RCM_ERROR, gettext("strdup failure\n"));

	return (RCM_FAILURE);
}

/*
 * Call with ip_list_lock held.
 */

static ip_status_t *
findreg(char *reg)
{
	ip_status_t *tlist;
	int done;

	tlist = ip_list;
	done = 0;
	while ((tlist != NULL) && (!done)) {
		if (strcmp(tlist->device, reg) == 0)
			done = 1;
		else
			tlist = tlist->next;
	}

	return (tlist);
}

static ip_status_t *
addreg(char *reg)
{
	ip_status_t *tlist, *tentry;

	tentry = (ip_status_t *)malloc(sizeof (ip_status_t));
	if (tentry == NULL)
		return (tentry);

	tentry->flags = IP_FLAG_NEW;
	tentry->next = NULL;
	(void) strcpy(tentry->device, reg);

	if (ip_list == NULL)
		ip_list = tentry;
	else {
		tlist = ip_list;
		while (tlist->next != NULL)
			tlist = tlist->next;
		tlist->next = tentry;
	}

	return (tentry);
}

static int
deletereg(ip_status_t *entry)
{
	ip_status_t *tlist;

	if (entry == NULL)
		return (-1);

	if (entry == ip_list) {
		ip_list = ip_list->next;
		free(entry);
	} else {
		tlist = ip_list;
		while ((tlist->next != NULL) && (tlist->next != entry))
			tlist = tlist->next;

		if (tlist->next != entry)
			return (-1);
		tlist->next = entry->next;
		free(entry);
	}
	return (0);
}
