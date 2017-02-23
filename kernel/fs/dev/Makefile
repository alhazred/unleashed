MODULE=		dev
MODULE_TYPE=	fs
MODULE_DEPS=	fs/devfs \
		misc/dls
SRCS=		sdev_comm.c \
		sdev_ipnetops.c \
		sdev_ncache.c \
		sdev_netops.c \
		sdev_profile.c \
		sdev_ptsops.c \
		sdev_subr.c \
		sdev_vfsops.c \
		sdev_vnops.c \
		sdev_vtops.c \
		sdev_zvolops.c
INCS=		${.CURDIR:H}/zfs \
		${REPOROOT}/usr/src/uts/common/io/bpf

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-parentheses

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
