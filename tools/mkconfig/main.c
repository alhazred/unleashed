/*
 * Copyright (c) 2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stddef.h>
#include <libgen.h>

#include <jeffpc/jeffpc.h>
#include <jeffpc/error.h>
#include <jeffpc/io.h>
#include <jeffpc/sexpr.h>

#include "main.h"

/*
 * Config
 * ======
 *
 * Each config item is represented by a list:
 *
 * (config
 *   NAME
 *   default-value)
 *
 * For example:
 *
 * (config
 *   LINUX_CORE_SUPPORT
 *   #t)
 *
 * This generates the following line when -H is specified:
 *
 * #define CONFIG_LINUX_CORE_SUPPORT
 *
 * and the following line when -M is specified:
 *
 * CONFIG_LINUX_CORE_SUPPORT=y
 *
 * Some configuration item do not should never be changed, however it is
 * convenient to keep track of them using config files.  To make it
 * paintfully obvious that these should not be changed (and later on
 * possibly give hints to an interactive configuration tool), one can use
 * const statements instead.  They have the same syntax as config
 * statements:
 *
 * (const
 *   NAME
 *   value)
 *
 * For example:
 *
 * (const
 *   MACH
 *   'amd64)
 *
 * The generated makefile and header lines are indistinguishable from config
 * statement lines.
 *
 * Mapping of Default Values
 * -------------------------
 *
 * bool true (#t):
 *
 *	#define CONFIG_FOO
 *	CONFIG_FOO=y
 *
 * bool false (#f):
 *
 *	#undef CONFIG_FOO
 *	CONFIG_FOO=n
 *
 * int (e.g., 5):
 *
 *	#define CONFIG_FOO 5
 *	CONFIG_FOO=5
 *
 * string (e.g., "abc")
 *
 *	#define CONFIG_FOO "abc"
 *	CONFIG_FOO="abc"
 *
 * quoted symbol (e.g., 'abc)
 *
 *	#define CONFIG_FOO abc
 *	CONFIG_FOO=abc
 *
 * Unquoted symbols are subject to evaluation.  For example, given the
 * this config:
 *
 *	((config
 *	   FOO
 *	   5)
 *
 *	 (config
 *	   BAR
 *	   FOO))
 *
 * We would end up with:
 *
 *	#define CONFIG_FOO 5
 *	#define CONFIG_BAR 5
 *
 * and
 *
 *	CONFIG_FOO=5
 *	CONFIG_BAR=5
 *
 * since when generating the CONFIG_BAR line, we evaluate the expression (FOO)
 * and obtain 5.
 *
 * Evaluation
 * ----------
 *
 * The values in config and const statements are subject to evaluation.  As
 * stated above, strings, integers, booleans and quoted symbols evaluate to
 * themselves.
 *
 * Unquoted symbols and lists are evaluated using a handful of simple built-in
 * functions.  The resulting value is used in the generated output file. This
 * allows for complex expressions without having to worry if every part of the
 * build system supports them (e.g., bitshifts).
 *
 * The supported functions include:
 *
 *	(or ...)
 *	(and ...)
 *	(+ ...)
 *	(* ...)
 *
 * TODO: expand
 *
 * Include
 * =======
 *
 * It is possible to include files by using the include statement:
 *
 * (include "filename")
 *
 * This includes the contents of the specified file as if they were included
 * in place of the include statement.
 */

#define CONFIG_STMT	"config"
#define CONST_STMT	"const"
#define INCLUDE_STMT	"include"

enum gen_what {
	GEN_HEADER,
	GEN_MAKEFILE,
	GEN_MAKEFILE_DMAKE,
};
#define NGEN (GEN_MAKEFILE_DMAKE + 1)

static avl_tree_t mapping_tree;
static list_t mapping_list;
static const char *include_guard_name = "__CONFIG_H";

static int cmp(const void *va, const void *vb)
{
	const struct config_item *a = va;
	const struct config_item *b = vb;
	int ret;

	ret = strcmp(str_cstr(a->name), str_cstr(b->name));
	if (ret < 0)
		return -1;
	else if (ret > 0)
		return 1;
	return 0;
}

static struct val *config_lookup(struct str *name, void *private)
{
	struct config_item key = {
		.name = name,
	};
	struct config_item *ci;

	ci = avl_find(&mapping_tree, &key, NULL);

	str_putref(name);

	return ci ? val_getref(ci->value) : NULL;
}

struct mapping {
	void (*file_start)(FILE *);
	void (*file_end)(FILE *);
	void (*entry[VT_CONS + 1])(FILE *, const struct str *, struct val *);
};

static struct mapping mapping[NGEN]; /* forward decl */

static void __map_int(FILE *f, const char *pfx, const char *name,
		      const char *sep, const uint64_t val)
{
	fprintf(f, "%sCONFIG_%s%s%"PRIu64"\n", pfx, name, sep, val);
}

