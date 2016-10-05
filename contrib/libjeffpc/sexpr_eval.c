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

#include <jeffpc/sexpr.h>

struct builtin_fxn {
	const char *name;
	struct val *(*f)(struct val *args,
			 struct val *(*lookup)(struct str *name, void *private),
			 void *private);
};

#define __REDUCE(fname, alloc, t, valmember, ident, op)				\
static struct val *fname(struct val *args,					\
			 struct val *(*lookup)(struct str *, void *),		\
			 void *private)						\
{										\
	struct val *ret;							\
										\
	ret = alloc(ident);							\
										\
	while (args) {								\
		struct val *el = sexpr_car(val_getref(args));			\
		struct val *next = sexpr_cdr(args);				\
										\
		while (el && (el->type == VT_CONS || el->type == VT_SYM))	\
			el = sexpr_eval(el, lookup, private);			\
										\
		ASSERT(el);							\
		ASSERT3U(el->type, ==, t);					\
										\
		ret->valmember = ret->valmember op el->valmember;		\
										\
		val_putref(el);							\
		args = next;							\
	}									\
										\
	return ret;								\
}

#define BOOL_REDUCE(fname, ident, op) \
	__REDUCE(fname, VAL_ALLOC_BOOL, VT_BOOL, b, ident, op)

BOOL_REDUCE(fxn_or,  false, ||)
BOOL_REDUCE(fxn_and, true,  &&)

#define INT_REDUCE(fname, ident, op) \
	__REDUCE(fname, VAL_ALLOC_INT, VT_INT, i, ident, op)

INT_REDUCE(fxn_add,  0, +)
INT_REDUCE(fxn_mult, 1, *)

static struct val *fxn_quote(struct val *args,
			     struct val *(*lookup)(struct str *name, void *private),
			     void *private)
{
	return sexpr_car(args);
}

static struct val *fxn_equal(struct val *args,
			     struct val *(*lookup)(struct str *name, void *private),
			     void *private)
{
	struct val *a, *b;

	VERIFY3U(sexpr_length(val_getref(args)), == ,2);

	a = sexpr_eval(sexpr_nth(val_getref(args), 1), lookup, private);
	b = sexpr_eval(sexpr_nth(args, 2), lookup, private);

	return VAL_ALLOC_BOOL(sexpr_equal(a, b));
}

static struct builtin_fxn builtins[] = {
	{ "and",   fxn_and, },
	{ "or",    fxn_or, },
	{ "&&",    fxn_and, },
	{ "||",    fxn_or, },
	{ "+",     fxn_add, },
	{ "*",     fxn_mult, },
	{ "quote", fxn_quote, },
	{ "=",     fxn_equal, },
	{ "==",    fxn_equal, },
	{ NULL, },
};

static struct val *eval_cons(struct val *expr,
			     struct val *(*lookup)(struct str *, void *),
			     void *private)
{
	struct val *args;
	struct val *op;
	size_t i;

	op = sexpr_car(val_getref(expr));
	args = sexpr_cdr(val_getref(expr));

	ASSERT(op);
	ASSERT3U(op->type, ==, VT_SYM);

	for (i = 0; builtins[i].name; i++)
		if (!strcmp(builtins[i].name, str_cstr(op->str)))
			return builtins[i].f(args, lookup, private);

	panic("unknown builtin function '%s'", str_cstr(op->str));
}

struct val *sexpr_eval(struct val *expr,
		       struct val *(*lookup)(struct str *, void *),
		       void *private)
{
	if (!expr)
		return NULL;

	switch (expr->type) {
		case VT_INT:
		case VT_STR:
		case VT_BOOL:
			return expr;
		case VT_SYM: {
			struct str *name;

			if (!lookup)
				panic("VT_SYM requires non-NULL lookup "
				      "function passed to sexpr_eval");

			name = str_getref(expr->str);
			val_putref(expr);

			return sexpr_eval(lookup(name, private), lookup, private);
		}
		case VT_CONS:
			return eval_cons(expr, lookup, private);
	}

	panic("impossible!");
}
