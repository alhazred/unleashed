#ifndef COMPAT_FCNTL_H
#define COMPAT_FCNTL_H
#include_next <fcntl.h>
#include <sys/file.h> /* flock */
/* this doesn't really belong here, but __dead is used in main.c and that file
 * doesn't include much more than fcntl.h */
#define __dead __attribute__((noreturn))
#endif
