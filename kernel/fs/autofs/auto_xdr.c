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

/*
 * This file can not be automatically generated by rpcgen from
 * autofs_prot.x because of the xdr routines that provide readdir
 * support, its own implementation of xdr_autofs_netbuf(). rpcgen will
 * also generate xdr routines with recursion which should not be used
 * in the kernel.
 */

#include <sys/param.h>
#include <sys/kmem.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/vfs.h>
#include <sys/vnode.h>
#include <sys/pathname.h>
#include <sys/cred.h>
#include <sys/mount.h>
#include <sys/cmn_err.h>
#include <sys/debug.h>
#include <sys/systm.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/auth.h>
#include <rpc/clnt.h>
#include <sys/ticotsord.h>
#include <sys/dirent.h>
#include <sys/sysmacros.h>
#include <sys/fs_subr.h>
#include <sys/fs/autofs.h>

bool_t xdr_autofs_netbuf(XDR *, struct netbuf *);
bool_t xdr_mounta(XDR *, struct mounta *);

bool_t
xdr_umntrequest(XDR *xdrs, umntrequest *objp)
{
	bool_t more_data;

	ASSERT(xdrs->x_op == XDR_ENCODE);

	for (; objp != NULL; objp = objp->next) {
		if (!xdr_bool_t(xdrs, &objp->isdirect))
			return (FALSE);
		if (!xdr_string(xdrs, &objp->mntresource, AUTOFS_MAXPATHLEN))
			return (FALSE);
		if (!xdr_string(xdrs, &objp->mntpnt, AUTOFS_MAXPATHLEN))
			return (FALSE);
		if (!xdr_string(xdrs, &objp->fstype, AUTOFS_MAXCOMPONENTLEN))
			return (FALSE);
		if (!xdr_string(xdrs, &objp->mntopts, AUTOFS_MAXOPTSLEN))
			return (FALSE);

		if (objp->next != NULL)
			more_data = TRUE;
		else
			more_data = FALSE;

		if (!xdr_bool(xdrs, &more_data))
			return (FALSE);
	}
	return (TRUE);
}

bool_t
xdr_umntres(XDR *xdrs, umntres *objp)
{
	return (xdr_int(xdrs, &objp->status));
}

bool_t
xdr_autofs_stat(XDR *xdrs, autofs_stat *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_autofs_action(XDR *xdrs, autofs_action *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_linka(XDR *xdrs, linka *objp)
{
	if (!xdr_string(xdrs, &objp->dir, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->link, AUTOFS_MAXPATHLEN))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_autofs_args(XDR *xdrs, autofs_args *objp)
{
	if (!xdr_autofs_netbuf(xdrs, &objp->addr))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->path, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->opts, AUTOFS_MAXOPTSLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->map, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->subdir, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->key, AUTOFS_MAXCOMPONENTLEN))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->mount_to))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->rpc_to))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->direct))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_action_list_entry(XDR *xdrs, action_list_entry *objp)
{
	if (!xdr_autofs_action(xdrs, &objp->action))
		return (FALSE);
	switch (objp->action) {
	case AUTOFS_MOUNT_RQ:
		if (!xdr_mounta(xdrs, &objp->action_list_entry_u.mounta))
			return (FALSE);
		break;
	case AUTOFS_LINK_RQ:
		if (!xdr_linka(xdrs, &objp->action_list_entry_u.linka))
			return (FALSE);
		break;
	default:
		break;
	}
	return (TRUE);
}

bool_t
xdr_action_list(XDR *xdrs, action_list *objp)
{
	bool_t more_data = TRUE;
	bool_t status = TRUE;
	action_list *p, *last;

	ASSERT((xdrs->x_op == XDR_DECODE) || (xdrs->x_op == XDR_FREE));

	more_data = (objp != NULL);
	p = objp;

	if (xdrs->x_op == XDR_FREE) {
		while (p != NULL) {
			if (!xdr_action_list_entry(xdrs, &p->action))
				cmn_err(CE_WARN, "xdr_action_list: "
				    "action_list_entry free failed %p\n",
				    (void *)&p->action);
			last = p;
			p = p->next;
			kmem_free(last, sizeof (*last));
		}
		return (status);
	}

	while (more_data) {
		if (!xdr_action_list_entry(xdrs, &p->action)) {
			status = FALSE;
			break;
		}

		if (!xdr_bool(xdrs, &more_data)) {
			status = FALSE;
			break;
		}

		if (more_data) {
			p->next = kmem_zalloc(sizeof (action_list), KM_SLEEP);
			p = p->next;
			if (p == NULL) {
				status = FALSE;
				break;
			}
		} else
			p->next = NULL;
	}
	return (status);
}

