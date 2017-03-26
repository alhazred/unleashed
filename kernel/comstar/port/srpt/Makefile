MODULE=		srpt
MODULE_TYPE=	drv
MODULE_DEPS=	drv/stmf \
		misc/ibtl \
		misc/ibcm
MODULE_CONF=	srpt.conf
SRCS=		srpt_ch.c \
		srpt_cm.c \
		srpt_ioc.c \
		srpt_mod.c \
		srpt_stp.c

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-unused-value

.include <kmod.mk>
