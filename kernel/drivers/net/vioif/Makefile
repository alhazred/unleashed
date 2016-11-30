MODULE=		vioif
MODULE_TYPE=	drv
MODULE_DEPS=	misc/virtio \
		misc/mac
SRCS=		vioif.c
INCS=		${REPOROOT}/kernel/drivers/virtio

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
