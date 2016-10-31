MODULE=		bnxe
MODULE_TYPE=	drv
MODULE_DEPS=	drv/ip \
		misc/mac
MODULE_CONF=	bnxe.conf
SRCS=		57710_init_values.c \
		57711_init_values.c \
		57712_init_values.c \
		bnxe_cfg.c \
		bnxe_clc.c \
		bnxe_context.c \
		bnxe_debug.c \
		bnxe_fcoe.c \
		bnxe_fw_funcs.c \
		bnxe_gld.c \
		bnxe_hw.c \
		bnxe_hw_debug.c \
		bnxe_intr.c \
		bnxe_kstat.c \
		bnxe_lock.c \
		bnxe_main.c \
		bnxe_mm.c \
		bnxe_mm_l4.c \
		bnxe_mm_l5.c \
		bnxe_rr.c \
		bnxe_rx.c \
		bnxe_timer.c \
		bnxe_tx.c \
		bnxe_workq.c \
		ecore_sp_verbs.c \
		lm_dcbx.c \
		lm_dcbx_mp.c \
		lm_devinfo.c \
		lm_dmae.c \
		lm_er.c \
		lm_hw_access.c \
		lm_hw_attn.c \
		lm_hw_init_reset.c \
		lm_l4fp.c \
		lm_l4rx.c \
		lm_l4sp.c \
		lm_l4tx.c \
		lm_l5.c \
		lm_l5sp.c \
		lm_main.c \
		lm_mcp.c \
		lm_niv.c \
		lm_nvram.c \
		lm_phy.c \
		lm_power.c \
		lm_recv.c \
		lm_resc.c \
		lm_sb.c \
		lm_send.c \
		lm_sp.c \
		lm_sp_req_mgr.c \
		lm_stats.c \
		lm_util.c
SRCS_DIRS=	577xx/common \
		577xx/drivers/common/ecore \
		577xx/drivers/common/lm/device \
		577xx/drivers/common/lm/fw \
		577xx/drivers/common/lm/l4 \
		577xx/drivers/common/lm/l5
INCS=		577xx/include \
		577xx/drivers/common/ecore \
		577xx/drivers/common/include \
		577xx/drivers/common/include/l4 \
		577xx/drivers/common/include/l5 \
		577xx/drivers/common/lm/device \
		577xx/drivers/common/lm/fw \
		577xx/drivers/common/lm/include \
		577xx/drivers/common/lm/l4 \
		577xx/drivers/common/lm/l4/include \
		577xx/drivers/common/lm/l5 \
		577xx/drivers/common/lm/l5/include \
		577xx/hsi/hw/include \
		577xx/hsi/mcp
DEFS=		-DLM_RXPKT_NON_CONTIGUOUS \
		-DELINK_ENHANCEMENTS \
		-DELINK_57711E_SUPPORT \
		-DELINK_DEBUG \
		-D__inline= \
		-D_inline= \
		-D__BASENAME__=\"bnxe\" \
		-D__SunOS \
		-D__S11 \
		-DILLUMOS \
		-DLITTLE_ENDIAN \
		-DLITTLE_ENDIAN_HOST \
		-D__LITTLE_ENDIAN
# TODO: on sparc, define BIG_ENDIAN, BIG_ENDIAN_HOST, and __BIG_ENDIAN

CERRWARN =  -Wno-unused-function
CERRWARN += -Wno-unused-value
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-switch
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-parentheses

REPOROOT = ../../../..
.include <kmod.mk>
