MODULE=		mega_sas
MODULE_TYPE=	drv
MODULE_DEPS=	misc/scsi
MODULE_CONF=	mega_sas.conf
SRCS=		megaraid_sas.c

CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-variable

.include <kmod.mk>
