#ifndef KSH_COMPAT_FCNTL_H
#define KSH_COMPAT_FCNTL_H
/* our flock and LOCK_* are in sys/file.h, openbsd has them in fcntl.h */
#include <sys/file.h>
#include_next <fcntl.h>
#endif
