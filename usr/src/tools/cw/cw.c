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
 * Copyright 2011, Richard Lowe.
 */
/*
 * Copyright 2010 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*
 * Wrapper for the GNU C compiler to make it accept the Sun C compiler
 * arguments where possible.
 *
 * Since the translation is inexact, this is something of a work-in-progress.
 *
 */

/* If you modify this file, you must increment CW_VERSION */
#define	CW_VERSION	"1.30"

/*
 * -A<name[(tokens)]>	Preprocessor predicate assertion
 * -B<[static|dynamic]>	Specify dynamic or static binding
 * -C		Prevent preprocessor from removing comments
 * -c		Compile only - produce .o files, suppress linking
 * -D<name[=token]>	Associate name with token as if by #define
 * -E		Compile source through preprocessor only, output to stdout
 * -errtags=<a>	Display messages with tags a(no, yes)
 *		as errors
 * -flags	Show this summary of compiler options
 * -G		Build a dynamic shared library
 * -g		Compile for debugging
 * -H		Print path name of each file included during compilation
 * -h <name>	Assign <name> to generated dynamic shared library
 * -I<dir>	Add <dir> to preprocessor #include file search path
 * -i		Passed to linker to ignore any LD_LIBRARY_PATH setting
 * -keeptmp	Keep temporary files created during compilation
 * -KPIC	Compile position independent code with 32-bit addresses
 * -Kpic	Compile position independent code
 * -L<dir>	Pass to linker to add <dir> to the library search path
 * -l<name>	Link with library lib<name>.a or lib<name>.so
 * -mc		Remove duplicate strings from .comment section of output files
 * -mr		Remove all strings from .comment section of output files
 * -mr,"string"	Remove all strings and append "string" to .comment section
 * -mt		Specify options needed when compiling multi-threaded code
 * -native	Find available processor, generate code accordingly
 * -nolib	Same as -xnolib
 * -O		Use default optimization level (-xO2 or -xO3. Check man page.)
 * -o <outputfile> Set name of output file to <outputfile>
 * -P		Compile source through preprocessor only, output to .i  file
 * -PIC		Alias for -KPIC or -xcode=pic32
 * -p		Compile for profiling with prof
 * -pic		Alias for -Kpic or -xcode=pic13
 * -R<dir[:dir]> Build runtime search path list into executable
 * -S		Compile and only generate assembly code (.s)
 * -s		Strip symbol table from the executable file
 * -t		Turn off duplicate symbol warnings when linking
 * -U<name>	Delete initial definition of preprocessor symbol <name>
 * -V		Report version number of each compilation phase
 * -v		Do stricter semantic checking
 * -W<c>,<arg>	Pass <arg> to specified component <c> (a,l,m,p,0,2,h,i,u)
 * -w		Suppress compiler warning messages
 * -Xa		Compile assuming ANSI C conformance, allow K & R extensions
 *		(default mode)
 * -Xc		Compile assuming strict ANSI C conformance
 * -Xs		Compile assuming (pre-ANSI) K & R C style code
 * -Xt		Compile assuming K & R conformance, allow ANSI C
 * -xarch=<a>	Specify target architecture instruction set
 *		for system functions, b={%all,%none}
 * -xe		Perform only syntax/semantic checking, no code generation
 * -xlicinfo	Show license server information
 * -xM		Generate makefile dependencies
 * -xM1		Generate makefile dependencies, but exclude /usr/include
 * -xmaxopt=[off,1,2,3,4,5] maximum optimization level allowed on #pragma opt
 * -xnolib	Do not link with default system libraries
 * -xpg		Compile for profiling with gprof
 * -xprofile=<p> Collect data for a profile or use a profile to optimize
 *		<p>={{collect,use}[:<path>],tcov}
 * -xs		Allow debugging without object (.o) files
 * -xsb		Compile for use with the WorkShop source browser
 * -xsbfast	Generate only WorkShop source browser info, no compilation
 * -Y<c>,<dir>	Specify <dir> for location of component <c> (a,l,m,p,0,h,i,u)
 * -YA,<dir>	Change default directory searched for components
 * -YI,<dir>	Change default directory searched for include files
 */

