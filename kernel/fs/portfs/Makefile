MODULE=		portfs
MODULE_TYPE=	sys
SRCS=		port.c \
		port_fd.c \
		port_fop.c \
		port_vnops.c

CERRWARN =  -Wno-uninitialized
CERRWARN += -Wno-parentheses

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
