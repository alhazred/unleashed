#	$OpenBSD: Makefile.inc,v 1.4 2015/11/23 09:14:44 nicm Exp $

COPTS+=	-Werror-implicit-function-declaration

UNLEASHED_OBJ?=	/usr/obj/${MACHINE}
# we don't have this tcsetattr flag
COPTS+=		-DTCSASOFT=0
# timespeccmp
COPTS+=		-I${.CURDIR:H}/compat/include
# pledge
LCRYPTO_SRC=	${SRCTOP}/lib/libcrypto
COPTS+=		-I${LCRYPTO_SRC}/compat/include
# XXX should move this to sys.mk
SHAREDIR?=	/usr/share
.include <unleashed.mk>
.include "../Makefile.inc"
