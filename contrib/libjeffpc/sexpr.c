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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jeffpc/str.h>
#include <jeffpc/sexpr.h>

#include "sexpr_impl.h"

static char *escape_str(const char *in)
{
	char *out, *tmp;
	const char *s;
	size_t outlen;

	outlen = strlen(in);

	for (s = in; *s; s++) {
		char c = *s;

		switch (c) {
			case '\n':
			case '\t':
			case '\r':
			case '\b':
			case '\f':
			case '"':
				/* "\n", "\t", ... */
				outlen++;
				break;
		}
	}

	out = malloc(outlen + 1);
	if (!out)
		return NULL;

	for (s = in, tmp = out; *s; s++, tmp++) {
		unsigned char c = *s;

		switch (c) {
			case '\n':
				strcpy(tmp, "\\n");
				tmp++;
				break;
			case '\t':
				strcpy(tmp, "\\t");
				tmp++;
				break;
			case '\r':
				strcpy(tmp, "\\r");
				tmp++;
				break;
			case '\b':
				strcpy(tmp, "\\b");
				tmp++;
				break;
			case '\f':
				strcpy(tmp, "\\f");
				tmp++;
				break;
			case '"':
				strcpy(tmp, "\\\"");
				tmp++;
				break;
			default:
				*tmp = c;
				break;
		}
	}

	*tmp = '\0';

	return out;
}

struct val *sexpr_parse(const char *str, size_t len)
{
	struct sexpr_parser_state x;

	x.input = str;
	x.len   = len;
	x.pos   = 0;

	sexpr_reader_lex_init(&x.scanner);
	sexpr_reader_set_extra(&x, x.scanner);

	ASSERT(sexpr_reader_parse(&x) == 0);

	sexpr_reader_lex_destroy(x.scanner);

	return x.output;
}

static struct str *dump_cons(struct val *lv, bool raw)
{
	static struct str dot = STR_STATIC_INITIALIZER(" . ");
	static struct str space = STR_STATIC_INITIALIZER(" ");
	struct val *head = lv->cons.head;
	struct val *tail = lv->cons.tail;

	if (raw)
		return str_cat(3, sexpr_dump(head, raw),
			       &dot,
			       sexpr_dump(tail, raw));
	else if (!head && !tail)
		return NULL;
	else if (head && !tail)
		return sexpr_dump(head, raw);
	else if (tail->type == VT_CONS)
		return str_cat(3, sexpr_dump(head, raw),
			       &space,
			       dump_cons(tail, raw));
	else
		return str_cat(3, sexpr_dump(head, raw),
			       &dot,
			       sexpr_dump(tail, raw));
}

struct str *sexpr_dump(struct val *lv, bool raw)
{
	static struct str dquote = STR_STATIC_INITIALIZER("\"");
	static struct str squote = STR_STATIC_INITIALIZER("'");
	static struct str poundt = STR_STATIC_INITIALIZER("#t");
	static struct str poundf = STR_STATIC_INITIALIZER("#f");
	static struct str oparen = STR_STATIC_INITIALIZER("(");
	static struct str cparen = STR_STATIC_INITIALIZER(")");
	static struct str empty = STR_STATIC_INITIALIZER("()");
	char *tmpstr;

	if (!lv)
		return &empty;

	switch (lv->type) {
		case VT_SYM:
			return str_getref(lv->str);
		case VT_STR:
			tmpstr = escape_str(str_cstr(lv->str));
			/* TODO: we leak tmpstr */

			return str_cat(3, &dquote, STR_DUP(tmpstr), &dquote);
		case VT_BOOL:
			return lv->b ? &poundt : &poundf;
		case VT_INT: {
			char tmp[32];

			snprintf(tmp, sizeof(tmp), "%"PRIu64, lv->i);

			return STR_DUP(tmp);
		}
		case VT_CONS: {
			struct val *head = lv->cons.head;
			struct val *tail = lv->cons.tail;

			/* handle quoting */
			if (!raw && head && (head->type == VT_SYM) &&
			    !strcmp(str_cstr(head->str), "quote")) {
				/* we're dealing with a (quote) */
				if (sexpr_is_null(tail))
					return str_cat(2, &squote, &empty);

				/* we're dealing with a (quote ...) */
				return str_cat(2, &squote, dump_cons(tail, raw));
			}

			/* nothing to quote */
			return str_cat(3, &oparen, dump_cons(lv, raw), &cparen);
		}
	}

