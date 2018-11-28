SUBDIR=	libc libcrypto libdemangle-sys libedit libncurses libssl libtls libuutil

.include "${.CURDIR:H}/cfgparam.mk"

.ifdef DESTDIR
# hack to build both 32/64 bit libs
build:
	${.MAKE} MACHINE=${CONFIG_MACH64} obj
	${.MAKE} MACHINE=${CONFIG_MACH64}
	${.MAKE} MACHINE=${CONFIG_MACH64} install
	${.MAKE} MACHINE=${CONFIG_MACH32} obj
	${.MAKE} MACHINE=${CONFIG_MACH32}
	${.MAKE} MACHINE=${CONFIG_MACH32} install
.endif

# hack to build certain libs as 64-bit only
.if "${MACHINE}" == "${CONFIG_MACH64}"
SUBDIR += libpcap
.endif

.include <subdir.mk>
