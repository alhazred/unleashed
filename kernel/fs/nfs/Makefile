MODULE=		nfs
MODULE_TYPE=	fs
MODULE_TYPE_LINKS=sys
MODULE_DEPS=	fs/specfs \
		strmod/rpcmod \
		misc/rpcsec
SRCS=		nfs3_vfsops.c \
		nfs3_vnops.c \
		nfs3_xdr.c \
		nfs4_acache.c \
		nfs4_acl.c \
		nfs4_attr.c \
		nfs4_callback.c \
		nfs4_client.c \
		nfs4_client_debug.c \
		nfs4_client_secinfo.c \
		nfs4_client_state.c \
		nfs4_common.c \
		nfs4_idmap.c \
		nfs4_recovery.c \
		nfs4_rnode.c \
		nfs4_shadow.c \
		nfs4_stub_vnops.c \
		nfs4_subr.c \
		nfs4_vfsops.c \
		nfs4_vnops.c \
		nfs4_xdr.c \
		nfs_acl_vnops.c \
		nfs_acl_xdr.c \
		nfs_client.c \
		nfs_cmd.c \
		nfs_common.c \
		nfs_dump.c \
		nfs_stats.c \
		nfs_strerror.c \
		nfs_subr.c \
		nfs_sys.c \
		nfs_vfsops.c \
		nfs_vnops.c \
		nfs_xdr.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-empty-body
CERRWARN += -Wno-switch
CERRWARN += -Wno-type-limits

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
