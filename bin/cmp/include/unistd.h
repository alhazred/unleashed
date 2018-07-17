#ifndef COMPAT_UNISTD_H
#define COMPAT_UNISTD_H
#define __dead	__attribute__((__noreturn__))
#define pledge(promises, execpromises) 0
#include_next <unistd.h>
#endif
