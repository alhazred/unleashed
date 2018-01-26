FILES=	README auto.dep.mk autoconf.mk autodep.mk config.mk \
	cython.mk dep.mk dirdeps.mk doc.mk dpadd.mk final.mk gendirdeps.mk \
	host-target.mk host.libnames.mk inc.mk init.mk install-new.mk \
	java.mk kmod.mk lib.mk libnames.mk libs.mk links.mk man.mk \
	manifest.mk nls.mk obj.mk options.mk own.mk prlist.mk prog.mk \
	progs.mk rst2htm.mk scripts.mk subdir.mk sys.clean-env.mk \
       	sys.debug.mk sys.dependfile.mk sys.mk sys.vars.mk target-flags.mk \
	warnings.mk whats.mk yacc.mk unleashed.mk files.mk
NOOBJ=

SYMLINKS=	dep.mk ${SHAREDIR}/mk/bsd.dep.mk \
		doc.mk ${SHAREDIR}/mk/bsd.doc.mk \
		init.mk ${SHAREDIR}/mk/bsd.init.mk \
		lib.mk ${SHAREDIR}/mk/bsd.lib.mk \
		links.mk ${SHAREDIR}/mk/bsd.links.mk \
		man.mk ${SHAREDIR}/mk/bsd.man.mk \
		nls.mk ${SHAREDIR}/mk/bsd.nls.mk \
		obj.mk ${SHAREDIR}/mk/bsd.obj.mk \
		own.mk ${SHAREDIR}/mk/bsd.own.mk \
		prog.mk ${SHAREDIR}/mk/bsd.prog.mk \
		subdir.mk ${SHAREDIR}/mk/bsd.subdir.mk 

proginstall:
	${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}${SHAREDIR}/mk
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m ${NONBINMODE} \
	    ${FILES} ${DESTDIR}${SHAREDIR}/mk

.include <prog.mk>
