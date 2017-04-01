MODULE=		ath
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211
SRCS=		ath_aux.c \
		ath_main.c \
		ath_osdep.c \
		ath_rate.c

CERRWARN =	-Wno-type-limits
CERRWARN +=	-Wno-unused-variable
CERRWARN +=	-Wno-empty-body

.include <kmod.mk>
