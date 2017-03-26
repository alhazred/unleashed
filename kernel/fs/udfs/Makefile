MODULE=		udfs
MODULE_TYPE=	fs
MODULE_DEPS=	fs/specfs
SRCS=		udf_alloc.c \
		udf_bmap.c \
		udf_dir.c \
		udf_inode.c \
		udf_subr.c \
		udf_vfsops.c \
		udf_vnops.c

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-type-limits

.include <kmod.mk>
