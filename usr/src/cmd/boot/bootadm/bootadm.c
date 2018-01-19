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
 * Copyright (c) 2005, 2010, Oracle and/or its affiliates. All rights reserved.
 * Copyright 2012 Milan Jurik. All rights reserved.
 * Copyright (c) 2015 by Delphix. All rights reserved.
 * Copyright 2016 Toomas Soome <tsoome@me.com>
 * Copyright 2016 Nexenta Systems, Inc.
 */

/*
 * bootadm(1M) is a new utility for managing bootability of
 * Solaris *Newboot* environments. It has two primary tasks:
 * 	- Allow end users to manage bootability of Newboot Solaris instances
 *	- Provide services to other subsystems in Solaris (primarily Install)
 */

/* Headers */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <alloca.h>
#include <stdarg.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mnttab.h>
#include <sys/mntent.h>
#include <sys/statvfs.h>
#include <libnvpair.h>
#include <ftw.h>
#include <fcntl.h>
#include <strings.h>
#include <utime.h>
#include <sys/systeminfo.h>
#include <sys/dktp/fdisk.h>
#include <sys/param.h>
#include <dirent.h>
#include <ctype.h>
#include <libgen.h>
#include <sys/sysmacros.h>
#include <sys/elf.h>
#include <libscf.h>
#include <zlib.h>
#include <sys/lockfs.h>
#include <sys/filio.h>
#include <libbe.h>
#include <deflt.h>
#ifdef i386
#include <libfdisk.h>
#endif

#if !defined(_OBP)
#include <sys/ucode.h>
#endif

#include <pwd.h>
#include <grp.h>
#include <device_info.h>
#include <sys/vtoc.h>
#include <sys/efi_partition.h>
#include <regex.h>
#include <locale.h>
#include <sys/mkdev.h>

#include "bootadm.h"

#ifndef TEXT_DOMAIN
#define	TEXT_DOMAIN	"SUNW_OST_OSCMD"
#endif	/* TEXT_DOMAIN */

/* Type definitions */

/* Primary subcmds */
typedef enum {
	BAM_MENU = 3,
	BAM_ARCHIVE,
	BAM_INSTALL
} subcmd_t;

#define	LINE_INIT	0	/* lineNum initial value */
#define	ENTRY_INIT	-1	/* entryNum initial value */
#define	ALL_ENTRIES	-2	/* selects all boot entries */

#define	PARTNO_NOTFOUND -1	/* Solaris partition not found */
#define	PARTNO_EFI	-2	/* EFI partition table found */

#define	RAMDISK_SPECIAL		"/devices/ramdisk"
#define	ZFS_LEGACY_MNTPT	"/tmp/bootadm_mnt_zfs_legacy"

#define	BOOTADM_RDONLY_TEST	"BOOTADM_RDONLY_TEST"

/* lock related */
#define	BAM_LOCK_FILE		"/var/run/bootadm.lock"
#define	LOCK_FILE_PERMS		(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#define	CREATE_RAMDISK		"boot/solaris/bin/create_ramdisk"
#define	EXTRACT_BOOT_FILELIST	"boot/solaris/bin/extract_boot_filelist"

#define	GRUB_fdisk		"/etc/lu/GRUB_fdisk"
#define	GRUB_fdisk_target	"/etc/lu/GRUB_fdisk_target"

/*
 * exec_cmd related
 */
typedef struct {
	line_t *head;
	line_t *tail;
} filelist_t;

#define	BOOT_FILE_LIST	"boot/solaris/filelist.ramdisk"
#define	ETC_FILE_LIST	"etc/boot/solaris/filelist.ramdisk"

#define	FILE_STAT	"boot/solaris/filestat.ramdisk"
#define	FILE_STAT_TMP	"boot/solaris/filestat.ramdisk.tmp"
#define	DIR_PERMS	(S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define	FILE_STAT_MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#define	FILE_STAT_TIMESTAMP	"boot/solaris/timestamp.cache"

/* Globals */
int bam_verbose;
int bam_force;
int bam_debug;
static char *prog;
static subcmd_t bam_cmd;
char *bam_root;
int bam_rootlen;
static int bam_root_readonly;
int bam_alt_root;
static int bam_purge = 0;
static char *bam_subcmd;
static char *bam_opt;
static char **bam_argv;
static char *bam_pool;
static int bam_argc;
static int bam_check;
static int bam_saved_check;
static int bam_smf_check;
static int bam_lock_fd = -1;
static int bam_zfs;
static int bam_mbr;
char rootbuf[PATH_MAX] = "/";
static int bam_update_all;
static int bam_alt_platform;
static char *bam_platform;
static char *bam_home_env = NULL;

/* function prototypes */
static void parse_args_internal(int, char *[]);
static void parse_args(int, char *argv[]);
static error_t bam_install(char *, char *);
static error_t bam_archive(char *, char *);

static void bam_lock(void);
static void bam_unlock(void);

static int exec_cmd(char *, filelist_t *);
static void linelist_free(line_t *);
static void filelist_free(filelist_t *);

static error_t install_bootloader(void);
static error_t update_archive(char *, char *);
static error_t list_archive(char *, char *);
static error_t update_all(char *, char *);
static error_t read_list(char *, filelist_t *);

static long s_strtol(char *);

static char *get_machine(void);
static void append_to_flist(filelist_t *, char *);

#if !defined(_OBP)
static void ucode_install();
#endif

#if 0
/* Menu related sub commands */
static subcmd_defn_t menu_subcmds[] = {
	"set_option",		OPT_ABSENT,	set_option, 0,	/* PUB */
	"list_entry",		OPT_OPTIONAL,	list_entry, 1,	/* PUB */
	"delete_all_entries",	OPT_ABSENT,	delete_all_entries, 0, /* PVT */
	"update_entry",		OPT_REQ,	update_entry, 0, /* menu */
	"update_temp",		OPT_OPTIONAL,	update_temp, 0,	/* reboot */
	"upgrade",		OPT_ABSENT,	upgrade_menu, 0, /* menu */
	"list_setting",		OPT_OPTIONAL,	list_setting, 1, /* menu */
	"disable_hypervisor",	OPT_ABSENT,	cvt_to_metal, 0, /* menu */
	"enable_hypervisor",	OPT_ABSENT,	cvt_to_hyper, 0, /* menu */
	NULL,			0,		NULL, 0	/* must be last */
};
#endif

/* Archive related sub commands */
static subcmd_defn_t arch_subcmds[] = {
	{ "update", OPT_ABSENT,	update_archive, 0 },	/* PUB */
	{ "update_all", OPT_ABSENT, update_all, 0 },	/* PVT */
	{ "list", OPT_OPTIONAL,	list_archive, 1 },	/* PUB */
	{ NULL, 0, NULL, 0 }			/* must be last */
};

/* Install related sub commands */
static subcmd_defn_t inst_subcmds[] = {
	{ "install_bootloader",	OPT_ABSENT, install_bootloader, 0 }, /* PUB */
	{ NULL, 0, NULL, 0 }				/* must be last */
};

enum dircache_copy_opt {
	FILE_BA = 0,
	CACHEDIR_NUM
};

/*
 * Directory specific flags:
 * NEED_UPDATE : the specified archive needs to be updated
 */
#define	NEED_UPDATE		0x00000001

#define	set_dir_flag(id, f)	(walk_arg.dirinfo[id].flags |= f)
#define	unset_dir_flag(id, f)	(walk_arg.dirinfo[id].flags &= ~f)
#define	is_dir_flag_on(id, f)	(walk_arg.dirinfo[id].flags & f ? 1 : 0)

#define	get_cachedir(id)	(walk_arg.dirinfo[id].cdir_path)
#define	get_count(id)		(walk_arg.dirinfo[id].count)
#define	has_cachedir(id)	(walk_arg.dirinfo[id].has_dir)
#define	set_dir_present(id)	(walk_arg.dirinfo[id].has_dir = 1)

/*
 * dirinfo_t (specific cache directory information):
 * cdir_path:   path to the archive cache directory
 * has_dir:	the specified cache directory is active
 * count:	the number of files to update
 * flags:	directory specific flags
 */
typedef struct _dirinfo {
	char	cdir_path[PATH_MAX];
	int	has_dir;
	int	count;
	int	flags;
} dirinfo_t;

/*
 * Update flags:
 * NEED_CACHE_DIR : cache directory is missing and needs to be created
 * UPDATE_ERROR : an error occourred while traversing the list of files
 * RDONLY_FSCHK : the target filesystem is read-only
 * RAMDSK_FSCHK : the target filesystem is on a ramdisk
 */
#define	NEED_CACHE_DIR		0x00000001
#define	UPDATE_ERROR		0x00000004
#define	RDONLY_FSCHK		0x00000008
#define	INVALIDATE_CACHE	0x00000010

#define	is_flag_on(flag)	(walk_arg.update_flags & flag ? 1 : 0)
#define	set_flag(flag)		(walk_arg.update_flags |= flag)
#define	unset_flag(flag)	(walk_arg.update_flags &= ~flag)

/*
 * struct walk_arg :
 * update_flags: flags related to the current updating process
 * new_nvlp/old_nvlp: new and old list of archive-files / attributes pairs
 */
static struct {
	int 		update_flags;
	nvlist_t 	*new_nvlp;
	nvlist_t 	*old_nvlp;
	dirinfo_t	dirinfo[CACHEDIR_NUM];
} walk_arg;

struct safefile {
	char *name;
	struct safefile *next;
};

static struct safefile *safefiles = NULL;

/*
 * svc:/system/filesystem/usr:default service checks for this file and
 * does a boot archive update and then reboot the system.
 */
#define	NEED_UPDATE_FILE "/etc/svc/volatile/boot_archive_needs_update"

/*
 * svc:/system/boot-archive-update:default checks for this file and
 * updates the boot archive.
 */
#define	NEED_UPDATE_SAFE_FILE "/etc/svc/volatile/boot_archive_safefile_update"

/* Thanks growisofs */
#define	CD_BLOCK	((off64_t)2048)
#define	VOLDESC_OFF	16
#define	DVD_BLOCK	(32*1024)
#define	MAX_IVDs	16

struct iso_pdesc {
    unsigned char type	[1];
    unsigned char id	[5];
    unsigned char void1	[80-5-1];
    unsigned char volume_space_size [8];
    unsigned char void2	[2048-80-8];
};

#define	bam_nowrite()		(bam_check || bam_smf_check)

static void
usage(void)
{
	(void) fprintf(stderr, "USAGE:\n");

	/* archive usage */
	(void) fprintf(stderr,
	    "\t%s update-archive [-vn] [-R altroot [-p platform]]\n", prog);
	(void) fprintf(stderr,
	    "\t%s list-archive [-R altroot [-p platform]]\n", prog);
#if defined(_OBP)
	(void) fprintf(stderr,
	    "\t%s install-bootloader [-fv] [-R altroot] [-P pool]\n", prog);
#else
	(void) fprintf(stderr,
	    "\t%s install-bootloader [-Mfv] [-R altroot] [-P pool]\n", prog);
#endif
#if !defined(_OBP)
	/* x86 only */
	(void) fprintf(stderr, "\t%s set-menu [-R altroot] key=value\n", prog);
	(void) fprintf(stderr, "\t%s list-menu [-R altroot]\n", prog);
#endif
}

/*
 * Best effort attempt to restore the $HOME value.
 */
static void
restore_env()
{
	char	home_env[PATH_MAX];

	if (bam_home_env) {
		(void) snprintf(home_env, sizeof (home_env), "HOME=%s",
		    bam_home_env);
		(void) putenv(home_env);
	}
}


#define		SLEEP_TIME	5
#define		MAX_TRIES	4

/*
 * Sanitize the environment in which bootadm will execute its sub-processes
 * (ex. mkisofs). This is done to prevent those processes from attempting
 * to access files (ex. .mkisofsrc) or stat paths that might be on NFS
 * or, potentially, insecure.
 */
static void
sanitize_env()
{
	int	stry = 0;

	/* don't depend on caller umask */
	(void) umask(0022);

	/* move away from a potential unsafe current working directory */
	while (chdir("/") == -1) {
		if (errno != EINTR) {
			bam_print("WARNING: unable to chdir to /");
			break;
		}
	}

	bam_home_env = getenv("HOME");
	while (bam_home_env != NULL && putenv("HOME=/") == -1) {
		if (errno == ENOMEM) {
			/* retry no more than MAX_TRIES times */
			if (++stry > MAX_TRIES) {
				bam_print("WARNING: unable to recover from "
				    "system memory pressure... aborting \n");
				bam_exit(EXIT_FAILURE);
			}
			/* memory is tight, try to sleep */
			bam_print("Attempting to recover from memory pressure: "
			    "sleeping for %d seconds\n", SLEEP_TIME * stry);
			(void) sleep(SLEEP_TIME * stry);
		} else {
			bam_print("WARNING: unable to sanitize HOME\n");
		}
	}
}

