#ifndef KSH__COMPAT_H
#define KSH__COMPAT_H
typedef void (*sig_t)(int);
#define timespecsub(tsp, usp, vsp)                                      \
        do {                                                            \
                (vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;          \
                (vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;       \
                if ((vsp)->tv_nsec < 0) {                               \
                        (vsp)->tv_sec--;                                \
                        (vsp)->tv_nsec += 1000000000L;                  \
                }                                                       \
        } while (0)

#define timespeccmp(tsp, usp, cmp)                                      \
        (((tsp)->tv_sec == (usp)->tv_sec) ?                             \
            ((tsp)->tv_nsec cmp (usp)->tv_nsec) :                       \
            ((tsp)->tv_sec cmp (usp)->tv_sec))

#define __dead  __attribute__((__noreturn__))

#include <utmpx.h>
#define UT_NAMESIZE (sizeof (((struct utmpx *)0)->ut_name))
#define _PW_NAME_LEN (UT_NAMESIZE - 1)

#define pledge(promises, execpromises) 0
#define srand_deterministic srand
/* FIXME we should really implement these... */
#define setresuid(r,e,s) setreuid(r,e)
#define setresgid(r,e,s) setregid(r,e)
#endif /* KSH_COMPAT_H */
