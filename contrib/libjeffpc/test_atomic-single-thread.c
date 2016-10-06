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

#include <jeffpc/atomic.h>
#include <jeffpc/int.h>

#include "test.c"

#define INITIAL		5ll

static void check(const char *type, const char *msg, uint64_t got, uint64_t exp)
{
	fprintf(stderr, "%s: checking: %s\n", type, msg);
	fprintf(stderr, "%s:    expected: %"PRIu64"\n", type, exp);
	fprintf(stderr, "%s:    got:      %"PRIu64"\n", type, got);

	if (exp != got)
		fail("%s mismatch!", type);
}

#define TEST(type, negone)						\
do {									\
	const char *t = #type;						\
	fprintf(stderr, "%s: testing...\n", t);				\
									\
	do {								\
		type v;							\
									\
		atomic_set(&v, INITIAL);				\
									\
		/* yes, we are reaching into the implementation here */	\
		check(t, "contents valid", v.v, INITIAL);		\
									\
		check(t, "read-after-init", atomic_read(&v), INITIAL);	\
									\
		check(t, "increment-return", atomic_inc(&v), INITIAL + 1);\
		check(t, "read-after-inc", atomic_read(&v), INITIAL + 1);\
		check(t, "add-return", atomic_add(&v, 10), INITIAL + 11);\
		check(t, "read-after-add", atomic_read(&v), INITIAL + 11);\
									\
		check(t, "decrement-return", atomic_dec(&v), INITIAL + 10);\
		check(t, "read-after-dec", atomic_read(&v), INITIAL + 10);\
		check(t, "sub-return", atomic_sub(&v, 10), INITIAL);	\
		check(t, "read-after-sub", atomic_read(&v), INITIAL);	\
									\
		check(t, "go-to-zero", atomic_sub(&v, INITIAL), 0);	\
		check(t, "go-negative", atomic_dec(&v), negone);	\
		check(t, "go-more-negative", atomic_sub(&v, 10),	\
		      negone - 10);					\
									\
		check(t, "go-positive", atomic_add(&v, 20), 9);		\
									\
		check(t, "cas-match", atomic_cas(&v, 9, 50), 9);	\
		check(t, "cas-mismatch", atomic_cas(&v, 50, 1), 50);	\
	} while (0);							\
									\
	fprintf(stderr, "%s: ok.\n", t);				\
} while (0)

void test(void)
{
	TEST(atomic_t, 4294967295);
	TEST(atomic64_t, 18446744073709551615ull);
}
