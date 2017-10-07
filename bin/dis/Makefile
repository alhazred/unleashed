PROG=	dis
SRCS=	dis_list.c dis_main.c dis_target.c dis_util.cc

LDADD=		-ldisasm -luutil -lelf
# FIXME: we should not be reaching into the proto area for libuutil
LDFLAGS+=	-L${SRCTOP}/proto/root_i386/lib
CFLAGS+=	-I${SRCTOP}/usr/src/lib/libdisasm/common
CFLAGS+=	-I${SRCTOP}/usr/src/lib/libuutil/common
COPTS.dis_util.cc=	-fno-exceptions -fno-rtti

.include <unleashed.mk>
.include <prog.mk>
