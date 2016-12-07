#ifndef _FREEBSD_COMPAT_H
#define _FREEBSD_COMPAT_H
#define ALLPERMS	(S_ISUID|S_ISGID|S_IRWXU|S_IRWXG|S_IRWXO)
#define DEFFILEMODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

#include <errno.h>
/* XXX: ourl libc doesn't provide errc() */
static void
errc(int eval, int code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	errno = code;
	verr(eval, fmt, args);
	va_end(args);
}
#endif /* _FREEBSD_COMPAT_H */
