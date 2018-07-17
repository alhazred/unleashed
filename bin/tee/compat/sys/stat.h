#ifndef TEE_COMPAT_H
#define TEE_COMPAT_H
#include_next <sys/stat.h>
#define DEFFILEMODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
#endif
