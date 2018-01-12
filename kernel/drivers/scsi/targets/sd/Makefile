MODULE=		sd
MODULE_TYPE=	drv
MODULE_DEPS=	misc/scsi \
		misc/cmlb
MODULE_CONF=	sd.conf
SRCS=		sd.c \
		sd_xbuf.c

CERRWARN	+= -Wno-unused-value

.include <kmod.mk>
