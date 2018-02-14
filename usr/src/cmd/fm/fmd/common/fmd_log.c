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
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

/*
 * FMD Log File Subsystem
 *
 * Events are written to one of two log files as they are received or created;
 * the error log tracks all ereport.* events received on the inbound event
 * transport, and the fault log tracks all list.* events generated by fmd or
 * its client modules.  In addition, we use the same log file format to cache
 * state and events associated with ASRUs that are named in a diagnosis.
 *
 * The log files use the exacct format manipulated by libexacct(3LIB) and
 * originally defined in PSARC 1999/119.  However, the exacct library was
 * designed primarily for read-only clients and without the synchronous i/o
 * considerations and seeking required for fmd, so we use libexacct here only
 * to read and write the file headers and to pack data from memory into a file
 * bytestream.  All of the i/o and file offset manipulations are performed by
 * the fmd code below.  Our exacct file management uses the following grammar:
 *
 * file := hdr toc event*
 * hdr := EXD_FMA_LABEL EXD_FMA_VERSION EXD_FMA_OSREL EXD_FMA_OSVER
 * EXD_FMA_PLAT EXD_FMA_UUID
 * toc := EXD_FMA_OFFSET
 * event := EXD_FMA_TODSEC EXD_FMA_TODNSEC EXD_FMA_NVLIST evref* or legacy evref
 * evref := EXD_FMA_UUID EXD_FMA_OFFSET
 * legacy evref := EXD_FMA_MAJOR EXD_FMA_MINOR EXD_FMA_INODE EXD_FMA_OFFSET
 *
 * Any event can be uniquely identified by the tuple (file, offset) where file
 * is encoded as (uuid) when we are cross-linking files.  For legacy file
 * formats we still support encoding the reference as (major, minor, inode).
 * Note that we break out of the file's dev_t into its two 32-bit components to
 * permit development of either 32-bit or 64-bit log readers and writers; the
 * LFS APIs do not yet export a 64-bit dev_t to fstat64(), so there is no way
 * for a 32-bit application to retrieve and store a 64-bit dev_t.
 *
 * In order to replay events in the event of an fmd crash, events are initially
 * written to the error log using the group catalog tag EXD_GROUP_RFMA by the
 * fmd_log_append() function.  Later, once an event transitions from the
 * received state to one of its other states (see fmd_event.c for details),
 * fmd_log_commit() is used to overwrite the tag with EXD_GROUP_FMA, indicating
 * that the event is fully processed and no longer needs to be replayed.
 */

#include <sys/types.h>
#include <sys/mkdev.h>
#include <sys/statvfs.h>
#include <sys/fm/protocol.h>
#include <sys/exacct_impl.h>
#include <uuid/uuid.h>

#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <ctype.h>

#include <fmd_alloc.h>
#include <fmd_error.h>
#include <fmd_string.h>
#include <fmd_event.h>
#include <fmd_conf.h>
#include <fmd_subr.h>
#include <fmd_case.h>
#include <fmd_log.h>

#include <fmd.h>

#define	CAT_FMA_RGROUP	(EXT_GROUP | EXC_DEFAULT | EXD_GROUP_RFMA)
#define	CAT_FMA_GROUP	(EXT_GROUP | EXC_DEFAULT | EXD_GROUP_FMA)

#define	CAT_FMA_LABEL	(EXT_STRING | EXC_DEFAULT | EXD_FMA_LABEL)
#define	CAT_FMA_VERSION	(EXT_STRING | EXC_DEFAULT | EXD_FMA_VERSION)
#define	CAT_FMA_OSREL	(EXT_STRING | EXC_DEFAULT | EXD_FMA_OSREL)
#define	CAT_FMA_OSVER	(EXT_STRING | EXC_DEFAULT | EXD_FMA_OSVER)
#define	CAT_FMA_PLAT	(EXT_STRING | EXC_DEFAULT | EXD_FMA_PLAT)
#define	CAT_FMA_UUID	(EXT_STRING | EXC_DEFAULT | EXD_FMA_UUID)
#define	CAT_FMA_TODSEC	(EXT_UINT64 | EXC_DEFAULT | EXD_FMA_TODSEC)
#define	CAT_FMA_TODNSEC	(EXT_UINT64 | EXC_DEFAULT | EXD_FMA_TODNSEC)
#define	CAT_FMA_NVLIST	(EXT_RAW | EXC_DEFAULT | EXD_FMA_NVLIST)
#define	CAT_FMA_MAJOR	(EXT_UINT32 | EXC_DEFAULT | EXD_FMA_MAJOR)
#define	CAT_FMA_MINOR	(EXT_UINT32 | EXC_DEFAULT | EXD_FMA_MINOR)
#define	CAT_FMA_INODE	(EXT_UINT64 | EXC_DEFAULT | EXD_FMA_INODE)
#define	CAT_FMA_OFFSET	(EXT_UINT64 | EXC_DEFAULT | EXD_FMA_OFFSET)

static ssize_t
fmd_log_write(fmd_log_t *lp, const void *buf, size_t n)
{
	ssize_t resid = n;
	ssize_t len;

	ASSERT(MUTEX_HELD(&lp->log_lock));

	while (resid != 0) {
		if ((len = write(lp->log_fd, buf, resid)) <= 0)
			break;

		resid -= len;
		buf = (char *)buf + len;
	}

	if (resid == n && n != 0)
		return (-1);

	return (n - resid);
}

