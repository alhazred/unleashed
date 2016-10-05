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

#include <jeffpc/hexdump.h>
#include <jeffpc/padding.h>
#include <jeffpc/types.h>

#include "test.c"

struct run {
	const void *in;
	size_t inlen;
	const char *out[2];
};

#define	TEST_ENTRY(i, l, u)			\
	{					\
		.in = (i),			\
		.inlen = sizeof(i) - 1,		\
		.out[true]  = (u),		\
		.out[false] = (l),		\
	}

static const struct run runs[] = {
	TEST_ENTRY("ABC",
		   "414243",
		   "414243"),
	TEST_ENTRY("abcdefghijklmnopqrstuvwxyz",
		   "6162636465666768696a6b6c6d6e6f707172737475767778797a",
		   "6162636465666768696A6B6C6D6E6F707172737475767778797A"),
	TEST_ENTRY("\0\1\2\3\4\5\6\7\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
		   "000102030405060708090a0b0c0d0e0f",
		   "000102030405060708090A0B0C0D0E0F"),
};

/*
 * We allocate a temporary buffer large enough to hold the expected output,
 * but also large enough for two redzones each REDZONE_SIZE bytes in size.
 * One redzone is located before the location for the output and one after.
 * We clear the whole buffer by setting each byte to a constant REDZONE_VAL.
 */
#define REDZONE_SIZE	128
#define REDZONE_VAL	0xff

static void __test(void (*fxn)(char *, const void *, size_t, bool),
		   const char *testname, int iter, bool upper,
		   bool nullterm)
{
	const struct run *run = &runs[iter];
	const size_t nulltermlen = (nullterm ? 1 : 0);
	const size_t buflen = run->inlen * 2 + REDZONE_SIZE * 2 + nulltermlen;
	const char *expected = run->out[upper];
	const size_t expectedlen = 2 * run->inlen + nulltermlen;
	char msgpfx[24];
	char *buf;

	snprintf(msgpfx, sizeof(msgpfx), "%d/%s/%s", iter, testname,
		 upper ? "upper" : "lower");

	buf = alloca(buflen);

	fprintf(stderr, "%s: input length: %zu\n", msgpfx, run->inlen);
	fprintf(stderr, "%s: expected length: %zu\n", msgpfx, expectedlen);
	fprintf(stderr, "%s: expected output: %s\n", msgpfx, expected);

	if (strlen(expected) + nulltermlen != expectedlen)
		fail("length of expected string (%zu + %zu) != calculated "
		     "length (%zu)", strlen(expected), nulltermlen,
		     expectedlen);

	/* clear the output buffer and redzones */
	memset(buf, REDZONE_VAL, buflen);

	/* run the function */
	fxn(buf + REDZONE_SIZE, run->in, run->inlen, upper);

	/* verify the output buffer */
	if (memcmp(buf + REDZONE_SIZE, expected, expectedlen))
		fail("unexpected bytes present");

	/* verify that redzones are still intact */
	if (!check_padding(buf, REDZONE_VAL, REDZONE_SIZE))
		fail("leading redzone modification detected");
	if (!check_padding(buf + REDZONE_SIZE + expectedlen,
			   REDZONE_VAL, REDZONE_SIZE))
		fail("trailing redzone modification detected");

	fprintf(stderr, "%s: ok.\n", msgpfx);
}

void test(void)
{
	int i;

	for (i = 0; i < ARRAY_LEN(runs) ; i++) {
		__test(hexdump,  "hexdump",  i, true,  false);
		__test(hexdump,  "hexdump",  i, false, false);
		__test(hexdumpz, "hexdumpz", i, true,  true);
		__test(hexdumpz, "hexdumpz", i, false, true);
	}
}
