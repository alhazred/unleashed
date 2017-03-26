MODULE=		iwh
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211 \
		drv/random \
		drv/ip
SRCS=		iwh.c
INCS=		${.CURDIR}

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-uninitialized

.include <kmod.mk>