int
main(int argc, char *argv[])
{
	error_t ret = BAM_SUCCESS;

	(void) setlocale(LC_ALL, "");
	(void) textdomain(TEXT_DOMAIN);

	if ((prog = strrchr(argv[0], '/')) == NULL) {
		prog = argv[0];
	} else {
		prog++;
	}

	INJECT_ERROR1("ASSERT_ON", assert(0))

	sanitize_env();

	parse_args(argc, argv);

	switch (bam_cmd) {
		case BAM_MENU:
			ret = bam_loader_menu(bam_subcmd, bam_opt,
			    bam_argc, bam_argv);
			break;
		case BAM_ARCHIVE:
			ret = bam_archive(bam_subcmd, bam_opt);
			break;
		case BAM_INSTALL:
			ret = bam_install(bam_subcmd, bam_opt);
			break;
		default:
			usage();
			bam_exit(1);
	}

	if (ret != BAM_SUCCESS)
		bam_exit((ret == BAM_NOCHANGE) ? 2 : 1);

	bam_unlock();
	return (0);
}

/*
 * Equivalence of public and internal commands:
 *	update-archive  -- -a update
 *	list-archive	-- -a list
 *	set-menu	-- -m set_option
 *	list-menu	-- -m list_entry
 *	update-menu	-- -m update_entry
 *	install-bootloader	-- -i install_bootloader
 */
static struct cmd_map {
	char *bam_cmdname;
	int bam_cmd;
	char *bam_subcmd;
} cmd_map[] = {
	{ "update-archive",	BAM_ARCHIVE,	"update"},
	{ "list-archive",	BAM_ARCHIVE,	"list"},
	{ "set-menu",		BAM_MENU,	"set_option"},
	{ "list-menu",		BAM_MENU,	"list_entry"},
	{ "update-menu",	BAM_MENU,	"update_entry"},
	{ "install-bootloader",	BAM_INSTALL,	"install_bootloader"},
	{ NULL,			0,		NULL}
};

/*
 * Commands syntax published in bootadm(1M) are parsed here
 */
static void
parse_args(int argc, char *argv[])
{
	struct cmd_map *cmp = cmd_map;

	/* command conforming to the final spec */
	if (argc > 1 && argv[1][0] != '-') {
		/*
		 * Map commands to internal table.
		 */
		while (cmp->bam_cmdname) {
			if (strcmp(argv[1], cmp->bam_cmdname) == 0) {
				bam_cmd = cmp->bam_cmd;
				bam_subcmd = cmp->bam_subcmd;
				break;
			}
			cmp++;
		}
		if (cmp->bam_cmdname == NULL) {
			usage();
			bam_exit(1);
		}
		argc--;
		argv++;
	}

	parse_args_internal(argc, argv);
}

/*
 * A combination of public and private commands are parsed here.
 * The internal syntax and the corresponding functionality are:
 *	-a update			-- update-archive
 *	-a list				-- list-archive
 *	-a update-all			-- (reboot to sync all mnted OS archive)
 *	-i install_bootloader		-- install-bootloader
 *	-m update_entry			-- update-menu
 *	-m list_entry			-- list-menu
 *	-m update_temp			-- (reboot -- [boot-args])
 *	-m delete_all_entries		-- (called from install)
 *	-m enable_hypervisor [args]	-- cvt_to_hyper
 *	-m disable_hypervisor		-- cvt_to_metal
 *	-m list_setting [entry] [value]	-- list_setting
 *
 * A set of private flags is there too:
 *	-F		-- purge the cache directories and rebuild them
 */
static void
parse_args_internal(int argc, char *argv[])
{
	int c, error;
	extern char *optarg;
	extern int optind, opterr;
#if defined(_OBP)
	const char *optstring = "a:d:fi:m:no:vFCR:p:P:XZ";
#else
	const char *optstring = "a:d:fi:m:no:vFCMR:p:P:XZ";
#endif

	/* Suppress error message from getopt */
	opterr = 0;

	error = 0;
	while ((c = getopt(argc, argv, optstring)) != -1) {
		switch (c) {
		case 'a':
			if (bam_cmd) {
				error = 1;
				bam_error(
				    _("multiple commands specified: -%c\n"), c);
			}
			bam_cmd = BAM_ARCHIVE;
			bam_subcmd = optarg;
			break;
		case 'd':
			if (bam_debug) {
				error = 1;
				bam_error(
				    _("duplicate options specified: -%c\n"), c);
			}
			bam_debug = s_strtol(optarg);
			break;
		case 'f':
			bam_force = 1;
			break;
		case 'F':
			bam_purge = 1;
			break;
		case 'i':
			if (bam_cmd) {
				error = 1;
				bam_error(
				    _("multiple commands specified: -%c\n"), c);
			}
			bam_cmd = BAM_INSTALL;
			bam_subcmd = optarg;
			break;
		case 'm':
			if (bam_cmd) {
				error = 1;
				bam_error(
				    _("multiple commands specified: -%c\n"), c);
			}
			bam_cmd = BAM_MENU;
			bam_subcmd = optarg;
			break;
#if !defined(_OBP)
		case 'M':
			bam_mbr = 1;
			break;
#endif
		case 'n':
			bam_check = 1;
			/*
			 * We save the original value of bam_check. The new
			 * approach in case of a read-only filesystem is to
			 * behave as a check, so we need a way to restore the
			 * original value after the evaluation of the read-only
			 * filesystem has been done.
			 * Even if we don't allow at the moment a check with
			 * update_all, this approach is more robust than
			 * simply resetting bam_check to zero.
			 */
			bam_saved_check = 1;
			break;
		case 'o':
			if (bam_opt) {
				error = 1;
				bam_error(
				    _("duplicate options specified: -%c\n"), c);
			}
			bam_opt = optarg;
			break;
		case 'v':
			bam_verbose = 1;
			break;
		case 'C':
			bam_smf_check = 1;
			break;
		case 'P':
			if (bam_pool != NULL) {
				error = 1;
				bam_error(
				    _("duplicate options specified: -%c\n"), c);
			}
			bam_pool = optarg;
			break;
		case 'R':
			if (bam_root) {
				error = 1;
				bam_error(
				    _("duplicate options specified: -%c\n"), c);
				break;
			} else if (realpath(optarg, rootbuf) == NULL) {
				error = 1;
				bam_error(_("cannot resolve path %s: %s\n"),
				    optarg, strerror(errno));
				break;
			}
			bam_alt_root = 1;
			bam_root = rootbuf;
			bam_rootlen = strlen(rootbuf);
			break;
		case 'p':
			bam_alt_platform = 1;
			bam_platform = optarg;
			if ((strcmp(bam_platform, "i86pc") != 0) &&
			    (strcmp(bam_platform, "sun4u") != 0) &&
			    (strcmp(bam_platform, "sun4v") != 0)) {
				error = 1;
				bam_error(_("invalid platform %s - must be "
				    "one of sun4u, sun4v or i86pc\n"),
				    bam_platform);
			}
			break;
		case 'X':
			bam_is_hv = BAM_HV_PRESENT;
			break;
		case 'Z':
			bam_zfs = 1;
			break;
		case '?':
			error = 1;
			bam_error(_("invalid option or missing option "
			    "argument: -%c\n"), optopt);
			break;
		default :
			error = 1;
			bam_error(_("invalid option or missing option "
			    "argument: -%c\n"), c);
			break;
		}
	}

	/*
	 * An alternate platform requires an alternate root
	 */
	if (bam_alt_platform && bam_alt_root == 0) {
		usage();
		bam_exit(0);
	}

	/*
	 * A command option must be specfied
	 */
	if (!bam_cmd) {
		if (bam_opt && strcmp(bam_opt, "all") == 0) {
			usage();
			bam_exit(0);
		}
		bam_error(_("a command option must be specified\n"));
		error = 1;
	}

	if (error) {
		usage();
		bam_exit(1);
	}

	if (optind > argc) {
		bam_error(_("Internal error: %s\n"), "parse_args");
		bam_exit(1);
	} else if (optind < argc) {
		bam_argv = &argv[optind];
		bam_argc = argc - optind;
	}

	/*
	 * mbr and pool are options for install_bootloader
	 */
	if (bam_cmd != BAM_INSTALL && (bam_mbr || bam_pool != NULL)) {
		usage();
		bam_exit(0);
	}

	/*
	 * -n implies verbose mode
	 */
	if (bam_check)
		bam_verbose = 1;
}

error_t
check_subcmd_and_options(
	char *subcmd,
	char *opt,
	subcmd_defn_t *table,
	error_t (**fp)())
{
	int i;

	if (subcmd == NULL) {
		bam_error(_("this command requires a sub-command\n"));
		return (BAM_ERROR);
	}

	if (strcmp(subcmd, "set_option") == 0) {
		if (bam_argc == 0 || bam_argv == NULL || bam_argv[0] == NULL) {
			bam_error(_("missing argument for sub-command\n"));
			usage();
			return (BAM_ERROR);
		} else if (bam_argc > 1 || bam_argv[1] != NULL) {
			bam_error(_("invalid trailing arguments\n"));
			usage();
			return (BAM_ERROR);
		}
	} else if (strcmp(subcmd, "update_all") == 0) {
		/*
		 * The only option we accept for the "update_all"
		 * subcmd is "fastboot".
		 */
		if (bam_argc > 1 || (bam_argc == 1 &&
		    strcmp(bam_argv[0], "fastboot") != 0)) {
			bam_error(_("invalid trailing arguments\n"));
			usage();
			return (BAM_ERROR);
		}
	} else if (((strcmp(subcmd, "enable_hypervisor") != 0) &&
	    (strcmp(subcmd, "list_setting") != 0)) && (bam_argc || bam_argv)) {
		/*
		 * Of the remaining subcommands, only "enable_hypervisor" and
		 * "list_setting" take trailing arguments.
		 */
		bam_error(_("invalid trailing arguments\n"));
		usage();
		return (BAM_ERROR);
	}

	if (bam_root == NULL) {
		bam_root = rootbuf;
		bam_rootlen = 1;
	}

	/* verify that subcmd is valid */
	for (i = 0; table[i].subcmd != NULL; i++) {
		if (strcmp(table[i].subcmd, subcmd) == 0)
			break;
	}

	if (table[i].subcmd == NULL) {
		bam_error(_("invalid sub-command specified: %s\n"), subcmd);
		return (BAM_ERROR);
	}

	if (table[i].unpriv == 0 && geteuid() != 0) {
		bam_error(_("you must be root to run this command\n"));
		return (BAM_ERROR);
	}

	/*
	 * Currently only privileged commands need a lock
	 */
	if (table[i].unpriv == 0)
		bam_lock();

	/* subcmd verifies that opt is appropriate */
	if (table[i].option != OPT_OPTIONAL) {
		if ((table[i].option == OPT_REQ) ^ (opt != NULL)) {
			if (opt)
				bam_error(_("this sub-command (%s) does not "
				    "take options\n"), subcmd);
			else
				bam_error(_("an option is required for this "
				    "sub-command: %s\n"), subcmd);
			return (BAM_ERROR);
		}
	}

	*fp = table[i].handler;

	return (BAM_SUCCESS);
}

/*
 * NOTE: A single "/" is also considered a trailing slash and will
 * be deleted.
 */
void
elide_trailing_slash(const char *src, char *dst, size_t dstsize)
{
	size_t dstlen;

	assert(src);
	assert(dst);

	(void) strlcpy(dst, src, dstsize);

	dstlen = strlen(dst);
	if (dst[dstlen - 1] == '/') {
		dst[dstlen - 1] = '\0';
	}
}

static int
is_safe_exec(char *path)
{
	struct stat	sb;

	if (lstat(path, &sb) != 0) {
		bam_error(_("stat of file failed: %s: %s\n"), path,
		    strerror(errno));
		return (BAM_ERROR);
	}

	if (!S_ISREG(sb.st_mode)) {
		bam_error(_("%s is not a regular file, skipping\n"), path);
		return (BAM_ERROR);
	}

	if (sb.st_uid != getuid()) {
		bam_error(_("%s is not owned by %d, skipping\n"),
		    path, getuid());
		return (BAM_ERROR);
	}

	if (sb.st_mode & S_IWOTH || sb.st_mode & S_IWGRP) {
		bam_error(_("%s is others or group writable, skipping\n"),
		    path);
		return (BAM_ERROR);
	}

	return (BAM_SUCCESS);
}

