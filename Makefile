SUBDIR = bin \
	 etc \
	 include \
	 kernel \
	 lib \
	 share

.-include "cfgparam.mk"

.ifdef CONFIG_MACH64
build:
	${.MAKE} obj
	${.MAKE}
	${.MAKE} install
	${.MAKE} -C lib MACHINE=${CONFIG_MACH64} obj
	${.MAKE} -C lib MACHINE=${CONFIG_MACH64}
	${.MAKE} -C lib MACHINE=${CONFIG_MACH64} install

.include <unleashed.mk>
.endif

.include <subdir.mk>

#
# Config related support
#

.if !empty(BUILD_ARCH)
CFGARCH=${BUILD_ARCH}
.elif ${MACHINE} == "i86pc" || ${MACHINE} == "i386" || ${MACHINE} == "amd64"
CFGARCH=x86
.elif ${MACHINE} == "sparc"
CFGARCH=sparc
.else
.error "Unknown machine architecture ${MACHINE}; override it via BUILD_ARCH"
.endif

CFGFILE=arch/${CFGARCH}/Sconfig

gen-config:
	${.MAKE} -C tools obj
	${.MAKE} -C tools
	${.CURDIR}/tools/mkconfig/obj/mkconfig -I _SYS_CFGPARAM_H -H -o include/sys/cfgparam.h ${CFGFILE}
	${.CURDIR}/tools/mkconfig/obj/mkconfig -m -o usr/src/Makefile.cfgparam ${CFGFILE}
	${.CURDIR}/tools/mkconfig/obj/mkconfig -M -o cfgparam.mk ${CFGFILE}

.PHONY: gen-config build
