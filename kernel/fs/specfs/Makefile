MODULE=		specfs
MODULE_TYPE=	fs
MODULE_DEPS=	fs/fifofs
SRCS=		specsubr.c \
		specvfsops.c \
		specvnops.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-uninitialized

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