bool_t
xdr_autofs_netbuf(XDR *xdrs, struct netbuf *objp)
{
	bool_t dummy;

	if (!xdr_u_int(xdrs, (uint_t *)&objp->maxlen))
		return (FALSE);
	dummy = xdr_bytes(xdrs, (char **)&(objp->buf),
	    (uint_t *)&(objp->len), objp->maxlen);
	return (dummy);
}

bool_t
xdr_mounta(XDR *xdrs, struct mounta *objp)
{
	if (!xdr_string(xdrs, &objp->spec, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->dir, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->flags))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->fstype, AUTOFS_MAXCOMPONENTLEN))
		return (FALSE);
	if (!xdr_pointer(xdrs, (char **)&objp->dataptr, sizeof (autofs_args),
	    (xdrproc_t)xdr_autofs_args))
		return (FALSE);
	/*
	 * The length is the original user-land length, not the
	 * length of the native kernel autofs_args structure provided
	 * after we decode the xdr buffer.  So passing the user's idea of
	 * the length is wrong and we need to stuff the length field with
	 * the length of the native structure.
	 */
	if (!xdr_int(xdrs, &objp->datalen))
		return (FALSE);
	if (xdrs->x_op == XDR_DECODE)
		objp->datalen = sizeof (struct autofs_args);
	if (!xdr_string(xdrs, &objp->optptr, AUTOFS_MAXOPTSLEN))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->optlen))
		return (FALSE);
	ASSERT((xdrs->x_op == XDR_DECODE) || (xdrs->x_op == XDR_FREE));
	return (TRUE);
}

bool_t
xdr_autofs_res(XDR *xdrs, autofs_res *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_autofs_lookupargs(XDR *xdrs, autofs_lookupargs *objp)
{
	if (!xdr_string(xdrs, &objp->map, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->path, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->name, AUTOFS_MAXCOMPONENTLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->subdir, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->opts, AUTOFS_MAXOPTSLEN))
		return (FALSE);
	if (!xdr_bool_t(xdrs, &objp->isdirect))
		return (FALSE);
	if (!xdr_u_int(xdrs, (uint_t *)&objp->uid))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_mount_result_type(XDR *xdrs, mount_result_type *objp)
{
	if (!xdr_autofs_stat(xdrs, &objp->status))
		return (FALSE);
	switch (objp->status) {
	case AUTOFS_ACTION:
		if (!xdr_pointer(xdrs,
		    (char **)&objp->mount_result_type_u.list,
		    sizeof (action_list), (xdrproc_t)xdr_action_list))
			return (FALSE);
		break;
	case AUTOFS_DONE:
		if (!xdr_int(xdrs, &objp->mount_result_type_u.error))
			return (FALSE);
		break;
	}
	return (TRUE);
}

bool_t
xdr_autofs_mountres(XDR *xdrs, autofs_mountres *objp)
{
	if (!xdr_mount_result_type(xdrs, &objp->mr_type))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->mr_verbose))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_lookup_result_type(XDR *xdrs, lookup_result_type *objp)
{
	if (!xdr_autofs_action(xdrs, &objp->action))
		return (FALSE);
	switch (objp->action) {
	case AUTOFS_LINK_RQ:
		if (!xdr_linka(xdrs, &objp->lookup_result_type_u.lt_linka))
			return (FALSE);
		break;
	default:
		break;
	}
	return (TRUE);
}

bool_t
xdr_autofs_lookupres(XDR *xdrs, autofs_lookupres *objp)
{
	if (!xdr_autofs_res(xdrs, &objp->lu_res))
		return (FALSE);
	if (!xdr_lookup_result_type(xdrs, &objp->lu_type))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->lu_verbose))
		return (FALSE);
	return (TRUE);
}

bool_t
xdr_autofs_rddirargs(XDR *xdrs, autofs_rddirargs *objp)
{
	if (!xdr_string(xdrs, &objp->rda_map, AUTOFS_MAXPATHLEN))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->rda_offset))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->rda_count))
		return (FALSE);
	if (!xdr_u_int(xdrs, (uint_t *)&objp->uid))
		return (FALSE);
	return (TRUE);
}

/*
 * Directory read reply:
 * union (enum autofs_res) {
 *	AUTOFS_OK: entlist;
 *		 boolean eof;
 *	default:
 * }
 *
 * Directory entries
 *	struct  direct {
 *		off_t   d_off;			* offset of next entry *
 *		u_long  d_fileno;		* inode number of entry *
 *		ushort_t d_reclen;		* length of this record *
 *		ushort_t d_namlen;		* length of string in d_name *
 *		char    d_name[MAXNAMLEN + 1];	* name no longer than this *
 *	};
 * are on the wire as:
 * union entlist (boolean valid) {
 * 	TRUE:	struct otw_dirent;
 *		uint_t nxtoffset;
 *		union entlist;
 *	FALSE:
 * }
 * where otw_dirent is:
 * 	struct dirent {
 *		uint_t	de_fid;
 *		string	de_name<AUTOFS_MAXPATHLEN>;
 *	}
 */

