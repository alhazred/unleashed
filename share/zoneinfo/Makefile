CONTRIBDIR=	${SRCTOP}/contrib/tzdata

ZONEINFODIR=	${SHAREDIR}/lib/zoneinfo

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
		zone.tab

all:

# NOTE: The proginstall target does two compatibility related things because
# tzselect expects them:
#
# (1) it installs zone_sun.tab which uses a different format than the
#     upstream zone.tab file
# (2) it installs a country.tab symlink to allow easier migration
proginstall:
	[ -d ${DESTDIR}${ZONEINFODIR} ] || \
	    ${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}${ZONEINFODIR}
	/usr/sbin/zic -d ${DESTDIR}${ZONEINFODIR} ${SRCFILES:%=${CONTRIBDIR}/%}
	ln -sf ./US/Eastern ${DESTDIR}${ZONEINFODIR}/posixrules
	[ -d ${DESTDIR}${ZONEINFODIR}/tab ] || \
	    ${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}${ZONEINFODIR}/tab
.for f in ${TABFILES}
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m 644 \
	    ${CONTRIBDIR}/${f} ${DESTDIR}${ZONEINFODIR}/tab
.endfor
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m 644 \
	    zone_sun.tab ${DESTDIR}${ZONEINFODIR}/tab
	ln -sf iso3166.tab ${DESTDIR}${ZONEINFODIR}/tab/country.tab

.include <unleashed.mk>
.include <prog.mk>
