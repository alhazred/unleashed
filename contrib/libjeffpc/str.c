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

#include <stdlib.h>
#include <stdbool.h>
#include <umem.h>
#include <alloca.h>

#include <jeffpc/str.h>
#include <jeffpc/jeffpc.h>

static umem_cache_t *str_cache;

void init_str_subsys(void)
{
	str_cache = umem_cache_create("str-cache", sizeof(struct str),
				      0, NULL, NULL, NULL, NULL, NULL, 0);
	ASSERT(str_cache);
}

static struct str *__alloc(char *s, bool copy)
{
	struct str *str;

	str = umem_cache_alloc(str_cache, 0);
	if (!str)
		return NULL;

	refcnt_init(&str->refcnt, 1);
	str->flags = 0;

	if (copy) {
		strcpy(str->inline_str, s);
		str->str = str->inline_str;
	} else {
		str->str = s;
	}

	return str;
}

struct str *str_dup(const char *s)
{
	if (!s)
		return __alloc("", true);

	if (strlen(s) <= STR_INLINE_LEN)
		return __alloc((char *) s, true);

	return str_alloc(strdup(s));
}

struct str *str_alloc(char *s)
{
	return __alloc(s, false);
}

size_t str_len(const struct str *s)
{
	return strlen(s->str);
}

int str_cmp(const struct str *a, const struct str *b)
{
	return strcmp(a->str, b->str);
}

struct str *str_cat(int n, ...)
{
	size_t totallen;
	struct str *ret;
	char *buf, *out;
	size_t *len;
	va_list ap;
	int i;

	if (!n)
		return NULL;

	if (n == 1) {
		va_start(ap, n);
		ret = va_arg(ap, struct str *);
		va_end(ap);
		return ret;
	}

	totallen = 0;
	len = alloca(sizeof(size_t) * n);

	va_start(ap, n);
	for (i = 0; i < n; i++) {
		struct str *str = va_arg(ap, struct str *);

		if (!str)
			continue;

		len[i] = strlen(str->str);

		totallen += len[i];
	}
	va_end(ap);

	buf = malloc(totallen + 1);
	ASSERT(buf);

	out = buf;

	va_start(ap, n);
	for (i = 0; i < n; i++) {
		struct str *str = va_arg(ap, struct str *);

		if (!str)
			continue;

		strcpy(out, str->str);

		out += len[i];

		str_putref(str);
	}
	va_end(ap);

	ret = str_alloc(buf);
	ASSERT(ret);

	return ret;
}

void str_free(struct str *str)
{
	if (!str)
		return;

	ASSERT3U(refcnt_read(&str->refcnt), ==, 0);

	if (str->str != str->inline_str)
		free(str->str);
	umem_cache_free(str_cache, str);
}

/*
 * We are not using REFCNT_FXNS() here because we want to support statically
 * defined strings.
 */
struct str *str_getref(struct str *x)
{
	if (!x)
		return NULL;

	if (!(x->flags & STR_FLAG_STATIC)) {
		ASSERT3U(refcnt_read(&x->refcnt), >=, 1);

		__refcnt_inc(&x->refcnt);
	}

	return x;
}

void str_putref(struct str *x)
{
	if (!x)
		return;

	if (x->flags & STR_FLAG_STATIC)
		return;

	ASSERT3S(refcnt_read(&x->refcnt), >=, 1);

	if (!__refcnt_dec(&x->refcnt))
		str_free(x);
}
