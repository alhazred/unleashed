MODULE=		rwn
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211
SRCS=		rt2860.c

CERRWARN =	-Wno-unused-value
CERRWARN +=	-Wno-unused-label
CERRWARN +=	-Wno-parentheses

.include <kmod.mk>
