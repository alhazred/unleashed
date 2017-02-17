PROG=	size
SRCS=	main.c process.c fcns.c
CPPFLAGS+= \
	-I${REPOROOT}/usr/src/cmd/sgs/include \
	-I${REPOROOT}/usr/src/common/elfcap
LDFLAGS+= -lelf

REPOROOT = ${.CURDIR:H:H}
.include <prog.mk>
