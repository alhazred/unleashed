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
 * Copyright 2010 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Fill in and write out the cpr state file
 *	1. Allocate and write headers, ELF and cpr dump header
 *	2. Allocate bitmaps according to phys_install
 *	3. Tag kernel pages into corresponding bitmap
 *	4. Write bitmaps to state file
 *	5. Write actual physical page data to state file
 */

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/vm.h>
#include <sys/memlist.h>
#include <sys/kmem.h>
#include <sys/vnode.h>
#include <sys/fs/ufs_inode.h>
#include <sys/errno.h>
#include <sys/cmn_err.h>
#include <sys/debug.h>
#include <vm/page.h>
#include <vm/seg.h>
#include <vm/seg_kmem.h>
#include <vm/seg_kpm.h>
#include <vm/hat.h>
#include <sys/cpr.h>
#include <sys/conf.h>
#include <sys/ddi.h>
#include <sys/panic.h>
#include <sys/thread.h>
#include <sys/note.h>

/* Local defines and variables */
#define	BTOb(bytes)	((bytes) << 3)		/* Bytes to bits, log2(NBBY) */
#define	bTOB(bits)	((bits) >> 3)		/* bits to Bytes, log2(NBBY) */


int cpr_flush_write(vnode_t *);

int cpr_contig_pages(vnode_t *, int);

void cpr_clear_bitmaps();

extern size_t cpr_get_devsize(dev_t);
extern int i_cpr_dump_setup(vnode_t *);
extern int i_cpr_blockzero(char *, char **, int *, vnode_t *);
extern int cpr_test_mode;
int cpr_setbit(pfn_t, int);
int cpr_clrbit(pfn_t, int);

ctrm_t cpr_term;

char *cpr_buf, *cpr_buf_end;
int cpr_buf_blocks;		/* size of cpr_buf in blocks */
size_t cpr_buf_size;		/* size of cpr_buf in bytes */
size_t cpr_bitmap_size;
int cpr_nbitmaps;

char *cpr_pagedata;		/* page buffer for compression / tmp copy */
size_t cpr_pagedata_size;	/* page buffer size in bytes */


char cpr_pagecopy[CPR_MAXCONTIG * MMU_PAGESIZE];



/*
 * creates the CPR state file, the following sections are
 * written out in sequence:
 *    - writes the cpr dump header
 *    - writes the memory usage bitmaps
 *    - writes the platform dependent info
 *    - writes the remaining user pages
 *    - writes the kernel pages
 */
#if defined(__x86)
	_NOTE(ARGSUSED(0))
#endif
int
cpr_dump(vnode_t *vp)
{

	return (0);
}


