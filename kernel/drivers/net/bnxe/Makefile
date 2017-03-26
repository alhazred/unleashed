MODULE=		bnxe
MODULE_TYPE=	drv
MODULE_DEPS=	drv/ip \
		misc/mac
MODULE_CONF=	bnxe.conf
SRCS=		577xx/common/bnxe_clc.c \
		577xx/drivers/common/ecore/ecore_sp_verbs.c \
		577xx/drivers/common/lm/device/bnxe_context.c \
		577xx/drivers/common/lm/device/bnxe_hw_debug.c \
		577xx/drivers/common/lm/device/lm_dcbx.c \
		577xx/drivers/common/lm/device/lm_dcbx_mp.c \
		577xx/drivers/common/lm/device/lm_devinfo.c \
		577xx/drivers/common/lm/device/lm_dmae.c \
		577xx/drivers/common/lm/device/lm_er.c \
		577xx/drivers/common/lm/device/lm_hw_access.c \
		577xx/drivers/common/lm/device/lm_hw_attn.c \
		577xx/drivers/common/lm/device/lm_hw_init_reset.c \
		577xx/drivers/common/lm/device/lm_main.c \
		577xx/drivers/common/lm/device/lm_mcp.c \
		577xx/drivers/common/lm/device/lm_niv.c \
		577xx/drivers/common/lm/device/lm_nvram.c \
		577xx/drivers/common/lm/device/lm_phy.c \
		577xx/drivers/common/lm/device/lm_power.c \
		577xx/drivers/common/lm/device/lm_recv.c \
		577xx/drivers/common/lm/device/lm_resc.c \
		577xx/drivers/common/lm/device/lm_sb.c \
		577xx/drivers/common/lm/device/lm_send.c \
		577xx/drivers/common/lm/device/lm_sp.c \
		577xx/drivers/common/lm/device/lm_sp_req_mgr.c \
		577xx/drivers/common/lm/device/lm_stats.c \
		577xx/drivers/common/lm/device/lm_util.c \
		577xx/drivers/common/lm/fw/57710_init_values.c \
		577xx/drivers/common/lm/fw/57711_init_values.c \
		577xx/drivers/common/lm/fw/57712_init_values.c \
		577xx/drivers/common/lm/fw/bnxe_fw_funcs.c \
		577xx/drivers/common/lm/l4/lm_l4fp.c \
		577xx/drivers/common/lm/l4/lm_l4rx.c \
		577xx/drivers/common/lm/l4/lm_l4sp.c \
		577xx/drivers/common/lm/l4/lm_l4tx.c \
		577xx/drivers/common/lm/l5/lm_l5.c \
		577xx/drivers/common/lm/l5/lm_l5sp.c \
		bnxe_cfg.c \
		bnxe_debug.c \
		bnxe_fcoe.c \
		bnxe_gld.c \
		bnxe_hw.c \
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
		bnxe_workq.c
INCS=		${.CURDIR}/577xx/include \
		${.CURDIR}/577xx/drivers/common/ecore \
		${.CURDIR}/577xx/drivers/common/include \
		${.CURDIR}/577xx/drivers/common/include/l4 \
		${.CURDIR}/577xx/drivers/common/include/l5 \
		${.CURDIR}/577xx/drivers/common/lm/device \
		${.CURDIR}/577xx/drivers/common/lm/fw \
		${.CURDIR}/577xx/drivers/common/lm/include \
		${.CURDIR}/577xx/drivers/common/lm/l4 \
		${.CURDIR}/577xx/drivers/common/lm/l4/include \
		${.CURDIR}/577xx/drivers/common/lm/l5 \
		${.CURDIR}/577xx/drivers/common/lm/l5/include \
		${.CURDIR}/577xx/hsi/hw/include \
		${.CURDIR}/577xx/hsi/mcp
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

.include <kmod.mk>
