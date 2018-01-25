#ifndef KSH_COMPAT_LIBGEN_H
#define KSH_COMPAT_LIBGEN_H
/* hide <libgen.h> gmatch() - misc.c defined a different one */
/* our libgen.h does have basename(), but it lacks const so override here */
char *basename(const char *);
#endif
