MODULE=		devfs
MODULE_TYPE=	fs
SRCS=		devfs_subr.c \
		devfs_vfsops.c \
		devfs_vnops.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-empty-body
CERRWARN += -Wno-unused-variable

REPOROOT = ../../..
.include <kmod.mk>