static error_t
install_bootloader(void)
{
	nvlist_t	*nvl;
	uint16_t	flags = 0;
	int		found = 0;
	struct extmnttab mnt;
	struct stat	statbuf = {0};
	be_node_list_t	*be_nodes, *node;
	FILE		*fp;
	char		*root_ds = NULL;
	int		ret = BAM_ERROR;

	if (nvlist_alloc(&nvl, NV_UNIQUE_NAME, 0) != 0) {
		bam_error(_("out of memory\n"));
		return (ret);
	}

	/*
	 * if bam_alt_root is set, the stage files are used from alt root.
	 * if pool is set, the target devices are pool devices, stage files
	 * are read from pool bootfs unless alt root is set.
	 *
	 * use arguments as targets, stage files are from alt or current root
	 * if no arguments and no pool, install on current boot pool.
	 */

	if (bam_alt_root) {
		if (stat(bam_root, &statbuf) != 0) {
			bam_error(_("stat of file failed: %s: %s\n"), bam_root,
			    strerror(errno));
			goto done;
		}
		if ((fp = fopen(MNTTAB, "r")) == NULL) {
			bam_error(_("failed to open file: %s: %s\n"),
			    MNTTAB, strerror(errno));
			goto done;
		}
		resetmnttab(fp);
		while (getextmntent(fp, &mnt, sizeof (mnt)) == 0) {
			if (mnt.mnt_major == major(statbuf.st_dev) &&
			    mnt.mnt_minor == minor(statbuf.st_dev)) {
				found = 1;
				root_ds = strdup(mnt.mnt_special);
				break;
			}
		}
		(void) fclose(fp);

		if (found == 0) {
			bam_error(_("alternate root %s not in mnttab\n"),
			    bam_root);
			goto done;
		}
		if (root_ds == NULL) {
			bam_error(_("out of memory\n"));
			goto done;
		}

		if (be_list(NULL, &be_nodes) != BE_SUCCESS) {
			bam_error(_("No BE's found\n"));
			goto done;
		}
		for (node = be_nodes; node != NULL; node = node->be_next_node)
			if (strcmp(root_ds, node->be_root_ds) == 0)
				break;

		if (node == NULL)
			bam_error(_("BE (%s) does not exist\n"), root_ds);

		free(root_ds);
		root_ds = NULL;
		if (node == NULL) {
			be_free_list(be_nodes);
			goto done;
		}
		ret = nvlist_add_string(nvl, BE_ATTR_ORIG_BE_NAME,
		    node->be_node_name);
		ret |= nvlist_add_string(nvl, BE_ATTR_ORIG_BE_ROOT,
		    node->be_root_ds);
		be_free_list(be_nodes);
		if (ret != 0) {
			ret = BAM_ERROR;
			goto done;
		}
	}

	if (bam_force)
		flags |= BE_INSTALLBOOT_FLAG_FORCE;
	if (bam_mbr)
		flags |= BE_INSTALLBOOT_FLAG_MBR;
	if (bam_verbose)
		flags |= BE_INSTALLBOOT_FLAG_VERBOSE;

	if (nvlist_add_uint16(nvl, BE_ATTR_INSTALL_FLAGS, flags) != 0) {
		bam_error(_("out of memory\n"));
		ret = BAM_ERROR;
		goto done;
	}

	/*
	 * if altroot was set, we got be name and be root, only need
	 * to set pool name as target.
	 * if no altroot, need to find be name and root from pool.
	 */
	if (bam_pool != NULL) {
		ret = nvlist_add_string(nvl, BE_ATTR_ORIG_BE_POOL, bam_pool);
		if (ret != 0) {
			ret = BAM_ERROR;
			goto done;
		}
		if (found) {
			ret = be_installboot(nvl);
			if (ret != 0)
				ret = BAM_ERROR;
			goto done;
		}
	}

	if (be_list(NULL, &be_nodes) != BE_SUCCESS) {
		bam_error(_("No BE's found\n"));
		ret = BAM_ERROR;
		goto done;
	}

	if (bam_pool != NULL) {
		/*
		 * find active be_node in bam_pool
		 */
		for (node = be_nodes; node != NULL; node = node->be_next_node) {
			if (strcmp(bam_pool, node->be_rpool) != 0)
				continue;
			if (node->be_active_on_boot)
				break;
		}
		if (node == NULL) {
			bam_error(_("No active BE in %s\n"), bam_pool);
			be_free_list(be_nodes);
			ret = BAM_ERROR;
			goto done;
		}
		ret = nvlist_add_string(nvl, BE_ATTR_ORIG_BE_NAME,
		    node->be_node_name);
		ret |= nvlist_add_string(nvl, BE_ATTR_ORIG_BE_ROOT,
		    node->be_root_ds);
		be_free_list(be_nodes);
		if (ret != 0) {
			ret = BAM_ERROR;
			goto done;
		}
		ret = be_installboot(nvl);
		if (ret != 0)
			ret = BAM_ERROR;
		goto done;
	}

	/*
	 * get dataset for "/" and fill up the args.
	 */
	if ((fp = fopen(MNTTAB, "r")) == NULL) {
		bam_error(_("failed to open file: %s: %s\n"),
		    MNTTAB, strerror(errno));
		ret = BAM_ERROR;
		be_free_list(be_nodes);
		goto done;
	}
	resetmnttab(fp);
	found = 0;
	while (getextmntent(fp, &mnt, sizeof (mnt)) == 0) {
		if (strcmp(mnt.mnt_mountp, "/") == 0) {
			found = 1;
			root_ds = strdup(mnt.mnt_special);
			break;
		}
	}
	(void) fclose(fp);

	if (found == 0) {
		bam_error(_("alternate root %s not in mnttab\n"), "/");
		ret = BAM_ERROR;
		be_free_list(be_nodes);
		goto done;
	}
	if (root_ds == NULL) {
		bam_error(_("out of memory\n"));
		ret = BAM_ERROR;
		be_free_list(be_nodes);
		goto done;
	}

	for (node = be_nodes; node != NULL; node = node->be_next_node) {
		if (strcmp(root_ds, node->be_root_ds) == 0)
			break;
	}

	if (node == NULL) {
		bam_error(_("No such BE: %s\n"), root_ds);
		free(root_ds);
		be_free_list(be_nodes);
		ret = BAM_ERROR;
		goto done;
	}
	free(root_ds);

	ret = nvlist_add_string(nvl, BE_ATTR_ORIG_BE_NAME, node->be_node_name);
	ret |= nvlist_add_string(nvl, BE_ATTR_ORIG_BE_ROOT, node->be_root_ds);
	ret |= nvlist_add_string(nvl, BE_ATTR_ORIG_BE_POOL, node->be_rpool);
	be_free_list(be_nodes);

	if (ret != 0)
		ret = BAM_ERROR;
	else
		ret = be_installboot(nvl) ? BAM_ERROR : 0;
done:
	nvlist_free(nvl);

	return (ret);
}

static error_t
bam_install(char *subcmd, char *opt)
{
	error_t (*f)(void);

	/*
	 * Check arguments
	 */
	if (check_subcmd_and_options(subcmd, opt, inst_subcmds, &f) ==
	    BAM_ERROR)
		return (BAM_ERROR);

	return (f());
}

static error_t
bam_archive(
	char *subcmd,
	char *opt)
{
	error_t			ret;
	error_t			(*f)(char *root, char *opt);
	const char		*fcn = "bam_archive()";

	/*
	 * Add trailing / for archive subcommands
	 */
	if (rootbuf[strlen(rootbuf) - 1] != '/')
		(void) strcat(rootbuf, "/");
	bam_rootlen = strlen(rootbuf);

	/*
	 * Check arguments
	 */
	ret = check_subcmd_and_options(subcmd, opt, arch_subcmds, &f);
	if (ret != BAM_SUCCESS) {
		return (BAM_ERROR);
	}

	ret = get_boot_cap(rootbuf);
	if (ret != BAM_SUCCESS) {
		BAM_DPRINTF(("%s: Failed to get boot capability\n", fcn));
		return (ret);
	}

	/*
	 * Check archive not supported with update_all
	 * since it is awkward to display out-of-sync
	 * information for each BE.
	 */
	if (bam_check && strcmp(subcmd, "update_all") == 0) {
		bam_error(_("the check option is not supported with "
		    "subcmd: %s\n"), subcmd);
		return (BAM_ERROR);
	}

	if (strcmp(subcmd, "update_all") == 0)
		bam_update_all = 1;

#if !defined(_OBP)
	ucode_install(bam_root);
#endif

	ret = f(bam_root, opt);

	bam_update_all = 0;

	return (ret);
}

/*PRINTFLIKE1*/
void
bam_error(char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	(void) fprintf(stderr, "%s: ", prog);
	(void) vfprintf(stderr, format, ap);
	va_end(ap);
}

/*PRINTFLIKE1*/
void
bam_derror(char *format, ...)
{
	va_list ap;

	assert(bam_debug);

	va_start(ap, format);
	(void) fprintf(stderr, "DEBUG: ");
	(void) vfprintf(stderr, format, ap);
	va_end(ap);
}

/*PRINTFLIKE1*/
void
bam_print(char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	(void) vfprintf(stdout, format, ap);
	va_end(ap);
}

/*PRINTFLIKE1*/
void
bam_print_stderr(char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	(void) vfprintf(stderr, format, ap);
	va_end(ap);
}

void
bam_exit(int excode)
{
	restore_env();
	bam_unlock();
	exit(excode);
}

static void
bam_lock(void)
{
	struct flock lock;
	pid_t pid;

	bam_lock_fd = open(BAM_LOCK_FILE, O_CREAT|O_RDWR, LOCK_FILE_PERMS);
	if (bam_lock_fd < 0) {
		/*
		 * We may be invoked early in boot for archive verification.
		 * In this case, root is readonly and /var/run may not exist.
		 * Proceed without the lock
		 */
		if (errno == EROFS || errno == ENOENT) {
			bam_root_readonly = 1;
			return;
		}

		bam_error(_("failed to open file: %s: %s\n"),
		    BAM_LOCK_FILE, strerror(errno));
		bam_exit(1);
	}

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	if (fcntl(bam_lock_fd, F_SETLK, &lock) == -1) {
		if (errno != EACCES && errno != EAGAIN) {
			bam_error(_("failed to lock file: %s: %s\n"),
			    BAM_LOCK_FILE, strerror(errno));
			(void) close(bam_lock_fd);
			bam_lock_fd = -1;
			bam_exit(1);
		}
		pid = 0;
		(void) pread(bam_lock_fd, &pid, sizeof (pid_t), 0);
		bam_print(
		    _("another instance of bootadm (pid %lu) is running\n"),
		    pid);

		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		if (fcntl(bam_lock_fd, F_SETLKW, &lock) == -1) {
			bam_error(_("failed to lock file: %s: %s\n"),
			    BAM_LOCK_FILE, strerror(errno));
			(void) close(bam_lock_fd);
			bam_lock_fd = -1;
			bam_exit(1);
		}
	}

	/* We own the lock now */
	pid = getpid();
	(void) write(bam_lock_fd, &pid, sizeof (pid));
}

static void
bam_unlock(void)
{
	struct flock unlock;

	/*
	 * NOP if we don't hold the lock
	 */
	if (bam_lock_fd < 0) {
		return;
	}

	unlock.l_type = F_UNLCK;
	unlock.l_whence = SEEK_SET;
	unlock.l_start = 0;
	unlock.l_len = 0;

	if (fcntl(bam_lock_fd, F_SETLK, &unlock) == -1) {
		bam_error(_("failed to unlock file: %s: %s\n"),
		    BAM_LOCK_FILE, strerror(errno));
	}

	if (close(bam_lock_fd) == -1) {
		bam_error(_("failed to close file: %s: %s\n"),
		    BAM_LOCK_FILE, strerror(errno));
	}
	bam_lock_fd = -1;
}

static error_t
list_archive(char *root, char *opt)
{
	filelist_t flist;
	filelist_t *flistp = &flist;
	line_t *lp;

	assert(root);
	assert(opt == NULL);

	flistp->head = flistp->tail = NULL;
	if (read_list(root, flistp) != BAM_SUCCESS) {
		return (BAM_ERROR);
	}
	assert(flistp->head && flistp->tail);

	for (lp = flistp->head; lp; lp = lp->next) {
		bam_print(_("%s\n"), lp->line);
	}

	filelist_free(flistp);

	return (BAM_SUCCESS);
}

/*
 * Checks if the path specified (without the file name at the end) exists
 * and creates it if not. If the path exists and is not a directory, an attempt
 * to unlink is made.
 */
static int
setup_path(char *path)
{
	char 		*p;
	int		ret;
	struct stat	sb;

	p = strrchr(path, '/');
	if (p != NULL) {
		*p = '\0';
		if (stat(path, &sb) != 0 || !(S_ISDIR(sb.st_mode))) {
			/* best effort attempt, mkdirp will catch the error */
			(void) unlink(path);
			if (bam_verbose)
				bam_print(_("need to create directory "
				    "path for %s\n"), path);
			ret = mkdirp(path, DIR_PERMS);
			if (ret == -1) {
				bam_error(_("mkdir of %s failed: %s\n"),
				    path, strerror(errno));
				*p = '/';
				return (BAM_ERROR);
			}
		}
		*p = '/';
		return (BAM_SUCCESS);
	}
	return (BAM_SUCCESS);
}

typedef union {
	gzFile	gzfile;
	int	fdfile;
} outfile;

typedef struct {
	char		path[PATH_MAX];
	outfile		out;
} cachefile;

