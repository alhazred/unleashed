MODULE=		ufs
MODULE_TYPE=	fs
MODULE_DEPS=	fs/specfs \
		misc/fssnap_if
SRCS=		lufs.c \
		lufs_debug.c \
		lufs_log.c \
		lufs_map.c \
		lufs_thread.c \
		lufs_top.c \
		quota.c \
		quota_ufs.c \
		quotacalls.c \
		ufs_acl.c \
		ufs_alloc.c \
		ufs_bmap.c \
		ufs_dir.c \
		ufs_directio.c \
		ufs_extvnops.c \
		ufs_filio.c \
		ufs_inode.c \
		ufs_lockfs.c \
		ufs_log.c \
		ufs_panic.c \
		ufs_snap.c \
		ufs_subr.c \
		ufs_tables.c \
		ufs_thread.c \
		ufs_trans.c \
		ufs_vfsops.c \
		ufs_vnops.c \
		ufs_xattr.c

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-type-limits

.include <kmod.mk>
