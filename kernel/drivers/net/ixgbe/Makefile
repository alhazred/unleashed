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
SRCS+=		ixgbe_82598.c \
		ixgbe_82599.c \
		ixgbe_api.c \
		ixgbe_common.c \
		ixgbe_dcb.c \
		ixgbe_dcb_82598.c \
		ixgbe_dcb_82599.c \
		ixgbe_mbx.c \
		ixgbe_phy.c \
		ixgbe_vf.c \
		ixgbe_x540.c \
		ixgbe_x550.c
SRCS_DIRS+=	core
INCS+=		core

CERRWARN =  -Wno-switch
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-empty-body

REPOROOT = ../../../..
.include <kmod.mk>