	return NULL;
}

void sexpr_dump_file(FILE *out, struct val *lv, bool raw)
{
	struct str *tmp;

	tmp = sexpr_dump(lv, raw);

	fprintf(out, "%s", str_cstr(tmp));

	str_putref(tmp);
}

/*
 * Convert a C array of vals into a sexpr list.  E.g.,
 *
 *	vals = { A, B, C }, nvals = 3
 *
 * turns into:
 *
 *	'(A . (B . (C . ())))
 *
 * which is the same as:
 *
 *	'(A B C)
 */
struct val *sexpr_array_to_list(struct val **vals, int nvals)
{
	struct val *last = NULL;
	struct val *tmp;

	for (nvals--; nvals >= 0; nvals--, last = tmp)
		tmp = VAL_ALLOC_CONS(vals[nvals], last);

	return last;
}

/*
 * Convert a sexpr list into a C array of vals.  E.g.,
 *
 *     '(A B C)
 *
 * turns into:
 *
 *     array = { A, B, C }, nvals = 3
 *
 * We fill in the passed in array with at most alen elements.  The number of
 * filled in elements is returned to the caller.
 */
int sexpr_list_to_array(struct val *list, struct val **array, int alen)
{
	struct val *tmp;
	int nvals = 0;

	for (tmp = list; tmp && (alen > nvals); tmp = tmp->cons.tail, nvals++) {
		if (tmp->type != VT_CONS)
			goto err;

		array[nvals] = val_getref(tmp->cons.head);
	}

	if ((alen == nvals) && tmp)
		goto err;

	return nvals;

err:
	while (nvals)
		val_putref(array[--nvals]);

	return -1;
}

struct val *sexpr_car(struct val *lv)
{
	struct val *ret;

	if (!lv)
		return NULL;

	if (lv->type == VT_CONS)
		ret = val_getref(lv->cons.head);
	else
		ret = NULL;

	val_putref(lv);

	return ret;
}

struct val *sexpr_cdr(struct val *lv)
{
	struct val *ret;

	if (!lv)
		return NULL;

	if (lv->type == VT_CONS)
		ret = val_getref(lv->cons.tail);
	else
		ret =  NULL;

	val_putref(lv);

	return ret;
}

ssize_t sexpr_length(struct val *lv)
{
	ssize_t len;

	len = 0;

	while (!sexpr_is_null(lv)) {
		if (lv->type != VT_CONS) {
			/* not a list */
			val_putref(lv);
			return -1;
		}

		len++;
		lv = sexpr_cdr(lv);
	}

	val_putref(lv);

	return len;
}

struct val *sexpr_nth(struct val *lv, uint64_t n)
{
	while (n-- && lv) {
		struct val *tmp;

		if (lv->type == VT_CONS) {
			/*
			 * If this is not the one we want, follow the tail.
			 * Otherwise, grab the head.
			 */
			if (n)
				tmp = val_getref(lv->cons.tail);
			else
				tmp = val_getref(lv->cons.head);
		} else {
			tmp = NULL;
		}

		val_putref(lv);

		lv = tmp;
	}

	return lv;
}

/*
 * Given a list, lookup a certain name.
 *
 * The input list looks like:
 *   '((a . b) (c . d))
 * which really is:
 *   '((a . b) . ((c . d) . ()))
 *
 * So, to check it, we examite the car of the list, if that's not the right
 * key, we recurse on cdr of the list.
 */
struct val *sexpr_assoc(struct val *lv, const char *name)
{
	struct val *head;
	struct val *tail;

