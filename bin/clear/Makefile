.include <unleashed.mk>

NCURSES_DIR=	${SRCTOP}/contrib/ncurses
PROGSDIR=	${NCURSES_DIR}/progs
.PATH:		${PROGSDIR}

PROG=		clear

CFLAGS+=	-D_XOPEN_SOURCE_EXTENDED
CFLAGS+=	-DENABLE_WIDEC
CFLAGS+=	-DSET_NCURSES_CH_T=cchar_t
CFLAGS+=	-DSET_NEED_WCHAR_H=1
CFLAGS+=	-I${PROGSDIR}
CFLAGS+=	-I.
CFLAGS+=        -I${NCURSES_DIR}/include
CFLAGS+=	-I${.CURDIR}
CFLAGS+=	-I${.OBJDIR}

LDADD=          -lncurses

# TODO: build libncurses first
libs:
	${.MAKE} -C ${SRCTOP}/lib/libncurses/ncurses


clear.o: ${PROGSDIR}/clear.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/clear.c -o $@

clear_cmd.o: ${PROGSDIR}/clear_cmd.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/clear_cmd.c -o $@

tty_settings.o: ${PROGSDIR}/tty_settings.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/tty_settings.c -o $@

SRCS+=	clear.c clear_cmd.c tty_settings.c

#all: libs

.include <bsd.prog.mk>