/*
 * Translation table:
 */
/*
 * -#				-v
 * -A<name[(tokens)]>		pass-thru
 * -B<[static|dynamic]>		pass-thru (syntax error for anything else)
 * -C				pass-thru
 * -c				pass-thru
 * -D<name[=token]>		pass-thru
 * -E				pass-thru
 * -errtags=%all		-Wall
 * -flags			--help
 * -G				pass-thru
 * -g				pass-thru
 * -H				pass-thru
 * -h <name>			pass-thru
 * -I<dir>			pass-thru
 * -i				pass-thru
 * -KPIC			-fPIC
 * -Kpic			-fpic
 * -L<dir>			pass-thru
 * -l<name>			pass-thru
 * -mc				error
 * -mr				error
 * -mr,"string"			error
 * -mt				-D_REENTRANT
 * -native			error
 * -nolib			-nodefaultlibs
 * -O				-O1 (Check the man page to be certain)
 * -o <outputfile>		pass-thru
 * -P				-E -o filename.i (or error)
 * -PIC				-fPIC (C++ only)
 * -p				pass-thru
 * -pic				-fpic (C++ only)
 * -R<dir[:dir]>		pass-thru
 * -S				pass-thru
 * -s				-Wl,-s
 * -t				-Wl,-t
 * -U<name>			pass-thru
 * -V				--version
 * -v				-Wall
 * -Wa,<arg>			pass-thru
 * -Wp,<arg>			pass-thru
 * -Wl,<arg>			pass-thru
 * -W{m,0,2,h,i,u>		error/ignore
 * -Wu,-xmodel=kernel		-ffreestanding -mcmodel=kernel -mno-red-zone
 * -xmodel=kernel		-ffreestanding -mcmodel=kernel -mno-red-zone
 * -Wu,-save_args		-msave-args
 * -w				pass-thru
 * -Xa				-std=iso9899:199409 or -ansi
 * -Xc				-ansi -pedantic
 * -Xt				error
 * -Xs				-traditional -std=c89
 * -xarch=<a>			table
 * -xe				error
 * -xM				-M
 * -xM1				-MM
 * -xmaxopt=[...]		error
 * -xnolib			-nodefaultlibs
 * -xpg				error
 * -xprofile=<p>		error
 * -xs				error
 * -xsb				error
 * -xsbfast			error
 * -W0,-xdbggen=no%usedonly	-fno-eliminate-unused-debug-symbols
 *				-fno-eliminate-unused-debug-types
 * -Y<c>,<dir>			error
 * -YA,<dir>			error
 * -YI,<dir>			-nostdinc -I<dir>
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/utsname.h>
#include <sys/param.h>
#include <sys/isa_defs.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define	CW_F_CXX	0x01
#define	CW_F_EXEC	0x04
#define	CW_F_ECHO	0x08
#define	CW_F_XLATE	0x10
#define	CW_F_PROG	0x20

typedef enum cw_compiler {
	CW_C_GCC = 0,
} cw_compiler_t;

static const char *cmds[] = {
	"gcc", "g++"
};

static char default_dir[2][MAXPATHLEN] = {
	DEFAULT_GCC_DIR,
};

#define	CC(ctx) (ctx)->i_compiler

#define	CIDX(compiler, flags)	\
	((int)(compiler) << 1) + ((flags) & CW_F_CXX ? 1 : 0)

typedef enum cw_op {
	CW_O_NONE = 0,
	CW_O_PREPROCESS,
	CW_O_COMPILE,
	CW_O_LINK
} cw_op_t;

struct aelist {
	struct ae {
		struct ae *ae_next;
		char *ae_arg;
	} *ael_head, *ael_tail;
	int ael_argc;
};

typedef struct cw_ictx {
	cw_compiler_t	i_compiler;
	struct aelist	*i_ae;
	uint32_t	i_flags;
	int		i_oldargc;
	char		**i_oldargv;
	pid_t		i_pid;
	char		*i_stderr;
} cw_ictx_t;

/*
 * Status values to indicate which Studio compiler and associated
 * flags are being used.
 */
