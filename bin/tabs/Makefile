.include <unleashed.mk>

NCURSES_DIR=	${SRCTOP}/contrib/ncurses
PROGSDIR=	${NCURSES_DIR}/progs
.PATH:		${PROGSDIR}

PROG=		tabs

CFLAGS+=	-D_XOPEN_SOURCE_EXTENDED
CFLAGS+=	-DENABLE_WIDEC
CFLAGS+=	-DSET_NCURSES_CH_T=cchar_t
CFLAGS+=	-DSET_NEED_WCHAR_H=1
CFLAGS+=	-I${PROGSDIR}
CFLAGS+=	-I.
CFLAGS+=	-I${NCURSES_DIR}/include
CFLAGS+=	-I${.CURDIR}
CFLAGS+=	-I${.OBJDIR}

LDADD=          -L${MAKEOBJDIRPREFIX}${SRCTOP}/lib/libncurses/ncurses -lncurses

tabs.o: ${PROGSDIR}/tabs.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tabs.c -o $@

tty_settings.o: ${PROGSDIR}/tty_settings.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tty_settings.c -o $@

SRCS+=	tabs.c tty_settings.c

.include <bsd.prog.mk>
