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

#include <smbsrv/smb_incl.h>
#include <smbsrv/smb_fsops.h>
#include <smbsrv/smbinfo.h>
#include <sys/nbmlock.h>

static int smb_delete_check_path(smb_request_t *, boolean_t *);
static int smb_delete_single_file(smb_request_t *, smb_error_t *);
static int smb_delete_multiple_files(smb_request_t *, smb_error_t *);
static int smb_delete_find_fname(smb_request_t *, smb_odir_t *);
static int smb_delete_check_attr(smb_request_t *, smb_error_t *);
static int smb_delete_remove_file(smb_request_t *, smb_error_t *);

static void smb_delete_error(smb_error_t *, uint32_t, uint16_t, uint16_t);

/*
 * smb_com_delete
 *
 * The delete file message is sent to delete a data file. The appropriate
 * Tid and additional pathname are passed. Read only files may not be
 * deleted, the read-only attribute must be reset prior to file deletion.
 *
 * NT supports a hidden permission known as File Delete Child (FDC). If
 * the user has FullControl access to a directory, the user is permitted
 * to delete any object in the directory regardless of the permissions
 * on the object.
 *
 * Client Request                     Description
 * ================================== =================================
 * UCHAR WordCount;                   Count of parameter words = 1
 * USHORT SearchAttributes;
 * USHORT ByteCount;                  Count of data bytes; min = 2
 * UCHAR BufferFormat;                0x04
 * STRING FileName[];                 File name
 *
 * Multiple files may be deleted in response to a single request as
 * SMB_COM_DELETE supports wildcards
 *
 * SearchAttributes indicates the attributes that the target file(s) must
 * have. If the attribute is zero then only normal files are deleted. If
 * the system file or hidden attributes are specified then the delete is
 * inclusive -both the specified type(s) of files and normal files are
 * deleted. Attributes are described in the "Attribute Encoding" section
 * of this document.
 *
 * If bit0 of the Flags2 field of the SMB header is set, a pattern is
 * passed in, and the file has a long name, then the passed pattern  much
 * match the long file name for the delete to succeed. If bit0 is clear, a
 * pattern is passed in, and the file has a long name, then the passed
 * pattern must match the file's short name for the deletion to succeed.
 *
 * Server Response                    Description
 * ================================== =================================
 * UCHAR WordCount;                   Count of parameter words = 0
 * USHORT ByteCount;                  Count of data bytes = 0
 *
 * 4.2.10.1  Errors
 *
 * ERRDOS/ERRbadpath
 * ERRDOS/ERRbadfile
 * ERRDOS/ERRnoaccess
 * ERRDOS/ERRbadshare	# returned by NT for files that are already open
 * ERRHRD/ERRnowrite
 * ERRSRV/ERRaccess
 * ERRSRV/ERRinvdevice
 * ERRSRV/ERRinvid
 * ERRSRV/ERRbaduid
 */
smb_sdrc_t
smb_pre_delete(smb_request_t *sr)
{
	int rc;
	smb_fqi_t *fqi;

	fqi = &sr->arg.dirop.fqi;

	if ((rc = smbsr_decode_vwv(sr, "w", &fqi->srch_attr)) == 0)
		rc = smbsr_decode_data(sr, "%S", sr, &fqi->path);

	DTRACE_SMB_2(op__Delete__start, smb_request_t *, sr, smb_fqi_t *, fqi);

	return ((rc == 0) ? SDRC_SUCCESS : SDRC_ERROR);
}

void
smb_post_delete(smb_request_t *sr)
{
	DTRACE_SMB_1(op__Delete__done, smb_request_t *, sr);
}

/*
 * smb_com_delete
 *
 * 1. pre-process pathname -  smb_delete_check_path()
 *    checks dot, bad path syntax, wildcards in path
 *
 * 2. process the path to get directory node & last_comp,
 *    store these in fqi
 *    - If smb_pathname_reduce cannot find the specified path,
 *      the error (ENOTDIR) is translated to NT_STATUS_OBJECT_PATH_NOT_FOUND
 *      if the target is a single file (no wildcards).  If there are
 *      wildcards in the last_comp, NT_STATUS_OBJECT_NAME_NOT_FOUND is
 *      used instead.
 *    - If the directory node is the mount point and the last component
 *      is ".." NT_STATUS_OBJECT_PATH_SYNTAX_BAD is returned.
 *
 * 3. check access permissions
 *
 * 4. invoke the appropriate deletion routine to find and remove
 *    the specified file(s).
 *    - if target is a single file (no wildcards) - smb_delete_single_file
 *    - if the target contains wildcards - smb_delete_multiple_files
 *
 * Returns: SDRC_SUCCESS or SDRC_ERROR
 */
