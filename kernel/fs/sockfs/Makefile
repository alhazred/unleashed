MODULE=		sockfs
MODULE_TYPE=	fs
MODULE_DEPS=	drv/ip
SRCS=		sock_notsupp.c \
		sockcommon.c \
		sockcommon_sops.c \
		sockcommon_subr.c \
		sockcommon_vnops.c \
		sockfilter.c \
		socknotify.c \
		sockparams.c \
		sockstr.c \
		socksubr.c \
		socksyscalls.c \
		socktpi.c \
		sockvfsops.c \
		sodirect.c

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-unused-value
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-parentheses

REPOROOT = ../../..
.include <kmod.mk>
