MODULE=		iwp
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211 \
		drv/random \
		drv/ip
SRCS=		iwp.c
INCS=		${.CURDIR}

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-uninitialized

REPOROOT = ${.CURDIR:H:H:H:H:H}
.include <kmod.mk>
