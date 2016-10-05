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

#include <jeffpc/int.h>
#include <jeffpc/types.h>

#include "test.c"

struct run {
	uint64_t in;
	uint8_t out8;
	uint16_t out16;
	uint32_t out32;
	uint64_t out64;
};

static const struct run runs[] = {
	{
		.in	= 0,
		.out8	= 0,
		.out16	= 0,
		.out32	= 0,
		.out64	= 0,
	},
	{
		.in	= 0xffffffffffffffff,
		.out8	= 0xff,
		.out16	= 0xffff,
		.out32	= 0xffffffff,
		.out64	= 0xffffffffffffffff,
	},
	{
		.in	= 0x123456789abcdef0,
		.out8	= 0xf0,
		.out16	= 0xf0de,
		.out32	= 0xf0debc9a,
		.out64	= 0xf0debc9a78563412,
	},
	{
		.in	= 0x8080808080808080,
		.out8	= 0x80,
		.out16	= 0x8080,
		.out32	= 0x80808080,
		.out64	= 0x8080808080808080,
	},
};

#define CHECK(iter, size, fmt, in, exp)					\
	do {								\
		uint##size##_t got = bswap_##size(in);			\
									\
		fprintf(stderr, "%d: expected: %#"fmt"\n", iter, exp);	\
		fprintf(stderr, "%d: got:      %#"fmt"\n", iter, got);	\
									\
		if (got != exp)						\
			fail("mismatch!");				\
	} while (0)

static void __test(int iter, const struct run *run)
{
	fprintf(stderr, "%d: input:    %#"PRIx64"\n", iter, run->in);
	CHECK(iter, 8, PRIx8, run->in & 0xff, run->out8);
	CHECK(iter, 16, PRIx16, run->in & 0xffff, run->out16);
	CHECK(iter, 32, PRIx32, run->in & 0xffffffff, run->out32);
	CHECK(iter, 64, PRIx64, run->in, run->out64);
	fprintf(stderr, "%d: ok.\n", iter);
}

void test(void)
{
	int i;

	for (i = 0; i < ARRAY_LEN(runs) ; i++)
		__test(i, &runs[i]);
}