static int
fmd_log_write_hdr(fmd_log_t *lp, const char *tag)
{
	ea_object_t hdr, toc, i0, i1, i2, i3, i4, i5, i6;
	const char *osrel, *osver, *plat;
	off64_t off = 0;
	int err = 0;
	uuid_t uuid;

	(void) fmd_conf_getprop(fmd.d_conf, "osrelease", &osrel);
	(void) fmd_conf_getprop(fmd.d_conf, "osversion", &osver);
	(void) fmd_conf_getprop(fmd.d_conf, "platform", &plat);
	(void) fmd_conf_getprop(fmd.d_conf, "uuidlen", &lp->log_uuidlen);

	lp->log_uuid = fmd_zalloc(lp->log_uuidlen + 1, FMD_SLEEP);
	uuid_generate(uuid);
	uuid_unparse(uuid, lp->log_uuid);

	err |= ea_set_group(&hdr, CAT_FMA_GROUP);
	err |= ea_set_group(&toc, CAT_FMA_GROUP);

	err |= ea_set_item(&i0, CAT_FMA_LABEL, tag, 0);
	err |= ea_set_item(&i1, CAT_FMA_VERSION, fmd.d_version, 0);
	err |= ea_set_item(&i2, CAT_FMA_OSREL, osrel, 0);
	err |= ea_set_item(&i3, CAT_FMA_OSVER, osver, 0);
	err |= ea_set_item(&i4, CAT_FMA_PLAT, plat, 0);
	err |= ea_set_item(&i5, CAT_FMA_UUID, lp->log_uuid, 0);
	err |= ea_set_item(&i6, CAT_FMA_OFFSET, &off, 0);

	(void) ea_attach_to_group(&hdr, &i0);
	(void) ea_attach_to_group(&hdr, &i1);
	(void) ea_attach_to_group(&hdr, &i2);
	(void) ea_attach_to_group(&hdr, &i3);
	(void) ea_attach_to_group(&hdr, &i4);
	(void) ea_attach_to_group(&hdr, &i5);
	(void) ea_attach_to_group(&toc, &i6);

	if (err == 0) {
		size_t hdr_size = ea_pack_object(&hdr, NULL, 0);
		size_t toc_size = ea_pack_object(&toc, NULL, 0);

		size_t size = hdr_size + toc_size;
		void *buf = fmd_alloc(size, FMD_SLEEP);

		(void) ea_pack_object(&hdr, buf, hdr_size);
		(void) ea_pack_object(&toc, (char *)buf + hdr_size, toc_size);

		if ((lp->log_off = lseek64(lp->log_fd, 0, SEEK_END)) == -1L)
			fmd_panic("failed to seek log %s", lp->log_name);

		if (fmd_log_write(lp, buf, size) != size)
			err = errno; /* save errno for fmd_set_errno() below */

		fmd_free(buf, size);

		lp->log_toc = lp->log_off + hdr_size;
		lp->log_beg = lp->log_off + hdr_size + toc_size;
		lp->log_off = lp->log_off + hdr_size + toc_size;

		if (lp->log_off != lseek64(lp->log_fd, 0, SEEK_END))
			fmd_panic("eof off != log_off 0x%llx\n", lp->log_off);
	} else
		err = EFMD_LOG_EXACCT;

	(void) ea_free_item(&i0, EUP_ALLOC);
	(void) ea_free_item(&i1, EUP_ALLOC);
	(void) ea_free_item(&i2, EUP_ALLOC);
	(void) ea_free_item(&i3, EUP_ALLOC);
	(void) ea_free_item(&i4, EUP_ALLOC);
	(void) ea_free_item(&i5, EUP_ALLOC);
	(void) ea_free_item(&i6, EUP_ALLOC);

	return (err ? fmd_set_errno(err) : 0);
}

static int
fmd_log_check_err(fmd_log_t *lp, int err, const char *msg)
{
	int eaerr = ea_error();
	char buf[BUFSIZ];

	(void) snprintf(buf, sizeof (buf), "%s: %s: %s\n",
	    lp->log_name, msg, eaerr != EXR_OK ?
	    fmd_ea_strerror(eaerr) : "catalog tag mismatch");

	fmd_error(err, buf);
	return (fmd_set_errno(err));
}

