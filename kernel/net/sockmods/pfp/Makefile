MODULE=		sockpfp
MODULE_TYPE=	socketmod
MODULE_DEPS=	fs/sockfs \
		misc/dls \
		misc/mac \
		drv/bpf \
		drv/ip
SRCS=		sockmod_pfp.c
INCS=		${SRCTOP}/usr/src/uts/common/io/bpf

.include <kmod.mk>
