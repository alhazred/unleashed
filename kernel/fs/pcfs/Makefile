MODULE=		pcfs
MODULE_TYPE=	fs
SRCS=		pc_alloc.c \
		pc_dir.c \
		pc_node.c \
		pc_subr.c \
		pc_vfsops.c \
		pc_vnops.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-type-limits
CERRWARN += -Wno-unused-label
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-function

.include <../../kmod.mk>