static int
fmd_log_check_hdr(fmd_log_t *lp, const char *tag)
{
	int got_version = 0, got_label = 0;
	ea_object_t *grp, *obj;
	off64_t hdr_off, hdr_size;
	int dvers, fvers;
	const char *p;

	ea_clear(&lp->log_ea); /* resync exacct file */

	if ((hdr_off = lseek64(lp->log_fd, 0, SEEK_CUR)) == -1L)
		fmd_panic("failed to seek log %s", lp->log_name);

	/*
	 * Read the first group of log meta-data: the write-once read-only
	 * file header.  We read all records in this group, ignoring all but
	 * the VERSION and LABEL, which are required and must be verified.
	 */
	if ((grp = ea_get_object_tree(&lp->log_ea, 1)) == NULL ||
	    grp->eo_catalog != CAT_FMA_GROUP) {
		ea_free_object(grp, EUP_ALLOC);
		return (fmd_log_check_err(lp, EFMD_LOG_INVAL,
		    "invalid fma hdr record group"));
	}

	for (obj = grp->eo_group.eg_objs; obj != NULL; obj = obj->eo_next) {
		switch (obj->eo_catalog) {
		case CAT_FMA_VERSION:
			for (dvers = 0, p = fmd.d_version;
			    *p != '\0'; p++) {
				if (isdigit(*p))
					dvers = dvers * 10 + (*p - '0');
				else
					break;
			}

			for (fvers = 0, p = obj->eo_item.ei_string;
			    *p != '\0'; p++) {
				if (isdigit(*p))
					fvers = fvers * 10 + (*p - '0');
				else
					break;
			}

			if (fvers > dvers) {
				fmd_error(EFMD_LOG_INVAL, "%s: log version "
				    "%s is not supported by this daemon\n",
				    lp->log_name, obj->eo_item.ei_string);
				ea_free_object(grp, EUP_ALLOC);
				return (fmd_set_errno(EFMD_LOG_VERSION));
			}

			got_version++;
			break;

		case CAT_FMA_LABEL:
			if (strcmp(obj->eo_item.ei_string, tag) != 0) {
				fmd_error(EFMD_LOG_INVAL, "%s: log tag '%s' "
				    "does not matched expected tag '%s'\n",
				    lp->log_name, obj->eo_item.ei_string, tag);
				ea_free_object(grp, EUP_ALLOC);
				return (fmd_set_errno(EFMD_LOG_INVAL));
			}
			got_label++;
			break;
		case CAT_FMA_UUID:
			lp->log_uuid = fmd_strdup(obj->eo_item.ei_string,
			    FMD_SLEEP);
			lp->log_uuidlen = strlen(lp->log_uuid);
			break;
		}
	}

	hdr_size = ea_pack_object(grp, NULL, 0);
	ea_free_object(grp, EUP_ALLOC);

	if (!got_version || !got_label) {
		fmd_error(EFMD_LOG_INVAL, "%s: fmd hdr record group did not "
		    "include mandatory version and/or label\n", lp->log_name);
		return (fmd_set_errno(EFMD_LOG_INVAL));
	}

	/*
	 * Read the second group of log meta-data: the table of contents.  We
	 * expect this group to contain an OFFSET object indicating the current
	 * value of log_skip.  We save this in our fmd_log_t and then return.
	 */
	if ((grp = ea_get_object_tree(&lp->log_ea, 1)) == NULL ||
	    grp->eo_catalog != CAT_FMA_GROUP || grp->eo_group.eg_nobjs < 1 ||
	    grp->eo_group.eg_objs->eo_catalog != CAT_FMA_OFFSET) {
		ea_free_object(grp, EUP_ALLOC);
		return (fmd_log_check_err(lp, EFMD_LOG_INVAL,
		    "invalid fma toc record group"));
	}

	lp->log_toc = hdr_off + hdr_size;
	lp->log_beg = hdr_off + hdr_size + ea_pack_object(grp, NULL, 0);
	lp->log_off = lseek64(lp->log_fd, 0, SEEK_END);
	lp->log_skip = grp->eo_group.eg_objs->eo_item.ei_uint64;

	if (lp->log_skip > lp->log_off) {
		fmd_error(EFMD_LOG_INVAL, "%s: skip %llx exceeds file size; "
		    "resetting to zero\n", lp->log_name, lp->log_skip);
		lp->log_skip = 0;
	}

	ea_free_object(grp, EUP_ALLOC);
	return (0);
}

static int
fmd_log_open_exacct(fmd_log_t *lp, int aflags, int oflags)
{
	int fd = dup(lp->log_fd);
	const char *creator;

	(void) fmd_conf_getprop(fmd.d_conf, "log.creator", &creator);

	if (ea_fdopen(&lp->log_ea, fd, creator, aflags, oflags) != 0) {
		fmd_error(EFMD_LOG_EXACCT, "%s: failed to open log file: %s\n",
		    lp->log_name, fmd_ea_strerror(ea_error()));
		(void) close(fd);
		return (fmd_set_errno(EFMD_LOG_EXACCT));
	}

	lp->log_flags |= FMD_LF_EAOPEN;
	return (0);
}

