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

#ifndef __JEFFPC_SYNCH_H
#define __JEFFPC_SYNCH_H

#include <stdbool.h>
#include <pthread.h>

struct lock {
	pthread_mutex_t lock;
};

struct rwlock {
	pthread_rwlock_t lock;
};

struct cond {
	pthread_cond_t cond;
};

extern void mxinit(struct lock *m);
extern void mxdestroy(struct lock *m);
extern void mxlock(struct lock *m);
extern void mxunlock(struct lock *m);

extern void rwinit(struct rwlock *l);
extern void rwdestroy(struct rwlock *l);
extern void rwlock(struct rwlock *l, bool wr);
extern void rwunlock(struct rwlock *l);

extern void condinit(struct cond *c);
extern void conddestroy(struct cond *c);
extern void condwait(struct cond *c, struct lock *m);
extern int condreltimedwait(struct cond *c, struct lock *m,
			    const struct timespec *reltime);
extern void condsig(struct cond *c);
extern void condbcast(struct cond *c);

/* compat macros */
#define MXINIT(l)	mxinit(l)
#define MXDESTROY(l)	mxdestroy(l)
#define MXLOCK(l)	mxlock(l)
#define MXUNLOCK(l)	mxunlock(l)
#define CONDINIT(c)	condinit(c)
#define CONDDESTROY(c)	conddestroy(c)
#define CONDWAIT(c,m)	condwait((c),(m))
#define CONDSIG(c)	condsig(c)
#define CONDBCAST(c)	condbcast(c)

#endif
