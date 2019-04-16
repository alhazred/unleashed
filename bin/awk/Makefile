.include <unleashed.mk>

AWKSRC=	${SRCTOP}/contrib/awk
.PATH: ${AWKSRC}

PROG=	awk
SRCS=	awkgram.y b.c lex.c lib.c main.c parse.c proctab.c run.c tran.c ytab.h

CFLAGS+= -DHAS_ISBLANK -I. -I${AWKSRC}

LDADD=	-lm

LINKS=	${BINDIR}/awk ${BINDIR}/nawk

CLEANFILES= awkgram.c maketab.dep maketab proctab.c ytab.h

BUILDFIRST = ytab.h

ytab.h: awkgram.c
	mv y.tab.h ${.TARGET}

proctab.c: maketab
	./maketab > ${.TARGET}

build-tools: maketab
maketab: ytab.h maketab.c

.include <bsd.prog.mk>
