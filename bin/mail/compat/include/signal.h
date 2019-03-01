#ifndef COMPAT_SIGNAL_H
#define COMPAT_SIGNAL_H
typedef void (*sig_t)(int);
#include_next <signal.h>
#endif
