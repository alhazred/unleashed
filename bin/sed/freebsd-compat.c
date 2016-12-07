#include <err.h>
#include <errno.h>
/* XXX: this should be removed once we're no longer building against a libc
 * which doesn't have errc */
void
errc(int eval, int code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	errno = code;
	verr(eval, fmt, args);
	va_end(args);
}
