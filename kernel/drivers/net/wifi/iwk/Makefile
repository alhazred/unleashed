MODULE=		iwk
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211 \
		drv/random \
		drv/ip
SRCS=		iwk2.c
INCS=		${.CURDIR}
DEFS=		-DIWL=4965

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-empty-body

.include <kmod.mk>