static fmd_log_t *
fmd_log_xopen(const char *root, const char *name, const char *tag, int oflags)
{
	fmd_log_t *lp = fmd_zalloc(sizeof (fmd_log_t), FMD_SLEEP);

	char buf[PATH_MAX];
	char *slash = "/";
	size_t len;
	int err;

	(void) pthread_mutex_init(&lp->log_lock, NULL);
	(void) pthread_cond_init(&lp->log_cv, NULL);
	(void) pthread_mutex_lock(&lp->log_lock);

	if (strcmp(root, "") == 0)
		slash = "";
	len = strlen(root) + strlen(name) + strlen(slash) + 1; /* for "\0" */
	lp->log_name = fmd_alloc(len, FMD_SLEEP);
	(void) snprintf(lp->log_name, len, "%s%s%s", root, slash, name);
	lp->log_tag = fmd_strdup(tag, FMD_SLEEP);
	(void) fmd_conf_getprop(fmd.d_conf, "log.minfree", &lp->log_minfree);

	if (strcmp(lp->log_tag, FMD_LOG_ERROR) == 0)
		lp->log_flags |= FMD_LF_REPLAY;

	if (strcmp(lp->log_tag, FMD_LOG_XPRT) == 0)
		oflags &= ~O_SYNC;

top:
	if ((lp->log_fd = open64(lp->log_name, oflags, 0644)) == -1 ||
	    fstat64(lp->log_fd, &lp->log_stat) == -1) {
		fmd_error(EFMD_LOG_OPEN, "failed to open log %s", lp->log_name);
		fmd_log_close(lp);
		return (NULL);
	}

	/*
	 * If our open() created the log file, use libexacct to write a header
	 * and position the file just after the header (EO_TAIL).  If the log
	 * file already existed, use libexacct to validate the header and again
	 * position the file just after the header (EO_HEAD).  Note that we lie
	 * to libexacct about 'oflags' in order to achieve the desired result.
	 */
	if (lp->log_stat.st_size == 0) {
		err = fmd_log_open_exacct(lp, EO_VALID_HDR | EO_TAIL,
		    O_CREAT | O_WRONLY) || fmd_log_write_hdr(lp, tag);
	} else {
		err = fmd_log_open_exacct(lp, EO_VALID_HDR | EO_HEAD,
		    O_RDONLY) || fmd_log_check_hdr(lp, tag);
	}

	/*
	 * If ea_fdopen() failed and the log was pre-existing, attempt to move
	 * it aside and start a new one.  If we created the log but failed to
	 * initialize it, then we have no choice but to give up (e.g. EROFS).
	 */
	if (err) {
		fmd_error(EFMD_LOG_OPEN,
		    "failed to initialize log %s", lp->log_name);

		if (lp->log_flags & FMD_LF_EAOPEN) {
			lp->log_flags &= ~FMD_LF_EAOPEN;
			(void) ea_close(&lp->log_ea);
		}

		(void) close(lp->log_fd);
		lp->log_fd = -1;

		if (lp->log_stat.st_size != 0 && snprintf(buf,
		    sizeof (buf), "%s-", lp->log_name) < PATH_MAX &&
		    rename(lp->log_name, buf) == 0) {
			TRACE((FMD_DBG_LOG, "mv %s to %s", lp->log_name, buf));
			if (oflags & O_CREAT)
				goto top;
		}

		fmd_log_close(lp);
		return (NULL);
	}

	lp->log_refs++;
	(void) pthread_mutex_unlock(&lp->log_lock);

	return (lp);
}

fmd_log_t *
fmd_log_tryopen(const char *root, const char *name, const char *tag)
{
	return (fmd_log_xopen(root, name, tag, O_RDWR | O_SYNC));
}

fmd_log_t *
fmd_log_open(const char *root, const char *name, const char *tag)
{
	return (fmd_log_xopen(root, name, tag, O_RDWR | O_CREAT | O_SYNC));
}

void
fmd_log_close(fmd_log_t *lp)
{
	ASSERT(MUTEX_HELD(&lp->log_lock));
	ASSERT(lp->log_refs == 0);

	if ((lp->log_flags & FMD_LF_EAOPEN) && ea_close(&lp->log_ea) != 0) {
		fmd_error(EFMD_LOG_CLOSE, "failed to close log %s: %s\n",
		    lp->log_name, fmd_ea_strerror(ea_error()));
	}

	if (lp->log_fd >= 0 && close(lp->log_fd) != 0) {
		fmd_error(EFMD_LOG_CLOSE,
		    "failed to close log %s", lp->log_name);
	}

	fmd_strfree(lp->log_name);
	fmd_strfree(lp->log_tag);
	if (lp->log_uuid != NULL)
		fmd_free(lp->log_uuid, lp->log_uuidlen + 1);

	fmd_free(lp, sizeof (fmd_log_t));
}

void
fmd_log_hold_pending(fmd_log_t *lp)
{
	(void) pthread_mutex_lock(&lp->log_lock);

	lp->log_refs++;
	ASSERT(lp->log_refs != 0);

	if (lp->log_flags & FMD_LF_REPLAY) {
		lp->log_pending++;
		ASSERT(lp->log_pending != 0);
	}

	(void) pthread_mutex_unlock(&lp->log_lock);
}

void
fmd_log_hold(fmd_log_t *lp)
{
	(void) pthread_mutex_lock(&lp->log_lock);
	lp->log_refs++;
	ASSERT(lp->log_refs != 0);
	(void) pthread_mutex_unlock(&lp->log_lock);
}

void
fmd_log_rele(fmd_log_t *lp)
{
	(void) pthread_mutex_lock(&lp->log_lock);
	ASSERT(lp->log_refs != 0);

	if (--lp->log_refs == 0)
		fmd_log_close(lp);
	else
		(void) pthread_mutex_unlock(&lp->log_lock);
}

