MODULE=		iwi
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211 \
		drv/random \
		drv/ip
SRCS=		ipw2200.c \
		ipw2200_hw.c
INCS=		.

CERRWARN = -Wno-unused-variable

REPOROOT = ../../../../..
.include <kmod.mk>
