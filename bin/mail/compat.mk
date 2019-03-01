COPTS+=	-I${.CURDIR}/compat/include
SRCS+=	compat/pwcache.c

afterinstall:
	${INSTALL} -d ${DESTDIR}/usr/share/misc ${DESTDIR}/etc
	cd ${.CURDIR}/misc; ${INSTALL} ${COPY} ${SHARE_INSTALL_OWN} -m ${NONBINMODE} \
		${SFILES} ${DESTDIR}/usr/share/misc
	cd ${.CURDIR}/misc; ${INSTALL} ${COPY} ${SHARE_INSTALL_OWN} -m 644 \
		${EFILES} ${DESTDIR}/etc
