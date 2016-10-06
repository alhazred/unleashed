SUBDIR=	bin

.include <bsd.subdir.mk>

#
# Config related support
#

CFGINTEL=config/intel
CFGSPARC=config/sparc

gen-config:
	${.MAKE} -C tools
	${.CURDIR}/tools/mkconfig/mkconfig -I _SYS_CFGPARAM_H -H -o usr/src/uts/intel/sys/cfgparam.h $(CFGINTEL)
	${.CURDIR}/tools/mkconfig/mkconfig -I _SYS_CFGPARAM_H -H -o usr/src/uts/sparc/sys/cfgparam.h $(CFGSPARC)
	${.CURDIR}/tools/mkconfig/mkconfig -m -o usr/src/Makefile.cfgparam.intel $(CFGINTEL)
	${.CURDIR}/tools/mkconfig/mkconfig -m -o usr/src/Makefile.cfgparam.sparc $(CFGSPARC)
	${.CURDIR}/tools/mkconfig/mkconfig -M -o Makefile.cfgparam.intel $(CFGINTEL)
	${.CURDIR}/tools/mkconfig/mkconfig -M -o Makefile.cfgparam.sparc $(CFGSPARC)
