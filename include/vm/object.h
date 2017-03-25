/*
 * Copyright 2017 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _VM_OBJECT_H
#define _VM_OBJECT_H

#include <sys/list.h>
#include <sys/avl.h>

#ifdef	__cplusplus
extern "C" {
#endif

struct vmobject {
	/*
	 * We keep all the pages in an AVL tree indexed by the offset.  This
	 * allows us to do quick offset lookups.
	 *
	 * We also keep all the pages on a list, which we use during
	 * eviction.
	 */
	avl_tree_t	tree;
	list_t		list;

	kmutex_t	lock;
};

#if defined(_KERNEL)
#define vmobject_add_page_head(o,p)	list_insert_head(&(o)->list, (p))
#define vmobject_add_page_tail(o,p)	list_insert_tail(&(o)->list, (p))
#define vmobject_remove_page(o,p)	list_remove(&(o)->list, (p))
#define vmobject_get_head(o)		list_head(&(o)->list)
#define vmobject_get_tail(o)		list_tail(&(o)->list)
#define vmobject_get_prev(o,p)		list_prev(&(o)->list, (p))
#define vmobject_get_next(o,p)		list_next(&(o)->list, (p))

static inline struct page *
vmobject_get_prev_loop(struct vmobject *obj, struct page *page)
{
	struct page *p;

	p = vmobject_get_prev(obj, page);
	if (p == NULL)
		p = vmobject_get_tail(obj);

	return (p);
}

static inline struct page *
vmobject_get_next_loop(struct vmobject *obj, struct page *page)
{
	struct page *p;

	p = vmobject_get_next(obj, page);
	if (p == NULL)
		p = vmobject_get_head(obj);

	return (p);
}

static inline void
vmobject_move_page_tail(struct vmobject *obj, struct page *page)
{
	vmobject_remove_page(obj, page);
	vmobject_add_page_tail(obj, page);
}
#endif

#ifdef	__cplusplus
}
#endif

#endif
