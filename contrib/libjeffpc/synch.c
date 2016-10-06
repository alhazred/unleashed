/*
 * Copyright (c) 2015-2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
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

#include <stdio.h>
#include <stdlib.h>

#include <jeffpc/error.h>
#include <jeffpc/synch.h>
#include <jeffpc/config.h>

void mxinit(struct lock *l)
{
	VERIFY0(pthread_mutex_init(&l->lock, NULL));
}

void mxdestroy(struct lock *l)
{
	VERIFY0(pthread_mutex_destroy(&l->lock));
}

void mxlock(struct lock *l)
{
	VERIFY0(pthread_mutex_lock(&l->lock));
}

void mxunlock(struct lock *l)
{
	VERIFY0(pthread_mutex_unlock(&l->lock));
}

void rwinit(struct rwlock *l)
{
	VERIFY0(pthread_rwlock_init(&l->lock, NULL));
}

void rwdestroy(struct rwlock *l)
{
	VERIFY0(pthread_rwlock_destroy(&l->lock));
}

void rwlock(struct rwlock *l, bool wr)
{
	if (wr)
		VERIFY0(pthread_rwlock_wrlock(&l->lock));
	else
		VERIFY0(pthread_rwlock_rdlock(&l->lock));
}

void rwunlock(struct rwlock *l)
{
	VERIFY0(pthread_rwlock_unlock(&l->lock));
}

void condinit(struct cond *c)
{
	VERIFY0(pthread_cond_init(&c->cond, NULL));
}

void conddestroy(struct cond *c)
{
	VERIFY0(pthread_cond_destroy(&c->cond));
}

void condwait(struct cond *c, struct lock *l)
{
	VERIFY0(pthread_cond_wait(&c->cond, &l->lock));
}

int condreltimedwait(struct cond *c, struct lock *l,
		     const struct timespec *reltime)
{
	int ret;

#ifdef HAVE_PTHREAD_COND_RELTIMEDWAIT_NP
	ret = -pthread_cond_reltimedwait_np(&c->cond, &l->lock, reltime);
#else
	struct timespec abstime;
	struct timespec now;

	VERIFY0(clock_gettime(CLOCK_REALTIME, &now));

	while ((now.tv_nsec + reltime->tv_nsec) >= 1000000000) {
		now.tv_sec++;
		now.tv_nsec -= 1000000000;
	}

	abstime.tv_sec  = now.tv_sec  + reltime->tv_sec;
	abstime.tv_nsec = now.tv_nsec + reltime->tv_nsec;

	ret = -pthread_cond_timedwait(&c->cond, &l->lock, &abstime);
#endif

	if ((ret != 0) && (ret != -ETIMEDOUT))
		panic("%s failed: %s", __func__, xstrerror(ret));

	return ret;
}

void condsig(struct cond *c)
{
	VERIFY0(pthread_cond_signal(&c->cond));
}

void condbcast(struct cond *c)
{
	VERIFY0(pthread_cond_broadcast(&c->cond));
}