#define	M32		0x01	/* -m32 - only on Studio 12 */
#define	M64		0x02	/* -m64 - only on Studio 12 */
#define	SS11		0x100	/* Studio 11 */
#define	SS12		0x200	/* Studio 12 */

#define	TRANS_ENTRY	5
/*
 * Translation table definition for the -xarch= flag. The "x_arg"
 * value is translated into the appropriate gcc flags according
 * to the values in x_trans[n]. The x_flags indicates what compiler
 * is being used and what flags have been set via the use of
 * "x_arg".
 */
typedef struct xarch_table {
	char	*x_arg;
	int	x_flags;
	char	*x_trans[TRANS_ENTRY];
} xarch_table_t;

/*
 * The translation table for the -xarch= flag used in the Studio compilers.
 */
static const xarch_table_t xtbl[] = {
#if defined(__x86)
	{ "generic",	SS11 },
	{ "generic64",	(SS11|M64), { "-m64", "-mtune=opteron" } },
	{ "amd64",	(SS11|M64), { "-m64", "-mtune=opteron" } },
	{ "386",	SS11,	{ "-march=i386" } },
	{ "pentium_pro", SS11,	{ "-march=pentiumpro" } },
	{ "sse",	SS11, { "-msse", "-mfpmath=sse" } },
	{ "sse2",	SS11, { "-msse2", "-mfpmath=sse" } },
#elif defined(__sparc)
	{ "generic",	(SS11|M32), { "-m32", "-mcpu=v8" } },
	{ "generic64",	(SS11|M64), { "-m64", "-mcpu=v9" } },
	{ "v8",		(SS11|M32), { "-m32", "-mcpu=v8", "-mno-v8plus" } },
	{ "v8plus",	(SS11|M32), { "-m32", "-mcpu=v9", "-mv8plus" } },
	{ "v8plusa",	(SS11|M32), { "-m32", "-mcpu=ultrasparc", "-mv8plus",
			"-mvis" } },
	{ "v8plusb",	(SS11|M32), { "-m32", "-mcpu=ultrasparc3", "-mv8plus",
			"-mvis" } },
	{ "v9",		(SS11|M64), { "-m64", "-mcpu=v9" } },
	{ "v9a",	(SS11|M64), { "-m64", "-mcpu=ultrasparc", "-mvis" } },
	{ "v9b",	(SS11|M64), { "-m64", "-mcpu=ultrasparc3", "-mvis" } },
	{ "sparc",	SS12, { "-mcpu=v9", "-mv8plus" } },
	{ "sparcvis",	SS12, { "-mcpu=ultrasparc", "-mvis" } },
	{ "sparcvis2",	SS12, { "-mcpu=ultrasparc3", "-mvis" } }
#endif
};

static int xtbl_size = sizeof (xtbl) / sizeof (xarch_table_t);

static const char *progname;

static void
nomem(void)
{
	(void) fprintf(stderr, "%s: error: out of memory\n", progname);
	exit(1);
}

static void
cw_perror(const char *fmt, ...)
{
	va_list ap;
	int saved_errno = errno;

	(void) fprintf(stderr, "%s: error: ", progname);

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);

	(void) fprintf(stderr, " (%s)\n", strerror(saved_errno));
}

static void
newae(struct aelist *ael, const char *arg)
{
	struct ae *ae;

	if ((ae = calloc(sizeof (*ae), 1)) == NULL)
		nomem();
	ae->ae_arg = strdup(arg);
	if (ael->ael_tail == NULL)
		ael->ael_head = ae;
	else
		ael->ael_tail->ae_next = ae;
	ael->ael_tail = ae;
	ael->ael_argc++;
}

static cw_ictx_t *
newictx(void)
{
	cw_ictx_t *ctx = calloc(sizeof (cw_ictx_t), 1);
	if (ctx)
		if ((ctx->i_ae = calloc(sizeof (struct aelist), 1)) == NULL) {
			free(ctx);
			return (NULL);
		}

	return (ctx);
}

static void
error(const char *arg)
{
	(void) fprintf(stderr,
	    "%s: error: mapping failed at or near arg '%s'\n", progname, arg);
	exit(2);
}

