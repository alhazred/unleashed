MODULE=		pcfs
MODULE_TYPE=	fs
SRCS=		pc_alloc.c \
		pc_dir.c \
		pc_node.c \
		pc_subr.c \
		pc_vfsops.c \
		pc_vnops.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-function

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
