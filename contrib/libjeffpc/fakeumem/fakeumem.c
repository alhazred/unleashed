/*
 * Copyright (c) 2015 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
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

#include <errno.h>
#include <string.h>

#include <umem.h>

umem_cache_t *umem_cache_create(char *name, size_t size, size_t align,
				umem_constructor_t *constructor,
				umem_destructor_t *destructor,
				umem_reclaim_t *reclaim,
				void *cbdata, vmem_t *source, int cflags)
{
	if (!size || align || constructor || destructor || reclaim || source ||
	    cflags) {
		errno = EINVAL;
		return NULL;
	}

	return (void *)(uintptr_t) size;
}

void umem_cache_destroy(umem_cache_t *cache)
{
	/* nothing to do */
}

void *umem_cache_alloc(umem_cache_t *cache, int flags)
{
	size_t size = (uintptr_t)cache;

	if (flags != UMEM_DEFAULT) {
		errno = EINVAL;
		return NULL;
	}

	return malloc(size);
}

void umem_cache_free(umem_cache_t *cache, void *buf)
{
	free(buf);
}

void *umem_alloc(size_t size, int flags)
{
	void *tmp;

	/*
	 * Yes, this is terrible busy wait loop if we're short on memory.
	 * Alas, it should work well enough for now.
	 */
	do {
		tmp = malloc(size);
		if (tmp)
			return tmp;
	} while (flags & UMEM_NOFAIL);

	return NULL;
}

void *umem_zalloc(size_t size, int flags)
{
	void *tmp;

	tmp = umem_alloc(size, flags);
	if (tmp)
		memset(tmp, 0, size);

	return tmp;
}

void umem_free(void *buf, size_t size)
{
	free(buf);
}