void
fmd_log_append(fmd_log_t *lp, fmd_event_t *e, fmd_case_t *cp)
{
	fmd_event_impl_t *ep = (fmd_event_impl_t *)e;
	fmd_case_impl_t *cip = (fmd_case_impl_t *)cp;
	int err = 0;

	ea_object_t grp0, grp1, i0, i1, i2, *items;
	ea_object_t **fe = NULL;
	size_t nvsize, easize, itsize, frsize;
	char *nvbuf, *eabuf;
	statvfs64_t stv;

	(void) pthread_mutex_lock(&ep->ev_lock);

	ASSERT(ep->ev_flags & FMD_EVF_VOLATILE);
	ASSERT(ep->ev_log == NULL);

	(void) nvlist_size(ep->ev_nvl, &nvsize, NV_ENCODE_XDR);
	nvbuf = fmd_alloc(nvsize, FMD_SLEEP);
	(void) nvlist_pack(ep->ev_nvl, &nvbuf, &nvsize, NV_ENCODE_XDR, 0);

	if (lp->log_flags & FMD_LF_REPLAY)
		err |= ea_set_group(&grp0, CAT_FMA_RGROUP);
	else
		err |= ea_set_group(&grp0, CAT_FMA_GROUP);

	err |= ea_set_item(&i0, CAT_FMA_TODSEC, &ep->ev_time.ftv_sec, 0);
	err |= ea_set_item(&i1, CAT_FMA_TODNSEC, &ep->ev_time.ftv_nsec, 0);
	err |= ea_set_item(&i2, CAT_FMA_NVLIST, nvbuf, nvsize);

	if (err != 0) {
		(void) pthread_mutex_unlock(&ep->ev_lock);
		err = EFMD_LOG_EXACCT;
		goto exerr;
	}

	(void) ea_attach_to_group(&grp0, &i0);
	(void) ea_attach_to_group(&grp0, &i1);
	(void) ea_attach_to_group(&grp0, &i2);

	/*
	 * If this event has a case associated with it (i.e. it is a list),
	 * then allocate a block of ea_object_t's and fill in a group for
	 * each event saved in the case's item list.  For each such group,
	 * we attach it to grp1, which in turn will be attached to grp0.
	 */
	if (cp != NULL) {
		ea_object_t *egrp, *ip, **fp;
		fmd_event_impl_t *eip;
		fmd_case_item_t *cit;

		(void) ea_set_group(&grp1, CAT_FMA_GROUP);
		frsize = sizeof (ea_object_t *) * cip->ci_nitems;
		itsize = sizeof (ea_object_t) * cip->ci_nitems * 5;
		items = ip = fmd_alloc(itsize, FMD_SLEEP);

		for (cit = cip->ci_items; cit != NULL; cit = cit->cit_next) {
			major_t maj;
			minor_t min;

			eip = (fmd_event_impl_t *)cit->cit_event;

			if (eip->ev_log == NULL)
				continue; /* event was never logged */

			maj = major(eip->ev_log->log_stat.st_dev);
			min = minor(eip->ev_log->log_stat.st_dev);

			(void) ea_set_group(ip, CAT_FMA_GROUP);
			egrp = ip++; /* first obj is group */

			/*
			 * If the event log file is in legacy format,
			 * then write the xref to the file in the legacy
			 * maj/min/inode method else write it using the
			 * file uuid.
			 */
			if (eip->ev_log->log_uuid == NULL) {
				(void) ea_set_item(ip, CAT_FMA_MAJOR, &maj, 0);
				(void) ea_attach_to_group(egrp, ip++);
				(void) ea_set_item(ip, CAT_FMA_MINOR, &min, 0);
				(void) ea_attach_to_group(egrp, ip++);
				(void) ea_set_item(ip, CAT_FMA_INODE,
				    &eip->ev_log->log_stat.st_ino, 0);
				(void) ea_attach_to_group(egrp, ip++);
			} else {
				if (ea_set_item(ip, CAT_FMA_UUID,
				    eip->ev_log->log_uuid, 0) == -1) {
					err = EFMD_LOG_EXACCT;
					goto exerrcp;
				}
				if (fe == NULL)
					fe = fp = fmd_zalloc(frsize, FMD_SLEEP);
				*fp++ = ip;
				(void) ea_attach_to_group(egrp, ip++);
			}
			(void) ea_set_item(ip, CAT_FMA_OFFSET, &eip->ev_off, 0);
			(void) ea_attach_to_group(egrp, ip++);
			(void) ea_attach_to_group(&grp1, egrp);
		}
		(void) ea_attach_to_group(&grp0, &grp1);
	}

	easize = ea_pack_object(&grp0, NULL, 0);
	eabuf = fmd_alloc(easize, FMD_SLEEP);
	(void) ea_pack_object(&grp0, eabuf, easize);

	/*
	 * Before writing the record, check to see if this would cause the free
	 * space in the filesystem to drop below our minfree threshold.  If so,
	 * don't bother attempting the write and instead pretend it failed.  As
	 * fmd(8) runs as root, it will be able to access the space "reserved"
	 * for root, and therefore can run the system of out of disk space in a
	 * heavy error load situation, violating the basic design principle of
	 * fmd(8) that we don't want to make a bad situation even worse.
	 */
	(void) pthread_mutex_lock(&lp->log_lock);

	if (lp->log_minfree != 0 && fstatvfs64(lp->log_fd, &stv) == 0 &&
	    stv.f_bavail * stv.f_frsize < lp->log_minfree + easize) {

		TRACE((FMD_DBG_LOG, "append %s crosses minfree", lp->log_tag));
		err = EFMD_LOG_MINFREE;

	} else if (fmd_log_write(lp, eabuf, easize) == easize) {
		TRACE((FMD_DBG_LOG, "append %s %p off=0x%llx",
		    lp->log_tag, (void *)ep, (u_longlong_t)lp->log_off));

		ep->ev_flags &= ~FMD_EVF_VOLATILE;
		ep->ev_log = lp;
		ep->ev_off = lp->log_off;
		ep->ev_len = easize;

		if (lp->log_flags & FMD_LF_REPLAY) {
			lp->log_pending++;
			ASSERT(lp->log_pending != 0);
		}

		lp->log_refs++;
		ASSERT(lp->log_refs != 0);
		lp->log_off += easize;
	} else {
		err = errno; /* save errno for fmd_error() call below */

		/*
		 * If we can't write append the record, seek the file back to
		 * the original location and truncate it there in order to make
		 * sure the file is always in a sane state w.r.t. libexacct.
		 */
		(void) lseek64(lp->log_fd, lp->log_off, SEEK_SET);
		(void) ftruncate64(lp->log_fd, lp->log_off);
	}

	(void) pthread_mutex_unlock(&lp->log_lock);
	(void) pthread_mutex_unlock(&ep->ev_lock);

	fmd_free(eabuf, easize);

exerrcp:
	if (cp != NULL) {
		if (fe != NULL) {
			ea_object_t **fp = fe;
			int i = 0;

			for (; *fp != NULL && i < cip->ci_nitems; i++)
				(void) ea_free_item(*fp++, EUP_ALLOC);
			fmd_free(fe, frsize);
		}

		fmd_free(items, itsize);
	}

exerr:
	fmd_free(nvbuf, nvsize);

	(void) ea_free_item(&i0, EUP_ALLOC);
	(void) ea_free_item(&i1, EUP_ALLOC);
	(void) ea_free_item(&i2, EUP_ALLOC);

	/*
	 * Keep track of out-of-space errors using global statistics.  As we're
	 * out of disk space, it's unlikely the EFMD_LOG_APPEND will be logged.
	 */
	if (err == ENOSPC || err == EFMD_LOG_MINFREE) {
		fmd_stat_t *sp;

		if (lp == fmd.d_errlog)
			sp = &fmd.d_stats->ds_err_enospc;
		else if (lp == fmd.d_fltlog)
			sp = &fmd.d_stats->ds_flt_enospc;
		else
			sp = &fmd.d_stats->ds_oth_enospc;

		(void) pthread_mutex_lock(&fmd.d_stats_lock);
		sp->fmds_value.ui64++;
		(void) pthread_mutex_unlock(&fmd.d_stats_lock);
	}

	if (err != 0) {
		fmd_error(EFMD_LOG_APPEND, "failed to log_append %s %p: %s\n",
		    lp->log_tag, (void *)ep, fmd_strerror(err));
	}
}