/*
 * Add the current favourite set of warnings to the gcc invocation.
 */
static void
warnings(struct aelist *h)
{
	static int warningsonce;

	if (warningsonce++)
		return;

	/*
	 * Enable as many warnings as exist, then disable those that we never
	 * ever want.
	 */
	newae(h, "-Wall");
	newae(h, "-Wextra");
}

/* ARGSUSED */
static void
Xamode(struct aelist *h)
{
}

static void
Xcmode(struct aelist *h)
{
	static int xconce;

	if (xconce++)
		return;

	newae(h, "-ansi");
	newae(h, "-pedantic-errors");
}

static void
Xsmode(struct aelist *h)
{
	static int xsonce;

	if (xsonce++)
		return;

	newae(h, "-traditional");
	newae(h, "-traditional-cpp");
}

static void
usage()
{
	(void) fprintf(stderr,
	    "usage: %s { -_gcc | -_g++ } ...\n",
	    progname);
	exit(2);
}

static int
xlate_xtb(struct aelist *h, const char *xarg)
{
	int	i, j;

	for (i = 0; i < xtbl_size; i++) {
		if (strcmp(xtbl[i].x_arg, xarg) == 0)
			break;
	}

	/*
	 * At the end of the table and so no matching "arg" entry
	 * found and so this must be a bad -xarch= flag.
	 */
	if (i == xtbl_size)
		error(xarg);

	for (j = 0; j < TRANS_ENTRY; j++) {
		if (xtbl[i].x_trans[j] != NULL)
			newae(h, xtbl[i].x_trans[j]);
		else
			break;
	}
	return (xtbl[i].x_flags);

}

