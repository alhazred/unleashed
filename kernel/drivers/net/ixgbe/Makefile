MODULE=		ixgbe
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac
MODULE_CONF=	ixgbe.conf
SRCS=		ixgbe_buf.c \
		ixgbe_debug.c \
		ixgbe_gld.c \
		ixgbe_log.c \
		ixgbe_main.c \
		ixgbe_osdep.c \
		ixgbe_rx.c \
		ixgbe_stat.c \
		ixgbe_tx.c
INCS=		.

# Intel generic code
SRCS+=		core/ixgbe_82598.c \
		core/ixgbe_82599.c \
		core/ixgbe_api.c \
		core/ixgbe_common.c \
		core/ixgbe_dcb.c \
		core/ixgbe_dcb_82598.c \
		core/ixgbe_dcb_82599.c \
		core/ixgbe_mbx.c \
		core/ixgbe_phy.c \
		core/ixgbe_vf.c \
		core/ixgbe_x540.c \
		core/ixgbe_x550.c
INCS+=		core

CERRWARN =  -Wno-switch
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-empty-body

REPOROOT = ../../../..
.include <kmod.mk>
