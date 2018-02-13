FILES=	.login \
	ksh.kshrc \
	profile \
	system
NOOBJ=
SUBDIR=	skel

NONBINMODE=644

proginstall:
	${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}/etc
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m ${NONBINMODE} \
	    ${FILES} ${DESTDIR}/etc
	# copy .bashrc into /root
	# FIXME: this doesn't really belong here
	${INSTALL} -d ${PROG_INSTALL_OWN} -m 700 ${DESTDIR}/root
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m ${NONBINMODE} \
	    .bashrc ${DESTDIR}/root

.include <prog.mk>
