#ifndef CMP_COMPAT_H
#define CMP_COMPAT_H
#define __dead	__attribute__((__noreturn__))
#define pledge(promises, execpromises) 0
#endif
