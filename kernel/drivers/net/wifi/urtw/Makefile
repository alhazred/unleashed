MODULE=		urtw
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211 \
		misc/usba
SRCS=		urtw.c

CERRWARN =	-Wno-char-subscripts
CERRWARN +=	-Wno-switch
CERRWARN +=	-Wno-empty-body
CERRWARN +=	-Wno-parentheses
CERRWARN +=	-Wno-uninitialized

.include <kmod.mk>
