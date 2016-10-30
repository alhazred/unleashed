MODULE=		chxge
MODULE_TYPE=	drv
MODULE_DEPS=	misc/gld \
		drv/ip
SRCS=		ch.c \
		glue.c \
		pe.c \
		sge.c \
		com/ch_mac.c \
		com/ch_subr.c \
		com/cspi.c \
		com/espi.c \
		com/ixf1010.c \
		com/mc3.c \
		com/mc4.c \
		com/mc5.c \
		com/mv88e1xxx.c \
		com/mv88x201x.c \
		com/my3126.c \
		com/pm3393.c \
		com/tp.c \
		com/ulp.c \
		com/vsc7321.c \
		com/vsc7326.c \
		com/xpak.c
INCS=		. \
		com
DEFS=		-DC99_NOT_SUPPORTED \
		-DCONFIG_CHELSIO_T1_1G \
		-DCONFIG_CHELSIO_T1_OFFLOAD \
		-DCONFIG_SMP \
		-DRX_MAX_COUNT=256 \
		-DSUN_KSTATS \
		-DHOST_PAUSE \
		-DTX_CKSUM_FIX \
		-DTX_THREAD_RECLAIM

CERRWARN =  -Wno-parentheses
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-unused-variable

REPOROOT = ../../../..
.include <kmod.mk>
