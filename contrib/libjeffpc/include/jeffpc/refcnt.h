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

#ifndef __JEFFPC_REFCNT_H
#define __JEFFPC_REFCNT_H

#include <jeffpc/atomic.h>
#include <jeffpc/error.h>

typedef struct {
	atomic_t count;
} refcnt_t;

static inline void refcnt_init(refcnt_t *x, uint32_t v)
{
	atomic_set(&x->count, v);
}

static inline uint32_t refcnt_read(refcnt_t *x)
{
	return atomic_read(&x->count);
}

/* INTERNAL FUNCTION - DO NOT USE DIRECTLY */
static inline void __refcnt_inc(refcnt_t *x)
{
	atomic_inc(&x->count);
}

/* INTERNAL FUNCTION - DO NOT USE DIRECTLY */
static inline uint32_t __refcnt_dec(refcnt_t *x)
{
	return atomic_dec(&x->count);
}

#define REFCNT_PROTOTYPES(type, name)					\
extern type *name##_getref(type *);					\
extern void name##_putref(type *);

#define __REFCNT_FXNS(vol, type, name, member, freefxn)			\
vol type *name##_getref(type *x)					\
{									\
	if (!x)								\
		return NULL;						\
									\
	ASSERT3U(refcnt_read(&x->member), >=, 1);			\
									\
	__refcnt_inc(&x->member);					\
									\
	return x;							\
}									\
vol void name##_putref(type *x)						\
{									\
	if (!x)								\
		return;							\
									\
	ASSERT3S(refcnt_read(&x->member), >=, 1);			\
									\
	if (!__refcnt_dec(&x->member))					\
		freefxn(x);						\
}

#define REFCNT_FXNS(type, name, member, freefxn)			\
	__REFCNT_FXNS(, type, name, member, freefxn)

#define REFCNT_INLINE_FXNS(type, name, member, freefxn)			\
	__REFCNT_FXNS(static inline, type, name, member, freefxn)

#endif
