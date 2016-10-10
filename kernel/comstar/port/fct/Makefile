MODULE=		fct
MODULE_TYPE=	drv
MODULE_DEPS=	drv/stmf
MODULE_CONF=	fct.conf
SRCS=		discovery.c \
		fct.c

CERRWARN	+= -Wno-unused-label
CERRWARN	+= -Wno-switch
CERRWARN	+= -Wno-uninitialized

REPOROOT = ../../../..
.include <kmod.mk>