static void
do_gcc(cw_ictx_t *ctx)
{
	int c;
	int pic = 0, nolibc = 0;
	int in_output = 0, seen_o = 0, c_files = 0;
	cw_op_t op = CW_O_LINK;
	char *model = NULL;
	int	mflag = 0;

	if (ctx->i_flags & CW_F_PROG) {
		newae(ctx->i_ae, "--version");
		return;
	}

	/*
	 * Walk the argument list, translating as we go ..
	 */

	while (--ctx->i_oldargc > 0) {
		char *arg = *++ctx->i_oldargv;
		size_t arglen = strlen(arg);

		if (*arg == '-') {
			arglen--;
		} else {
			/*
			 * Discard inline files that gcc doesn't grok
			 */
			if (!in_output && arglen > 3 &&
			    strcmp(arg + arglen - 3, ".il") == 0)
				continue;

			if (!in_output && arglen > 2 &&
			    arg[arglen - 2] == '.' &&
			    (arg[arglen - 1] == 'S' || arg[arglen - 1] == 's' ||
			    arg[arglen - 1] == 'c' || arg[arglen - 1] == 'i'))
				c_files++;

			/*
			 * Otherwise, filenames and partial arguments
			 * are passed through for gcc to chew on.
			 */
			newae(ctx->i_ae, arg);
			in_output = 0;
			continue;
		}

		if (ctx->i_flags & CW_F_CXX) {
			if (strcmp(arg, "-pic") == 0) {
				newae(ctx->i_ae, "-fpic");
				pic = 1;
				continue;
			}
			if (strcmp(arg, "-PIC") == 0) {
				newae(ctx->i_ae, "-fPIC");
				pic = 1;
				continue;
			}
			if (strcmp(arg, "-nolib") == 0) {
				/* -nodefaultlibs is on by default */
				nolibc = 1;
				continue;
			}
		}

		switch ((c = arg[1])) {
		case '_':
			if (strcmp(arg, "-_noecho") == 0)
				ctx->i_flags &= ~CW_F_ECHO;
			else if (strncmp(arg, "-_gcc=", 6) == 0 ||
			    strncmp(arg, "-_g++=", 6) == 0)
				newae(ctx->i_ae, arg + 6);
			else
				error(arg);
			break;
		case 'g':
			newae(ctx->i_ae, "-gdwarf-2");
			break;
		case 'E':
			if (arglen == 1) {
				newae(ctx->i_ae, "-xc");
				newae(ctx->i_ae, arg);
				op = CW_O_PREPROCESS;
				nolibc = 1;
				break;
			}
			error(arg);
			break;
		case 'c':
		case 'S':
			if (arglen == 1) {
				op = CW_O_COMPILE;
				nolibc = 1;
			}
			/* FALLTHROUGH */
		case 'C':
		case 'H':
		case 'p':
		case 'O':
			if (arglen == 1) {
				newae(ctx->i_ae, arg);
				break;
			}
			error(arg);
			break;
		case 'A':
		case 'h':
		case 'I':
		case 'i':
		case 'L':
		case 'l':
		case 'R':
		case 'U':
		case 'u':
		case 'w':
			newae(ctx->i_ae, arg);
			break;
		case 'o':
			seen_o = 1;
			if (arglen == 1) {
				in_output = 1;
				newae(ctx->i_ae, arg);
			} else {
				newae(ctx->i_ae, arg);
			}
			break;
		case 'D':
			newae(ctx->i_ae, arg);
			/*
			 * XXX	Clearly a hack ... do we need _KADB too?
			 */
			if (strcmp(arg, "-D_KERNEL") == 0 ||
			    strcmp(arg, "-D_BOOT") == 0)
				newae(ctx->i_ae, "-ffreestanding");
			break;
		case 'e':
			if (strcmp(arg, "-errtags=yes") == 0) {
				warnings(ctx->i_ae);
				break;
			}
			error(arg);
			break;
		case 'G':
			newae(ctx->i_ae, "-shared");
			nolibc = 1;
			break;
		case 'K':
			if (arglen == 1) {
				if ((arg = *++ctx->i_oldargv) == NULL ||
				    *arg == '\0')
					error("-K");
				ctx->i_oldargc--;
			} else {
				arg += 2;
			}
			if (strcmp(arg, "pic") == 0) {
				newae(ctx->i_ae, "-fpic");
				pic = 1;
				break;
			}
			if (strcmp(arg, "PIC") == 0) {
				newae(ctx->i_ae, "-fPIC");
				pic = 1;
				break;
			}
			error("-K");
			break;
		case 'm':
			if (strcmp(arg, "-m64") == 0) {
				newae(ctx->i_ae, "-m64");
#if defined(__x86)
				newae(ctx->i_ae, "-mtune=opteron");
#endif
				mflag |= M64;
				break;
			}
			if (strcmp(arg, "-m32") == 0) {
				newae(ctx->i_ae, "-m32");
				mflag |= M32;
				break;
			}
			error(arg);
			break;
		case 'B':	/* linker options */
		case 'M':
		case 'z':
			{
				char *opt;
				size_t len;
				char *s;

				if (arglen == 1) {
					opt = *++ctx->i_oldargv;
					if (opt == NULL || *opt == '\0')
						error(arg);
					ctx->i_oldargc--;
				} else {
					opt = arg + 2;
				}
				len = strlen(opt) + 7;
				if ((s = malloc(len)) == NULL)
					nomem();
				(void) snprintf(s, len, "-Wl,-%c%s", c, opt);
				newae(ctx->i_ae, s);
				free(s);
			}
			break;
		case 'P':
			/*
			 * We could do '-E -o filename.i', but that's hard,
			 * and we don't need it for the case that's triggering
			 * this addition.  We'll require the user to specify
			 * -o in the Makefile.  If they don't they'll find out
			 * in a hurry.
			 */
			newae(ctx->i_ae, "-E");
			op = CW_O_PREPROCESS;
			nolibc = 1;
			break;
		case 's':
			if (arglen == 1) {
				newae(ctx->i_ae, "-Wl,-s");
				break;
			}
			error(arg);
			break;
		case 't':
			if (arglen == 1) {
				newae(ctx->i_ae, "-Wl,-t");
				break;
			}
			error(arg);
			break;
		case 'v':
			if (arglen == 1) {
				warnings(ctx->i_ae);
				break;
			}
			error(arg);
			break;
		case 'W':
			if (strncmp(arg, "-Wa,", 4) == 0 ||
			    strncmp(arg, "-Wp,", 4) == 0 ||
			    strncmp(arg, "-Wl,", 4) == 0) {
				newae(ctx->i_ae, arg);
				break;
			}
			if (strcmp(arg, "-W0,-Lt") == 0) {
				/*
				 * Generate tests at the top of loops.
				 * There is no direct gcc equivalent, ignore.
				 */
				break;
			}
#if defined(__x86)
			if (strcmp(arg, "-Wu,-xmodel=kernel") == 0) {
				newae(ctx->i_ae, "-ffreestanding");
				newae(ctx->i_ae, "-mno-red-zone");
				model = "-mcmodel=kernel";
				nolibc = 1;
				break;
			}
			if (strcmp(arg, "-Wu,-save_args") == 0) {
				newae(ctx->i_ae, "-msave-args");
				break;
			}
#endif	/* __x86 */
			error(arg);
			break;
		case 'X':
			if (strcmp(arg, "-Xa") == 0 ||
			    strcmp(arg, "-Xt") == 0) {
				Xamode(ctx->i_ae);
				break;
			}
			if (strcmp(arg, "-Xc") == 0) {
				Xcmode(ctx->i_ae);
				break;
			}
			if (strcmp(arg, "-Xs") == 0) {
				Xsmode(ctx->i_ae);
				break;
			}
			error(arg);
			break;
		case 'x':
			if (arglen == 1)
				error(arg);
			switch (arg[2]) {
			case 'a':
				if (strncmp(arg, "-xarch=", 7) == 0) {
					mflag |= xlate_xtb(ctx->i_ae, arg + 7);
					break;
				}
				error(arg);
				break;
#if defined(__x86)
			case 'm':
				if (strcmp(arg, "-xmodel=kernel") == 0) {
					newae(ctx->i_ae, "-ffreestanding");
					newae(ctx->i_ae, "-mno-red-zone");
					model = "-mcmodel=kernel";
					nolibc = 1;
					break;
				}
				error(arg);
				break;
#endif	/* __x86 */
			case 'M':
				if (strcmp(arg, "-xM") == 0) {
					newae(ctx->i_ae, "-M");
					break;
				}
				if (strcmp(arg, "-xM1") == 0) {
					newae(ctx->i_ae, "-MM");
					break;
				}
				error(arg);
				break;
			case 'n':
				if (strcmp(arg, "-xnolib") == 0) {
					nolibc = 1;
					break;
				}
				error(arg);
				break;
			case 'p':
				if (strcmp(arg, "-xpg") == 0) {
					newae(ctx->i_ae, "-pg");
					break;
				}
				error(arg);
				break;
			case 's':
				if (strcmp(arg, "-xs") == 0)
					break;
				error(arg);
				break;
			case 'e':
			case 'h':
			case 'l':
			default:
				error(arg);
				break;
			}
			break;
		case 'Y':
			if (arglen == 1) {
				if ((arg = *++ctx->i_oldargv) == NULL ||
				    *arg == '\0')
					error("-Y");
				ctx->i_oldargc--;
				arglen = strlen(arg + 1);
			} else {
				arg += 2;
			}
			if (strncmp(arg, "l,", 2) == 0) {
				char *s = strdup(arg);
				s[0] = '-';
				s[1] = 'B';
				newae(ctx->i_ae, s);
				free(s);
				break;
			}
			if (strncmp(arg, "I,", 2) == 0) {
				char *s = strdup(arg);
				s[0] = '-';
				s[1] = 'I';
				newae(ctx->i_ae, "-nostdinc");
				newae(ctx->i_ae, s);
				free(s);
				break;
			}
			error(arg);
			break;
		default:
			error(arg);
			break;
		}
	}

	/*
	 * Make sure that we do not have any unintended interactions between
	 * the xarch options passed in and the version of the Studio compiler
	 * used.
	 */
	if ((mflag & (SS11|SS12)) == (SS11|SS12)) {
		(void) fprintf(stderr,
		    "Conflicting \"-xarch=\" flags (both Studio 11 and 12)\n");
		exit(2);
	}

	switch (mflag) {
	case 0:
		/* FALLTHROUGH */
	case M32:
#if defined(__sparc)
		/*
		 * Only -m32 is defined and so put in the missing xarch
		 * translation.
		 */
		newae(ctx->i_ae, "-mcpu=v8");
		newae(ctx->i_ae, "-mno-v8plus");
#endif
		break;
	case M64:
#if defined(__sparc)
		/*
		 * Only -m64 is defined and so put in the missing xarch
		 * translation.
		 */
		newae(ctx->i_ae, "-mcpu=v9");
#endif
		break;
	case SS12:
#if defined(__sparc)
		/* no -m32/-m64 flag used - this is an error for sparc builds */
		(void) fprintf(stderr, "No -m32/-m64 flag defined\n");
		exit(2);
#endif
		break;
	case SS11:
		/* FALLTHROUGH */
	case (SS11|M32):
	case (SS11|M64):
		break;
	case (SS12|M32):
#if defined(__sparc)
		/*
		 * Need to add in further 32 bit options because with SS12
		 * the xarch=sparcvis option can be applied to 32 or 64
		 * bit, and so the translatation table (xtbl) cannot handle
		 * that.
		 */
		newae(ctx->i_ae, "-mv8plus");
#endif
		break;
	case (SS12|M64):
		break;
	default:
		(void) fprintf(stderr,
		    "Incompatible -xarch= and/or -m32/-m64 options used.\n");
		exit(2);
	}

	if (model && !pic)
		newae(ctx->i_ae, model);
	if (!nolibc)
		newae(ctx->i_ae, "-lc");
}

