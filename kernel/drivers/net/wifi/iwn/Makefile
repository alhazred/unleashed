MODULE=		iwn
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		misc/net80211 \
		drv/random \
		drv/ip
MODULE_FW=	fw/iwlwifi-100-5.ucode \
		fw/iwlwifi-1000-3.ucode \
		fw/iwlwifi-105-6.ucode \
		fw/iwlwifi-135-6.ucode \
		fw/iwlwifi-2000-6.ucode \
		fw/iwlwifi-2030-6.ucode \
		fw/iwlwifi-4965-2.ucode \
		fw/iwlwifi-5000-2.ucode \
		fw/iwlwifi-5150-2.ucode \
		fw/iwlwifi-6000-4.ucode \
		fw/iwlwifi-6000g2a-6.ucode \
		fw/iwlwifi-6000g2b-6.ucode \
		fw/iwlwifi-6050-5.ucode
SRCS=		if_iwn.c

REPOROOT = ../../../../..
.include <kmod.mk>
