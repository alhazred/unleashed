PROG=	size
SRCS=	main.c process.c fcns.c
CPPFLAGS+= \
	-I${SRCTOP}/usr/src/cmd/sgs/include \
	-I${SRCTOP}/usr/src/common/elfcap
LDFLAGS+= -lelf

.include <unleashed.mk>
.include <prog.mk>
