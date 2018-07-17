CMP_SRC=	${.CURDIR:H}/cmp
COPTS+=		-I${CMP_SRC}/include
KSH_SRC=	${.CURDIR:H}/ksh
.PATH: ${KSH_SRC}/compat
SRCS=		${PROG}.c signames.c
