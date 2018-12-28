PROG=	dis
SRCS=	dis_list.c dis_main.c dis_target.c dis_util.c

LDADD=		-ldisasm -luutil -lelf -ldemangle-sys
CFLAGS+=	-I${SRCTOP}/usr/src/lib/libdisasm/common
CFLAGS+=	-I${SRCTOP}/usr/src/lib/libuutil/common

.include <unleashed.mk>
.include <prog.mk>
