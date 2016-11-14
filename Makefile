SUBDIR = bin \
	 lib \
	 share \
	 kernel

.include <bsd.subdir.mk>

#
# Config related support
#

.if !empty(BUILD_ARCH)
CFGARCH=${BUILD_ARCH}
.elif ${MACHINE} == "i86pc"
CFGARCH=x86
.elif ${MACHINE} == "sparc"
CFGARCH=sparc
.else
.error "Unknown machine architecture ${MACHINE}; override it via BUILD_ARCH"
.endif

CFGFILE=kernel/arch/${CFGARCH}/Sconfig

gen-config:
	${.MAKE} -C tools
	${.CURDIR}/tools/mkconfig/mkconfig -I _SYS_CFGPARAM_H -H -o usr/src/uts/common/sys/cfgparam.h ${CFGFILE}
	${.CURDIR}/tools/mkconfig/mkconfig -m -o usr/src/Makefile.cfgparam ${CFGFILE}
	${.CURDIR}/tools/mkconfig/mkconfig -M -o Makefile.cfgparam ${CFGFILE}

.PHONY: gen-config
