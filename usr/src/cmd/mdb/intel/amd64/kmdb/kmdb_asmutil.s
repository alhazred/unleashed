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
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"%Z%%M%	%I%	%E% SMI"


#include <sys/asm_linkage.h>


	ENTRY_NP(cas)
	movq	%rsi, %rax
	lock
	  cmpxchgq %rdx, (%rdi)
	ret
	SET_SIZE(cas)


	ENTRY(membar_producer)
	sfence
	ret
	SET_SIZE(membar_producer)



	ENTRY(rdmsr)
	movl	%edi, %ecx
	rdmsr
	movl	%eax, (%rsi)
	movl	%edx, 4(%rsi)
	ret
	SET_SIZE(rdmsr)



	ENTRY(wrmsr)
	movl	(%rsi), %eax
	movl	4(%rsi), %edx
	movl	%edi, %ecx
	wrmsr
	ret
	SET_SIZE(wrmsr)



	ENTRY(get_fp)
	movq	%rbp, %rax
	ret
	SET_SIZE(get_fp)



	ENTRY_NP(kmt_in)
	cmpq	$4, %rsi
	je	4f	
	cmpq	$2, %rsi
	je	2f

1:	inb	(%dx)
	movb	%al, 0(%rdi)
	ret

2:	inw	(%dx)
	movw	%ax, 0(%rdi)
	ret

4:	inl	(%dx)
	movl	%eax, 0(%rdi)
	ret
	SET_SIZE(kmt_in)

	ENTRY_NP(kmt_out)
	cmpq	$4, %rsi
	je	4f
	cmpq	$2, %rsi
	je	2f

1:	movb	0(%rdi), %al
	outb	(%dx)
	ret

2:	movw	0(%rdi), %ax
	outw	(%dx)
	ret

4:	movl	0(%rdi), %eax
	outl	(%dx)
	ret
	SET_SIZE(kmt_out)

