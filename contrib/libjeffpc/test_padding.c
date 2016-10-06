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

#include <jeffpc/padding.h>
#include <jeffpc/types.h>

#include "test.c"

struct run {
	const uint8_t *in;
	size_t inlen;
	uint8_t byte;
	bool success;
};

#define TEST_ENTRY(i, b, s)			\
	{					\
		.in = (i),			\
		.inlen = sizeof(i),		\
		.byte = (b),			\
		.success = (s),			\
	}

static const uint8_t a4[] = { 'A', 'A', 'A', 'A' };
static const uint8_t null4[] = { '\0', '\0', '\0', '\0' };
static const uint8_t mid5[] = { 'A', 'A', 'X', 'A', 'A' };
static const uint8_t end4[] = { 'A', 'A', 'A', 'X' };

static const struct run runs[] = {
	TEST_ENTRY(a4,		'A',  true),
	TEST_ENTRY(a4,		'X',  false),
	TEST_ENTRY(a4,		'a',  false),
	TEST_ENTRY(null4,	'\0', true),
	TEST_ENTRY(null4,	'X',  false),
	TEST_ENTRY(mid5,	'X',  false),
	TEST_ENTRY(mid5,	'A',  false),
	TEST_ENTRY(end4,	'A',  false),
	TEST_ENTRY(end4,	'X',  false),
};

static void __test(int idx, const uint8_t *in, size_t inlen, uint8_t byte,
		   bool success)
{
	fprintf(stderr, "%d: check_padding(%p, %#x, %zu)...", idx, in, byte,
		inlen);

	if (check_padding(in, byte, inlen) != success)
		fail("%s when it shouldn't have",
		     success ? "failed" : "success");

	fprintf(stderr, "%s...ok\n", success ? "true" : "false");
}

void test(void)
{
	int i;

	for (i = 0; i < ARRAY_LEN(runs) ; i++)
		__test(i, runs[i].in, runs[i].inlen, runs[i].byte,
		       runs[i].success);
}
