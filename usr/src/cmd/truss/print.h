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
 * Copyright (c) 1989, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2015, Joyent, Inc.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/* Copyright (c) 2013, OmniTI Computer Consulting, Inc. All right reserved. */

#ifndef	_TRUSS_PRINT_H
#define	_TRUSS_PRINT_H

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Argument & return value print codes.
 */
#define	NOV	0		/* no value */
#define	DEC	1		/* print value in decimal */
#define	OCT	2		/* print value in octal */
#define	HEX	3		/* print value in hexadecimal */
#define	DEX	4		/* print value in hexadecimal if big enough */
#define	STG	5		/* print value as string */
#define	IOC	6		/* print ioctl code */
#define	FCN	7		/* print fcntl code */
#define	S86	8		/* print sysi86 code */
#define	UTS	9		/* print utssys code */
#define	OPN	10		/* print open code */
#define	SIG	11		/* print signal name plus flags */
#define	UAT	12		/* print unlinkat() flag */
#define	MSC	13		/* print msgsys command */
#define	MSF	14		/* print msgsys flags */
#define	SMC	15		/* print semsys command */
#define	SEF	16		/* print semsys flags */
#define	SHC	17		/* print shmsys command */
#define	SHF	18		/* print shmsys flags */
#define	FAT	19		/* print faccessat() flag */
#define	SFS	20		/* print sysfs code */
#define	RST	21		/* print string returned by sys call */
#define	SMF	22		/* print streams message flags */
#define	IOA	23		/* print ioctl argument */
#define	PIP	24		/* print pipe flags */
#define	MTF	25		/* print mount flags */
#define	MFT	26		/* print mount file system type */
#define	IOB	27		/* print contents of I/O buffer */
#define	HHX	28		/* print value in hexadecimal (half size) */
#define	WOP	29		/* print waitsys() options */
#define	SPM	30		/* print sigprocmask argument */
#define	RLK	31		/* print readlink buffer */
#define	MPR	32		/* print mmap()/mprotect() flags */
#define	MTY	33		/* print mmap() mapping type flags */
#define	MCF	34		/* print memcntl() function */
#define	MC4	35		/* print memcntl() (fourth) argument */
#define	MC5	36		/* print memcntl() (fifth) argument */
#define	MAD	37		/* print madvise() argument */
#define	ULM	38		/* print ulimit() argument */
#define	RLM	39		/* print get/setrlimit() argument */
#define	CNF	40		/* print sysconfig() argument */
#define	INF	41		/* print sysinfo() argument */
#define	PTC	42		/* print pathconf/fpathconf() argument */
#define	FUI	43		/* print fusers() input argument */
#define	IDT	44		/* print idtype_t, waitid() argument */
#define	LWF	45		/* print lwp_create() flags */
#define	ITM	46		/* print [get|set]itimer() arg */
#define	LLO	47		/* print long long offset */
#define	MOD	48		/* print modctl() code */
#define	WHN	49		/* print lseek() whence argument */
#define	ACL	50		/* print acl() code */
#define	AIO	51		/* print kaio() code */
#define	AUD	52		/* print auditsys() code */
#define	UNS	53		/* print value in unsigned decimal */
#define	COR	54		/* print corectl() subcode */
#define	CCO	55		/* print corectl() options */
#define	CCC	56		/* print corectl() content */
#define	RCC	57		/* print corectl() content */
#define	CPC	58		/* print cpc() subcode */
#define	SQC	59		/* print sigqueue() si_code argument */
#define	PC4	60		/* print priocntlsys() (fourth) argument */
#define	PC5	61		/* print priocntlsys() (key-value) pairs */
#define	PST	62		/* print processor set id */
#define	MIF	63		/* print meminfo() argument */
#define	PFM	64		/* print so_socket() proto-family (1st) arg */
#define	SKT	65		/* print so_socket() socket type (2nd) arg */
#define	SKP	66		/* print so_socket() protocol (3rd) arg */
#define	SKV	67		/* print so_socket() version (5th) arg */
#define	SOL	68		/* print [sg]etsockopt() level (2nd) arg */
#define	SON	69		/* print [sg]etsockopt() name (3rd) arg */
#define	UTT	70		/* print utrap type */
#define	UTH	71		/* print utrap handler */
#define	ACC	72		/* print access flags */
#define	SHT	73		/* print shutdown() "how" (2nd) arg */
#define	FFG	74		/* print fcntl() flags (3rd) arg */
#define	PRS	75		/* privilege set */
#define	PRO	76		/* privilege set operation */
#define	PRN	77		/* privilege set name */
#define	PFL	78		/* privilege/process flag name */
#define	LAF	79		/* print lgrp_affinity arguments */
#define	KEY	80		/* print key_t 0 as IPC_PRIVATE */
#define	ZGA	81		/* print zone_getattr attribute types */
#define	ATC	82		/* print AT_FDCWD or file descriptor */
#define	LIO	83		/* print LIO_XX flags */
#define	DFL	84		/* print door_create() flags */
#define	DPM	85		/* print DOOR_PARAM_XX flags */
#define	TND	86		/* print trusted network data base opcode */
#define	RSC	87		/* print rctlsys subcode */
#define	RGF	88		/* print rctlsys_get flags */
#define	RSF	89		/* print rctlsys_set flags */
#define	RCF	90		/* print rctlsys_ctl flags */
#define	FXF	91		/* print forkx flags */
#define	SPF	92		/* print rctlsys_projset flags */
#define	UN1	93		/* unsigned except for -1 */
#define	MOB	94		/* print mmapobj() flags */
#define	SNF	95		/* print AT_SYMLINK_[NO]FOLLOW flag */
#define	SKC	96		/* print sockconfig subcode */
#define	ACF	97		/* accept4 flags */
#define	PFD	98		/* pipe fds[2] */
#define	GRF	99		/* getrandom flags */
#define	PSDLT	100		/* secflagsdelta_t */
#define	PSFW	101		/* psecflagswhich_t */
#define	HID	102		/* hidden argument, don't print */
				/* make sure HID is always the last member */

/*
 * Print routines, indexed by print codes.
 */
extern void (* const Print[])();

#ifdef	__cplusplus
}
#endif

#endif	/* _TRUSS_PRINT_H */
