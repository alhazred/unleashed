MODULE=		rtw
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211
SRCS=		rtw.c \
		rtwphy.c \
		rtwphyio.c \
		smc93cx6.c

CERRWARN =	-Wno-unused-value
CERRWARN +=	-Wno-unused-label
CERRWARN +=	-Wno-switch

.include <kmod.mk>
