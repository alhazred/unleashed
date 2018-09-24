SUBDIR=	libc libcrypto libedit libncurses libssl libtls libuutil

.include "${.CURDIR:H}/cfgparam.mk"

.ifdef DESTDIR
# hack to build both 32/64 bit libs
build:
	${.MAKE} MACHINE=${CONFIG_MACH64} obj
	${.MAKE} MACHINE=${CONFIG_MACH64}
	${.MAKE} MACHINE=${CONFIG_MACH64} install
	${.MAKE} obj
	${.MAKE}
	${.MAKE} install
.endif

# hack to build certain libs as 64-bit only
.if "${MACHINE}" == "${CONFIG_MACH64}"
SUBDIR += libpcap
.endif

.include <subdir.mk>
