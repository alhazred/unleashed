BINDIR?=	/usr/bin

MAPFILES=	${.CURDIR:H:H}/usr/src/common/mapfiles/common
_PROGLDOPTS=	-Wl,-Bdirect
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.noexstk
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.pagealign
.if ${MACHINE_ARCH} != "sparc"
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.noexdata
.endif
.if defined(PROG) && !target(beforeinstall)
BUILDVERSION!=	git describe --long --all HEAD | cut -d/ -f2-
beforeinstall:
	mcs -d -a 'illumos-unleashed ${BUILDVERSION}' ${PROG}
.endif
