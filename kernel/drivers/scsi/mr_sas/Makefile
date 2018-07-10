MODULE=		mr_sas
MODULE_TYPE=	drv
MODULE_DEPS=	misc/scsi
MODULE_CONF=	mr_sas.conf
SRCS=		ld_pd_map.c \
		mr_sas.c \
		mr_sas_tbolt.c \
		mr_sas_list.c

CERRWARN += -Wno-unused-label
CERRWARN += -Wno-switch
CERRWARN += -Wno-uninitialized

.include <kmod.mk>
