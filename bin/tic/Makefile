.include <unleashed.mk>

NCURSES_DIR=	${SRCTOP}/contrib/ncurses
TERMINFO_CAPS=  ${NCURSES_DIR}/include/Caps
PROGSDIR=	${NCURSES_DIR}/progs
.PATH:		${PROGSDIR}

PROG=		tic

CFLAGS+=	-D_XOPEN_SOURCE_EXTENDED
CFLAGS+=	-DENABLE_WIDEC
CFLAGS+=	-DSET_NCURSES_CH_T=cchar_t
CFLAGS+=	-DSET_NEED_WCHAR_H=1
CFLAGS+=	-I${PROGSDIR}
CFLAGS+=	-I.
CFLAGS+=	-I${NCURSES_DIR}/include
CFLAGS+=	-I${.CURDIR}
CFLAGS+=	-I${.OBJDIR}

LDADD=          -lncurses

ncurses_def.h:
	AWK=awk sh ${NCURSES_DIR}/include/MKncurses_def.sh \
	  ${NCURSESDIR}/include/ncurses_defs > $@

parametrized.h:
	AWK=awk sh ${NCURSES_DIR}/include/MKparametrized.sh \
	  ${TERMINFO_CAPS} > $@

_termsortc=	termsort.c
beforedepend:	termsort.c

${_termsortc}:
	sh ${PROGSDIR}/MKtermsort.sh awk ${TERMINFO_CAPS} > termsort.c

tic.o: ${PROGSDIR}/tic.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tic.c -o $@

transform.o: ${PROGSDIR}/transform.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/transform.c -o $@

dump_entry.o: ${PROGSDIR}/dump_entry.c ${_termsortc}
	${CC} ${CFLAGS} -c ${PROGSDIR}/dump_entry.c -o $@

CLEANFILES=	ncurses_def.h parametrized.h termsort.c clear.dep

SRCS=	ncurses_def.h parametrized.h

SRCS+=	tic.c dump_entry.c tparm_type.c transform.c

afterinstall:
	cd ${DESTDIR}/usr/bin && ln -f tic captoinfo

.include <bsd.prog.mk>
