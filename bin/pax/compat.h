#ifndef _COMPAT_H
#define _COMPAT_H

typedef uint32_t u_int32_t;
typedef uint16_t u_int16_t;

char	*group_from_gid(gid_t, int);
char	*user_from_uid(uid_t, int);

int	dprintf(int fd, const char *fmt, ...);

void	strmode(int mode, char *p);

#include <utmpx.h>
#define	UT_NAMESIZE	(sizeof (((struct utmpx *)0)->ut_name))
#define	_PW_NAME_LEN	(UT_NAMESIZE - 1)
#include <nss_dbdefs.h>
#define	_PW_BUF_LEN	NSS_BUFLEN_PASSWD
/* XXX: sysconf(_SC_GETGR_R_SIZE_MAX) actually returns this value instead of
 * NSS_BUFLEN_GROUP; see NSS_BUFLEN_PRACTICAL in libc nss_common.c. */
#define	_GR_BUF_LEN	(512*1024)

#define	_PATH_DEFTAPE	"/dev/rmt/0"

#define	setpassent(x)	setpwent()
#define	setgroupent(x)	setgrent()

#define timespeccmp(tsp, usp, cmp)					\
	(((tsp)->tv_sec == (usp)->tv_sec) ?				\
	    ((tsp)->tv_nsec cmp (usp)->tv_nsec) :			\
	    ((tsp)->tv_sec cmp (usp)->tv_sec))
#endif
