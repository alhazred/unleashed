PROG=	zic
SRCS=	zic.c
MAN=	zic.8

BINDIR=	/usr/sbin

CONTRIB_PATH = ${.CURDIR:H:H}/contrib/tzcode

VERSION!=cat ${CONTRIB_PATH}/version

.PATH: ${CONTRIB_PATH}

CPPFLAGS+=	-I${CONTRIB_PATH}
CPPFLAGS+=	-I.
CPPFLAGS+=	-DTZDIR='"/usr/share/zoneinfo"'

version.h:
	printf '%s\n' \
	  'static char const PKGVERSION[]="(tzcode) ";' \
	  "static char const TZVERSION[]=\"$$VERSION\";" \
	  'static char const REPORT_BUGS_TO[]="tz@iana.org";' \
	  >$@.out
	mv $@.out $@

${.CURDIR}/zic.8: ${CONTRIB_PATH}/zic.8
	sed \
	  -e 's@/usr/local/etc@/usr/share/lib@' \
	  -e 's@, zdump(8)@@' \
	  < ${.ALLSRC} > $@

# FIXME: This is a gross hack to generate a cleaner version of the manpage
# in the current directory so that the install target finds it and installs
# it.
all: ${.CURDIR}/zic.8

CLEANFILES+=	version.h zic.8

zic.o: version.h

.include <unleashed.mk>
.include <prog.mk>