smb_sdrc_t
smb_com_delete(smb_request_t *sr)
{
	int rc;
	smb_error_t err;
	uint32_t status;
	boolean_t wildcards;
	smb_fqi_t *fqi;

	fqi = &sr->arg.dirop.fqi;

	if (smb_delete_check_path(sr, &wildcards) != 0)
		return (SDRC_ERROR);

	rc = smb_pathname_reduce(sr, sr->user_cr, fqi->path,
	    sr->tid_tree->t_snode, sr->tid_tree->t_snode,
	    &fqi->dir_snode, fqi->last_comp);
	if (rc == 0) {
		if (fqi->dir_snode->vp->v_type != VDIR) {
			smb_node_release(fqi->dir_snode);
			rc = ENOTDIR;
		}
	}
	if (rc != 0) {
		if (rc == ENOTDIR) {
			if (wildcards)
				status = NT_STATUS_OBJECT_NAME_NOT_FOUND;
			else
				status = NT_STATUS_OBJECT_PATH_NOT_FOUND;
			smbsr_error(sr, status, ERRDOS, ERROR_FILE_NOT_FOUND);
		} else {
			smbsr_errno(sr, rc);
		}

		return (SDRC_ERROR);
	}

	if ((fqi->dir_snode == sr->tid_tree->t_snode) &&
	    (strcmp(fqi->last_comp, "..") == 0)) {
		smb_node_release(fqi->dir_snode);
		smbsr_error(sr, NT_STATUS_OBJECT_PATH_SYNTAX_BAD,
		    ERRDOS, ERROR_BAD_PATHNAME);
		return (SDRC_ERROR);
	}

	rc = smb_fsop_access(sr, sr->user_cr, fqi->dir_snode,
	    FILE_LIST_DIRECTORY);
	if (rc != 0) {
		smb_node_release(fqi->dir_snode);
		smbsr_error(sr, NT_STATUS_ACCESS_DENIED,
		    ERRDOS, ERROR_ACCESS_DENIED);
		return (SDRC_ERROR);
	}

	if (wildcards)
		rc = smb_delete_multiple_files(sr, &err);
	else
		rc = smb_delete_single_file(sr, &err);

	smb_node_release(fqi->dir_snode);

	if (rc != 0)
		smbsr_set_error(sr, &err);
	else
		rc = smbsr_encode_empty_result(sr);

	return (rc == 0 ? SDRC_SUCCESS : SDRC_ERROR);
}

/*
 * smb_delete_single_file
 *
 * Find the specified file and, if its attributes match the search
 * criteria, delete it.
 *
 * Returns 0 - success (file deleted)
 *        -1 - error, err is populated with error details
 */
static int
smb_delete_single_file(smb_request_t *sr, smb_error_t *err)
{
	smb_fqi_t *fqi;
	smb_attr_t ret_attr;
	uint32_t status;

	fqi = &sr->arg.dirop.fqi;

	status = smb_validate_object_name(fqi->path, 0);
	if (status != NT_STATUS_SUCCESS) {
		smb_delete_error(err, status, ERRDOS, ERROR_INVALID_NAME);
		return (-1);
	}

	if (smb_fsop_lookup_name(sr, sr->user_cr, 0, sr->tid_tree->t_snode,
	    fqi->dir_snode, fqi->last_comp, &fqi->last_snode, &ret_attr) != 0) {
		smb_delete_error(err, NT_STATUS_OBJECT_NAME_NOT_FOUND,
		    ERRDOS, ERROR_FILE_NOT_FOUND);
		return (-1);
	}

	if (smb_delete_check_attr(sr, err) != 0) {
		smb_node_release(fqi->last_snode);
		return (-1);
	}

	if (smb_delete_remove_file(sr, err) != 0) {
		smb_node_release(fqi->last_snode);
		return (-1);
	}

	smb_node_release(fqi->last_snode);
	return (0);
}