	/* empty list */
	if (!lv)
		return NULL;

	/* not a list */
	if (lv->type != VT_CONS)
		return NULL;

	head = lv->cons.head;
	tail = lv->cons.tail;

	/*
	 * check the head of current cons cell: '(head . tail)
	 *   (1) must be non-null
	 *   (2) must be a cons cell, i.e.,  head == '(a . b)
	 *   (3) (car head) must be a string or symbol
	 *   (4) (car head) must be equal to the value passed in
	 */
	if (head && (head->type == VT_CONS) &&
	    head->cons.head &&
	    ((head->cons.head->type == VT_STR) ||
	     (head->cons.head->type == VT_SYM)) &&
	    !strcmp(str_cstr(head->cons.head->str), name))
		return val_getref(head);

	return sexpr_assoc(tail, name);
}

bool sexpr_equal(struct val *lhs, struct val *rhs)
{
	bool ret;

	/* if they are the same object, they are equal - even if NULL */
	if (lhs == rhs) {
		ret = true;
		goto out;
	}

	/* if one is NULL, they are unequal */
	if (!lhs || !rhs) {
		/* ... unless we're comparing a NULL with a '() */
		if ((!lhs && ((rhs->type != VT_CONS) ||
			      rhs->cons.head ||
			      rhs->cons.tail)) ||
		    (!rhs && ((lhs->type != VT_CONS) ||
			      lhs->cons.head ||
			      lhs->cons.tail))) {
			ret = false;
			goto out;
		}

		ret = true;
		goto out;
	}

	/*
	 * At this point, we have two non-NULL values.
	 */

	/* different type -> unequal */
	if (lhs->type != rhs->type) {
		ret = false;
		goto out;
	}

	ret = true; /* pacify gcc */

	switch (lhs->type) {
		case VT_INT:
			ret = (lhs->i == rhs->i);
			break;
		case VT_STR:
		case VT_SYM:
			ret = str_cmp(lhs->str, rhs->str) == 0;
			break;
		case VT_BOOL:
			ret = (lhs->b == rhs->b);
			break;
		case VT_CONS:
			ret = sexpr_equal(val_getref(lhs->cons.head),
					  val_getref(rhs->cons.head)) &&
			      sexpr_equal(val_getref(lhs->cons.tail),
					  val_getref(rhs->cons.tail));
			break;
	}

out:
	val_putref(lhs);
	val_putref(rhs);
	return ret;
}

struct val *sexpr_alist_lookup_val(struct val *lv, const char *name)
{
	if (!lv || !name)
		return NULL;

	return sexpr_cdr(sexpr_assoc(lv, name));
}

struct str *sexpr_alist_lookup_str(struct val *lv, const char *name)
{
	struct str *ret;
	struct val *v;

	if (!lv || !name)
		return NULL;

	v = sexpr_cdr(sexpr_assoc(lv, name));
	if (!v || (v->type != VT_STR))
		ret = NULL;
	else
		ret = str_getref(v->str);

	val_putref(v);

	return ret;
}

uint64_t sexpr_alist_lookup_int(struct val *lv, const char *name, bool *found)
{
	struct val *v;
	uint64_t ret;
	bool ok;

	if (!lv || !name) {
		if (found)
			*found = false;
		return 0;
	}

	v = sexpr_cdr(sexpr_assoc(lv, name));
	ok = v && (v->type == VT_INT);

	if (!ok)
		ret = 0;
	else
		ret = v->i;

	val_putref(v);

	if (found)
		*found = ok;

	return ret;
}

struct val *sexpr_alist_lookup_list(struct val *lv, const char *name)
{
	struct val *ret;
	struct val *v;

	if (!lv || !name)
		return NULL;

	v = sexpr_cdr(sexpr_assoc(lv, name));
	if (!v || (v->type != VT_CONS))
		ret = NULL;
	else
		ret = val_getref(v);

	val_putref(v);

	return ret;
}
