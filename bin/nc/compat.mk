.include <unleashed.mk>
LCRYPTO_SRC=	${SRCTOP}/lib/libcrypto
CPPFLAGS+=	-I${.CURDIR}/compat/include -I${LCRYPTO_SRC}/compat/include
SRCS+=		readpassphrase.c strtonum.c base64.c
.PATH:		${.CURDIR}/compat ${LCRYPTO_SRC}/compat
# ssl, crypto are only needed indirectly and cause check_rtime warnings, so
# override LDADD here
LDADD=		-ltls
