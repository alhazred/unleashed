MODULE=		hxge
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		drv/ip
MODULE_CONF=	hxge.conf
SRCS=		hpi.c \
		hpi_pfc.c \
		hpi_rxdma.c \
		hpi_txdma.c \
		hpi_vir.c \
		hpi_vmac.c \
		hxge_fm.c \
		hxge_fzc.c \
		hxge_hw.c \
		hxge_kstats.c \
		hxge_main.c \
		hxge_ndd.c \
		hxge_pfc.c \
		hxge_rxdma.c \
		hxge_send.c \
		hxge_txdma.c \
		hxge_virtual.c \
		hxge_vmac.c
INCS=		.
DEFS=		-DSOLARIS

CERRWARN =  -Wno-uninitialized
CERRWARN += -Wno-switch
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-unused-label

REPOROOT = ../../../..
.include <kmod.mk>
