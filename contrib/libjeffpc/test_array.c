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

#include <jeffpc/types.h>

#include "test.c"

struct foo {
	int a;
	char b;
};

union bar {
	int a;
	char b;
};

#define TEST_ONE(type, nelem)					\
	do {							\
		type test_array[nelem];				\
		size_t res = ARRAY_LEN(test_array);		\
								\
		fprintf(stderr, "   %-16s test_array[%lu]\t=> %zu...",\
			#type, nelem, res);			\
								\
		if (res != nelem)				\
			fail("expected %zu, got %zu", nelem,	\
			     res);				\
								\
		fprintf(stderr, "ok.\n");			\
	} while (0)

#define TEST_TYPES(nelem)					\
	do {							\
		TEST_ONE(int8_t, nelem);			\
		TEST_ONE(int16_t, nelem);			\
		TEST_ONE(int32_t, nelem);			\
		TEST_ONE(int64_t, nelem);			\
		TEST_ONE(uint8_t, nelem);			\
		TEST_ONE(uint16_t, nelem);			\
		TEST_ONE(uint32_t, nelem);			\
		TEST_ONE(uint64_t, nelem);			\
		TEST_ONE(char, nelem);				\
		TEST_ONE(signed char, nelem);			\
		TEST_ONE(unsigned char, nelem);			\
		TEST_ONE(short, nelem);				\
		TEST_ONE(signed short, nelem);			\
		TEST_ONE(unsigned short, nelem);			\
		TEST_ONE(int, nelem);				\
		TEST_ONE(signed int, nelem);			\
		TEST_ONE(unsigned int, nelem);			\
		TEST_ONE(long, nelem);				\
		TEST_ONE(signed long, nelem);			\
		TEST_ONE(unsigned long, nelem);			\
		TEST_ONE(void *, nelem);			\
		TEST_ONE(intptr_t, nelem);			\
		TEST_ONE(uintptr_t, nelem);			\
		TEST_ONE(ptrdiff_t, nelem);			\
		TEST_ONE(struct foo, nelem);			\
		TEST_ONE(struct foo *, nelem);			\
		TEST_ONE(union bar, nelem);			\
		TEST_ONE(union bar *, nelem);			\
	} while (0)

static void test_array_len(void)
{
	int i;

	fprintf(stderr, "testing ARRAY_LEN with various types & lengths\n");

	/*
	 * Note:
	 *
	 * We try to create arrays up to 2**24 (16777216) elements in size
	 * right on the stack.  This can make the process size huge in no
	 * time if we feed TEST_ONE a type that is large to begin with.
	 */
	for (i = 0; i <= 24; i++)
		TEST_TYPES(1UL << i);

	fprintf(stderr, "All ARRAY_LEN tests passed.\n");
}

void test(void)
{
	test_array_len();
}
