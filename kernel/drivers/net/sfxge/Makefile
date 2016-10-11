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
INCS=		.
DEFS=		-D_USE_MTU_UPDATE

# Intel generic code
SRCS+=		ef10_ev.c \
		ef10_filter.c \
		ef10_intr.c \
		ef10_mac.c \
		ef10_mcdi.c \
		ef10_nic.c \
		ef10_nvram.c \
		ef10_phy.c \
		ef10_rx.c \
		ef10_tx.c \
		ef10_vpd.c \
		efx_bootcfg.c \
		efx_crc32.c \
		efx_ev.c \
		efx_filter.c \
		efx_hash.c \
		efx_intr.c \
		efx_lic.c \
		efx_mac.c \
		efx_mcdi.c \
		efx_mon.c \
		efx_nic.c \
		efx_nvram.c \
		efx_phy.c \
		efx_port.c \
		efx_rx.c \
		efx_sram.c \
		efx_tx.c \
		efx_vpd.c \
		efx_wol.c \
		hunt_nic.c \
		hunt_phy.c \
		mcdi_mon.c \
		siena_mac.c \
		siena_mcdi.c \
		siena_nic.c \
		siena_nvram.c \
		siena_phy.c \
		siena_sram.c \
		siena_vpd.c
SRCS_DIRS+=	common
INCS+=		common

CERRWARN =  -Wno-unused-variable
CERRWARN += -Wno-unused-function

REPOROOT = ../../../..
.include <kmod.mk>
