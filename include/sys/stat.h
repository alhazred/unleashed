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
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 * Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
 */

/*	Copyright (c) 1990, 1991 UNIX System Laboratories, Inc.	*/
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	All Rights Reserved	*/

#ifndef _SYS_STAT_H
#define	_SYS_STAT_H

#include <sys/feature_tests.h>
#include <sys/types.h>
#include <sys/time_impl.h>

#define	_ST_FSTYPSZ 16		/* array size for file system type name */

struct stat {
	dev_t		st_dev;
	ino_t		st_ino;
	mode_t		st_mode;
	nlink_t		st_nlink;
	uid_t		st_uid;
	gid_t		st_gid;
	dev_t		st_rdev;
	off_t		st_size;
	struct timespec	st_atim;
	struct timespec	st_mtim;
	struct timespec	st_ctim;
	blksize_t	st_blksize;
	blkcnt_t	st_blocks;
	char		st_fstype[_ST_FSTYPSZ];
};

#if defined(_KERNEL) && defined(_SYSCALL32_IMPL)
/*
 * 32-bit process' stat struct, as viewed by the kernel.
 *
 * this is almost exactly same as above, with 3 differences:
 *  - dev_t (ulong_t) vs. dev32_t (uint32_t)
 *  - struct timespec vs. struct timespec32
 *  - member alignment (struct packing)
 * if the first two differences were fixed, we could swap this around by always
 * using the same struct, *but* with aligned(8) for 32-bit user processes.
 */
struct stat32 {
	dev32_t		st_dev;
	ino_t		st_ino;
	mode_t		st_mode;
	nlink_t		st_nlink;
	uid_t		st_uid;
	gid_t		st_gid;
	dev32_t		st_rdev;
	off_t		st_size;
	struct timespec32	st_atim;
	struct timespec32	st_mtim;
	struct timespec32	st_ctim;
	blksize_t	st_blksize;
	blkcnt_t	st_blocks;
	char		st_fstype[_ST_FSTYPSZ];
} __attribute__((packed));
#endif /* defined(_KERNEL) && defined(_SYSCALL32_IMPL) */

#define	st_atime	st_atim.tv_sec
#define	st_mtime	st_mtim.tv_sec
#define	st_ctime	st_ctim.tv_sec

/* MODE MASKS */

/* de facto standard definitions */

#define	S_IFMT		0xF000	/* type of file */
#define	S_IAMB		0x1FF	/* access mode bits */
#define	S_IFIFO		0x1000	/* fifo */
#define	S_IFCHR		0x2000	/* character special */
#define	S_IFDIR		0x4000	/* directory */
/* XENIX definitions are not relevant to Solaris */
#define	S_IFNAM		0x5000  /* XENIX special named file */
#define	S_INSEM		0x1	/* XENIX semaphore subtype of IFNAM */
#define	S_INSHD		0x2	/* XENIX shared data subtype of IFNAM */
#define	S_IFBLK		0x6000	/* block special */
#define	S_IFREG		0x8000	/* regular */
#define	S_IFLNK		0xA000	/* symbolic link */
#define	S_IFSOCK	0xC000	/* socket */
#define	S_IFDOOR	0xD000	/* door */
#define	S_IFPORT	0xE000	/* event port */
#define	S_ISUID		0x800	/* set user id on execution */
#define	S_ISGID		0x400	/* set group id on execution */
#define	S_ISVTX		0x200	/* save swapped text even after use */
#define	S_IREAD		00400	/* read permission, owner */
#define	S_IWRITE	00200	/* write permission, owner */
#define	S_IEXEC		00100	/* execute/search permission, owner */
#define	S_ENFMT		S_ISGID	/* record locking enforcement flag */

/* the following macros are for POSIX conformance */

#define	S_IRWXU		00700	/* read, write, execute: owner */
#define	S_IRUSR		00400	/* read permission: owner */
#define	S_IWUSR		00200	/* write permission: owner */
#define	S_IXUSR		00100	/* execute permission: owner */
#define	S_IRWXG		00070	/* read, write, execute: group */
#define	S_IRGRP		00040	/* read permission: group */
#define	S_IWGRP		00020	/* write permission: group */
#define	S_IXGRP		00010	/* execute permission: group */
#define	S_IRWXO		00007	/* read, write, execute: other */
#define	S_IROTH		00004	/* read permission: other */
#define	S_IWOTH		00002	/* write permission: other */
#define	S_IXOTH		00001	/* execute permission: other */

#define	S_ISFIFO(mode)	(((mode)&0xF000) == 0x1000)
#define	S_ISCHR(mode)	(((mode)&0xF000) == 0x2000)
#define	S_ISDIR(mode)	(((mode)&0xF000) == 0x4000)
#define	S_ISBLK(mode)	(((mode)&0xF000) == 0x6000)
#define	S_ISREG(mode)	(((mode)&0xF000) == 0x8000)
#define	S_ISLNK(mode)	(((mode)&0xF000) == 0xa000)
#define	S_ISSOCK(mode)	(((mode)&0xF000) == 0xc000)
#define	S_ISDOOR(mode)	(((mode)&0xF000) == 0xd000)
#define	S_ISPORT(mode)	(((mode)&0xF000) == 0xe000)

/* POSIX.4 macros */
#define	S_TYPEISMQ(_buf)	(0)
#define	S_TYPEISSEM(_buf)	(0)
#define	S_TYPEISSHM(_buf)	(0)

#if __POSIX_VISIBLE >= 200809 || defined(_KERNEL)
#define UTIME_NOW	-1L
#define UTIME_OMIT	-2L
#endif

#if !defined(_KERNEL)

#ifdef __cplusplus
extern "C" {
#endif

int chmod(const char *, mode_t);
int fstat(int, struct stat *);
int mkdir(const char *, mode_t);
int mkfifo(const char *, mode_t);
int stat(const char *_RESTRICT_KYWD, struct stat *_RESTRICT_KYWD);
mode_t umask(mode_t);
#if __XPG_VISIBLE >= 420
int fchmod(int, mode_t);
int lstat(const char *_RESTRICT_KYWD, struct stat *_RESTRICT_KYWD);
int mknod(const char *, mode_t, dev_t);
#endif
#if __POSIX_VISIBLE >= 200809
int fstatat(int, const char *, struct stat *, int);
int fchmodat(int, const char *, mode_t, int);
int futimens(int, const struct timespec[2]);
int mkdirat(int, const char *, mode_t);
int mkfifoat(int, const char *, mode_t);
int mknodat(int, const char *, mode_t, dev_t);
int utimensat(int, const char *, const struct timespec[2], int);
#endif

#ifdef	__cplusplus
}
#endif

#endif /* !defined(_KERNEL) */
#endif /* _SYS_STAT_H */
