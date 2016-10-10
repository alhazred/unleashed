MODULE=		lofi
MODULE_TYPE=	drv
MODULE_DEPS=	misc/cmlb
MODULE_CONF=	lofi.conf
SRCS=		lofi.c \
		LzmaDec.c
SRCS_DIRS=	../../../usr/src/common/lzma
INCS=		../../../usr/src/common/lzma

CERRWARN = -Wno-uninitialized

REPOROOT = ../../..
.include <kmod.mk>