static void
prepctx(cw_ictx_t *ctx)
{
	const char *dir = NULL, *cmd;
	char *program = NULL;
	size_t len;

	switch (CIDX(CC(ctx), ctx->i_flags)) {
		case CIDX(CW_C_GCC, 0):
			program = getenv("CW_GCC");
			dir = getenv("CW_GCC_DIR");
			break;
		case CIDX(CW_C_GCC, CW_F_CXX):
			program = getenv("CW_GPLUSPLUS");
			dir = getenv("CW_GPLUSPLUS_DIR");
			break;
	}

	if (program == NULL) {
		if (dir == NULL)
			dir = default_dir[CC(ctx)];
		cmd = cmds[CIDX(CC(ctx), ctx->i_flags)];
		len = strlen(dir) + strlen(cmd) + 2;
		if ((program = malloc(len)) == NULL)
			nomem();
		(void) snprintf(program, len, "%s/%s", dir, cmd);
	}

	newae(ctx->i_ae, program);

	if (ctx->i_flags & CW_F_PROG) {
		(void) printf("compiler: %s\n", program);
		(void) fflush(stdout);
	}

	if (!(ctx->i_flags & CW_F_XLATE))
		return;

	switch (CC(ctx)) {
	case CW_C_GCC:
		do_gcc(ctx);
		break;
	}
}

