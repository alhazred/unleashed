MODULE=		igb
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac
MODULE_CONF=	igb.conf
SRCS=		igb_buf.c \
		igb_debug.c \
		igb_gld.c \
		igb_log.c \
		igb_main.c \
		igb_osdep.c \
		igb_rx.c \
		igb_stat.c \
		igb_tx.c
INCS=		${.CURDIR}

CERRWARN =  -Wno-uninitialized
CERRWARN += -Wno-switch
CERRWARN += -Wno-unused-variable

.include <../e1000api/Makefile.common>

.include <kmod.mk>
