MODULE=		zyd
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211 \
		misc/usba
SRCS=		zyd.c \
		zyd_fw.c \
		zyd_hw.c \
		zyd_usb.c

CERRWARN =  -Wno-empty-body
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-uninitialized

REPOROOT = ${.CURDIR:H:H:H:H:H}
.include <kmod.mk>
