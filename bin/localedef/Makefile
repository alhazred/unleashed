PROG=	localedef
SRCS=	parser.y \
	charmap.c \
	collate.c \
	ctype.c \
	localedef.c \
	messages.c \
	monetary.c \
	numeric.c \
	scanner.c \
	time.c \
	wide.c

CPPFLAGS+=	-I${.CURDIR:H:H}/usr/src/lib/libc/port/locale
LDADD=		-lavl -lgen

${SRCS:M*.c}: parser.h
parser.h: parser.y

.include <yacc.mk>
.include <prog.mk>
