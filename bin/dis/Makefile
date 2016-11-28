PROG=	dis
SRCS=	dis_list.c dis_main.c dis_target.c dis_util.cc

LDADD=		-ldisasm -luutil -lelf
# FIXME: we should not be reaching into the proto area for libuutil
LDFLAGS+=	-L${.CURDIR:H:H}/proto/root_i386/lib
CFLAGS+=	-I${.CURDIR:H:H}/usr/src/lib/libdisasm/common
COPTS.dis_util.cc=	-fno-exceptions -fno-rtti

.include <prog.mk>
