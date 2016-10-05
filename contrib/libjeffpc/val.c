/*
 * Copyright (c) 2014-2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
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

#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <umem.h>

#include <jeffpc/val.h>
#include <jeffpc/jeffpc.h>
#include <jeffpc/error.h>

static umem_cache_t *val_cache;

void init_val_subsys(void)
{
	val_cache = umem_cache_create("val-cache", sizeof(struct val),
				      0, NULL, NULL, NULL, NULL, NULL, 0);
	ASSERT(val_cache);
}

static void __val_init(struct val *val, enum val_type type)
{
	val->type = type;

	switch (type) {
		case VT_BOOL:
			val->b = false;
			break;
		case VT_INT:
			val->i = 0;
			break;
		case VT_STR:
		case VT_SYM:
			val->str = NULL;
			break;
		case VT_CONS:
			val->cons.head = NULL;
			val->cons.tail = NULL;
			break;
	}
}

static void __val_cleanup(struct val *val)
{
	switch (val->type) {
		case VT_INT:
		case VT_BOOL:
			break;
		case VT_STR:
		case VT_SYM:
			str_putref(val->str);
			break;
		case VT_CONS:
			val_putref(val->cons.head);
			val_putref(val->cons.tail);
			break;
	}
}

struct val *val_alloc(enum val_type type)
{
	struct val *val;

	val = umem_cache_alloc(val_cache, 0);
	if (!val)
		return val;

	refcnt_init(&val->refcnt, 1);

	__val_init(val, type);

	return val;
}

void val_free(struct val *val)
{
	ASSERT(val);
	ASSERT3U(refcnt_read(&val->refcnt), ==, 0);

	__val_cleanup(val);

	umem_cache_free(val_cache, val);
}

#define DEF_VAL_SET(fxn, vttype, valelem, ctype)		\
int val_set_##fxn(struct val *val, ctype v)			\
{								\
	__val_cleanup(val);					\
								\
	val->type = vttype;					\
	val->valelem = v;					\
								\
	return 0;						\
}

DEF_VAL_SET(int, VT_INT, i, uint64_t)
DEF_VAL_SET(str, VT_STR, str, struct str *)
DEF_VAL_SET(sym, VT_SYM, str, struct str *)
DEF_VAL_SET(bool, VT_BOOL, b, bool)

int val_set_cons(struct val *val, struct val *head, struct val *tail)
{
	__val_cleanup(val);

	val->type = VT_CONS;
	val->cons.head = head;
	val->cons.tail = tail;

	return 0;
}

void val_dump(struct val *val, int indent)
{
	if (!val)
		return;

	switch (val->type) {
		case VT_STR:
		case VT_SYM:
			fprintf(stderr, "%*s'%s'\n", indent, "", str_cstr(val->str));
			break;
		case VT_INT:
			fprintf(stderr, "%*s%"PRIu64"\n", indent, "", val->i);
			break;
		case VT_BOOL:
			fprintf(stderr, "%*s%s\n", indent, "",
				val->b ? "true" : "false");
			break;
		case VT_CONS:
			fprintf(stderr, "%*scons head:\n", indent, "");
			val_dump(val->cons.head, indent + 2);
			fprintf(stderr, "%*scons tail:\n", indent, "");
			val_dump(val->cons.tail, indent + 2);
			break;
		default:
			fprintf(stderr, "Unknown type %d\n", val->type);
			break;
	}
}
