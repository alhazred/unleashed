.include <unleashed.mk>

NCURSES_DIR=	${SRCTOP}/contrib/ncurses
PROGSDIR=	${NCURSES_DIR}/progs
.PATH:		${PROGSDIR}

PROG=		toe

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

toe.o: ${PROGSDIR}/toe.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/toe.c -o $@

SRCS+=	toe.c

.include <bsd.prog.mk>
