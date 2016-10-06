/*
 * Copyright (c) 2011-2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
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

#ifndef __JEFFPC_IO_H
#define __JEFFPC_IO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <jeffpc/error.h>

/*
 * An incomplete read/write is considered an error.
 */
extern int xread(int fd, void *buf, size_t nbyte);
extern int xwrite(int fd, const void *buf, size_t nbyte);
extern int xpread(int fd, void *buf, size_t nbyte, off_t off);
extern int xpwrite(int fd, const void *buf, size_t nbyte, off_t off);

extern char *read_file_common(int dirfd, const char *fname, struct stat *sb);
extern int write_file(const char *fname, const char *data, size_t len);

static inline int xwrite_str(int fd, const char *s)
{
	return xwrite(fd, s, strlen(s));
}

static inline char *read_file(const char *fname)
{
	return read_file_common(AT_FDCWD, fname, NULL);
}

static inline char *read_file_at(int dirfd, const char *fname)
{
	return read_file_common(dirfd, fname, NULL);
}

static inline char *read_file_len(const char *fname, size_t *len)
{
	struct stat sb;
	char *ret;

	ret = read_file_common(AT_FDCWD, fname, &sb);
	if (!IS_ERR(ret))
		*len = sb.st_size;

	return ret;
}

static inline int xopen(const char *fname, int flags, mode_t mode)
{
	int fd;

	fd = open(fname, flags, mode);
	return (fd == -1) ? -errno : fd;
}

static inline int xopenat(int dirfd, const char *fname, int flags, mode_t mode)
{
	int fd;

	fd = openat(dirfd, fname, flags, mode);
	return (fd == -1) ? -errno : fd;
}

static inline int xclose(int fd)
{
	if (close(fd) == -1)
		return -errno;
	return 0;
}

static inline int xstat(const char *restrict path, struct stat *restrict buf)
{
	if (stat(path, buf) == -1)
		return -errno;
	return 0;
}

static inline int xlstat(const char *restrict path, struct stat *restrict buf)
{
	if (lstat(path, buf) == -1)
		return -errno;
	return 0;
}

static inline int xmkdir(const char *path, mode_t mode)
{
	if (mkdir(path, mode) == -1)
		return -errno;
	return 0;
}

static inline int xrename(const char *old, const char *new)
{
	if (rename(old, new) == -1)
		return -errno;
	return 0;
}

static inline int xunlink(const char *path)
{
	if (unlink(path) == -1)
		return -errno;
	return 0;
}

static inline int xunlinkat(int dirfd, const char *path, int flag)
{
	if (unlinkat(dirfd, path, flag) == -1)
		return -errno;
	return 0;
}

static inline int xftruncate(int fd, off_t len)
{
	if (ftruncate(fd, len) == -1)
		return -errno;
	return 0;
}

#endif
