BINDIR?=	/usr/bin

.if defined(PROG)
MAPFILES=	${.CURDIR:H:H}/usr/src/common/mapfiles/common
_PROGLDOPTS=	-Wl,-Bdirect
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.noexstk
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.pagealign
.if ${MACHINE_ARCH} != "sparc"
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.noexdata
.endif
BUILDVERSION!=	git describe --long --all HEAD | cut -d/ -f2-
.endif