/*
 * Commit an event to the log permanently, indicating that it should not be
 * replayed on restart.  This is done by overwriting the event group's catalog
 * code with EXD_GROUP_FMA (from EXD_GROUP_RFMA used in fmd_log_append()).  We
 * use pwrite64() to update the existing word directly, using somewhat guilty
 * knowledge that exacct stores the 32-bit catalog word first for each object.
 * Since we are overwriting an existing log location using pwrite64() and hold
 * the event lock, we do not need to hold the log_lock during the i/o.
 */
void
fmd_log_commit(fmd_log_t *lp, fmd_event_t *e)
{
	fmd_event_impl_t *ep = (fmd_event_impl_t *)e;
	ea_catalog_t c;
	int err = 0;

	if (!(lp->log_flags & FMD_LF_REPLAY))
		return; /* log does not require replay tagging */

	ASSERT(MUTEX_HELD(&ep->ev_lock));
	ASSERT(ep->ev_log == lp && ep->ev_off != 0);

	c = CAT_FMA_GROUP;
	exacct_order32(&c);

	if (pwrite64(lp->log_fd, &c, sizeof (c), ep->ev_off) == sizeof (c)) {
		TRACE((FMD_DBG_LOG, "commit %s %p", lp->log_tag, (void *)ep));
		ep->ev_flags &= ~FMD_EVF_REPLAY;

		/*
		 * If we have committed the event, check to see if the TOC skip
		 * offset needs to be updated, and decrement the pending count.
		 */
		(void) pthread_mutex_lock(&lp->log_lock);

		if (lp->log_skip == ep->ev_off) {
			lp->log_flags |= FMD_LF_DIRTY;
			lp->log_skip += ep->ev_len;
		}

		ASSERT(lp->log_pending != 0);
		lp->log_pending--;

		(void) pthread_cond_broadcast(&lp->log_cv);
		(void) pthread_mutex_unlock(&lp->log_lock);

	} else {
		fmd_error(EFMD_LOG_COMMIT, "failed to log_commit %s %p: %s\n",
		    lp->log_tag, (void *)ep, fmd_strerror(err));
	}
}

/*
 * If we need to destroy an event and it wasn't able to be committed, we permit
 * the owner to decommit from ever trying again.  This operation decrements the
 * pending count on the log and broadcasts to anyone waiting on log_cv.
 */
void
fmd_log_decommit(fmd_log_t *lp, fmd_event_t *e)
{
	fmd_event_impl_t *ep = (fmd_event_impl_t *)e;

	if (!(lp->log_flags & FMD_LF_REPLAY))
		return; /* log does not require replay tagging */

	ASSERT(MUTEX_HELD(&ep->ev_lock));
	ASSERT(ep->ev_log == lp);

	(void) pthread_mutex_lock(&lp->log_lock);

	TRACE((FMD_DBG_LOG, "decommit %s %p", lp->log_tag, (void *)ep));
	ep->ev_flags &= ~FMD_EVF_REPLAY;

	ASSERT(lp->log_pending != 0);
	lp->log_pending--;

	(void) pthread_cond_broadcast(&lp->log_cv);
	(void) pthread_mutex_unlock(&lp->log_lock);
}

