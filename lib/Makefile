SUBDIR=	libc libcrypto libedit libncurses libpcap libssl libtls libuutil

.ifdef DESTDIR
# hack to build both 32/64 bit libs
.include "${.CURDIR:H}/cfgparam.mk"
build:
	${.MAKE} MACHINE=${CONFIG_MACH64} obj
	${.MAKE} MACHINE=${CONFIG_MACH64}
	${.MAKE} MACHINE=${CONFIG_MACH64} install
	${.MAKE} obj
	${.MAKE}
	${.MAKE} install
.endif

.include <subdir.mk>
