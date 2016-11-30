MODULE=		pppt
MODULE_TYPE=	drv
MODULE_DEPS=	drv/stmf
MODULE_CONF=	pppt.conf
SRCS=		alua_ic_if.c \
		pppt.c \
		pppt_msg.c \
		pppt_tgt.c

CERRWARN =  -Wno-switch
CERRWARN += -Wno-uninitialized

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
