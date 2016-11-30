MODULE=		objfs
MODULE_TYPE=	fs
SRCS=		objfs_common.c \
		objfs_data.c \
		objfs_odir.c \
		objfs_root.c \
		objfs_vfs.c

CERRWARN =  -Wno-unused-function
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-parentheses

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
