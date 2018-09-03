PROG=	tzselect
SRCS=
MAN=	tzselect.8

CONTRIB_PATH = ${.CURDIR:H:H}/contrib/tzcode

VERSION!=cat ${CONTRIB_PATH}/version

.PATH: ${CONTRIB_PATH}

tzselect: tzselect.ksh
	sed \
		-e 's|#!/bin/bash|#!/bin/sh|g' \
		-e 's|AWK=[^}]*|AWK=/usr/bin/awk|g' \
		-e 's|\(PKGVERSION\)=.*|\1='\''(tzcode) '\''|' \
		-e 's|\(REPORT_BUGS_TO\)=.*|\1=tz@iana.org|' \
		-e 's|TZDIR=[^}]*|TZDIR=/usr/share/zoneinfo|' \
		-e 's|\(TZVERSION\)=.*|\1='"$$VERSION"'|' \
		< ${.ALLSRC} >$@

${.CURDIR}/tzselect.8: ${CONTRIB_PATH}/tzselect.8
	sed \
	  -e 's@/usr/local/etc@/usr/share/lib@' \
	  -e 's@newctime(3), @@' \
	  -e 's@zdump(8), @@' \
	  < ${.ALLSRC} > $@

# FIXME: This is a gross hack to generate a cleaner version of the manpage
# in the current directory so that the install target finds it and installs
# it.
all: ${.CURDIR}/tzselect.8

CLEANFILES+=	tzselect.8

.include <unleashed.mk>
.include <prog.mk>
