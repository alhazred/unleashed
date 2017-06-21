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
 * Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_VFS_OPREG_H
#define	_SYS_VFS_OPREG_H

#include <sys/vfs.h>

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _KERNEL

/*
 * The following union allows us to use C99's "designated initializer"
 * feature so that we can have strong typechecking for the operations
 * used in the the fs_operation_def structures.
 */

typedef union fs_func {
	fs_generic_func_p fs_generic;	/* Generic function signature */
	int (*error)();			/* Signature of error function */
	VFS_OPS;		/* Signatures of all vfs operations (vfsops) */
} fs_func_p;

/*
 * File systems use arrays of fs_operation_def structures to form
 * name/value pairs of operations.  These arrays get passed to:
 *
 * 	- vfs_setfsops() to create vfsops.
 */
typedef struct fs_operation_def {
	char *name;			/* name of operation (NULL at end) */
	fs_func_p func;			/* function implementing operation */
} fs_operation_def_t;

/*
 * The operation registration mechanism uses a master table of operations:
 * one for vfs operations (vfs_ops_table[]).  This table is an array of
 * fs_operation_trans_def structures.  It contains all of the information
 * necessary for the system to populate an operations structure (e.g.,
 * vfsops).
 *
 * File systems call registration routines (vfs_setfsops()) and pass in
 * their operations specification tables (arrays of fs_operation_def
 * structures).  These routines use the master table(s) of operations to
 * build a vnodeops or vfsops structure.
 */
typedef struct fs_operation_trans_def {
	char *name;			/* name of operation (NULL at end) */
	int offset;			/* byte offset within ops vector */
	fs_generic_func_p defaultFunc;	/* default function */
} fs_operation_trans_def_t;

int fs_build_vector(void *vector, int *unused_ops,
    const fs_operation_trans_def_t *translation,
    const fs_operation_def_t *operations);

/*
 * Public operations.
 */

int	vfs_setfsops(int, const fs_operation_def_t *, vfsops_t **);
int	vfs_setfsops_const(int, const struct vfsops *);
int	vfs_freevfsops_by_type(int);

#endif /* _KERNEL */

#ifdef	__cplusplus
}
#endif

#endif	/* _SYS_VFS_OPREG_H */
