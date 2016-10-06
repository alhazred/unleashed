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
#include <jeffpc/sexpr.h>
#include <jeffpc/jeffpc.h>

#include "test.c"

static inline struct val *op0(char *op)
{
	struct val *arr[1];
	struct val *ret;

	arr[0] = VAL_ALLOC_SYM_CSTR(op);

	ret = sexpr_array_to_list(arr, ARRAY_LEN(arr));

	sexpr_dump_file(stderr, ret, false);

	return ret;
}

static inline struct val *op1(char *op, struct val *a)
{
	struct val *arr[2];
	struct val *ret;

	arr[0] = VAL_ALLOC_SYM_CSTR(op);
	arr[1] = a;

	ret = sexpr_array_to_list(arr, ARRAY_LEN(arr));

	sexpr_dump_file(stderr, ret, false);

	return ret;
}

static inline struct val *op2(char *op, struct val *a, struct val *b)
{
	struct val *arr[3];
	struct val *ret;

	arr[0] = VAL_ALLOC_SYM_CSTR(op);
	arr[1] = a;
	arr[2] = b;

	ret = sexpr_array_to_list(arr, ARRAY_LEN(arr));

	sexpr_dump_file(stderr, ret, false);

	return ret;
}

static inline struct val *op3(char *op, struct val *a, struct val *b, struct val *c)
{
	struct val *arr[4];
	struct val *ret;

	arr[0] = VAL_ALLOC_SYM_CSTR(op);
	arr[1] = a;
	arr[2] = b;
	arr[3] = c;

	ret = sexpr_array_to_list(arr, ARRAY_LEN(arr));

	sexpr_dump_file(stderr, ret, false);

	return ret;
}

static void test_int_op(char *opname, enum val_type type, uint64_t expected,
			struct val *expr)
{
	struct val *res;

	res = sexpr_eval(expr, NULL, NULL);

	fprintf(stderr, " -> ");
	sexpr_dump_file(stderr, res, false);
	fprintf(stderr, "\n");

	ASSERT3U(res->type, ==, type);
	switch (type) {
		case VT_BOOL:
			ASSERT3U(res->b, ==, expected);
			break;
		case VT_INT:
			ASSERT3U(res->i, ==, expected);
			break;
		default:
			fail("unhandled val type %u", type);
	}

	val_putref(res);
}

#define TEST_BOOL_OP0(n, e)			test_int_op((n), VT_BOOL, (e), op0(n))
#define TEST_BOOL_OP1(n, e, v1)			test_int_op((n), VT_BOOL, (e), op1((n), \
								   VAL_ALLOC_BOOL(v1)))
#define TEST_BOOL_OP2(n, e, v1, v2)		test_int_op((n), VT_BOOL, (e), op2((n), \
								   VAL_ALLOC_BOOL(v1), \
								   VAL_ALLOC_BOOL(v2)))
#define TEST_BOOL_OP3(n, e, v1, v2, v3)		test_int_op((n), VT_BOOL, (e), op3((n), \
								   VAL_ALLOC_BOOL(v1), \
								   VAL_ALLOC_BOOL(v2), \
								   VAL_ALLOC_BOOL(v3)))

#define TEST_INT_OP0(n, e)			test_int_op((n), VT_INT, (e), op0(n))
#define TEST_INT_OP1(n, e, v1)			test_int_op((n), VT_INT, (e), op1((n), \
								   VAL_ALLOC_INT(v1)))
#define TEST_INT_OP2(n, e, v1, v2)		test_int_op((n), VT_INT, (e), op2((n), \
								   VAL_ALLOC_INT(v1), \
								   VAL_ALLOC_INT(v2)))
#define TEST_INT_OP3(n, e, v1, v2, v3)		test_int_op((n), VT_INT, (e), op3((n), \
								   VAL_ALLOC_INT(v1), \
								   VAL_ALLOC_INT(v2), \
								   VAL_ALLOC_INT(v3)))

static void test_bools(char *or, char *and)
{
	int i, j, k;

	fprintf(stderr, "Testing booleans...\n");

	TEST_BOOL_OP0(or, false);
	TEST_BOOL_OP0(and, true);

	for (i = 0; i < 2; i++) {
		TEST_BOOL_OP1(or,  !!i, !!i);
		TEST_BOOL_OP1(and, !!i, !!i);
	}

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			TEST_BOOL_OP2(or,  (!!i) || (!!j), !!i, !!j);
			TEST_BOOL_OP2(and, (!!i) && (!!j), !!i, !!j);
		}
	}

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			for (k = 0; k < 2; k++) {
				TEST_BOOL_OP3(or,  (!!i) || (!!j) || (!!k),
					      !!i, !!j, !!k);
				TEST_BOOL_OP3(and, (!!i) && (!!j) && (!!k),
					      !!i, !!j, !!k);
			}
		}
	}
}

static void test_ints(void)
{
	int i, j, k;

	fprintf(stderr, "Testing integers...\n");

	TEST_INT_OP0("+", 0);
	TEST_INT_OP0("*", 1);

	for (i = 0; i < 3; i++) {
		TEST_INT_OP1("+", i, i);
		TEST_INT_OP1("*", i, i);
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			TEST_INT_OP2("+", i + j, i, j);
			TEST_INT_OP2("*", i * j, i, j);
		}
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			for (k = 0; k < 3; k++) {
				TEST_INT_OP3("+", i + j + k, i, j, k);
				TEST_INT_OP3("*", i * j * k, i, j, k);
			}
		}
	}
}

void test(void)
{
	jeffpc_init(NULL);

	test_bools("or", "and");
	test_bools("||", "&&");
	test_ints();
}
