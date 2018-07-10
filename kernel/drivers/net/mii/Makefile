MODULE=		mii
MODULE_TYPE=	misc
MODULE_DEPS=	misc/mac
SRCS=		mii.c \
		mii_cicada.c \
		mii_natsemi.c \
		mii_intel.c \
		mii_qualsemi.c \
		mii_marvell.c \
		mii_realtek.c \
		mii_other.c

CERRWARN += -Wno-switch
CERRWARN += -Wno-uninitialized

.include <kmod.mk>
