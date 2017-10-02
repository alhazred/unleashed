.include <unleashed.mk>

NCURSES_DIR=	${SRCTOP}/contrib/ncurses
PROGSDIR=	${NCURSES_DIR}/progs
.PATH:		${PROGSDIR}

PROG=		tput

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

tput.o: ${PROGSDIR}/tput.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tput.c -o $@

transform.o: ${PROGSDIR}/transform.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/transform.c -o $@

tty_settings.o: ${PROGSDIR}/tty_settings.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tty_settings.c -o $@

reset_cmd.o: ${PROGSDIR}/reset_cmd.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/reset_cmd.c -o $@

clear_cmd.o: ${PROGSDIR}/clear_cmd.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/clear_cmd.c -o $@


SRCS+=	tput.c tparm_type.c transform.c tty_settings.c reset_cmd.c clear_cmd.c

.include <bsd.prog.mk>
