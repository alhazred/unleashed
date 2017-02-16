MODULE=		socksdp
MODULE_TYPE=	socketmod
MODULE_DEPS=	fs/sockfs \
		drv/ip \
		drv/sdpib
SRCS=		sockmod_sdp.c \
		socksdp.c \
		socksdpsubr.c

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
