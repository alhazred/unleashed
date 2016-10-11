MODULE=		nxge
MODULE_TYPE=	drv
MODULE_CONF=	nxge.conf
MODULE_DEPS=	misc/mac \
		drv/ip
SRCS=		nxge_classify.c \
		nxge_espc.c \
		nxge_fflp.c \
		nxge_fflp_hash.c \
		nxge_fm.c \
		nxge_fzc.c \
		nxge_hio.c \
		nxge_hio_guest.c \
		nxge_hv.c \
		nxge_hw.c \
		nxge_intr.c \
		nxge_ipp.c \
		nxge_kstats.c \
		nxge_mac.c \
		nxge_main.c \
		nxge_ndd.c \
		nxge_rxdma.c \
		nxge_send.c \
		nxge_txc.c \
		nxge_txdma.c \
		nxge_virtual.c \
		nxge_zcp.c
# sunv4 also needs: nxge_hcall.s
INCS=		../../../../usr/src/uts/common/sys/nxge
DEFS=		-DSOLARIS

SRCS+=		npi.c \
		npi_espc.c \
		npi_fflp.c \
		npi_ipp.c \
		npi_mac.c \
		npi_rxdma.c \
		npi_txc.c \
		npi_txdma.c \
		npi_vir.c \
		npi_zcp.c
INCS+=		npi
SRCS_DIRS+=	npi

CERRWARN =  -Wno-uninitialized
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-switch
CERRWARN += -Wno-type-limits

REPOROOT = ../../../..
.include <kmod.mk>
