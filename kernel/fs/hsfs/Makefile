MODULE=		hsfs
MODULE_TYPE=	fs
MODULE_DEPS=	fs/specfs
SRCS=		hsfs_node.c \
		hsfs_rrip.c \
		hsfs_subr.c \
		hsfs_susp.c \
		hsfs_susp_subr.c \
		hsfs_vfsops.c \
		hsfs_vnops.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-type-limits
CERRWARN += -Wno-switch

.include <../../kmod.mk>
