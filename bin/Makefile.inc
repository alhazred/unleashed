BINDIR?=	/usr/bin

.include <unleashed.mk>

MAPFILES=	${SRCTOP}/usr/src/common/mapfiles/common
_PROGLDOPTS=	-Wl,-Bdirect
_PROGLDOPTS+=	-Wl,-M${MAPFILES}/map.pagealign
BUILDVERSION!=	cd ${SRCTOP} && git describe HEAD
UNLEASHED_OBJ?=	/usr/obj/${MACHINE}
