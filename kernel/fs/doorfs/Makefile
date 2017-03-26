MODULE=		doorfs
MODULE_TYPE=	sys
SRCS=		door_sys.c \
		door_vnops.c \
		door_support_${CONFIG_MACH}.c

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-uninitialized

.include <kmod.mk>
