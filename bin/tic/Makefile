PROG=	tic
SRCS=	tic_captab.c \
	tic_error.c \
	tic_hash.c \
	tic_main.c \
	tic_parse.c \
	tic_read.c \
	tic_scan.c

CPPFLAGS+=	-I${SRCTOP}/usr/src/lib/libcurses/screen
CFLAGS+=	-Wno-char-subscripts
LDADD+=		-lcurses
LDADD+=		-Wl,-Mmapfile-intf

.include <unleashed.mk>
.include <prog.mk>
