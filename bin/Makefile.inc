BINDIR?=	/usr/bin
COPTS+=		-Werror-implicit-function-declaration

.ifdef DESTDIR
LIBCRT0=	${DESTDIR}/usr/lib/amd64/crt1.o
LIBCRTBEGIN=	${DESTDIR}/usr/lib/amd64/crti.o
LIBCRTEND=	${DESTDIR}/usr/lib/amd64/crtn.o
.endif

.include <unleashed.mk>

UNLEASHED_OBJ?=	/usr/obj/${MACHINE}
