.include <unleashed.mk>
LCRYPTO_SRC=	${SRCTOP}/lib/libcrypto
# pledge
CPPFLAGS+=	-I${LCRYPTO_SRC}/compat/include
