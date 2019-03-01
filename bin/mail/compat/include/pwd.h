#ifndef COMPAT_PWD_H
#define COMPAT_PWD_H
#include_next <pwd.h>
#define _PW_BUF_LEN 1024
#define _GR_BUF_LEN 1024
int uid_from_user(const char *, uid_t *);
const char *user_from_uid(uid_t, int);
#endif
