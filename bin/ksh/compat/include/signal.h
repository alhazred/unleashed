#ifndef KSH_COMPAT_SIGNAL_H
#define KSH_COMPAT_SIGNAL_H
#include_next <signal.h>
/* we have a suitable struct in libc for sys_siglist */
#define sys_siglist _sys_siglistp
/* but not for sys_signame; we need to use our own */
extern const char *const sys_signame[NSIG];
#endif
