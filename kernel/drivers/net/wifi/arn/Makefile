MODULE=		arn
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211
SRCS=		arn_ani.c \
		arn_beacon.c \
		arn_calib.c \
		arn_eeprom.c \
		arn_hw.c \
		arn_mac.c \
		arn_main.c \
		arn_phy.c \
		arn_rc.c \
		arn_recv.c \
		arn_regd.c \
		arn_xmit.c

CERRWARN =	-Wno-empty-body
CERRWARN +=	-Wno-uninitialized
CERRWARN +=	-Wno-unused-variable
CERRWARN +=	-Wno-unused-function
CERRWARN +=	-Wno-char-subscripts

.include <kmod.mk>
