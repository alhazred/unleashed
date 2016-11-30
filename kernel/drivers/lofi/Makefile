MODULE=		lofi
MODULE_TYPE=	drv
MODULE_DEPS=	misc/cmlb
MODULE_CONF=	lofi.conf
SRCS=		lofi.c \
		LzmaDec.c
SRCS_DIRS=	${REPOROOT}/usr/src/common/lzma
INCS=		${REPOROOT}/usr/src/common/lzma

CERRWARN = -Wno-uninitialized

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
