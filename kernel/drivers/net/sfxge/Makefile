MODULE=		sfxge
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac
SRCS=		sfxge.c \
		sfxge_bar.c \
		sfxge_dma.c \
		sfxge_err.c \
		sfxge_ev.c \
		sfxge_gld_v3.c \
		sfxge_hash.c \
		sfxge_intr.c \
		sfxge_mac.c \
		sfxge_mcdi.c \
		sfxge_mon.c \
		sfxge_nvram.c \
		sfxge_pci.c \
		sfxge_phy.c \
		sfxge_rx.c \
		sfxge_sram.c \
		sfxge_tcp.c \
		sfxge_tx.c \
		sfxge_vpd.c
INCS=		${.CURDIR}
DEFS=		-D_USE_MTU_UPDATE

# Intel generic code
SRCS+=		common/ef10_ev.c \
		common/ef10_filter.c \
		common/ef10_intr.c \
		common/ef10_mac.c \
		common/ef10_mcdi.c \
		common/ef10_nic.c \
		common/ef10_nvram.c \
		common/ef10_phy.c \
		common/ef10_rx.c \
		common/ef10_tx.c \
		common/ef10_vpd.c \
		common/efx_bootcfg.c \
		common/efx_crc32.c \
		common/efx_ev.c \
		common/efx_filter.c \
		common/efx_hash.c \
		common/efx_intr.c \
		common/efx_lic.c \
		common/efx_mac.c \
		common/efx_mcdi.c \
		common/efx_mon.c \
		common/efx_nic.c \
		common/efx_nvram.c \
		common/efx_phy.c \
		common/efx_port.c \
		common/efx_rx.c \
		common/efx_sram.c \
		common/efx_tx.c \
		common/efx_vpd.c \
		common/efx_wol.c \
		common/hunt_nic.c \
		common/hunt_phy.c \
		common/mcdi_mon.c \
		common/siena_mac.c \
		common/siena_mcdi.c \
		common/siena_nic.c \
		common/siena_nvram.c \
		common/siena_phy.c \
		common/siena_sram.c \
		common/siena_vpd.c
INCS+=		${.CURDIR}/common

CERRWARN =  -Wno-unused-variable
CERRWARN += -Wno-unused-function

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
