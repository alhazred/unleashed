#ifndef COMPAT_TERMIOS_H
#define COMPAT_TERMIOS_H
#define CCEQ(val, c)     (c == val ? val != _POSIX_VDISABLE : 0)
/* ALTWERASE is checked with & - make it always false */
#define ALTWERASE 0
#include_next <termios.h>
#endif
