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
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved	*/


/*
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SYS_ELFTYPES_H
#define	_SYS_ELFTYPES_H

#include <stdint.h>

typedef uint32_t		Elf32_Addr;
typedef uint16_t		Elf32_Half;
typedef uint32_t		Elf32_Off;
typedef int32_t			Elf32_Sword;
typedef uint32_t		Elf32_Word;

typedef uint64_t		Elf64_Addr;
typedef uint16_t		Elf64_Half;
typedef uint64_t		Elf64_Off;
typedef int32_t			Elf64_Sword;
typedef int64_t			Elf64_Sxword;
typedef	uint32_t		Elf64_Word;
typedef	uint64_t		Elf64_Xword;
typedef uint64_t		Elf64_Lword;
typedef uint64_t		Elf32_Lword;

#endif	/* _SYS_ELFTYPES_H */
