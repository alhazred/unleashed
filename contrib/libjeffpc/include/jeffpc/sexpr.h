/*
 * Copyright (c) 2015-2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
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

#ifndef __JEFFPC_SEXPR_H
#define __JEFFPC_SEXPR_H

#include <stdbool.h>

#include <jeffpc/val.h>
#include <jeffpc/str.h>

extern struct val *sexpr_parse(const char *str, size_t len);
extern struct str *sexpr_dump(struct val *lv, bool raw);
extern void sexpr_dump_file(FILE *out, struct val *lv, bool raw);
extern struct val *sexpr_array_to_list(struct val **vals, int nvals);
extern int sexpr_list_to_array(struct val *list, struct val **array, int alen);

static inline struct val *sexpr_parse_str(struct str *str)
{
	if (!str)
		return NULL;

	return sexpr_parse(str_cstr(str), str_len(str));
}

static inline struct val *sexpr_parse_cstr(const char *str)
{
	if (!str)
		return NULL;

	return sexpr_parse(str, strlen(str));
}

extern struct val *sexpr_car(struct val *val);
extern struct val *sexpr_cdr(struct val *val);
/* get nth entry; 0 is the same val, 1 is the same a (car val) */
extern ssize_t sexpr_length(struct val *lv);
extern struct val *sexpr_nth(struct val *val, uint64_t n);
extern struct val *sexpr_assoc(struct val *val, const char *name);
extern bool sexpr_equal(struct val *lhs, struct val *rhs);

extern struct val *sexpr_eval(struct val *val,
			      struct val *(*lookup)(struct str *, void *),
			      void *private);

/*
 * Assorted helpers to make alists more pleasant to use.
 */
extern struct val *sexpr_alist_lookup_val(struct val *lv, const char *name);
extern struct str *sexpr_alist_lookup_str(struct val *lv, const char *name);
extern uint64_t sexpr_alist_lookup_int(struct val *lv, const char *name,
				       bool *found);
extern struct val *sexpr_alist_lookup_list(struct val *lv, const char *name);

#endif
