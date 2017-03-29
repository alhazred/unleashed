MK_ARCHIVE?=	no
MK_PROFILE?=	no
MK_PICLIB?=	no

.if ${MACHINE} == "amd64"
CFLAGS+=	-m64
LDFLAGS+=	-m64
AFLAGS+=	-m64
LIBDIR?=	${libprefix}/lib/amd64
.elif ${MACHINE} == "i86pc"
MACHINE=	i386
.endif
