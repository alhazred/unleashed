#ifndef COMPAT_UNISTD_H
#define COMPAT_UNISTD_H
#define pledge(request, paths) 0
#include_next <unistd.h>
#endif
