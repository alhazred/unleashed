BINDIR?=	/usr/bin

.include <unleashed.mk>

MAPFILES=	${SRCTOP}/usr/src/common/mapfiles/common
_PROGLDOPTS=	-Wl,-Bdirect
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.pagealign
BUILDVERSION!=	cd ${SRCTOP} && git describe --long --all HEAD | cut -d/ -f2-
.if make(obj) || make(objlink)
MAKEOBJDIRPREFIX?=      /usr/obj/${MACHINE}
.endif