static int
setup_file(char *base, const char *path, cachefile *cf)
{
	int	ret;
	char	*strip;

	/* init gzfile or fdfile in case we fail before opening */
	if (bam_direct == BAM_DIRECT_DBOOT)
		cf->out.gzfile = NULL;
	else
		cf->out.fdfile = -1;

	/* strip the trailing altroot path */
	strip = (char *)path + strlen(rootbuf);

	ret = snprintf(cf->path, sizeof (cf->path), "%s/%s", base, strip);
	if (ret >= sizeof (cf->path)) {
		bam_error(_("unable to create path on mountpoint %s, "
		    "path too long\n"), rootbuf);
		return (BAM_ERROR);
	}

	/* Check if path is present in the archive cache directory */
	if (setup_path(cf->path) == BAM_ERROR)
		return (BAM_ERROR);

	if (bam_direct == BAM_DIRECT_DBOOT) {
		if ((cf->out.gzfile = gzopen(cf->path, "wb")) == NULL) {
			bam_error(_("failed to open file: %s: %s\n"),
			    cf->path, strerror(errno));
			return (BAM_ERROR);
		}
		(void) gzsetparams(cf->out.gzfile, Z_BEST_SPEED,
		    Z_DEFAULT_STRATEGY);
	} else {
		if ((cf->out.fdfile = open(cf->path, O_WRONLY | O_CREAT, 0644))
		    == -1) {
			bam_error(_("failed to open file: %s: %s\n"),
			    cf->path, strerror(errno));
			return (BAM_ERROR);
		}
	}

	return (BAM_SUCCESS);
}

static int
cache_write(cachefile cf, char *buf, int size)
{
	int	err;

	if (bam_direct == BAM_DIRECT_DBOOT) {
		if (gzwrite(cf.out.gzfile, buf, size) < 1) {
			bam_error(_("failed to write to %s\n"),
			    gzerror(cf.out.gzfile, &err));
			if (err == Z_ERRNO && bam_verbose) {
				bam_error(_("write to file failed: %s: %s\n"),
				    cf.path, strerror(errno));
			}
			return (BAM_ERROR);
		}
	} else {
		if (write(cf.out.fdfile, buf, size) < 1) {
			bam_error(_("write to file failed: %s: %s\n"),
			    cf.path, strerror(errno));
			return (BAM_ERROR);
		}
	}
	return (BAM_SUCCESS);
}

static int
cache_close(cachefile cf)
{
	int	ret;

	if (bam_direct == BAM_DIRECT_DBOOT) {
		if (cf.out.gzfile) {
			ret = gzclose(cf.out.gzfile);
			if (ret != Z_OK) {
				bam_error(_("failed to close file: %s: %s\n"),
				    cf.path, strerror(errno));
				return (BAM_ERROR);
			}
		}
	} else {
		if (cf.out.fdfile != -1) {
			ret = close(cf.out.fdfile);
			if (ret != 0) {
				bam_error(_("failed to close file: %s: %s\n"),
				    cf.path, strerror(errno));
				return (BAM_ERROR);
			}
		}
	}

	return (BAM_SUCCESS);
}

static int
dircache_updatefile(const char *path, int what)
{
	int 		ret, exitcode;
	char 		buf[4096 * 4];
	FILE 		*infile;
	cachefile 	outfile;

	if (bam_nowrite()) {
		set_dir_flag(what, NEED_UPDATE);
		return (BAM_SUCCESS);
	}

	if (!has_cachedir(what))
		return (BAM_SUCCESS);

	if ((infile = fopen(path, "rb")) == NULL) {
		bam_error(_("failed to open file: %s: %s\n"), path,
		    strerror(errno));
		return (BAM_ERROR);
	}

	ret = setup_file(get_cachedir(what), path, &outfile);
	if (ret == BAM_ERROR) {
		exitcode = BAM_ERROR;
		goto out;
	}

	while ((ret = fread(buf, 1, sizeof (buf), infile)) > 0) {
		if (cache_write(outfile, buf, ret) == BAM_ERROR) {
			exitcode = BAM_ERROR;
			goto out;
		}
	}

	set_dir_flag(what, NEED_UPDATE);
	get_count(what)++;
	exitcode = BAM_SUCCESS;
out:
	(void) fclose(infile);
	if (cache_close(outfile) == BAM_ERROR)
		exitcode = BAM_ERROR;
	if (exitcode == BAM_ERROR)
		set_flag(UPDATE_ERROR);
	return (exitcode);
}

static int
dircache_updatedir(const char *path, int what)
{
	int		ret;
	char		dpath[PATH_MAX];
	char		*strip;
	struct stat	sb;

	strip = (char *)path + strlen(rootbuf);

	ret = snprintf(dpath, sizeof (dpath), "%s/%s", get_cachedir(what), strip);

	if (ret >= sizeof (dpath)) {
		bam_error(_("unable to create path on mountpoint %s, "
		    "path too long\n"), rootbuf);
		set_flag(UPDATE_ERROR);
		return (BAM_ERROR);
	}

	if (stat(dpath, &sb) == 0 && S_ISDIR(sb.st_mode))
		return (BAM_SUCCESS);

	if (!bam_nowrite() && mkdirp(dpath, DIR_PERMS) == -1) {
		set_flag(UPDATE_ERROR);
		return (BAM_ERROR);
	}

	set_dir_flag(what, NEED_UPDATE);
	return (BAM_SUCCESS);
}

#define	DO_CACHE_DIR	0
#define	DO_UPDATE_DIR	1

#if defined(_LP64) || defined(_LONGLONG_TYPE)
typedef		Elf64_Ehdr	_elfhdr;
#else
typedef		Elf32_Ehdr	_elfhdr;
#endif

/*
 * This routine updates the contents of the cache directory
 */
static int
update_dircache(const char *path, int flags)
{
	int rc = BAM_SUCCESS;

	switch (flags) {
	case FTW_F:
		{
		int	fd;
		_elfhdr	elf;

		if ((fd = open(path, O_RDONLY)) < 0) {
			bam_error(_("failed to open file: %s: %s\n"),
			    path, strerror(errno));
			set_flag(UPDATE_ERROR);
			rc = BAM_ERROR;
			break;
		}

		/*
		 * libelf and gelf would be a cleaner and easier way to handle
		 * this, but libelf fails compilation if _ILP32 is defined &&
		 * _FILE_OFFSET_BITS is != 32 ...
		 */
		if (read(fd, (void *)&elf, sizeof (_elfhdr)) < 0) {
			bam_error(_("read failed for file: %s: %s\n"),
			    path, strerror(errno));
			set_flag(UPDATE_ERROR);
			(void) close(fd);
			rc = BAM_ERROR;
			break;
		}
		(void) close(fd);

		rc = dircache_updatefile(path, FILE_BA);
		break;
		}
	case FTW_D:
		rc = dircache_updatedir(path, FILE_BA);
		break;
	default:
		rc = BAM_ERROR;
		break;
	}

	return (rc);
}

/*ARGSUSED*/
static int
cmpstat(
	const char *file,
	const struct stat *st,
	int flags,
	struct FTW *ftw)
{
	uint_t 		sz;
	uint64_t 	*value;
	uint64_t 	filestat[2];
	int 		error, ret, status;

	struct safefile *safefilep;
	FILE 		*fp;
	struct stat	sb;
	regex_t re;

	if (flags != FTW_F && flags != FTW_D && flags == FTW_SL)
		return (0);

	/*
	 * Ignore broken links
	 */
	if (flags == FTW_SL && stat(file, &sb) < 0)
		return (0);

	/*
	 * new_nvlp may be NULL if there were errors earlier
	 * but this is not fatal to update determination.
	 */
	if (walk_arg.new_nvlp) {
		filestat[0] = st->st_size;
		filestat[1] = st->st_mtime;
		error = nvlist_add_uint64_array(walk_arg.new_nvlp,
		    file + bam_rootlen, filestat, 2);
		if (error)
			bam_error(_("failed to update stat data for: %s: %s\n"),
			    file, strerror(error));
	}

	/*
	 * If we are invoked as part of system/filesystem/boot-archive, then
	 * there are a number of things we should not worry about
	 */
	if (bam_smf_check) {
		/* read in list of safe files */
		if (safefiles == NULL) {
			fp = fopen("/boot/solaris/filelist.safe", "r");
			if (fp != NULL) {
				safefiles = s_calloc(1,
				    sizeof (struct safefile));
				safefilep = safefiles;
				safefilep->name = s_calloc(1, MAXPATHLEN +
				    MAXNAMELEN);
				safefilep->next = NULL;
				while (s_fgets(safefilep->name, MAXPATHLEN +
				    MAXNAMELEN, fp) != NULL) {
					safefilep->next = s_calloc(1,
					    sizeof (struct safefile));
					safefilep = safefilep->next;
					safefilep->name = s_calloc(1,
					    MAXPATHLEN + MAXNAMELEN);
					safefilep->next = NULL;
				}
				(void) fclose(fp);
			}
		}
	}

	/*
	 * We are transitioning from the old model to the dircache or the cache
	 * directory was removed: create the entry without further checkings.
	 */
	if (is_flag_on(NEED_CACHE_DIR)) {
		if (bam_verbose)
			bam_print(_("    new     %s\n"), file);

		ret = update_dircache(file, flags);
		if (ret == BAM_ERROR) {
			bam_error(_("directory cache update failed for %s\n"),
			    file);
			return (-1);
		}

		return (0);
	}

	/*
	 * We need an update if file doesn't exist in old archive
	 */
	if (walk_arg.old_nvlp == NULL ||
	    nvlist_lookup_uint64_array(walk_arg.old_nvlp,
	    file + bam_rootlen, &value, &sz) != 0) {
		if (bam_smf_check)	/* ignore new during smf check */
			return (0);

		ret = update_dircache(file, flags);
		if (ret == BAM_ERROR) {
			bam_error(_("directory cache update "
			    "failed for %s\n"), file);
			return (-1);
		}

		if (bam_verbose)
			bam_print(_("    new     %s\n"), file);
		return (0);
	}

	/*
	 * If we got there, the file is already listed as to be included in the
	 * iso image. We just need to know if we are going to rebuild it or not
	 */

	/*
	 * File exists in old archive. Check if file has changed
	 */
	assert(sz == 2);
	bcopy(value, filestat, sizeof (filestat));

	if (flags != FTW_D && (filestat[0] != st->st_size ||
	    filestat[1] != st->st_mtime)) {
		if (bam_smf_check) {
			safefilep = safefiles;
			while (safefilep != NULL &&
			    safefilep->name[0] != '\0') {
				if (regcomp(&re, safefilep->name,
				    REG_EXTENDED|REG_NOSUB) == 0) {
					status = regexec(&re,
					    file + bam_rootlen, 0, NULL, 0);
					regfree(&re);
					if (status == 0) {
						(void) creat(
						    NEED_UPDATE_SAFE_FILE,
						    0644);
						return (0);
					}
				}
				safefilep = safefilep->next;
			}
		}

		ret = update_dircache(file, flags);
		if (ret == BAM_ERROR) {
			bam_error(_("directory cache update failed "
			    "for %s\n"), file);
			return (-1);
		}

		if (bam_verbose) {
			if (bam_smf_check)
				bam_print("    %s\n", file);
			else
				bam_print(_("    changed %s\n"), file);
		}
	}

	return (0);
}

/*
 * Remove a directory path recursively
 */
static int
rmdir_r(char *path)
{
	struct dirent 	*d = NULL;
	DIR 		*dir = NULL;
	char 		tpath[PATH_MAX];
	struct stat 	sb;

	if ((dir = opendir(path)) == NULL)
		return (-1);

	while ((d = readdir(dir)) != NULL) {
		if ((strcmp(d->d_name, ".") != 0) &&
		    (strcmp(d->d_name, "..") != 0)) {
			(void) snprintf(tpath, sizeof (tpath), "%s/%s",
			    path, d->d_name);
			if (stat(tpath, &sb) == 0) {
				if (sb.st_mode & S_IFDIR)
					(void) rmdir_r(tpath);
				else
					(void) remove(tpath);
			}
		}
	}
	return (remove(path));
}

/*
 * Check if cache directory exists and, if not, create it and update flags
 * accordingly. If the path exists, but it's not a directory, a best effort
 * attempt to remove and recreate it is made.
 * If the user requested a 'purge', always recreate the directory from scratch.
 */
static int
set_cache_dir(char *root, int what)
{
	struct stat	sb;
	int		ret = 0;

	ret = snprintf(get_cachedir(what), sizeof (get_cachedir(what)),
	    "%s%s%s%s", root, ARCHIVE_PREFIX, get_machine(), CACHEDIR_SUFFIX);

	if (ret >= sizeof (get_cachedir(what))) {
		bam_error(_("unable to create path on mountpoint %s, "
		    "path too long\n"), rootbuf);
		return (BAM_ERROR);
	}

	if (bam_purge || is_flag_on(INVALIDATE_CACHE))
		(void) rmdir_r(get_cachedir(what));

	if (stat(get_cachedir(what), &sb) != 0 || !(S_ISDIR(sb.st_mode))) {
		/* best effort unlink attempt, mkdir will catch errors */
		(void) unlink(get_cachedir(what));

		if (bam_verbose)
			bam_print(_("archive cache directory not found: %s\n"),
			    get_cachedir(what));
		ret = mkdir(get_cachedir(what), DIR_PERMS);
		if (ret < 0) {
			bam_error(_("mkdir of %s failed: %s\n"),
			    get_cachedir(what), strerror(errno));
			get_cachedir(what)[0] = '\0';
			return (ret);
		}
		set_flag(NEED_CACHE_DIR);
	}

	return (BAM_SUCCESS);
}

