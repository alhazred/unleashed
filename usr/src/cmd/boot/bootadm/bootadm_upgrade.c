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
 * Copyright 2016 Toomas Soome <tsoome@me.com>
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <strings.h>

#include <sys/mman.h>
#include <sys/elf.h>
#include <sys/multiboot.h>

#include "bootadm.h"

direct_or_multi_t bam_direct = BAM_DIRECT_NOT_SET;
hv_t bam_is_hv = BAM_HV_UNKNOWN;
findroot_t bam_is_findroot = BAM_FINDROOT_PRESENT;

error_t
get_boot_cap(const char *osroot)
{
	char		fname[PATH_MAX];
	char		*image;
	uchar_t		*ident;
	int		fd;
	int		m;
	multiboot_header_t *mbh;
	struct stat	sb;
	int		error;
	const char	*fcn = "get_boot_cap()";

	if (is_sparc()) {
		/* there is no non dboot sparc new-boot */
		bam_direct = BAM_DIRECT_DBOOT;
		BAM_DPRINTF(("%s: is sparc - always DBOOT\n", fcn));
		return (BAM_SUCCESS);
	}

	(void) snprintf(fname, PATH_MAX, "%s/%s", osroot,
	    "platform/i86pc/kernel/unix");
	fd = open(fname, O_RDONLY);
	error = errno;
	INJECT_ERROR1("GET_CAP_UNIX_OPEN", fd = -1);
	if (fd < 0) {
		bam_error(_("failed to open file: %s: %s\n"), fname,
		    strerror(error));
		return (BAM_ERROR);
	}

	/*
	 * Verify that this is a sane unix at least 8192 bytes in length
	 */
	if (fstat(fd, &sb) == -1 || sb.st_size < 8192) {
		(void) close(fd);
		bam_error(_("invalid or corrupted binary: %s\n"), fname);
		return (BAM_ERROR);
	}

	/*
	 * mmap the first 8K
	 */
	image = mmap(NULL, 8192, PROT_READ, MAP_SHARED, fd, 0);
	error = errno;
	INJECT_ERROR1("GET_CAP_MMAP", image = MAP_FAILED);
	if (image == MAP_FAILED) {
		bam_error(_("failed to mmap file: %s: %s\n"), fname,
		    strerror(error));
		return (BAM_ERROR);
	}

	ident = (uchar_t *)image;
	if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 ||
	    ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3) {
		bam_error(_("%s is not an ELF file.\n"), fname);
		return (BAM_ERROR);
	}
	if (ident[EI_CLASS] != ELFCLASS32) {
		bam_error(_("%s is wrong ELF class 0x%x\n"), fname,
		    ident[EI_CLASS]);
		return (BAM_ERROR);
	}

	/*
	 * The GRUB multiboot header must be 32-bit aligned and completely
	 * contained in the 1st 8K of the file.  If the unix binary has
	 * a multiboot header, then it is a 'dboot' kernel.  Otherwise,
	 * this kernel must be booted via multiboot -- we call this a
	 * 'multiboot' kernel.
	 */
	bam_direct = BAM_DIRECT_MULTIBOOT;
	for (m = 0; m < 8192 - sizeof (multiboot_header_t); m += 4) {
		mbh = (void *)(image + m);
		if (mbh->magic == MB_HEADER_MAGIC) {
			BAM_DPRINTF(("%s: is DBOOT unix\n", fcn));
			bam_direct = BAM_DIRECT_DBOOT;
			break;
		}
	}
	(void) munmap(image, 8192);
	(void) close(fd);

	INJECT_ERROR1("GET_CAP_MULTIBOOT", bam_direct = BAM_DIRECT_MULTIBOOT);
	if (bam_direct == BAM_DIRECT_DBOOT) {
		if (bam_is_hv == BAM_HV_PRESENT) {
			BAM_DPRINTF(("%s: is xVM system\n", fcn));
		} else {
			BAM_DPRINTF(("%s: is *NOT* xVM system\n", fcn));
		}
	} else {
		BAM_DPRINTF(("%s: is MULTIBOOT unix\n", fcn));
	}

	BAM_DPRINTF(("%s: returning SUCCESS\n", fcn));
	return (BAM_SUCCESS);
}
