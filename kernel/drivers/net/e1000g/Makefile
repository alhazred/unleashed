MODULE=		e1000g
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac
MODULE_CONF=	e1000g.conf
SRCS=		e1000g_alloc.c \
		e1000g_debug.c \
		e1000g_main.c \
		e1000g_osdep.c \
		e1000g_rx.c \
		e1000g_stat.c \
		e1000g_tx.c \
		e1000g_workarounds.c
INCS=		${.CURDIR}

CERRWARN =  -Wno-uninitialized
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-empty-body
CERRWARN += -Wno-switch
CERRWARN += -Wno-unused-variable

.include <../e1000api/Makefile.common>

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
