.include <unleashed.mk>
CPPFLAGS+=	-D__dead= -D__unused=
LCRYPTO_SRC=	${SRCTOP}/lib/libcrypto
# pledge
COPTS+=		-I${LCRYPTO_SRC}/compat/include
