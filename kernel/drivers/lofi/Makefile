MODULE=		lofi
MODULE_TYPE=	drv
MODULE_DEPS=	misc/cmlb
MODULE_CONF=	lofi.conf
SRCS=		lofi.c \
		LzmaDec.c
SRCS_DIRS=	${SRCTOP}/usr/src/common/lzma
INCS=		${SRCTOP}/usr/src/common/lzma

CERRWARN = -Wno-uninitialized

.include <kmod.mk>