static fmd_event_t *
fmd_log_unpack(fmd_log_t *lp, ea_object_t *grp, off64_t off)
{
	fmd_timeval_t ftv = { -1ULL, -1ULL };
	nvlist_t *nvl = NULL;

	ea_object_t *obj;
	char *class;
	int err;

	for (obj = grp->eo_group.eg_objs; obj != NULL; obj = obj->eo_next) {
		switch (obj->eo_catalog) {
		case CAT_FMA_NVLIST:
			if ((err = nvlist_xunpack(obj->eo_item.ei_raw,
			    obj->eo_item.ei_size, &nvl, &fmd.d_nva)) != 0) {
				fmd_error(EFMD_LOG_UNPACK, "failed to unpack "
				    "log nvpair: %s\n", fmd_strerror(err));
				return (NULL);
			}
			break;

		case CAT_FMA_TODSEC:
			ftv.ftv_sec = obj->eo_item.ei_uint64;
			break;

		case CAT_FMA_TODNSEC:
			ftv.ftv_nsec = obj->eo_item.ei_uint64;
			break;
		}
	}

	if (nvl == NULL || ftv.ftv_sec == -1ULL || ftv.ftv_nsec == -1ULL) {
		fmd_error(EFMD_LOG_UNPACK, "failed to unpack log event: "
		    "required object(s) missing from record group\n");
		nvlist_free(nvl);
		return (NULL);
	}

	if (nvlist_lookup_string(nvl, FM_CLASS, &class) != 0) {
		fmd_error(EFMD_LOG_UNPACK, "failed to unpack log event: "
		    "record is missing required '%s' nvpair\n", FM_CLASS);
		nvlist_free(nvl);
		return (NULL);
	}

	return (fmd_event_recreate(FMD_EVT_PROTOCOL,
	    &ftv, nvl, class, lp, off, ea_pack_object(grp, NULL, 0)));
}

/*
 * Replay event(s) from the specified log by invoking the specified callback
 * function 'func' for each event.  If the log has the FMD_LF_REPLAY flag set,
 * we replay all events after log_skip that have the FMA_RGROUP group tag.
 * This mode is used for the error telemetry log.  If the log does not have
 * this flag set (used for ASRU logs), only the most recent event is replayed.
 */
void
fmd_log_replay(fmd_log_t *lp, fmd_log_f *func, void *data)
{
	ea_object_t obj, *grp;
	ea_object_type_t type;
	ea_catalog_t c;
	fmd_event_t *ep;
	off64_t off, skp;
	uint_t n = 0;

	(void) pthread_mutex_lock(&lp->log_lock);

	if (lp->log_stat.st_size == 0 && (lp->log_flags & FMD_LF_REPLAY)) {
		(void) pthread_mutex_unlock(&lp->log_lock);
		return; /* we just created this log: never replay events */
	}

	while (lp->log_flags & FMD_LF_BUSY)
		(void) pthread_cond_wait(&lp->log_cv, &lp->log_lock);

	if (lp->log_off == lp->log_beg) {
		(void) pthread_mutex_unlock(&lp->log_lock);
		return; /* no records appended yet */
	}

	lp->log_flags |= FMD_LF_BUSY;
	skp = lp->log_skip;
	ea_clear(&lp->log_ea); /* resync exacct file */

	/*
	 * If FMD_LF_REPLAY is set, begin our replay at either log_skip (if it
	 * is non-zero) or at log_beg.  Otherwise replay from the end (log_off)
	 */
	if (lp->log_flags & FMD_LF_REPLAY) {
		off = MAX(lp->log_beg, lp->log_skip);
		c = CAT_FMA_RGROUP;
	} else {
		off = lp->log_off;
		c = CAT_FMA_GROUP;
	}

	if (lseek64(lp->log_fd, off, SEEK_SET) != off) {
		fmd_panic("failed to seek %s to 0x%llx\n",
		    lp->log_name, (u_longlong_t)off);
	}

	/*
	 * If FMD_LF_REPLAY is not set, back up to the start of the previous
	 * object and make sure this object is an EO_GROUP; otherwise return.
	 */
	if (!(lp->log_flags & FMD_LF_REPLAY) &&
	    (type = ea_previous_object(&lp->log_ea, &obj)) != EO_GROUP) {
		fmd_error(EFMD_LOG_REPLAY, "last log object is of unexpected "
		    "type %d (log may be truncated or corrupt)\n", type);
		goto out;
	}

	while ((grp = ea_get_object_tree(&lp->log_ea, 1)) != NULL) {
		if (!(lp->log_flags & FMD_LF_REPLAY))
			off -= ea_pack_object(grp, NULL, 0);
		else if (n == 0 && grp->eo_catalog == CAT_FMA_GROUP)
			skp = off; /* update skip */

		/*
		 * We temporarily drop log_lock around the call to unpack the
		 * event, hold it, and perform the callback, because these
		 * operations may try to acquire log_lock to bump log_refs.
		 * We cannot lose control because the FMD_LF_BUSY flag is set.
		 */
		(void) pthread_mutex_unlock(&lp->log_lock);

		if (grp->eo_catalog == c &&
		    (ep = fmd_log_unpack(lp, grp, off)) != NULL) {

			TRACE((FMD_DBG_LOG, "replay %s %p off %llx",
			    lp->log_tag, (void *)ep, (u_longlong_t)off));

			fmd_event_hold(ep);
			func(lp, ep, data);
			fmd_event_rele(ep);
			n++;
		}

		(void) pthread_mutex_lock(&lp->log_lock);
		off += ea_pack_object(grp, NULL, 0);
		ea_free_object(grp, EUP_ALLOC);
	}

	if (ea_error() != EXR_EOF) {
		fmd_error(EFMD_LOG_REPLAY, "failed to replay %s event at "
		    "offset 0x%llx: %s\n", lp->log_name, (u_longlong_t)off,
		    fmd_ea_strerror(ea_error()));
	}

	if (n == 0)
		skp = off; /* if no replays, move skip to where we ended up */

out:
	if (lseek64(lp->log_fd, lp->log_off, SEEK_SET) != lp->log_off) {
		fmd_panic("failed to seek %s to 0x%llx\n",
		    lp->log_name, (u_longlong_t)lp->log_off);
	}

	if (skp != lp->log_skip) {
		lp->log_flags |= FMD_LF_DIRTY;
		lp->log_skip = skp;
	}

	lp->log_flags &= ~FMD_LF_BUSY;
	(void) pthread_cond_broadcast(&lp->log_cv);
	(void) pthread_mutex_unlock(&lp->log_lock);
}

