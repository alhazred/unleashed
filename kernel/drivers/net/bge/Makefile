MODULE=		bge
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac
# TODO: do we need to depend on drv/ip as well?
MODULE_CONF=	bge.conf
SRCS=		bge_atomic.c \
		bge_chip2.c \
		bge_kstats.c \
		bge_log.c \
		bge_main2.c \
		bge_mii.c \
		bge_mii_5906.c \
		bge_ndd.c \
		bge_recv2.c \
		bge_send.c

CERRWARN =  -Wno-unused-variable
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-switch

.include <kmod.mk>
