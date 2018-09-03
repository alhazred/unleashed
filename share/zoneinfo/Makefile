.include <unleashed.mk>

MAN=	tzfile.5

CONTRIB_PATH=	${SRCTOP}/contrib/tzcode
CONTRIBDIR=	${SRCTOP}/contrib/tzdata

ZONEINFODIR=	${SHAREDIR}/zoneinfo

# "etcetera" must come before "backward" because "backward" depends on
# "etcetera"'s files
SRCFILES=	africa \
		antarctica \
		asia \
		australasia \
		europe \
		etcetera \
		factory \
		northamerica \
		pacificnew \
		southamerica \
		backward

TABFILES=	iso3166.tab \
		zone.tab \
		zone1970.tab

all:

proginstall:
	[ -d ${DESTDIR}${ZONEINFODIR} ] || \
	    ${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}${ZONEINFODIR}
	/usr/sbin/zic -d ${DESTDIR}${ZONEINFODIR} ${SRCFILES:%=${CONTRIBDIR}/%}
	ln -sf ./US/Eastern ${DESTDIR}${ZONEINFODIR}/posixrules
.for f in ${TABFILES}
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m 644 \
	    ${CONTRIBDIR}/${f} ${DESTDIR}${ZONEINFODIR}
.endfor

${.CURDIR}/tzfile.5: ${CONTRIB_PATH}/tzfile.5
	sed \
	  -e 's@/usr/local/etc@/usr/share/lib@' \
	  -e 's@newctime(3), @@' \
	  -e 's@newtzset(3), @@' \
	  -e 's@zdump(8), @@' \
	  < ${.ALLSRC} > $@

# FIXME: This is a gross hack to generate a cleaner version of the manpage
# in the current directory so that the install target finds it and installs
# it.
all: ${.CURDIR}/tzfile.5

CLEANFILES+=	tzfile.5

.include <prog.mk>
