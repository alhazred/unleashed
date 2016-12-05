OBJMACHINE=	1
MK_AUTO_OBJ=	yes
MK_ARCHIVE=	no
MK_PROFILE=	no

.if ${MACHINE} == "i86pc"
all clean cleandir install: .MAKE
	${.MAKE} MACHINE=i386 ${.TARGET}
	${.MAKE} MACHINE=amd64 ${.TARGET}
.elif ${MACHINE} == "amd64"
CFLAGS+=	-m64
LDFLAGS+=	-m64
LIBDIR?=	${libprefix}/lib/amd64
.include <lib.mk>
.elif ${MACHINE} == "i386"
.include <lib.mk>
.endif