#ifdef nextdp
#undef nextdp
#endif
#define	nextdp(dp)	((struct dirent64 *)((char *)(dp) + (dp)->d_reclen))

/*
 * ENCODE ONLY
 */
bool_t
xdr_autofs_putrddirres(XDR *xdrs, struct autofsrddir *rddir, uint_t reqsize)
{
	struct dirent64 *dp;
	char *name;
	int size;
	uint_t namlen;
	bool_t t = TRUE;
	bool_t f = FALSE;
	int entrysz;
	int tofit;
	int bufsize;
	uint_t ino, off;

	bufsize = 1 * BYTES_PER_XDR_UNIT;
	for (size = rddir->rddir_size, dp = rddir->rddir_entries;
	    size > 0;
	    /* LINTED pointer alignment */
	    size -= dp->d_reclen, dp = nextdp(dp)) {
		if (dp->d_reclen == 0 /* || DIRSIZ(dp) > dp->d_reclen */)
			return (FALSE);
		if (dp->d_ino == 0)
			continue;
		name = dp->d_name;
		namlen = (uint_t)strlen(name);
		ino = (uint_t)dp->d_ino;
		off = (uint_t)dp->d_off;
		entrysz = (1 + 1 + 1 + 1) * BYTES_PER_XDR_UNIT +
		    roundup(namlen, BYTES_PER_XDR_UNIT);
		tofit = entrysz + 2 * BYTES_PER_XDR_UNIT;
		if (bufsize + tofit > reqsize) {
			rddir->rddir_eof = FALSE;
			break;
		}
		if (!xdr_bool(xdrs, &t) ||
		    !xdr_u_int(xdrs, &ino) ||
		    !xdr_bytes(xdrs, &name, &namlen, AUTOFS_MAXPATHLEN) ||
		    !xdr_u_int(xdrs, &off)) {
			return (FALSE);
		}
		bufsize += entrysz;
	}
	if (!xdr_bool(xdrs, &f))
		return (FALSE);
	if (!xdr_bool(xdrs, &rddir->rddir_eof))
		return (FALSE);
	return (TRUE);
}


/*
 * DECODE ONLY
 */
bool_t
xdr_autofs_getrddirres(XDR *xdrs, struct autofsrddir *rddir)
{
	struct dirent64 *dp;
	uint_t namlen;
	int size;
	bool_t valid;
	uint_t offset;
	uint_t fileid;

	offset = (uint_t)-1;

	size = rddir->rddir_size;
	dp = rddir->rddir_entries;
	for (;;) {
		if (!xdr_bool(xdrs, &valid))
			return (FALSE);
		if (!valid)
			break;
		if (!xdr_u_int(xdrs, &fileid) ||
		    !xdr_u_int(xdrs, &namlen))
			return (FALSE);
		if (DIRENT64_RECLEN(namlen) > size) {
			rddir->rddir_eof = FALSE;
			goto bufovflw;
		}
		if (!xdr_opaque(xdrs, dp->d_name, namlen)||
		    !xdr_u_int(xdrs, &offset))
			return (FALSE);
		dp->d_ino = fileid;
		dp->d_reclen = (ushort_t)DIRENT64_RECLEN(namlen);
		bzero(&dp->d_name[namlen],
		    DIRENT64_NAMELEN(dp->d_reclen) - namlen);
		dp->d_off = offset;
		size -= dp->d_reclen;
		/* LINTED pointer alignment */
		dp = nextdp(dp);
	}
	if (!xdr_bool(xdrs, &rddir->rddir_eof))
		return (FALSE);
bufovflw:
	rddir->rddir_size = (uint_t)((char *)dp - (char *)rddir->rddir_entries);
	rddir->rddir_offset = offset;
	return (TRUE);
}

bool_t
xdr_autofs_rddirres(XDR *xdrs, autofs_rddirres *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)&objp->rd_status))
		return (FALSE);
	if (objp->rd_status != AUTOFS_OK)
		return (TRUE);
	if (xdrs->x_op == XDR_ENCODE)
		return (xdr_autofs_putrddirres(xdrs,
		    (struct autofsrddir *)&objp->rd_rddir, objp->rd_bufsize));
	else if (xdrs->x_op == XDR_DECODE)
		return (xdr_autofs_getrddirres(xdrs,
		    (struct autofsrddir *)&objp->rd_rddir));
	return (FALSE);
}
