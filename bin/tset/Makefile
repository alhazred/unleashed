.include <unleashed.mk>

NCURSES_DIR=	${SRCTOP}/contrib/ncurses
PROGSDIR=	${NCURSES_DIR}/progs
.PATH:		${PROGSDIR}

PROG=		tset

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

tset.o: ${PROGSDIR}/tset.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tset.c -o $@

transform.o: ${PROGSDIR}/transform.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/transform.c -o $@

tty_settings.o: ${PROGSDIR}/tty_settings.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tty_settings.c -o $@

reset_cmd.o: ${PROGSDIR}/reset_cmd.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/reset_cmd.c -o $@


SRCS+=	tset.c transform.c tty_settings.c reset_cmd.c

afterinstall:
	cd ${DESTDIR}/usr/bin && ln -f tset reset


.include <bsd.prog.mk>
