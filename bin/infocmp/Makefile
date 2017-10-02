.include <unleashed.mk>

NCURSES_DIR=	${SRCTOP}/contrib/ncurses
TERMINFO_CAPS=  ${NCURSES_DIR}/include/Caps
PROGSDIR=	${NCURSES_DIR}/progs
.PATH:		${PROGSDIR}

PROG=		infocmp

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

_termsortc=	termsort.c
beforedepend:	termsort.c

${_termsortc}:
	sh ${PROGSDIR}/MKtermsort.sh awk ${TERMINFO_CAPS} > termsort.c

infocmp.o: ${PROGSDIR}/infocmp.c
	${CC} ${CFLAGS} -c ${PROGSDIR}/infocmp.c -o $@

dump_entry.o: ${PROGSDIR}/dump_entry.c ${_termsortc}
	${CC} ${CFLAGS} -c ${PROGSDIR}/dump_entry.c -o $@

CLEANFILES=	termsort.c

SRCS+=	infocmp.c dump_entry.c

.include <bsd.prog.mk>