void
fmd_log_update(fmd_log_t *lp)
{
	ea_object_t toc, item;
	off64_t skip = 0;
	size_t size;
	void *buf;

	(void) pthread_mutex_lock(&lp->log_lock);

	if (lp->log_flags & FMD_LF_DIRTY) {
		lp->log_flags &= ~FMD_LF_DIRTY;
		skip = lp->log_skip;
	}

	(void) pthread_mutex_unlock(&lp->log_lock);

	/*
	 * If the skip needs to be updated, construct a TOC record group
	 * containing the skip offset and overwrite the TOC in-place.
	 */
	if (skip != 0 && ea_set_group(&toc, CAT_FMA_GROUP) == 0 &&
	    ea_set_item(&item, CAT_FMA_OFFSET, &skip, 0) == 0) {

		(void) ea_attach_to_group(&toc, &item);
		size = ea_pack_object(&toc, NULL, 0);
		buf = fmd_alloc(size, FMD_SLEEP);

		(void) ea_pack_object(&toc, buf, size);
		ASSERT(lp->log_toc + size == lp->log_beg);

		if (pwrite64(lp->log_fd, buf, size, lp->log_toc) == size) {
			TRACE((FMD_DBG_LOG, "updated skip to %llx", skip));
		} else {
			fmd_error(EFMD_LOG_UPDATE,
			    "failed to log_update %s", lp->log_tag);
		}

		fmd_free(buf, size);
		(void) ea_free_item(&item, EUP_ALLOC);
	}
}

/*
 * Rotate the specified log by renaming its underlying file to a staging file
 * that can be handed off to logadm(8) or an administrator script.  If the
 * rename succeeds, open a new log file using the old path and return it.
 * Note that we are relying our caller to use some higher-level mechanism to
 * ensure that fmd_log_rotate() cannot be called while other threads are
 * attempting fmd_log_append() using the same log (fmd's d_log_lock is used
 * for the global errlog and fltlog).
 */
fmd_log_t *
fmd_log_rotate(fmd_log_t *lp)
{
	char npath[PATH_MAX];
	fmd_log_t *nlp;

	(void) snprintf(npath, sizeof (npath), "%s+", lp->log_name);

	/*
	 * Open new log file.
	 */
	if ((nlp = fmd_log_open("", npath, lp->log_tag)) == NULL) {
		fmd_error(EFMD_LOG_ROTATE, "failed to open %s", npath);
		(void) fmd_set_errno(EFMD_LOG_ROTATE);
		return (NULL);
	}

	(void) snprintf(npath, sizeof (npath), "%s.0-", lp->log_name);
	(void) pthread_mutex_lock(&lp->log_lock);

	/*
	 * Check for any pending commits to drain before proceeding.  We can't
	 * rotate the log out if commits are pending because if we die after
	 * the log is moved aside, we won't be able to replay them on restart.
	 */
	if (lp->log_pending != 0) {
		(void) pthread_mutex_unlock(&lp->log_lock);
		(void) unlink(nlp->log_name);
		fmd_log_rele(nlp);
		(void) fmd_set_errno(EFMD_LOG_ROTBUSY);
		return (NULL);
	}

	if (rename(lp->log_name, npath) != 0) {
		(void) pthread_mutex_unlock(&lp->log_lock);
		fmd_error(EFMD_LOG_ROTATE, "failed to rename %s", lp->log_name);
		(void) unlink(nlp->log_name);
		fmd_log_rele(nlp);
		(void) fmd_set_errno(EFMD_LOG_ROTATE);
		return (NULL);
	}

	if (rename(nlp->log_name, lp->log_name) != 0) {
		(void) pthread_mutex_unlock(&lp->log_lock);
		fmd_error(EFMD_LOG_ROTATE, "failed to rename %s",
		    nlp->log_name);
		(void) unlink(nlp->log_name);
		fmd_log_rele(nlp);
		(void) fmd_set_errno(EFMD_LOG_ROTATE);
		return (NULL);
	}

	/*
	 * Change name of new log file
	 */
	fmd_strfree(nlp->log_name);
	nlp->log_name = fmd_strdup(lp->log_name, FMD_SLEEP);

	/*
	 * If we've rotated the log, no pending events exist so we don't have
	 * any more commits coming, and our caller should have arranged for
	 * no more calls to append.  As such, we can close log_fd for good.
	 */
	if (lp->log_flags & FMD_LF_EAOPEN) {
		(void) ea_close(&lp->log_ea);
		lp->log_flags &= ~FMD_LF_EAOPEN;
	}

	(void) close(lp->log_fd);
	lp->log_fd = -1;

	(void) pthread_mutex_unlock(&lp->log_lock);
	return (nlp);
}