static int
is_valid_archive(char *root, int what)
{
	char 		archive_path[PATH_MAX];
	char		timestamp_path[PATH_MAX];
	struct stat 	sb, timestamp;
	int 		ret;

	ret = snprintf(archive_path, sizeof (archive_path), "%s%s%s%s",
	    root, ARCHIVE_PREFIX, get_machine(), ARCHIVE_SUFFIX);

	if (ret >= sizeof (archive_path)) {
		bam_error(_("unable to create path on mountpoint %s, "
		    "path too long\n"), rootbuf);
		return (BAM_ERROR);
	}

	if (stat(archive_path, &sb) != 0) {
		if (bam_verbose && !bam_check)
			bam_print(_("archive not found: %s\n"), archive_path);
		set_dir_flag(what, NEED_UPDATE);
		return (BAM_SUCCESS);
	}

	/*
	 * The timestamp file is used to prevent stale files in the archive
	 * cache.
	 * Stale files can happen if the system is booted back and forth across
	 * the transition from bootadm-before-the-cache to
	 * bootadm-after-the-cache, since older versions of bootadm don't know
	 * about the existence of the archive cache.
	 *
	 * Since only bootadm-after-the-cache versions know about about this
	 * file, we require that the boot archive be older than this file.
	 */
	ret = snprintf(timestamp_path, sizeof (timestamp_path), "%s%s", root,
	    FILE_STAT_TIMESTAMP);

	if (ret >= sizeof (timestamp_path)) {
		bam_error(_("unable to create path on mountpoint %s, "
		    "path too long\n"), rootbuf);
		return (BAM_ERROR);
	}

	if (stat(timestamp_path, &timestamp) != 0 ||
	    sb.st_mtime > timestamp.st_mtime) {
		if (bam_verbose && !bam_check)
			bam_print(
			    _("archive cache is out of sync. Rebuilding.\n"));
		/*
		 * Don't generate a false positive for the boot-archive service
		 * but trigger an update of the archive cache in
		 * boot-archive-update.
		 */
		if (bam_smf_check) {
			(void) creat(NEED_UPDATE_FILE, 0644);
			return (BAM_SUCCESS);
		}

		set_flag(INVALIDATE_CACHE);
		set_dir_flag(what, NEED_UPDATE);
		return (BAM_SUCCESS);
	}

	return (BAM_SUCCESS);
}

/*
 * Check flags and presence of required files and directories.
 * The force flag and/or absence of files should
 * trigger an update.
 * Suppress stdout output if check (-n) option is set
 * (as -n should only produce parseable output.)
 */
static int
check_flags_and_files(char *root)
{

	int 		ret;
	int		what;

	/*
	 * If archive is missing, create archive
	 */
	what = FILE_BA;
	do {
		ret = is_valid_archive(root, what);
		if (ret == BAM_ERROR)
			return (BAM_ERROR);
		what++;
	} while (bam_direct == BAM_DIRECT_DBOOT && what < CACHEDIR_NUM);

	if (bam_nowrite())
		return (BAM_SUCCESS);


	/*
	 * check if cache directories exist.
	 */
	what = FILE_BA;

	do {
		if (set_cache_dir(root, what) != 0)
			return (BAM_ERROR);

		set_dir_present(what);

		what++;
	} while (bam_direct == BAM_DIRECT_DBOOT && what < CACHEDIR_NUM);

	/*
	 * if force, create archive unconditionally
	 */
	if (bam_force) {
		set_dir_flag(FILE_BA, NEED_UPDATE);
		if (bam_verbose)
			bam_print(_("forced update of archive requested\n"));
	}

	return (BAM_SUCCESS);
}

static error_t
read_one_list(char *root, filelist_t  *flistp, char *filelist)
{
	char 		path[PATH_MAX];
	FILE 		*fp;
	char 		buf[BAM_MAXLINE];
	const char 	*fcn = "read_one_list()";

	(void) snprintf(path, sizeof (path), "%s%s", root, filelist);

	fp = fopen(path, "r");
	if (fp == NULL) {
		BAM_DPRINTF(("%s: failed to open archive filelist: %s: %s\n",
		    fcn, path, strerror(errno)));
		return (BAM_ERROR);
	}
	while (s_fgets(buf, sizeof (buf), fp) != NULL) {
		/* skip blank lines */
		if (strspn(buf, " \t") == strlen(buf))
			continue;
		append_to_flist(flistp, buf);
	}
	if (fclose(fp) != 0) {
		bam_error(_("failed to close file: %s: %s\n"),
		    path, strerror(errno));
		return (BAM_ERROR);
	}
	return (BAM_SUCCESS);
}

static error_t
read_list(char *root, filelist_t  *flistp)
{
	char 		path[PATH_MAX];
	char 		cmd[PATH_MAX];
	struct stat 	sb;
	int 		n, rval;
	const char 	*fcn = "read_list()";

	flistp->head = flistp->tail = NULL;

	/*
	 * build and check path to extract_boot_filelist.ksh
	 */
	n = snprintf(path, sizeof (path), "%s%s", root, EXTRACT_BOOT_FILELIST);
	if (n >= sizeof (path)) {
		bam_error(_("archive filelist is empty\n"));
		return (BAM_ERROR);
	}

	if (is_safe_exec(path) == BAM_ERROR)
		return (BAM_ERROR);

	/*
	 * If extract_boot_filelist is present, exec it, otherwise read
	 * the filelists directly, for compatibility with older images.
	 */
	if (stat(path, &sb) == 0) {
		/*
		 * build arguments to exec extract_boot_filelist.ksh
		 */
		char *rootarg, *platarg;
		int platarglen = 1, rootarglen = 1;
		if (strlen(root) > 1)
			rootarglen += strlen(root) + strlen("-R ");
		if (bam_alt_platform)
			platarglen += strlen(bam_platform) + strlen("-p ");
		platarg = s_calloc(1, platarglen);
		rootarg = s_calloc(1, rootarglen);
		*platarg = 0;
		*rootarg = 0;

		if (strlen(root) > 1) {
			(void) snprintf(rootarg, rootarglen,
			    "-R %s", root);
		}
		if (bam_alt_platform) {
			(void) snprintf(platarg, platarglen,
			    "-p %s", bam_platform);
		}
		n = snprintf(cmd, sizeof (cmd), "%s %s %s /%s /%s",
		    path, rootarg, platarg, BOOT_FILE_LIST, ETC_FILE_LIST);
		free(platarg);
		free(rootarg);
		if (n >= sizeof (cmd)) {
			bam_error(_("archive filelist is empty\n"));
			return (BAM_ERROR);
		}
		if (exec_cmd(cmd, flistp) != 0) {
			BAM_DPRINTF(("%s: failed to open archive "
			    "filelist: %s: %s\n", fcn, path, strerror(errno)));
			return (BAM_ERROR);
		}
	} else {
		/*
		 * Read current lists of files - only the first is mandatory
		 */
		rval = read_one_list(root, flistp, BOOT_FILE_LIST);
		if (rval != BAM_SUCCESS)
			return (rval);
		(void) read_one_list(root, flistp, ETC_FILE_LIST);
	}

	if (flistp->head == NULL) {
		bam_error(_("archive filelist is empty\n"));
		return (BAM_ERROR);
	}

	return (BAM_SUCCESS);
}

static void
getoldstat(char *root)
{
	char 		path[PATH_MAX];
	int 		fd, error;
	struct stat 	sb;
	char 		*ostat;

	(void) snprintf(path, sizeof (path), "%s%s", root, FILE_STAT);
	fd = open(path, O_RDONLY);
	if (fd == -1) {
		if (bam_verbose)
			bam_print(_("failed to open file: %s: %s\n"),
			    path, strerror(errno));
		goto out_err;
	}

	if (fstat(fd, &sb) != 0) {
		bam_error(_("stat of file failed: %s: %s\n"), path,
		    strerror(errno));
		goto out_err;
	}

	ostat = s_calloc(1, sb.st_size);

	if (read(fd, ostat, sb.st_size) != sb.st_size) {
		bam_error(_("read failed for file: %s: %s\n"), path,
		    strerror(errno));
		free(ostat);
		goto out_err;
	}

	(void) close(fd);
	fd = -1;

	walk_arg.old_nvlp = NULL;
	error = nvlist_unpack(ostat, sb.st_size, &walk_arg.old_nvlp, 0);

	free(ostat);

	if (error) {
		bam_error(_("failed to unpack stat data: %s: %s\n"),
		    path, strerror(error));
		walk_arg.old_nvlp = NULL;
		goto out_err;
	} else {
		return;
	}

out_err:
	if (fd != -1)
		(void) close(fd);
	set_dir_flag(FILE_BA, NEED_UPDATE);
}

/* Best effort stale entry removal */
static void
delete_stale(char *file, int what)
{
	char		path[PATH_MAX];
	struct stat	sb;

	(void) snprintf(path, sizeof (path), "%s/%s", get_cachedir(what), file);
	if (!bam_check && stat(path, &sb) == 0) {
		if (sb.st_mode & S_IFDIR)
			(void) rmdir_r(path);
		else
			(void) unlink(path);

		set_dir_flag(what, NEED_UPDATE);
	}
}

/*
 * Checks if a file in the current (old) archive has
 * been deleted from the root filesystem.
 */
static void
check4stale(char *root)
{
	nvpair_t	*nvp;
	nvlist_t	*nvlp;
	char 		*file;
	char		path[PATH_MAX];

	/*
	 * Skip stale file check during smf check
	 */
	if (bam_smf_check)
		return;

	/*
	 * If we need to (re)create the cache, there's no need to check for
	 * stale files
	 */
	if (is_flag_on(NEED_CACHE_DIR))
		return;

	/* Nothing to do if no old stats */
	if ((nvlp = walk_arg.old_nvlp) == NULL)
		return;

	for (nvp = nvlist_next_nvpair(nvlp, NULL); nvp;
	    nvp = nvlist_next_nvpair(nvlp, nvp)) {
		file = nvpair_name(nvp);
		if (file == NULL)
			continue;
		(void) snprintf(path, sizeof (path), "%s/%s",
		    root, file);
		if (access(path, F_OK) < 0) {
			int	what;

			if (bam_verbose)
				bam_print(_("    stale %s\n"), path);

			for (what = FILE_BA; what < CACHEDIR_NUM; what++)
				if (has_cachedir(what))
					delete_stale(file, what);
		}
	}
}

static void
create_newstat(void)
{
	int error;

	error = nvlist_alloc(&walk_arg.new_nvlp, NV_UNIQUE_NAME, 0);
	if (error) {
		/*
		 * Not fatal - we can still create archive
		 */
		walk_arg.new_nvlp = NULL;
		bam_error(_("failed to create stat data: %s\n"),
		    strerror(error));
	}
}

static int
walk_list(char *root, filelist_t *flistp)
{
	char path[PATH_MAX];
	line_t *lp;

	for (lp = flistp->head; lp; lp = lp->next) {
		/*
		 * Don't follow symlinks.  A symlink must refer to
		 * a file that would appear in the archive through
		 * a direct reference.  This matches the archive
		 * construction behavior.
		 */
		(void) snprintf(path, sizeof (path), "%s%s", root, lp->line);
		if (nftw(path, cmpstat, 20, FTW_PHYS) == -1) {
			if (is_flag_on(UPDATE_ERROR))
				return (BAM_ERROR);
			/*
			 * Some files may not exist.
			 * For example: etc/rtc_config on a x86 diskless system
			 * Emit verbose message only
			 */
			if (bam_verbose)
				bam_print(_("cannot find: %s: %s\n"),
				    path, strerror(errno));
		}
	}

	return (BAM_SUCCESS);
}

/*
 * Update the timestamp file.
 */
static void
update_timestamp(char *root)
{
	char	timestamp_path[PATH_MAX];

	/* this path length has already been checked in check_flags_and_files */
	(void) snprintf(timestamp_path, sizeof (timestamp_path), "%s%s", root,
	    FILE_STAT_TIMESTAMP);

	/*
	 * recreate the timestamp file. Since an outdated or absent timestamp
	 * file translates in a complete rebuild of the archive cache, notify
	 * the user of the performance issue.
	 */
	if (creat(timestamp_path, FILE_STAT_MODE) < 0) {
		bam_error(_("failed to open file: %s: %s\n"), timestamp_path,
		    strerror(errno));
		bam_error(_("failed to update the timestamp file, next"
		    " archive update may experience reduced performance\n"));
	}
}


