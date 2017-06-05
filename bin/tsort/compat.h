#ifndef _COMPAT_H
#define _COMPAT_H
char *fgetln(FILE *fp, size_t *lenp);

#define __dead

static inline int pledge(const char *promises, const char *paths[]) {
	return 0;
}
#endif