static void map_int_header(FILE *f, const struct str *name, struct val *val)
{
	__map_int(f, "#define\t", str_cstr(name), " ", val->i);
}

static void map_int_makefile(FILE *f, const struct str *name, struct val *val)
{
	__map_int(f, "", str_cstr(name), "=", val->i);
}

static void __map_str(FILE *f, const char *pfx, const char *name,
		      const char *sep, const char *val, bool quoted)
{
	const char *q = quoted ? "\"" : "";

	fprintf(f, "%sCONFIG_%s%s%s%s%s\n", pfx, name, sep, q, val, q);
}

static void map_str_header(FILE *f, const struct str *name, struct val *val)
{
	// XXX: return val with quotes escaped
	__map_str(f, "#define\t", str_cstr(name), " ", str_cstr(val->str), true);
}

static void map_str_makefile(FILE *f, const struct str *name, struct val *val)
{
	// XXX: return val with quotes escaped
	__map_str(f, "", str_cstr(name), "=", str_cstr(val->str), true);
}

static void map_sym_header(FILE *f, const struct str *name, struct val *val)
{
	__map_str(f, "#define\t", str_cstr(name), " ", str_cstr(val->str), false);
}

static void map_sym_makefile(FILE *f, const struct str *name, struct val *val)
{
	__map_str(f, "", str_cstr(name), "=", str_cstr(val->str), false);
}

static void map_bool_header(FILE *f, const struct str *name,
			    struct val *val)
{
	if (val->b)
		fprintf(f, "#define\tCONFIG_%s\n", str_cstr(name));
	else
		fprintf(f, "#undef\tCONFIG_%s\n", str_cstr(name));
}

static void map_bool_makefile(FILE *f, const struct str *name,
			      struct val *val)
{
	fprintf(f, "CONFIG_%s=%s\n", str_cstr(name), val->b ? "y" : "n");
}

static void map_bool_makefile_dmake(FILE *f, const struct str *name,
				    struct val *val)
{
	fprintf(f, "CONFIG_%s=%s\n", str_cstr(name), val->b ? "" : "$(POUND_SIGN)");
}

static void __map_cons(FILE *f, const struct str *name,
		       struct val *val, enum gen_what what)
{
	struct val *ret;

	ret = sexpr_eval(val, config_lookup, NULL);

	mapping[what].entry[ret->type](f, name, ret);
}

static void map_cons_header(FILE *f, const struct str *name,
			    struct val *val)
{
	__map_cons(f, name, val, GEN_HEADER);
}

static void map_cons_makefile(FILE *f, const struct str *name,
			      struct val *val)
{
	__map_cons(f, name, val, GEN_MAKEFILE);
}

static void map_cons_makefile_dmake(FILE *f, const struct str *name,
				    struct val *val)
{
	__map_cons(f, name, val, GEN_MAKEFILE_DMAKE);
}

static void map_file_start_header(FILE *f)
{
	fprintf(f, "/* GENERATED FILE - DO NOT EDIT */\n");
	fprintf(f, "\n");
	fprintf(f, "#ifndef\t%s\n", include_guard_name);
	fprintf(f, "#define\t%s\n", include_guard_name);
	fprintf(f, "\n");
}

static void map_file_end_header(FILE *f)
{
	fprintf(f, "\n");
	fprintf(f, "#endif\n");
}

static void map_file_start_makefile(FILE *f)
{
	fprintf(f, "# GENERATED FILE - DO NOT EDIT\n");
	fprintf(f, "\n");
}

static void map_file_start_makefile_dmake(FILE *f)
{
	fprintf(f, "# GENERATED FILE - DO NOT EDIT\n");
	fprintf(f, "\n");
	fprintf(f, "PRE_POUND=pre\\#\n");
	fprintf(f, "POUND_SIGN=$(PRE_POUND:pre\\%%=%%)\n");
	fprintf(f, "\n");
}

static struct mapping mapping[NGEN] = {
	[GEN_HEADER] = {
		.file_start = map_file_start_header,
		.file_end = map_file_end_header,
		.entry = {
			[VT_INT] = map_int_header,
			[VT_STR] = map_str_header,
			[VT_SYM] = map_sym_header,
			[VT_BOOL] = map_bool_header,
			[VT_CONS] = map_cons_header,
		},
	},
	[GEN_MAKEFILE] = {
		.file_start = map_file_start_makefile,
		.entry = {
			[VT_INT] = map_int_makefile,
			[VT_STR] = map_str_makefile,
			[VT_SYM] = map_sym_makefile,
			[VT_BOOL] = map_bool_makefile,
			[VT_CONS] = map_cons_makefile,
		},
	},
	[GEN_MAKEFILE_DMAKE] = {
		.file_start = map_file_start_makefile_dmake,
		.entry = {
			[VT_INT] = map_int_makefile,
			[VT_STR] = map_str_makefile,
			[VT_SYM] = map_sym_makefile,
			[VT_BOOL] = map_bool_makefile_dmake,
			[VT_CONS] = map_cons_makefile_dmake,
		},
	},
};

