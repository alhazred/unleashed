MODULE=		sockfs
MODULE_TYPE=	fs
MODULE_DEPS=	drv/ip
SRCS=		nl7c.c \
		nl7chttp.c \
		nl7clogd.c \
		nl7cnca.c \
		nl7curi.c \
		sock_notsupp.c \
		sockcommon.c \
		sockcommon_sops.c \
		sockcommon_subr.c \
		sockcommon_vnops.c \
		sockfilter.c \
		socknotify.c \
		sockparams.c \
		sockstr.c \
		socksubr.c \
		socksyscalls.c \
		socktpi.c \
		sockvfsops.c \
		sodirect.c

CERRWARN =  -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-unused-value
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-parentheses

REPOROOT = ../../..
.include <kmod.mk>

# FIXME: we should remove the generated header during `make clean`

${.CURDIR}/nl7chttp.c: nl7ctokgen.h

nl7ctokgen.h: ${.CURDIR}/nl7ctokreq.txt ${.CURDIR}/nl7ctokres.txt
	/bin/ksh ${.CURDIR}/nl7ctokgen ${.ALLSRC} > $@