static int
invoke(cw_ictx_t *ctx)
{
	char **newargv;
	int ac;
	struct ae *a;

	if ((newargv = calloc(sizeof (*newargv), ctx->i_ae->ael_argc + 1)) ==
	    NULL)
		nomem();

	if (ctx->i_flags & CW_F_ECHO)
		(void) fprintf(stderr, "+ ");

	for (ac = 0, a = ctx->i_ae->ael_head; a; a = a->ae_next, ac++) {
		newargv[ac] = a->ae_arg;
		if (ctx->i_flags & CW_F_ECHO)
			(void) fprintf(stderr, "%s ", a->ae_arg);
		if (a == ctx->i_ae->ael_tail)
			break;
	}

	if (ctx->i_flags & CW_F_ECHO) {
		(void) fprintf(stderr, "\n");
		(void) fflush(stderr);
	}

	if (!(ctx->i_flags & CW_F_EXEC))
		return (0);

	(void) execv(newargv[0], newargv);
	cw_perror("couldn't run %s", newargv[0]);

	return (-1);
}

static int
reap(cw_ictx_t *ctx)
{
	int status, ret = 0;
	char buf[1024];
	struct stat s;

	/*
	 * Only wait for one specific child.
	 */
	if (ctx->i_pid <= 0)
		return (-1);

	do {
		if (waitpid(ctx->i_pid, &status, 0) < 0) {
			cw_perror("cannot reap child");
			return (-1);
		}
		if (status != 0) {
			if (WIFSIGNALED(status)) {
				ret = -WTERMSIG(status);
				break;
			} else if (WIFEXITED(status)) {
				ret = WEXITSTATUS(status);
				break;
			}
		}
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	if (stat(ctx->i_stderr, &s) < 0) {
		cw_perror("stat failed on child cleanup");
		return (-1);
	}
	if (s.st_size != 0) {
		FILE *f;

		if ((f = fopen(ctx->i_stderr, "r")) != NULL) {
			while (fgets(buf, sizeof (buf), f))
				(void) fprintf(stderr, "%s", buf);
			(void) fflush(stderr);
			(void) fclose(f);
		}
	}
	(void) unlink(ctx->i_stderr);
	free(ctx->i_stderr);

	/*
	 * cc returns an error code when given -V; we want that to succeed.
	 */
	if (ctx->i_flags & CW_F_PROG)
		return (0);

	return (ret);
}

static int
exec_ctx(cw_ictx_t *ctx, int block)
{
	char *file;

	/*
	 * To avoid offending cc's sensibilities, the name of its output
	 * file must end in '.o'.
	 */
	if ((file = tempnam(NULL, ".cw")) == NULL) {
		nomem();
		return (-1);
	}
	free(file);

	if ((ctx->i_stderr = tempnam(NULL, ".cw")) == NULL) {
		nomem();
		return (-1);
	}

	if ((ctx->i_pid = fork()) == 0) {
		int fd;

		(void) fclose(stderr);
		if ((fd = open(ctx->i_stderr, O_WRONLY | O_CREAT | O_EXCL,
		    0666)) < 0) {
			cw_perror("open failed for standard error");
			exit(1);
		}
		if (dup2(fd, 2) < 0) {
			cw_perror("dup2 failed for standard error");
			exit(1);
		}
		if (fd != 2)
			(void) close(fd);
		if (freopen("/dev/fd/2", "w", stderr) == NULL) {
			cw_perror("freopen failed for /dev/fd/2");
			exit(1);
		}
		prepctx(ctx);
		exit(invoke(ctx));
	}

	if (ctx->i_pid < 0) {
		cw_perror("fork failed");
		return (1);
	}

	if (block)
		return (reap(ctx));

	return (0);
}

int
main(int argc, char **argv)
{
	cw_ictx_t *ctx = newictx();
	cw_ictx_t *ctx_shadow = newictx();
	const char *dir;
	int do_reap = 0;
	int ret = 0;

	if ((progname = strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	if (ctx == NULL || ctx_shadow == NULL)
		nomem();

	ctx->i_flags = CW_F_ECHO|CW_F_XLATE;

	/*
	 * Figure out where to get our tools from.  This depends on
	 * the environment variables set at run time.
	 */
	if ((dir = getenv("GCC_ROOT")) != NULL) {
		(void) snprintf(default_dir[CW_C_GCC], MAXPATHLEN,
		    "%s/bin", dir);
	}

	if (getenv("CW_NO_EXEC") == NULL)
		ctx->i_flags |= CW_F_EXEC;

	/*
	 * The first argument must be one of "-_gcc", or "-_g++"
	 */
	if (argc == 1)
		usage();
	argc--;
	argv++;
	if (strcmp(argv[0], "-_gcc") == 0) {
		ctx->i_compiler = CW_C_GCC;
	} else if (strcmp(argv[0], "-_g++") == 0) {
		ctx->i_compiler = CW_C_GCC;
		ctx->i_flags |= CW_F_CXX;
	} else {
		/* assume "-_gcc" by default */
		argc++;
		argv--;
		ctx->i_compiler = CW_C_GCC;
	}

	ctx->i_oldargc = argc;
	ctx->i_oldargv = argv;

	ret |= exec_ctx(ctx, do_reap);

	if (!do_reap)
		ret |= reap(ctx);

	return (ret);
}