/*
 * smb_delete_multiple_files
 *
 * For each matching file found by smb_delete_find_fname:
 * 1. lookup file
 * 2. check the file's attributes
 *    - The search ends with an error if a readonly file
 *      (NT_STATUS_CANNOT_DELETE) is matched.
 *    - The search ends (but not an error) if a directory is
 *      matched and the request's search did not include
 *      directories.
 *    - Otherwise, if smb_delete_check_attr fails the file
 *      is skipped and the search continues (at step 1)
 * 3. delete the file
 *
 * Returns 0 - success
 *        -1 - error, err is populated with error details
 */
static int
smb_delete_multiple_files(smb_request_t *sr, smb_error_t *err)
{
	int rc, deleted = 0;
	smb_fqi_t *fqi;
	smb_attr_t ret_attr;
	uint16_t odid;
	smb_odir_t *od;

	fqi = &sr->arg.dirop.fqi;

	/*
	 * Specify all search attributes (SMB_SEARCH_ATTRIBUTES) so that
	 * delete-specific checking can be done (smb_delete_check_attr).
	 */
	if ((odid = smb_odir_open(sr, fqi->path, SMB_SEARCH_ATTRIBUTES)) == 0)
		return (-1);
	if ((od = smb_tree_lookup_odir(sr->tid_tree, odid)) == NULL)
		return (-1);

	for (;;) {
		rc = smb_delete_find_fname(sr, od);
		if (rc != 0)
			break;

		rc = smb_fsop_lookup_name(sr, sr->user_cr, 0,
		    sr->tid_tree->t_snode, fqi->dir_snode,
		    fqi->last_comp_od, &fqi->last_snode, &ret_attr);
		if (rc != 0)
			break;

		if (smb_delete_check_attr(sr, err) != 0) {
			smb_node_release(fqi->last_snode);
			if (err->status == NT_STATUS_CANNOT_DELETE) {
				smb_odir_release(od);
				smb_odir_close(od);
				return (-1);
			}
			if ((err->status == NT_STATUS_FILE_IS_A_DIRECTORY) &&
			    (SMB_SEARCH_DIRECTORY(fqi->srch_attr) != 0))
				break;
			continue;
		}

		if (smb_delete_remove_file(sr, err) == 0) {
			++deleted;
			smb_node_release(fqi->last_snode);
			continue;
		}
		if (err->status == NT_STATUS_OBJECT_NAME_NOT_FOUND) {
			smb_node_release(fqi->last_snode);
			continue;
		}

		smb_odir_release(od);
		smb_odir_close(od);
		smb_node_release(fqi->last_snode);
		return (-1);
	}

	smb_odir_release(od);
	smb_odir_close(od);

	if ((rc != 0) && (rc != ENOENT)) {
		smbsr_map_errno(rc, err);
		return (-1);
	}

	if (deleted == 0) {
		smb_delete_error(err, NT_STATUS_NO_SUCH_FILE,
		    ERRDOS, ERROR_FILE_NOT_FOUND);
		return (-1);
	}

	return (0);
}

/*
 * smb_delete_find_fname
 *
 * Find next filename that matches search pattern (fqi->last_comp)
 * and save it in fqi->last_comp_od.
 *
 * Case insensitivity note:
 * If the tree is case insensitive and there's a case conflict
 * with the name returned from smb_odir_read, smb_delete_find_fname
 * performs case conflict name mangling to produce a unique filename.
 * This ensures that any subsequent smb_fsop_lookup, (which will
 * find the first case insensitive match) will find the correct file.
 *
 * Returns: 0 - success
 *          errno
 */
static int
smb_delete_find_fname(smb_request_t *sr, smb_odir_t *od)
{
	int		rc;
	smb_odirent_t	*odirent;
	boolean_t	eos;
	char		*name;
	char		shortname[SMB_SHORTNAMELEN];
	char		name83[SMB_SHORTNAMELEN];
	smb_fqi_t	*fqi;

	fqi = &sr->arg.dirop.fqi;
	odirent = kmem_alloc(sizeof (smb_odirent_t), KM_SLEEP);

	rc = smb_odir_read(sr, od, odirent, &eos);
	if (rc != 0) {
		kmem_free(odirent, sizeof (smb_odirent_t));
		return (rc);
	}
	if (eos) {
		kmem_free(odirent, sizeof (smb_odirent_t));
		return (ENOENT);
	}

	/* if case conflict, force mangle and use shortname */
	if ((od->d_ignore_case) && (odirent->od_eflags & ED_CASE_CONFLICT)) {
		(void) smb_mangle_name(odirent->od_ino, odirent->od_name,
		    shortname, name83, 1);
		name = shortname;
	} else {
		name = odirent->od_name;
	}
	(void) strlcpy(fqi->last_comp_od, name, sizeof (fqi->last_comp_od));

	kmem_free(odirent, sizeof (smb_odirent_t));
	return (0);
}

