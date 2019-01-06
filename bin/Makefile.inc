BINDIR?=	/usr/bin

.ifdef DESTDIR
LIBCRT0=	${DESTDIR}/usr/lib/amd64/crt1.o
LIBCRTBEGIN=	${DESTDIR}/usr/lib/amd64/crti.o
LIBCRTEND=	${DESTDIR}/usr/lib/amd64/crtn.o
.endif

.include <unleashed.mk>

MAPFILES=	${SRCTOP}/usr/src/common/mapfiles/common
_PROGLDOPTS=	-Wl,-Bdirect
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.pagealign
UNLEASHED_OBJ?=	/usr/obj/${MACHINE}