static void
savenew(char *root)
{
	char 	path[PATH_MAX];
	char 	path2[PATH_MAX];
	size_t 	sz;
	char 	*nstat;
	int 	fd, wrote, error;

	nstat = NULL;
	sz = 0;
	error = nvlist_pack(walk_arg.new_nvlp, &nstat, &sz,
	    NV_ENCODE_XDR, 0);
	if (error) {
		bam_error(_("failed to pack stat data: %s\n"),
		    strerror(error));
		return;
	}

	(void) snprintf(path, sizeof (path), "%s%s", root, FILE_STAT_TMP);
	fd = open(path, O_RDWR|O_CREAT|O_TRUNC, FILE_STAT_MODE);
	if (fd == -1) {
		bam_error(_("failed to open file: %s: %s\n"), path,
		    strerror(errno));
		free(nstat);
		return;
	}
	wrote = write(fd, nstat, sz);
	if (wrote != sz) {
		bam_error(_("write to file failed: %s: %s\n"), path,
		    strerror(errno));
		(void) close(fd);
		free(nstat);
		return;
	}
	(void) close(fd);
	free(nstat);

	(void) snprintf(path2, sizeof (path2), "%s%s", root, FILE_STAT);
	if (rename(path, path2) != 0) {
		bam_error(_("rename to file failed: %s: %s\n"), path2,
		    strerror(errno));
	}
}

#define	init_walk_args()	bzero(&walk_arg, sizeof (walk_arg))

static void
clear_walk_args(void)
{
	nvlist_free(walk_arg.old_nvlp);
	nvlist_free(walk_arg.new_nvlp);
	walk_arg.old_nvlp = NULL;
	walk_arg.new_nvlp = NULL;
}

/*
 * Returns:
 *	0 - no update necessary
 *	1 - update required.
 *	BAM_ERROR (-1) - An error occurred
 *
 * Special handling for check (-n):
 * ================================
 * The check (-n) option produces parseable output.
 * To do this, we suppress all stdout messages unrelated
 * to out of sync files.
 * All stderr messages are still printed though.
 *
 */
static int
update_required(char *root)
{
	struct stat 	sb;
	char 		path[PATH_MAX];
	filelist_t 	flist;
	filelist_t 	*flistp = &flist;
	int 		ret;

	flistp->head = flistp->tail = NULL;

	/*
	 * Check if cache directories and archives are present
	 */

	ret = check_flags_and_files(root);
	if (ret < 0)
		return (BAM_ERROR);

	/*
	 * In certain deployment scenarios, filestat may not
	 * exist. Do not stop the boot process, but trigger an update
	 * of the archives (which will recreate filestat.ramdisk).
	 */
	if (bam_smf_check) {
		(void) snprintf(path, sizeof (path), "%s%s", root, FILE_STAT);
		if (stat(path, &sb) != 0) {
			(void) creat(NEED_UPDATE_FILE, 0644);
			return (0);
		}
	}

	getoldstat(root);

	/*
	 * Check if the archive contains files that are no longer
	 * present on the root filesystem.
	 */
	check4stale(root);

	/*
	 * read list of files
	 */
	if (read_list(root, flistp) != BAM_SUCCESS) {
		clear_walk_args();
		return (BAM_ERROR);
	}

	assert(flistp->head && flistp->tail);

	/*
	 * At this point either the update is required
	 * or the decision is pending. In either case
	 * we need to create new stat nvlist
	 */
	create_newstat();
	/*
	 * This walk does 2 things:
	 *  	- gets new stat data for every file
	 *	- (optional) compare old and new stat data
	 */
	ret = walk_list(root, &flist);

	/* done with the file list */
	filelist_free(flistp);

	/* something went wrong */

	if (ret == BAM_ERROR) {
		bam_error(_("Failed to gather cache files, archives "
		    "generation aborted\n"));
		return (BAM_ERROR);
	}

	if (walk_arg.new_nvlp == NULL) {
		bam_error(_("cannot create new stat data\n"));
	}

	/* If nothing was updated, discard newstat. */

	if (!is_dir_flag_on(FILE_BA, NEED_UPDATE)) {
		clear_walk_args();
		return (0);
	}

	return (1);
}

static boolean_t
is_be(char *root)
{
	zfs_handle_t	*zhp;
	libzfs_handle_t	*hdl;
	be_node_list_t	*be_nodes = NULL;
	be_node_list_t	*cur_be;
	boolean_t	be_exist = B_FALSE;
	char		ds_path[ZFS_MAX_DATASET_NAME_LEN];

	if (!is_zfs(root))
		return (B_FALSE);
	/*
	 * Get dataset for mountpoint
	 */
	if ((hdl = libzfs_init()) == NULL)
		return (B_FALSE);

	if ((zhp = zfs_path_to_zhandle(hdl, root,
	    ZFS_TYPE_FILESYSTEM)) == NULL) {
		libzfs_fini(hdl);
		return (B_FALSE);
	}

	(void) strlcpy(ds_path, zfs_get_name(zhp), sizeof (ds_path));

	/*
	 * Check if the current dataset is BE
	 */
	if (be_list(NULL, &be_nodes) == BE_SUCCESS) {
		for (cur_be = be_nodes; cur_be != NULL;
		    cur_be = cur_be->be_next_node) {

			/*
			 * Because we guarantee that cur_be->be_root_ds
			 * is null-terminated by internal data structure,
			 * we can safely use strcmp()
			 */
			if (strcmp(ds_path, cur_be->be_root_ds) == 0) {
				be_exist = B_TRUE;
				break;
			}
		}
		be_free_list(be_nodes);
	}
	zfs_close(zhp);
	libzfs_fini(hdl);

	return (be_exist);
}

static error_t
create_ramdisk(char *root)
{
	char *cmdline, path[PATH_MAX];
	size_t len;
	struct stat sb;

	/*
	 * Setup command args for create_ramdisk.ksh for the cpio archives
	 * Note: we will not create hash here, CREATE_RAMDISK should create it.
	 */

	(void) snprintf(path, sizeof (path), "%s/%s", root, CREATE_RAMDISK);
	if (stat(path, &sb) != 0) {
		bam_error(_("archive creation file not found: %s: %s\n"),
		    path, strerror(errno));
		return (BAM_ERROR);
	}

	if (is_safe_exec(path) == BAM_ERROR)
		return (BAM_ERROR);

	len = strlen(path) + strlen(root) + 10;	/* room for space + -R */
	if (bam_alt_platform)
		len += strlen(bam_platform) + strlen("-p ");
	cmdline = s_calloc(1, len);

	if (bam_alt_platform) {
		assert(strlen(root) > 1);
		(void) snprintf(cmdline, len, "%s -p %s -R %s",
		    path, bam_platform, root);
		/* chop off / at the end */
		cmdline[strlen(cmdline) - 1] = '\0';
	} else if (strlen(root) > 1) {
		(void) snprintf(cmdline, len, "%s -R %s", path, root);
		/* chop off / at the end */
		cmdline[strlen(cmdline) - 1] = '\0';
	} else
		(void) snprintf(cmdline, len, "%s", path);

	if (exec_cmd(cmdline, NULL) != 0) {
		bam_error(_("boot-archive creation FAILED, command: '%s'\n"),
		    cmdline);
		free(cmdline);
		return (BAM_ERROR);
	}
	free(cmdline);
	/*
	 * The existence of the expected archives used to be
	 * verified here. This check is done in create_ramdisk as
	 * it needs to be in sync with the altroot operated upon.
	 */
	return (BAM_SUCCESS);
}

/*
 * Checks if target filesystem is on a ramdisk
 * 1 - is miniroot
 * 0 - is not
 * When in doubt assume it is not a ramdisk.
 */