/*
 * smb_delete_check_attr
 *
 * Check file's dos atributes to ensure that
 * 1. the file is not a directory - NT_STATUS_FILE_IS_A_DIRECTORY
 * 2. the file is not readonly - NT_STATUS_CANNOT_DELETE
 * 3. the file's dos attributes comply with the specified search attributes
 *     If the file is either hidden or system and those attributes
 *     are not specified in the search attributes - NT_STATUS_NO_SUCH_FILE
 *
 * Returns: 0 - file's attributes pass all checks
 *         -1 - err populated with error details
 */
static int
smb_delete_check_attr(smb_request_t *sr, smb_error_t *err)
{
	smb_fqi_t *fqi;
	smb_node_t *node;
	uint16_t dosattr, sattr;

	fqi = &sr->arg.dirop.fqi;
	sattr = fqi->srch_attr;
	node = fqi->last_snode;
	dosattr = smb_node_get_dosattr(node);

	if (dosattr & FILE_ATTRIBUTE_DIRECTORY) {
		smb_delete_error(err, NT_STATUS_FILE_IS_A_DIRECTORY,
		    ERRDOS, ERROR_ACCESS_DENIED);
		return (-1);
	}

	if (SMB_PATHFILE_IS_READONLY(sr, node)) {
		smb_delete_error(err, NT_STATUS_CANNOT_DELETE,
		    ERRDOS, ERROR_ACCESS_DENIED);
		return (-1);
	}

	if ((dosattr & FILE_ATTRIBUTE_HIDDEN) && !(SMB_SEARCH_HIDDEN(sattr))) {
		smb_delete_error(err, NT_STATUS_NO_SUCH_FILE,
		    ERRDOS, ERROR_FILE_NOT_FOUND);
		return (-1);
	}

	if ((dosattr & FILE_ATTRIBUTE_SYSTEM) && !(SMB_SEARCH_SYSTEM(sattr))) {
		smb_delete_error(err, NT_STATUS_NO_SUCH_FILE,
		    ERRDOS, ERROR_FILE_NOT_FOUND);
		return (-1);
	}

	return (0);
}

/*
 * smb_delete_remove_file
 *
 * For consistency with Windows 2000, the range check should be done
 * after checking for sharing violations.  Attempting to delete a
 * locked file will result in sharing violation, which is the same
 * thing that will happen if you try to delete a non-locked open file.
 *
 * Note that windows 2000 rejects lock requests on open files that
 * have been opened with metadata open modes.  The error is
 * STATUS_ACCESS_DENIED.
 *
 * NT does not always close a file immediately, which can cause the
 * share and access checking to fail (the node refcnt is greater
 * than one), and the file doesn't get deleted. Breaking the oplock
 * before share and access checking gives the client a chance to
 * close the file.
 *
 * Returns: 0 - success
 *         -1 - error, err populated with error details
 */
static int
smb_delete_remove_file(smb_request_t *sr, smb_error_t *err)
{
	int rc;
	uint32_t status;
	smb_fqi_t *fqi;
	smb_node_t *node;

	fqi = &sr->arg.dirop.fqi;
	node = fqi->last_snode;

	(void) smb_oplock_break(node, SMB_SESSION_GET_ID(sr->session), B_FALSE);

	smb_node_start_crit(node, RW_READER);

	status = smb_node_delete_check(node);
	if (status != NT_STATUS_SUCCESS) {
		smb_delete_error(err, NT_STATUS_SHARING_VIOLATION,
		    ERRDOS, ERROR_SHARING_VIOLATION);
		smb_node_end_crit(node);
		return (-1);
	}

	status = smb_range_check(sr, node, 0, UINT64_MAX, B_TRUE);
	if (status != NT_STATUS_SUCCESS) {
		smb_delete_error(err, NT_STATUS_ACCESS_DENIED,
		    ERRDOS, ERROR_ACCESS_DENIED);
		smb_node_end_crit(node);
		return (-1);
	}

	rc = smb_fsop_remove(sr, sr->user_cr, node->dir_snode,
	    node->od_name, 1);
	if (rc != 0) {
		if (rc == ENOENT)
			smb_delete_error(err, NT_STATUS_OBJECT_NAME_NOT_FOUND,
			    ERRDOS, ERROR_FILE_NOT_FOUND);
		else
			smbsr_map_errno(rc, err);

		smb_node_end_crit(node);
		return (-1);
	}

	smb_node_end_crit(node);
	return (0);
}


