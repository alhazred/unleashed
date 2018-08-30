BINDIR?=	/usr/bin
CFLAGS+=	-m64
LDFLAGS+=	-m64
AFLAGS+=	-m64
LIBDIR=		/usr/lib/amd64

.ifdef DESTDIR
LIBCRT0=	${DESTDIR}/usr/lib/amd64/crt1.o
LIBCRTBEGIN=	${DESTDIR}/usr/lib/amd64/crti.o
LIBCRTEND=	${DESTDIR}/usr/lib/amd64/crtn.o
.endif

.include <unleashed.mk>

MAPFILES=	${SRCTOP}/usr/src/common/mapfiles/common
_PROGLDOPTS=	-Wl,-Bdirect
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.pagealign
BUILDVERSION!=	cd ${SRCTOP} && git describe HEAD
UNLEASHED_OBJ?=	/usr/obj/${MACHINE}