static int
is_ramdisk(char *root)
{
	struct extmnttab mnt;
	FILE *fp;
	int found;
	char mntpt[PATH_MAX];
	char *cp;

	/*
	 * There are 3 situations where creating archive is
	 * of dubious value:
	 *	- create boot_archive on a lofi-mounted boot_archive
	 *	- create it on a ramdisk which is the root filesystem
	 *	- create it on a ramdisk mounted somewhere else
	 * The first is not easy to detect and checking for it is not
	 * worth it.
	 * The other two conditions are handled here
	 */
	fp = fopen(MNTTAB, "r");
	if (fp == NULL) {
		bam_error(_("failed to open file: %s: %s\n"),
		    MNTTAB, strerror(errno));
		return (0);
	}

	resetmnttab(fp);

	/*
	 * Remove any trailing / from the mount point
	 */
	(void) strlcpy(mntpt, root, sizeof (mntpt));
	if (strcmp(root, "/") != 0) {
		cp = mntpt + strlen(mntpt) - 1;
		if (*cp == '/')
			*cp = '\0';
	}
	found = 0;
	while (getextmntent(fp, &mnt, sizeof (mnt)) == 0) {
		if (strcmp(mnt.mnt_mountp, mntpt) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		if (bam_verbose)
			bam_error(_("alternate root %s not in mnttab\n"),
			    mntpt);
		(void) fclose(fp);
		return (0);
	}

	if (strncmp(mnt.mnt_special, RAMDISK_SPECIAL,
	    strlen(RAMDISK_SPECIAL)) == 0) {
		if (bam_verbose)
			bam_error(_("%s is on a ramdisk device\n"), bam_root);
		(void) fclose(fp);
		return (1);
	}

	(void) fclose(fp);

	return (0);
}

static int
is_boot_archive(char *root)
{
	char		path[PATH_MAX];
	struct stat	sb;
	int		error;
	const char	*fcn = "is_boot_archive()";

	/*
	 * We can't create an archive without the create_ramdisk script
	 */
	(void) snprintf(path, sizeof (path), "%s/%s", root, CREATE_RAMDISK);
	error = stat(path, &sb);
	INJECT_ERROR1("NOT_ARCHIVE_BASED", error = -1);
	if (error == -1) {
		if (bam_verbose)
			bam_print(_("file not found: %s\n"), path);
		BAM_DPRINTF(("%s: not a boot archive based Solaris "
		    "instance: %s\n", fcn, root));
		return (0);
	}

	BAM_DPRINTF(("%s: *IS* a boot archive based Solaris instance: %s\n",
	    fcn, root));
	return (1);
}

int
is_zfs(char *root)
{
	struct statvfs		vfs;
	int			ret;
	const char		*fcn = "is_zfs()";

	ret = statvfs(root, &vfs);
	INJECT_ERROR1("STATVFS_ZFS", ret = 1);
	if (ret != 0) {
		bam_error(_("statvfs failed for %s: %s\n"), root,
		    strerror(errno));
		return (0);
	}

	if (strncmp(vfs.f_basetype, "zfs", strlen("zfs")) == 0) {
		BAM_DPRINTF(("%s: is a ZFS filesystem: %s\n", fcn, root));
		return (1);
	} else {
		BAM_DPRINTF(("%s: is *NOT* a ZFS filesystem: %s\n", fcn, root));
		return (0);
	}
}

static int
is_readonly(char *root)
{
	int		fd;
	int		error;
	char		testfile[PATH_MAX];
	const char	*fcn = "is_readonly()";

	/*
	 * Using statvfs() to check for a read-only filesystem is not
	 * reliable. The only way to reliably test is to attempt to
	 * create a file
	 */
	(void) snprintf(testfile, sizeof (testfile), "%s/%s.%d",
	    root, BOOTADM_RDONLY_TEST, getpid());

	(void) unlink(testfile);

	errno = 0;
	fd = open(testfile, O_RDWR|O_CREAT|O_EXCL, 0644);
	error = errno;
	INJECT_ERROR2("RDONLY_TEST_ERROR", fd = -1, error = EACCES);
	if (fd == -1 && error == EROFS) {
		BAM_DPRINTF(("%s: is a READONLY filesystem: %s\n", fcn, root));
		return (1);
	} else if (fd == -1) {
		bam_error(_("error during read-only test on %s: %s\n"),
		    root, strerror(error));
	}

	(void) close(fd);
	(void) unlink(testfile);

	BAM_DPRINTF(("%s: is a RDWR filesystem: %s\n", fcn, root));
	return (0);
}

static error_t
update_archive(char *root, char *opt)
{
	error_t ret;

	assert(root);
	assert(opt == NULL);

	init_walk_args();
	(void) umask(022);

	/*
	 * Never update non-BE root in update_all
	 */
	if (!is_be(root) && bam_update_all)
		return (BAM_SUCCESS);
	/*
	 * root must belong to a boot archive based OS,
	 */
	if (!is_boot_archive(root)) {
		/*
		 * Emit message only if not in context of update_all.
		 * If in update_all, emit only if verbose flag is set.
		 */
		if (!bam_update_all || bam_verbose)
			bam_print(_("%s: not a boot archive based Solaris "
			    "instance\n"), root);
		return (BAM_ERROR);
	}

	/*
	 * If smf check is requested when / is writable (can happen
	 * on first reboot following an upgrade because service
	 * dependency is messed up), skip the check.
	 */
	if (bam_smf_check && !bam_root_readonly && !is_zfs(root))
		return (BAM_SUCCESS);

	/*
	 * Don't generate archive on ramdisk.
	 */
	if (is_ramdisk(root))
		return (BAM_SUCCESS);

	/*
	 * root must be writable. This check applies to alternate
	 * root (-R option); bam_root_readonly applies to '/' only.
	 * The behaviour translates into being the one of a 'check'.
	 */
	if (!bam_smf_check && !bam_check && is_readonly(root)) {
		set_flag(RDONLY_FSCHK);
		bam_check = 1;
	}

	/*
	 * Now check if an update is really needed.
	 */
	ret = update_required(root);

	/*
	 * The check command (-n) is *not* a dry run.
	 * It only checks if the archive is in sync.
	 * A readonly filesystem has to be considered an error only if an update
	 * is required.
	 */
	if (bam_nowrite()) {
		if (is_flag_on(RDONLY_FSCHK)) {
			bam_check = bam_saved_check;
			if (ret > 0)
				bam_error(_("%s filesystem is read-only, "
				    "skipping archives update\n"), root);
			if (bam_update_all)
				return ((ret != 0) ? BAM_ERROR : BAM_SUCCESS);
		}

		bam_exit((ret != 0) ? 1 : 0);
	}

	if (ret == 1) {
		/* create the ramdisk */
		ret = create_ramdisk(root);
	}

	/*
	 * if the archive is updated, save the new stat data and update the
	 * timestamp file
	 */
	if (ret == 0 && walk_arg.new_nvlp != NULL) {
		savenew(root);
		update_timestamp(root);
	}

	clear_walk_args();

	return (ret);
}

static error_t
update_all(char *root, char *opt)
{
	struct extmnttab mnt;
	struct stat sb;
	FILE *fp;
	char multibt[PATH_MAX];
	char creatram[PATH_MAX];
	error_t ret = BAM_SUCCESS;

	assert(root);
	assert(opt == NULL);

	if (bam_rootlen != 1 || *root != '/') {
		elide_trailing_slash(root, multibt, sizeof (multibt));
		bam_error(_("an alternate root (%s) cannot be used with this "
		    "sub-command\n"), multibt);
		return (BAM_ERROR);
	}

	/*
	 * First update archive for current root
	 */
	if (update_archive(root, opt) != BAM_SUCCESS)
		ret = BAM_ERROR;

	if (ret == BAM_ERROR)
		goto out;

	/*
	 * Now walk the mount table, performing archive update
	 * for all mounted Newboot root filesystems
	 */
	fp = fopen(MNTTAB, "r");
	if (fp == NULL) {
		bam_error(_("failed to open file: %s: %s\n"),
		    MNTTAB, strerror(errno));
		ret = BAM_ERROR;
		goto out;
	}

	resetmnttab(fp);

	while (getextmntent(fp, &mnt, sizeof (mnt)) == 0) {
		if (mnt.mnt_special == NULL)
			continue;
		if ((strcmp(mnt.mnt_fstype, MNTTYPE_ZFS) != 0) &&
		    (strncmp(mnt.mnt_special, "/dev/", strlen("/dev/")) != 0))
			continue;
		if (strcmp(mnt.mnt_mountp, "/") == 0)
			continue;

		(void) snprintf(creatram, sizeof (creatram), "%s/%s",
		    mnt.mnt_mountp, CREATE_RAMDISK);

		if (stat(creatram, &sb) == -1)
			continue;

		/*
		 * We put a trailing slash to be consistent with root = "/"
		 * case, such that we don't have to print // in some cases.
		 */
		(void) snprintf(rootbuf, sizeof (rootbuf), "%s/",
		    mnt.mnt_mountp);
		bam_rootlen = strlen(rootbuf);

		/*
		 * It's possible that other mounts may be an alternate boot
		 * architecture, so check it again.
		 */
		if ((get_boot_cap(rootbuf) != BAM_SUCCESS) ||
		    (update_archive(rootbuf, opt) != BAM_SUCCESS))
			ret = BAM_ERROR;
	}

	(void) fclose(fp);

out:
	/*
	 * We no longer use biosdev for Live Upgrade. Hence
	 * there is no need to defer (to shutdown time) any fdisk
	 * updates
	 */
	if (stat(GRUB_fdisk, &sb) == 0 || stat(GRUB_fdisk_target, &sb) == 0) {
		bam_error(_("Deferred FDISK update file(s) found: %s, %s. "
		    "Not supported.\n"), GRUB_fdisk, GRUB_fdisk_target);
	}

	return (ret);
}

static char *
get_mountpoint(char *special, char *fstype)
{
	FILE		*mntfp;
	struct mnttab	mp = {0};
	struct mnttab	mpref = {0};
	int		error;
	int		ret;
	const char	*fcn = "get_mountpoint()";

	BAM_DPRINTF(("%s: entered. args: %s %s\n", fcn, special, fstype));

	mntfp = fopen(MNTTAB, "r");
	error = errno;
	INJECT_ERROR1("MNTTAB_ERR_GET_MNTPT", mntfp = NULL);
	if (mntfp == NULL) {
		bam_error(_("failed to open file: %s: %s\n"),
		    MNTTAB, strerror(error));
		return (NULL);
	}

	mpref.mnt_special = special;
	mpref.mnt_fstype = fstype;

	ret = getmntany(mntfp, &mp, &mpref);
	INJECT_ERROR1("GET_MOUNTPOINT_MNTANY", ret = 1);
	if (ret != 0) {
		(void) fclose(mntfp);
		BAM_DPRINTF(("%s: no mount-point for special=%s and "
		    "fstype=%s\n", fcn, special, fstype));
		return (NULL);
	}
	(void) fclose(mntfp);

	assert(mp.mnt_mountp);

	BAM_DPRINTF(("%s: returning mount-point for special %s: %s\n",
	    fcn, special, mp.mnt_mountp));

	return (s_strdup(mp.mnt_mountp));
}

/*
 * Mounts a "legacy" top dataset (if needed)
 * Returns:	The mountpoint of the legacy top dataset or NULL on error
 * 		mnted returns one of the above values defined for zfs_mnted_t
 */
static char *
mount_legacy_dataset(char *pool, zfs_mnted_t *mnted)
{
	char		cmd[PATH_MAX];
	char		tmpmnt[PATH_MAX];
	filelist_t	flist = {0};
	char		*is_mounted;
	struct stat	sb;
	int		ret;
	const char	*fcn = "mount_legacy_dataset()";

	BAM_DPRINTF(("%s: entered. arg: %s\n", fcn, pool));

	*mnted = ZFS_MNT_ERROR;

	(void) snprintf(cmd, sizeof (cmd),
	    "/sbin/zfs get -Ho value mounted %s",
	    pool);

	ret = exec_cmd(cmd, &flist);
	INJECT_ERROR1("Z_MOUNT_LEG_GET_MOUNTED_CMD", ret = 1);
	if (ret != 0) {
		bam_error(_("failed to determine mount status of ZFS "
		    "pool %s\n"), pool);
		return (NULL);
	}

	INJECT_ERROR1("Z_MOUNT_LEG_GET_MOUNTED_OUT", flist.head = NULL);
	if ((flist.head == NULL) || (flist.head != flist.tail)) {
		bam_error(_("ZFS pool %s has bad mount status\n"), pool);
		filelist_free(&flist);
		return (NULL);
	}

	is_mounted = strtok(flist.head->line, " \t\n");
	INJECT_ERROR1("Z_MOUNT_LEG_GET_MOUNTED_STRTOK_YES", is_mounted = "yes");
	INJECT_ERROR1("Z_MOUNT_LEG_GET_MOUNTED_STRTOK_NO", is_mounted = "no");
	if (strcmp(is_mounted, "no") != 0) {
		filelist_free(&flist);
		*mnted = LEGACY_ALREADY;
		/* get_mountpoint returns a strdup'ed string */
		BAM_DPRINTF(("%s: legacy pool %s already mounted\n",
		    fcn, pool));
		return (get_mountpoint(pool, "zfs"));
	}

	filelist_free(&flist);

	/*
	 * legacy top dataset is not mounted. Mount it now
	 * First create a mountpoint.
	 */
	(void) snprintf(tmpmnt, sizeof (tmpmnt), "%s.%d",
	    ZFS_LEGACY_MNTPT, getpid());

	ret = stat(tmpmnt, &sb);
	if (ret == -1) {
		BAM_DPRINTF(("%s: legacy pool %s mount-point %s absent\n",
		    fcn, pool, tmpmnt));
		ret = mkdirp(tmpmnt, DIR_PERMS);
		INJECT_ERROR1("Z_MOUNT_TOP_LEG_MNTPT_MKDIRP", ret = -1);
		if (ret == -1) {
			bam_error(_("mkdir of %s failed: %s\n"), tmpmnt,
			    strerror(errno));
			return (NULL);
		}
	} else {
		BAM_DPRINTF(("%s: legacy pool %s mount-point %s is already "
		    "present\n", fcn, pool, tmpmnt));
	}

	(void) snprintf(cmd, sizeof (cmd),
	    "/sbin/mount -F zfs %s %s",
	    pool, tmpmnt);

	ret = exec_cmd(cmd, NULL);
	INJECT_ERROR1("Z_MOUNT_TOP_LEG_MOUNT_CMD", ret = 1);
	if (ret != 0) {
		bam_error(_("mount of ZFS pool %s failed\n"), pool);
		(void) rmdir(tmpmnt);
		return (NULL);
	}

	*mnted = LEGACY_MOUNTED;
	BAM_DPRINTF(("%s: legacy pool %s successfully mounted at %s\n",
	    fcn, pool, tmpmnt));
	return (s_strdup(tmpmnt));
}

/*
 * Mounts the top dataset (if needed)
 * Returns:	The mountpoint of the top dataset or NULL on error
 * 		mnted returns one of the above values defined for zfs_mnted_t
 */
char *
mount_top_dataset(char *pool, zfs_mnted_t *mnted)
{
	char		cmd[PATH_MAX];
	filelist_t	flist = {0};
	char		*is_mounted;
	char		*mntpt;
	char		*zmntpt;
	int		ret;
	const char	*fcn = "mount_top_dataset()";

	*mnted = ZFS_MNT_ERROR;

	BAM_DPRINTF(("%s: entered. arg: %s\n", fcn, pool));

	/*
	 * First check if the top dataset is a "legacy" dataset
	 */
	(void) snprintf(cmd, sizeof (cmd),
	    "/sbin/zfs get -Ho value mountpoint %s",
	    pool);
	ret = exec_cmd(cmd, &flist);
	INJECT_ERROR1("Z_MOUNT_TOP_GET_MNTPT", ret = 1);
	if (ret != 0) {
		bam_error(_("failed to determine mount point of ZFS pool %s\n"),
		    pool);
		return (NULL);
	}

	if (flist.head && (flist.head == flist.tail)) {
		char *legacy = strtok(flist.head->line, " \t\n");
		if (legacy && strcmp(legacy, "legacy") == 0) {
			filelist_free(&flist);
			BAM_DPRINTF(("%s: is legacy, pool=%s\n", fcn, pool));
			return (mount_legacy_dataset(pool, mnted));
		}
	}

	filelist_free(&flist);

	BAM_DPRINTF(("%s: is *NOT* legacy, pool=%s\n", fcn, pool));

	(void) snprintf(cmd, sizeof (cmd),
	    "/sbin/zfs get -Ho value mounted %s",
	    pool);

	ret = exec_cmd(cmd, &flist);
	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_GET_MOUNTED", ret = 1);
	if (ret != 0) {
		bam_error(_("failed to determine mount status of ZFS "
		    "pool %s\n"), pool);
		return (NULL);
	}

	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_GET_MOUNTED_VAL", flist.head = NULL);
	if ((flist.head == NULL) || (flist.head != flist.tail)) {
		bam_error(_("ZFS pool %s has bad mount status\n"), pool);
		filelist_free(&flist);
		return (NULL);
	}

	is_mounted = strtok(flist.head->line, " \t\n");
	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_GET_MOUNTED_YES", is_mounted = "yes");
	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_GET_MOUNTED_NO", is_mounted = "no");
	if (strcmp(is_mounted, "no") != 0) {
		filelist_free(&flist);
		*mnted = ZFS_ALREADY;
		BAM_DPRINTF(("%s: non-legacy pool %s mounted already\n",
		    fcn, pool));
		goto mounted;
	}

	filelist_free(&flist);
	BAM_DPRINTF(("%s: non-legacy pool %s *NOT* already mounted\n",
	    fcn, pool));

	/* top dataset is not mounted. Mount it now */
	(void) snprintf(cmd, sizeof (cmd),
	    "/sbin/zfs mount %s", pool);
	ret = exec_cmd(cmd, NULL);
	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_MOUNT_CMD", ret = 1);
	if (ret != 0) {
		bam_error(_("mount of ZFS pool %s failed\n"), pool);
		return (NULL);
	}
	*mnted = ZFS_MOUNTED;
	BAM_DPRINTF(("%s: non-legacy pool %s mounted now\n", fcn, pool));
	/*FALLTHRU*/
mounted:
	/*
	 * Now get the mountpoint
	 */
	(void) snprintf(cmd, sizeof (cmd),
	    "/sbin/zfs get -Ho value mountpoint %s",
	    pool);

	ret = exec_cmd(cmd, &flist);
	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_GET_MNTPT_CMD", ret = 1);
	if (ret != 0) {
		bam_error(_("failed to determine mount point of ZFS pool %s\n"),
		    pool);
		goto error;
	}

	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_GET_MNTPT_OUT", flist.head = NULL);
	if ((flist.head == NULL) || (flist.head != flist.tail)) {
		bam_error(_("ZFS pool %s has no mount-point\n"), pool);
		goto error;
	}

	mntpt = strtok(flist.head->line, " \t\n");
	INJECT_ERROR1("Z_MOUNT_TOP_NONLEG_GET_MNTPT_STRTOK", mntpt = "foo");
	if (*mntpt != '/') {
		bam_error(_("ZFS pool %s has bad mount-point %s\n"),
		    pool, mntpt);
		goto error;
	}
	zmntpt = s_strdup(mntpt);

	filelist_free(&flist);

	BAM_DPRINTF(("%s: non-legacy pool %s is mounted at %s\n",
	    fcn, pool, zmntpt));

	return (zmntpt);

error:
	filelist_free(&flist);
	(void) umount_top_dataset(pool, *mnted, NULL);
	BAM_DPRINTF(("%s: returning FAILURE\n", fcn));
	return (NULL);
}

int
umount_top_dataset(char *pool, zfs_mnted_t mnted, char *mntpt)
{
	char		cmd[PATH_MAX];
	int		ret;
	const char	*fcn = "umount_top_dataset()";

	INJECT_ERROR1("Z_UMOUNT_TOP_INVALID_STATE", mnted = ZFS_MNT_ERROR);
	switch (mnted) {
	case LEGACY_ALREADY:
	case ZFS_ALREADY:
		/* nothing to do */
		BAM_DPRINTF(("%s: pool %s was already mounted at %s, Nothing "
		    "to umount\n", fcn, pool, mntpt ? mntpt : "NULL"));
		free(mntpt);
		return (BAM_SUCCESS);
	case LEGACY_MOUNTED:
		(void) snprintf(cmd, sizeof (cmd),
		    "/sbin/umount %s", pool);
		ret = exec_cmd(cmd, NULL);
		INJECT_ERROR1("Z_UMOUNT_TOP_LEGACY_UMOUNT_FAIL", ret = 1);
		if (ret != 0) {
			bam_error(_("umount of %s failed\n"), pool);
			free(mntpt);
			return (BAM_ERROR);
		}
		if (mntpt)
			(void) rmdir(mntpt);
		free(mntpt);
		BAM_DPRINTF(("%s: legacy pool %s was mounted by us, "
		    "successfully unmounted\n", fcn, pool));
		return (BAM_SUCCESS);
	case ZFS_MOUNTED:
		free(mntpt);
		(void) snprintf(cmd, sizeof (cmd),
		    "/sbin/zfs unmount %s", pool);
		ret = exec_cmd(cmd, NULL);
		INJECT_ERROR1("Z_UMOUNT_TOP_NONLEG_UMOUNT_FAIL", ret = 1);
		if (ret != 0) {
			bam_error(_("umount of %s failed\n"), pool);
			return (BAM_ERROR);
		}
		BAM_DPRINTF(("%s: nonleg pool %s was mounted by us, "
		    "successfully unmounted\n", fcn, pool));
		return (BAM_SUCCESS);
	default:
		bam_error(_("Internal error: bad saved mount state for "
		    "pool %s\n"), pool);
		return (BAM_ERROR);
	}
	/*NOTREACHED*/
}

char *
get_special(char *mountp)
{
	FILE		*mntfp;
	struct mnttab	mp = {0};
	struct mnttab	mpref = {0};
	int		error;
	int		ret;
	const char 	*fcn = "get_special()";

	INJECT_ERROR1("GET_SPECIAL_MNTPT", mountp = NULL);
	if (mountp == NULL) {
		bam_error(_("cannot get special file: NULL mount-point\n"));
		return (NULL);
	}

	mntfp = fopen(MNTTAB, "r");
	error = errno;
	INJECT_ERROR1("GET_SPECIAL_MNTTAB_OPEN", mntfp = NULL);
	if (mntfp == NULL) {
		bam_error(_("failed to open file: %s: %s\n"), MNTTAB,
		    strerror(error));
		return (NULL);
	}

	if (*mountp == '\0')
		mpref.mnt_mountp = "/";
	else
		mpref.mnt_mountp = mountp;

	ret = getmntany(mntfp, &mp, &mpref);
	INJECT_ERROR1("GET_SPECIAL_MNTTAB_SEARCH", ret = 1);
	if (ret != 0) {
		(void) fclose(mntfp);
		BAM_DPRINTF(("%s: Cannot get special file:  mount-point %s "
		    "not in mnttab\n", fcn, mountp));
		return (NULL);
	}
	(void) fclose(mntfp);

	BAM_DPRINTF(("%s: returning special: %s\n", fcn, mp.mnt_special));

	return (s_strdup(mp.mnt_special));
}

static void
line_free(line_t *lp)
{
	if (lp == NULL)
		return;

	free(lp->cmd);
	free(lp->sep);
	free(lp->arg);
	free(lp->line);
	free(lp);
}

static void
linelist_free(line_t *start)
{
	line_t *lp;

	while (start) {
		lp = start;
		start = start->next;
		line_free(lp);
	}
}

static void
filelist_free(filelist_t *flistp)
{
	linelist_free(flistp->head);
	flistp->head = NULL;
	flistp->tail = NULL;
}

/*
 * Utility routines
 */


/*
 * Returns 0 on success
 * Any other value indicates an error
 */
static int
exec_cmd(char *cmdline, filelist_t *flistp)
{
	char buf[BUFSIZ];
	int ret;
	FILE *ptr;
	sigset_t set;
	void (*disp)(int);

	/*
	 * For security
	 * - only absolute paths are allowed
	 * - set IFS to space and tab
	 */
	if (*cmdline != '/') {
		bam_error(_("path is not absolute: %s\n"), cmdline);
		return (-1);
	}
	(void) putenv("IFS= \t");

	/*
	 * We may have been exec'ed with SIGCHLD blocked
	 * unblock it here
	 */
	(void) sigemptyset(&set);
	(void) sigaddset(&set, SIGCHLD);
	if (sigprocmask(SIG_UNBLOCK, &set, NULL) != 0) {
		bam_error(_("cannot unblock SIGCHLD: %s\n"), strerror(errno));
		return (-1);
	}

	/*
	 * Set SIGCHLD disposition to SIG_DFL for popen/pclose
	 */
	disp = sigset(SIGCHLD, SIG_DFL);
	if (disp == SIG_ERR) {
		bam_error(_("cannot set SIGCHLD disposition: %s\n"),
		    strerror(errno));
		return (-1);
	}
	if (disp == SIG_HOLD) {
		bam_error(_("SIGCHLD signal blocked. Cannot exec: %s\n"),
		    cmdline);
		return (-1);
	}

	ptr = popen(cmdline, "r");
	if (ptr == NULL) {
		bam_error(_("popen failed: %s: %s\n"), cmdline,
		    strerror(errno));
		return (-1);
	}

	/*
	 * If we simply do a pclose() following a popen(), pclose()
	 * will close the reader end of the pipe immediately even
	 * if the child process has not started/exited. pclose()
	 * does wait for cmd to terminate before returning though.
	 * When the executed command writes its output to the pipe
	 * there is no reader process and the command dies with
	 * SIGPIPE. To avoid this we read repeatedly until read
	 * terminates with EOF. This indicates that the command
	 * (writer) has closed the pipe and we can safely do a
	 * pclose().
	 *
	 * Since pclose() does wait for the command to exit,
	 * we can safely reap the exit status of the command
	 * from the value returned by pclose()
	 */
	while (s_fgets(buf, sizeof (buf), ptr) != NULL) {
		if (flistp == NULL) {
			/* s_fgets strips newlines, so insert them at the end */
			bam_print(_("%s\n"), buf);
		} else {
			append_to_flist(flistp, buf);
		}
	}

	ret = pclose(ptr);
	if (ret == -1) {
		bam_error(_("pclose failed: %s: %s\n"), cmdline,
		    strerror(errno));
		return (-1);
	}

	if (WIFEXITED(ret)) {
		return (WEXITSTATUS(ret));
	} else {
		bam_error(_("command terminated abnormally: %s: %d\n"),
		    cmdline, ret);
		return (-1);
	}
}

/*
 * Since this function returns -1 on error
 * it cannot be used to convert -1. However,
 * that is sufficient for what we need.
 */
static long
s_strtol(char *str)
{
	long l;
	char *res = NULL;

	if (str == NULL) {
		return (-1);
	}

	errno = 0;
	l = strtol(str, &res, 10);
	if (errno || *res != '\0') {
		return (-1);
	}

	return (l);
}

/*
 * Wrapper around fgets, that strips newlines returned by fgets
 */
char *
s_fgets(char *buf, int buflen, FILE *fp)
{
	int n;

	buf = fgets(buf, buflen, fp);
	if (buf) {
		n = strlen(buf);
		if (n == buflen - 1 && buf[n-1] != '\n')
			bam_error(_("the following line is too long "
			    "(> %d chars)\n\t%s\n"), buflen - 1, buf);
		buf[n-1] = (buf[n-1] == '\n') ? '\0' : buf[n-1];
	}

	return (buf);
}

void *
s_calloc(size_t nelem, size_t sz)
{
	void *ptr;

	ptr = calloc(nelem, sz);
	if (ptr == NULL) {
		bam_error(_("could not allocate memory: size = %u\n"),
		    nelem*sz);
		bam_exit(1);
	}
	return (ptr);
}

void *
s_realloc(void *ptr, size_t sz)
{
	ptr = realloc(ptr, sz);
	if (ptr == NULL) {
		bam_error(_("could not allocate memory: size = %u\n"), sz);
		bam_exit(1);
	}
	return (ptr);
}

char *
s_strdup(char *str)
{
	char *ptr;

	if (str == NULL)
		return (NULL);

	ptr = strdup(str);
	if (ptr == NULL) {
		bam_error(_("could not allocate memory: size = %u\n"),
		    strlen(str) + 1);
		bam_exit(1);
	}
	return (ptr);
}

static char *
get_machine(void)
{
	static int cached = -1;
	static char mbuf[257];	/* from sysinfo(2) manpage */

	if (cached == 0)
		return (mbuf);

	if (bam_alt_platform) {
		return (bam_platform);
	} else {
		if (sysinfo(SI_MACHINE, mbuf, sizeof (mbuf)) > 0) {
			cached = 1;
		}
	}
	if (cached == -1) {
		mbuf[0] = '\0';
		cached = 0;
	}

	return (mbuf);
}

static void
append_to_flist(filelist_t *flistp, char *s)
{
	line_t *lp;

	lp = s_calloc(1, sizeof (line_t));
	lp->line = s_strdup(s);
	if (flistp->head == NULL)
		flistp->head = lp;
	else
		flistp->tail->next = lp;
	flistp->tail = lp;
}

#if !defined(_OBP)

UCODE_VENDORS;

/*ARGSUSED*/
static void
ucode_install(char *root)
{
	int i;

	for (i = 0; ucode_vendors[i].filestr != NULL; i++) {
		int cmd_len = PATH_MAX + 256;
		char cmd[PATH_MAX + 256];
		char file[PATH_MAX];
		char timestamp[PATH_MAX];
		struct stat fstatus, tstatus;
		struct utimbuf u_times;

		(void) snprintf(file, PATH_MAX, "%s/%s/%s-ucode.%s",
		    bam_root, UCODE_INSTALL_PATH, ucode_vendors[i].filestr,
		    ucode_vendors[i].extstr);

		if (stat(file, &fstatus) != 0 || !(S_ISREG(fstatus.st_mode)))
			continue;

		(void) snprintf(timestamp, PATH_MAX, "%s.ts", file);

		if (stat(timestamp, &tstatus) == 0 &&
		    fstatus.st_mtime <= tstatus.st_mtime)
			continue;

		(void) snprintf(cmd, cmd_len, "/usr/sbin/ucodeadm -i -R "
		    "%s/%s/%s %s > /dev/null 2>&1", bam_root,
		    UCODE_INSTALL_PATH, ucode_vendors[i].vendorstr, file);
		if (system(cmd) != 0)
			return;

		if (creat(timestamp, S_IRUSR | S_IWUSR) == -1)
			return;

		u_times.actime = fstatus.st_atime;
		u_times.modtime = fstatus.st_mtime;
		(void) utime(timestamp, &u_times);
	}
}
#endif