/*
 * smb_delete_check_path
 *
 * Perform initial validation on the pathname and last_comp.
 *
 * wildcards in path:
 * Wildcards in the path (excluding the last_comp) should result
 * in NT_STATUS_OBJECT_NAME_INVALID.
 *
 * bad path syntax:
 * On unix .. at the root of a file system links to the root. Thus
 * an attempt to lookup "/../../.." will be the same as looking up "/"
 * CIFs clients expect the above to result in
 * NT_STATUS_OBJECT_PATH_SYNTAX_BAD. It is currently not possible
 * (and questionable if it's desirable) to deal with all cases
 * but paths beginning with \\.. are handled. See bad_paths[].
 * Cases like "\\dir\\..\\.." will be caught and handled after the
 * pnreduce.  Cases like "\\dir\\..\\..\\filename" will still result
 * in "\\filename" which is contrary to windows behavior.
 *
 * dot:
 * A filename of '.' should result in NT_STATUS_OBJECT_NAME_INVALID
 * Any wildcard filename that resolves to '.' should result in
 * NT_STATUS_OBJECT_NAME_INVALID if the search attributes include
 * FILE_ATTRIBUTE_DIRECTORY
 *
 * Returns:
 *   0:  path is valid. Sets *wildcard to TRUE if wildcard delete
 *	         i.e. if wildcards in last component
 *  -1: path is invalid. Sets error information in sr.
 */
static int
smb_delete_check_path(smb_request_t *sr, boolean_t *wildcard)
{
	smb_fqi_t *fqi = &sr->arg.dirop.fqi;
	char *p, *last_comp;
	int i, wildcards;

	struct {
		char *name;
		int len;
	} *bad, bad_paths[] = {
		{"\\..\0", 4},
		{"\\..\\", 4},
		{"..\0", 3},
		{"..\\", 3}
	};

	/* find last component, strip trailing '\\' */
	p = fqi->path + strlen(fqi->path) - 1;
	while (*p == '\\') {
		*p = '\0';
		--p;
	}

	if ((p = strrchr(fqi->path, '\\')) == NULL)
		last_comp = fqi->path;
	else
		last_comp = ++p;

	wildcards = smb_convert_wildcards(last_comp);

	if (last_comp != fqi->path) {
		/*
		 * Wildcards are only allowed in the last component.
		 * Check for additional wildcards in the path.
		 */
		if (smb_convert_wildcards(fqi->path) != wildcards) {
			smbsr_error(sr, NT_STATUS_OBJECT_NAME_INVALID,
			    ERRDOS, ERROR_INVALID_NAME);
			return (-1);
		}
	}

	/* path above the mount point */
	for (i = 0; i < sizeof (bad_paths) / sizeof (bad_paths[0]); ++i) {
		bad = &bad_paths[i];
		if (strncmp(fqi->path, bad->name, bad->len) == 0) {
			smbsr_error(sr, NT_STATUS_OBJECT_PATH_SYNTAX_BAD,
			    ERRDOS, ERROR_BAD_PATHNAME);
			return (-1);
		}
	}

	/* last component is, or resolves to, '.' (dot) */
	if ((strcmp(last_comp, ".") == 0) ||
	    (SMB_SEARCH_DIRECTORY(fqi->srch_attr) &&
	    (smb_match(last_comp, ".")))) {
		smbsr_error(sr, NT_STATUS_OBJECT_NAME_INVALID,
		    ERRDOS, ERROR_INVALID_NAME);
		return (-1);
	}

	*wildcard = (wildcards != 0);
	return (0);
}

/*
 * smb_delete_error
 */
static void
smb_delete_error(smb_error_t *err,
    uint32_t status, uint16_t errcls, uint16_t errcode)
{
	err->severity = ERROR_SEVERITY_ERROR;
	err->status = status;
	err->errcls = errcls;
	err->errcode = errcode;
}
