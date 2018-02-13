FILES=	.kshrc \
	.profile
NOOBJ=

NONBINMODE=644

proginstall:
	${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}/etc/skel
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m ${NONBINMODE} \
	    ${FILES} ${DESTDIR}/etc/skel
	# also copy .profile to /root
	# FIXME: this doesn't really belong here
	${INSTALL} -d ${PROG_INSTALL_OWN} -m 700 ${DESTDIR}/root
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m ${NONBINMODE} \
	    .profile ${DESTDIR}/root

.include <prog.mk>
