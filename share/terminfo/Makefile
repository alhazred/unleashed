.include "Makefile.entries"

NOMAN=

CLEANFILES+=	run_tic.sed run_tic.sh terminfo.tmp termcap
CLEANDIRS+=	terminfo

TINFODIR=	${SHAREDIR}/terminfo
MISCDIR=	${.CURDIR}/../../contrib/ncurses/misc
PSOURCE=	${.CURDIR}/../../contrib/ncurses/progs
TSOURCE=	${MISCDIR}/terminfo.src
CFLAGS+=	-I${PSOURCE} -I${.CURDIR}/../../lib/libncurses/include

all: terminfo.tmp
run_tic.sed:
	WHICH_XTERM=xterm-new \
	ticdir=terminfo \
	sh ${MISCDIR}/gen_edit.sh > $@

terminfo.tmp: run_tic.sed ${TSOURCE}
	sed -f run_tic.sed ${TSOURCE} > $@

#termcap: terminfo.tmp
#	egrep -v '^(#|$$)' terminfo.tmp > $@

# termorarily use gnu tic
install: terminfo.tmp
	TIC_PATH=/usr/bin/tic \
	DESTDIR=${DESTDIR} \
        prefix=${prefix} \
        exec_prefix=${exec_prefix} \
        bindir=${bindir} \
        top_srcdir=${top_srcdir} \
        srcdir=${srcdir} \
        datadir=${datadir} \
        ticdir=${ticdir} \
        source=terminfo.tmp \
        cross_compiling=no \
        sh ${MISCDIR}/run_tic.sh  2>/dev/null
	egrep -v '^(#|$$)' terminfo.tmp > ${DESTDIR}/etc/termcap


#	${INSTALL} ${INSTALL_COPY} -o ${BINOWN} -g sys -m 644 termcap \
#		${DESTDIR}/etc/termcap

.include <bsd.prog.mk>
