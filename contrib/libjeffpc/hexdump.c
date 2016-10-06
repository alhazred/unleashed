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

#include <stdint.h>

#include <jeffpc/hexdump.h>

void hexdump(char *out, const void *in, size_t inlen, bool upper)
{
	static const char uppercase[16] = "0123456789ABCDEF";
	static const char lowercase[16] = "0123456789abcdef";
	const char *table = upper ? uppercase : lowercase;
	const uint8_t *tmp = in;
	size_t i;

	for (i = 0; i < inlen; i++) {
		out[i * 2]     = table[tmp[i] >> 4];
		out[i * 2 + 1] = table[tmp[i] & 0xf];
	}
}

void hexdumpz(char *out, const void *in, size_t inlen, bool upper)
{
	hexdump(out, in, inlen, upper);

	out[inlen * 2] = '\0';
}
