MODULE=		tmpfs
MODULE_TYPE=	fs
SRCS=		tmp_dir.c \
		tmp_subr.c \
		tmp_tnode.c \
		tmp_vfsops.c \
		tmp_vnops.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-unused-value
CERRWARN += -Wno-uninitialized

.include <kmod.mk>
