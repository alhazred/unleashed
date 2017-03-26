MODULE=		lofi
MODULE_TYPE=	drv
MODULE_DEPS=	misc/cmlb
MODULE_CONF=	lofi.conf
SRCS=		lofi.c \
		LzmaDec.c
INCS=		${SRCTOP}/usr/src/common/lzma
.PATH: ${SRCTOP}/usr/src/common/lzma

CERRWARN = -Wno-uninitialized

.include <kmod.mk>
