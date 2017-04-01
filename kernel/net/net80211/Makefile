MODULE=		net80211
MODULE_TYPE=	misc
MODULE_DEPS=	misc/mac \
		mac/mac_wifi \
		drv/ip
SRCS=		net80211.c \
		net80211_amrr.c \
		net80211_crypto.c \
		net80211_crypto_ccmp.c \
		net80211_crypto_none.c \
		net80211_crypto_tkip.c \
		net80211_crypto_wep.c \
		net80211_ht.c \
		net80211_input.c \
		net80211_ioctl.c \
		net80211_node.c \
		net80211_output.c \
		net80211_proto.c

CERRWARN =  -Wno-uninitialized
CERRWARN += -Wno-switch
CERRWARN += -Wno-parentheses

.include <kmod.mk>