static char *prog;

static void __config(struct val *args)
{
	struct val *name = sexpr_nth(val_getref(args), 1);
	struct val *value = sexpr_nth(val_getref(args), 2);
	struct config_item *item;

	if (name->type != VT_SYM)
		die("name not a VT_SYM");

	item = malloc(sizeof(struct config_item));
	if (!item)
		die("failed to allocate config item");

	item->name = str_getref(name->str);
	item->value = val_getref(value);

	avl_add(&mapping_tree, item);
	list_insert_tail(&mapping_list, item);

	val_putref(name);
	val_putref(value);
}

static int process(int dirfd, const char *infile);

static void __include(int dirfd, struct val *args)
{
	struct val *fname = sexpr_nth(val_getref(args), 1);
	char *dname;
	char *bname;
	int dir;

	if (fname->type != VT_STR)
		die("fname not a VT_STR");

	dname = strdup(str_cstr(fname->str));
	bname = strdup(str_cstr(fname->str));

	dir = xopenat(dirfd, dirname(dname), O_RDONLY, 0);

	ASSERT0(process(dir, basename(bname)));

	xclose(dir);

	free(bname);
	free(dname);

	val_putref(fname);
}

static int process(int dirfd, const char *infile)
{
	struct val *cur;
	char *in;

	in = read_file_at(dirfd, infile);
	if (IS_ERR(in)) {
		fprintf(stderr, "%s: cannot read: %s\n",
			infile, xstrerror(PTR_ERR(in)));
		die("failed");
	}

	cur = sexpr_parse(in, strlen(in));

	free(in);

	while (cur != NULL) {
		struct val *item = sexpr_car(val_getref(cur));
		struct val *stmt = sexpr_car(val_getref(item));
		struct val *args = sexpr_cdr(val_getref(item));
		struct val *next = sexpr_cdr(cur);

		if (item->type != VT_CONS)
			die("item not a VT_CONS");
		if (stmt->type != VT_SYM)
			die("statement not a VT_SYM");

		if (strcmp(str_cstr(stmt->str), CONFIG_STMT) == 0 ||
		    strcmp(str_cstr(stmt->str), CONST_STMT) == 0) {
			__config(args);
		} else if (strcmp(str_cstr(stmt->str), INCLUDE_STMT) == 0) {
			__include(dirfd, args);
		} else {
			die("unknown statement");
		}

		val_putref(item);
		val_putref(stmt);
		val_putref(args);
		cur = next;
	}

	VERIFY3P(cur, ==, NULL);

	return 0;
}

static void map(const char *outfile, enum gen_what what)
{
	struct mapping *m = &mapping[what];
	struct config_item *cur;
	FILE *f;

	if (!outfile)
		f = stdout;
	else
		f = fopen(outfile, "w");

	if (m->file_start)
		m->file_start(f);

	for (cur = list_head(&mapping_list);
	     cur != NULL;
	     cur = list_next(&mapping_list, cur))
		m->entry[cur->value->type](f, cur->name, cur->value);

	if (m->file_end)
		m->file_end(f);

	if (outfile)
		fclose(f);
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s {-H | -M | -m} [-I <guard name>] "
		"[-o <outfile>] <infile>\n", prog);
	exit(2);
}

int main(int argc, char **argv)
{
	enum gen_what what = GEN_HEADER;
	const char *outfile = NULL;
	char opt;

	prog = argv[0];

	ASSERT0(putenv("UMEM_DEBUG=default,verbose"));
	ASSERT0(putenv("BLAHG_DISABLE_SYSLOG=1"));

	jeffpc_init(NULL);

	avl_create(&mapping_tree, cmp, sizeof(struct config_item),
		   offsetof(struct config_item, tree));
	list_create(&mapping_list, sizeof(struct config_item),
		    offsetof(struct config_item, list));

	while ((opt = getopt(argc, argv, "HI:Mmo:")) != -1) {
		switch (opt) {
			case 'H':
				/* header */
				what = GEN_HEADER;
				break;
			case 'I':
				include_guard_name = optarg;
				break;
			case 'M':
				/* makefile (bmake) */
				what = GEN_MAKEFILE;
				break;
			case 'm':
				/* makefile (dmake) */
				what = GEN_MAKEFILE_DMAKE;
				break;
			case 'o':
				/* output file */
				outfile = optarg;
				break;
			default:
				usage();
				break;
		}
	}

	if (optind == argc)
		usage();

	/*
	 * Start processing by faking a config which looks like:
	 *
	 *	((include "<the path user specified>"))
	 *
	 * This allows make relative path includes work properly.  We could
	 * alternatively do the same path munging that __include() does
	 * here, but that would duplicate some ugly code.
	 */
	__include(AT_FDCWD, VAL_ALLOC_CONS(VAL_ALLOC_CSTR(argv[argc - 1]), NULL));

	map(outfile, what);

	return 0;
}
