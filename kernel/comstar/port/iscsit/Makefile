MODULE=		iscsit
MODULE_TYPE=	drv
MODULE_DEPS=	drv/stmf \
		misc/idm \
		fs/sockfs \
		misc/md5 \
		misc/ksocket
MODULE_CONF=	iscsit.conf
SRCS=		iscsit.c \
		iscsit_auth.c \
		iscsit_authclient.c \
		iscsit_isns.c \
		iscsit_login.c \
		iscsit_radiusauth.c \
		iscsit_radiuspacket.c \
		iscsit_sess.c \
		iscsit_text.c \
		iscsit_tgt.c \
		iscsit_common.c
SRCS_DIRS=	${REPOROOT}/usr/src/common/iscsit

CERRWARN =  -Wno-switch
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-label

REPOROOT = ${.CURDIR:H:H:H:H}
.include <kmod.mk>
