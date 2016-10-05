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

#ifndef __JEFFPC_VAL_H
#define __JEFFPC_VAL_H

#include <stdint.h>
#include <stdbool.h>

#include <jeffpc/str.h>
#include <jeffpc/refcnt.h>
#include <jeffpc/error.h>

enum val_type {
	VT_INT = 0,	/* 64-bit uint */
	VT_STR,		/* a struct str string */
	VT_SYM,		/* symbol */
	VT_BOOL,	/* boolean */
	VT_CONS,	/* cons cell */
};

struct val {
	enum val_type type;
	refcnt_t refcnt;
	union {
		uint64_t i;
		bool b;
		struct str *str;
		struct {
			struct val *head;
			struct val *tail;
		} cons;
	};
};

extern struct val *val_alloc(enum val_type type);
extern void val_free(struct val *v);
extern int val_set_bool(struct val *val, bool v);
extern int val_set_int(struct val *val, uint64_t v);
extern int val_set_str(struct val *val, struct str *v);
extern int val_set_sym(struct val *val, struct str *v);
extern int val_set_cons(struct val *val, struct val *head, struct val *tail);
extern void val_dump(struct val *v, int indent);

REFCNT_INLINE_FXNS(struct val, val, refcnt, val_free)

#define VAL_ALLOC(t)				\
	({					\
		struct val *_x;			\
		_x = val_alloc(t);		\
		ASSERT(_x);			\
		_x;				\
	})

#define VAL_ALLOC_SYM(v)			\
	({					\
		struct val *_x;			\
		_x = VAL_ALLOC(VT_SYM);		\
		VAL_SET_SYM(_x, (v));		\
		_x;				\
	})

#define VAL_ALLOC_SYM_CSTR(v)			\
	VAL_ALLOC_SYM(STR_DUP(v))

#define VAL_ALLOC_STR(v)			\
	({					\
		struct val *_x;			\
		_x = VAL_ALLOC(VT_STR);		\
		VAL_SET_STR(_x, (v));		\
		_x;				\
	})

#define VAL_ALLOC_CSTR(v)			\
	VAL_ALLOC_STR(STR_ALLOC(v))

#define VAL_ALLOC_INT(v)			\
	({					\
		struct val *_x;			\
		_x = VAL_ALLOC(VT_INT);		\
		VAL_SET_INT(_x, (v));		\
		_x;				\
	})

#define VAL_ALLOC_BOOL(v)			\
	({					\
		struct val *_x;			\
		_x = VAL_ALLOC(VT_BOOL);	\
		VAL_SET_BOOL(_x, (v));		\
		_x;				\
	})

#define VAL_ALLOC_CONS(head, tail)		\
	({					\
		struct val *_x;			\
		_x = VAL_ALLOC(VT_CONS);	\
		VAL_SET_CONS(_x, (head), (tail));\
		_x;				\
	})

#define VAL_SET_INT(val, v)		ASSERT0(val_set_int((val), (v)))
#define VAL_SET_BOOL(val, v)		ASSERT0(val_set_bool((val), (v)))
#define VAL_SET_STR(val, v)		ASSERT0(val_set_str((val), (v)))
#define VAL_SET_SYM(val, v)		ASSERT0(val_set_sym((val), (v)))
#define VAL_SET_CONS(val, head, tail)	ASSERT0(val_set_cons((val), (head), (tail)))

#define VAL_DUP_CSTR(v)			VAL_ALLOC_STR(STR_DUP(v))

#endif
