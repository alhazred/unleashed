MK_ARCHIVE?=	no
MK_PROFILE?=	no
MK_PICLIB?=	no

.if ${MACHINE} == "amd64"
LIBDIR?=	${libprefix}/lib/amd64
.elif ${MACHINE} == "i386"
CFLAGS+=	-m32
LDFLAGS+=	-m32
AFLAGS+=	-m32
.endif
