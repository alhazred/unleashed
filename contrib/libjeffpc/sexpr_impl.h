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

#ifndef __SEXPR_IMPL_H
#define __SEXPR_IMPL_H

#include <jeffpc/sexpr.h>
#include <jeffpc/int.h>

static inline bool sexpr_is_null(struct val *v)
{
	return !v || (v->type == VT_CONS && !v->cons.head && !v->cons.tail);
}

struct sexpr_parser_state {
	void *scanner;
	const char *input;
	size_t len;
	size_t pos;
	struct val *output;
	int lineno;
};

extern int sexpr_reader_lex_destroy(void *yyscanner);
extern int sexpr_reader_parse(struct sexpr_parser_state *data);
extern void sexpr_reader_set_extra(void *user, void *yyscanner);
extern int sexpr_reader_lex_init(void **scanner);

#endif
