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
enum printcode {
	NOV = 0,		/* no value */
	DEC,		/* print value in decimal */
	OCT,		/* print value in octal */
	HEX,		/* print value in hexadecimal */
	DEX,		/* print value in hexadecimal if big enough */
	STG,		/* print value as string */
	IOC,		/* print ioctl code */
	FCN,		/* print fcntl code */
	S86,		/* print sysi86 code */
	UTS,		/* print utssys code */
	OPN,		/* print open code */
	SIG,		/* print signal name plus flags */
	UAT,		/* print unlinkat() flag */
	MSC,		/* print msgsys command */
	MSF,		/* print msgsys flags */
	SMC,		/* print semsys command */
	SEF,		/* print semsys flags */
	SHC,		/* print shmsys command */
	SHF,		/* print shmsys flags */
	FAT,		/* print faccessat() flag */
	SFS,		/* print sysfs code */
	RST,		/* print string returned by sys call */
	SMF,		/* print streams message flags */
	IOA,		/* print ioctl argument */
	PIP,		/* print pipe flags */
	MTF,		/* print mount flags */
	MFT,		/* print mount file system type */
	IOB,		/* print contents of I/O buffer */
	HHX,		/* print value in hexadecimal (half size) */
	WOP,		/* print waitsys() options */
	SPM,		/* print sigprocmask argument */
	RLK,		/* print readlink buffer */
	MPR,		/* print mmap()/mprotect() flags */
	MTY,		/* print mmap() mapping type flags */
	MCF,		/* print memcntl() function */
	MC4,		/* print memcntl() (fourth) argument */
	MC5,		/* print memcntl() (fifth) argument */
	MAD,		/* print madvise() argument */
	ULM,		/* print ulimit() argument */
	RLM,		/* print get/setrlimit() argument */
	CNF,		/* print sysconfig() argument */
	INF,		/* print sysinfo() argument */
	PTC,		/* print pathconf/fpathconf() argument */
	FUI,		/* print fusers() input argument */
	IDT,		/* print idtype_t, waitid() argument */
	LWF,		/* print lwp_create() flags */
	ITM,		/* print [get|set]itimer() arg */
	LLO,		/* print long long offset */
	MOD,		/* print modctl() code */
	WHN,		/* print lseek() whence argument */
	ACL,		/* print acl() code */
	AIO,		/* print kaio() code */
	UNS,		/* print value in unsigned decimal */
	COR,		/* print corectl() subcode */
	CCO,		/* print corectl() options */
	CCC,		/* print corectl() content */
	RCC,		/* print corectl() content */
	CPC,		/* print cpc() subcode */
	SQC,		/* print sigqueue() si_code argument */
	PC4,		/* print priocntlsys() (fourth) argument */
	PC5,		/* print priocntlsys() (key-value) pairs */
	PST,		/* print processor set id */
	MIF,		/* print meminfo() argument */
	PFM,		/* print so_socket() proto-family (1st) arg */
	SKT,		/* print so_socket() socket type (2nd) arg */
	SKP,		/* print so_socket() protocol (3rd) arg */
	SKV,		/* print so_socket() version (5th) arg */
	SOL,		/* print [sg]etsockopt() level (2nd) arg */
	SON,		/* print [sg]etsockopt() name (3rd) arg */
	UTT,		/* print utrap type */
	UTH,		/* print utrap handler */
	ACC,		/* print access flags */
	SHT,		/* print shutdown() "how" (2nd) arg */
	FFG,		/* print fcntl() flags (3rd) arg */
	PRS,		/* privilege set */
	PRO,		/* privilege set operation */
	PRN,		/* privilege set name */
	PFL,		/* privilege/process flag name */
	LAF,		/* print lgrp_affinity arguments */
	KEY,		/* print key_t 0 as IPC_PRIVATE */
	ZGA,		/* print zone_getattr attribute types */
	ATC,		/* print AT_FDCWD or file descriptor */
	LIO,		/* print LIO_XX flags */
	DFL,		/* print door_create() flags */
	DPM,		/* print DOOR_PARAM_XX flags */
	TND,		/* print trusted network data base opcode */
	RSC,		/* print rctlsys subcode */
	RGF,		/* print rctlsys_get flags */
	RSF,		/* print rctlsys_set flags */
	RCF,		/* print rctlsys_ctl flags */
	FXF,		/* print forkx flags */
	SPF,		/* print rctlsys_projset flags */
	UN1,		/* unsigned except for -1 */
	MOB,		/* print mmapobj() flags */
	SNF,		/* print AT_SYMLINK_[NO]FOLLOW flag */
	SKC,		/* print sockconfig subcode */
	ACF,		/* accept4 flags */
	PFD,		/* pipe fds[2] */
	GRF,		/* getrandom flags */
	PSDLT,		/* secflagsdelta_t */
	PSFW,		/* psecflagswhich_t */
	HID		/* hidden argument, don't print */
			/* make sure HID is always the last member */
};

/*
 * Print routines, indexed by print codes.
 */
extern void (* const Print[])();

#ifdef	__cplusplus
}
#endif

#endif	/* _TRUSS_PRINT_H */
