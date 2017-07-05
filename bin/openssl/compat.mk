.include <unleashed.mk>
LCRYPTO_SRC=	${SRCTOP}/lib/libcrypto
CPPFLAGS+=	-I${LCRYPTO_SRC}/compat/include
SRCS+=		strtonum.c
.PATH:		${LCRYPTO_SRC}/compat
