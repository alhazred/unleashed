BINDIR?=	/usr/bin

.if defined(PROG)
MAPFILES=	${.CURDIR:H:H}/usr/src/common/mapfiles/common
_PROGLDOPTS=	-Wl,-Bdirect
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.pagealign
BUILDVERSION!=	git describe --long --all HEAD | cut -d/ -f2-
.endif
