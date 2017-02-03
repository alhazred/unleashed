#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int
dprintf(int fd, const char *fmt, ...)
{
	char *s = NULL;
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = vasprintf(&s, fmt, ap);
	va_end(ap);
	if (ret < 0)
		return ret;
	ret = write(fd, s, ret);
	free(s);
	return ret;
}
