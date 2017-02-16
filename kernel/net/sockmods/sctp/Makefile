MODULE=		socksctp
MODULE_TYPE=	socketmod
MODULE_DEPS=	fs/sockfs \
		drv/ip
SRCS=		sockmod_sctp.c \
		socksctp.c \
		socksctpsubr.c

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
