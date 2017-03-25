.if !empty(MAN) && !target(install)
MANDIR=	${DESTDIR}/usr/share/man/${.CURDIR:T}
install:
	${INSTALL} -d ${MAN_INSTALL_OWN} -m ${DIRMODE} ${MANDIR}
	${INSTALL} ${COPY} ${MAN_INSTALL_OWN}  -m ${MANMODE} ${MAN} \
	    ${MANDIR}
.endif
