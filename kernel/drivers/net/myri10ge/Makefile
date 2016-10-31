MODULE=		myri10ge
MODULE_TYPE=	drv
MODULE_DEPS=	misc/mac \
		drv/ip
SRCS=		myri10ge.c \
		myri10ge_lro.c
INCS=		firmware
DEFS=		-DSOLARIS_11

REPOROOT = ../../../..
.include <kmod.mk>
