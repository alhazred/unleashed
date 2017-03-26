MODULE=		autofs
MODULE_TYPE=	fs
MODULE_TYPE_LINKS=sys
MODULE_DEPS=	strmod/rpcmod \
		misc/rpcsec \
		fs/mntfs
SRCS=		auto_subr.c \
		auto_sys.c \
		auto_vfsops.c \
		auto_vnops.c \
		auto_xdr.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-unused-label
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-empty-body
CERRWARN += -Wno-unused-variable

.include <kmod.mk>
