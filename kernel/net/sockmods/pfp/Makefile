MODULE=		sockpfp
MODULE_TYPE=	socketmod
MODULE_DEPS=	fs/sockfs \
		misc/dls \
		misc/mac \
		drv/bpf \
		drv/ip
SRCS=		sockmod_pfp.c
INCS=		${REPOROOT}/usr/src/uts/common/io/bpf

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
