MODULE=		stmf
MODULE_TYPE=	drv
MODULE_CONF=	stmf.conf
SRCS=		lun_map.c \
		stmf.c

CERRWARN =  -Wno-uninitialized
CERRWARN += -Wno-switch
CERRWARN += -Wno-unused-variable

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
