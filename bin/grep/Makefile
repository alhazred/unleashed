PROG=	grep
SRCS=	file.c grep.c queue.c util.c

SRCS+=	fastmatch.c hashtable.c tre-compile.c tre-fastmatch.c

.PATH: ${.CURDIR}/regex
CFLAGS+=-I${.CURDIR}/regex

CFLAGS+= --param max-inline-insns-single=500 -std=gnu99

LINKS=	${BINDIR}/${PROG} ${BINDIR}/egrep \
	${BINDIR}/${PROG} ${BINDIR}/fgrep \
	${BINDIR}/${PROG} ${BINDIR}/rgrep \
	${BINDIR}/${PROG} ${BINDIR}/zgrep \
	${BINDIR}/${PROG} ${BINDIR}/zegrep \
	${BINDIR}/${PROG} ${BINDIR}/zfgrep \
	${BINDIR}/${PROG} ${BINDIR}/xzgrep \
	${BINDIR}/${PROG} ${BINDIR}/xzegrep \
	${BINDIR}/${PROG} ${BINDIR}/xzfgrep \
	${BINDIR}/${PROG} ${BINDIR}/lzgrep \
	${BINDIR}/${PROG} ${BINDIR}/lzegrep \
	${BINDIR}/${PROG} ${BINDIR}/lzfgrep \
	${BINDIR}/${PROG} ${BINDIR}/bzgrep \
	${BINDIR}/${PROG} ${BINDIR}/bzegrep \
	${BINDIR}/${PROG} ${BINDIR}/bzfgrep

LDFLAGS+=	-lz -llzma -lbz2

.include <prog.mk>
