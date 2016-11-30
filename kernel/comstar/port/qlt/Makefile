MODULE=		qlt
MODULE_TYPE=	drv
MODULE_DEPS=	drv/fct \
		drv/stmf
MODULE_CONF=	qlt.conf
SRCS=		2400.c \
		2500.c \
		8100.c \
		qlt.c \
		qlt_dma.c
DEFS=		-DUNIQUE_FW_NAME

CERRWARN = -Wno-uninitialized

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
