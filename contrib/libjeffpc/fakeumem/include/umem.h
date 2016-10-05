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

#ifndef __FAKEUMEM_H
#define __FAKEUMEM_H

#include <stdlib.h>
#include <stdint.h>

/*
 * Not every distro has libumem available.  If we are unlucky enough to be
 * stuck on one of those, we just direct everything to standard malloc/free.
 */

#define UMEM_DEFAULT	0x0000 /* normal allocation - may fail */
#define UMEM_NOFAIL	0x0100 /* never fails - not implemented */

typedef uintptr_t umem_cache_t;
typedef uintptr_t umem_constructor_t;
typedef uintptr_t umem_destructor_t;
typedef uintptr_t umem_reclaim_t;
typedef uintptr_t vmem_t;

extern umem_cache_t *umem_cache_create(char *name, size_t size, size_t align,
				       umem_constructor_t *constructor,
				       umem_destructor_t *destructor,
				       umem_reclaim_t *reclaim,
				       void *cbdata, vmem_t *source,
				       int cflags);
extern void umem_cache_destroy(umem_cache_t *cache);

extern void *umem_cache_alloc(umem_cache_t *cache, int flags);
extern void umem_cache_free(umem_cache_t *cache, void *buf);

extern void *umem_alloc(size_t size, int flags);
extern void *umem_zalloc(size_t size, int flags);
extern void umem_free(void *buf, size_t size);

#endif
