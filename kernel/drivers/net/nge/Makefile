MODULE=		nge
MODULE_TYPE=	drv
MODULE_CONF=	nge.conf
MODULE_DEPS=	misc/mac
SRCS=		nge_atomic.c \
		nge_chip.c \
		nge_kstats.c \
		nge_log.c \
		nge_main.c \
		nge_ndd.c \
		nge_rx.c \
		nge_tx.c \
		nge_xmii.c

CERRWARN =  -Wno-unused-function
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-switch

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
