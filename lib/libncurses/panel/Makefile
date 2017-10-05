.include "${.CURDIR:H}/config.mk"

SRCDIR=	${NCURSES_DIR}/panel

LIB=	panel

.PATH: ${SRCDIR}
SRCS=	\
	ncurses_def.h \
	p_above.c \
	p_below.c \
	p_bottom.c \
	p_delete.c \
	p_hidden.c \
	p_hide.c \
	p_move.c \
	p_new.c \
	p_replace.c \
	p_show.c \
	p_top.c \
	p_update.c \
	p_user.c \
	p_win.c \
	panel.c

CLEANFILES=	ncurses_def.h

BSDOBJDIR?=     ${.OBJDIR:tA:H:H}
CFLAGS+=	-I${SRCDIR}
CFLAGS+=        -I${.CURDIR:H}/ncurses
CFLAGS+=	-I${BSDOBJDIR}/ncurses/ncurses

LDADD+= -L${BSDOBJDIR}/ncurses/ncurses -lncurses
SHLIB_LDADD+=  ${LDADD}

INCS=	${SRCDIR}/panel.h

includes: incinstall

pc_files:
	${INSTALL} ${INSTALL_COPY} -o ${SHAREOWN} -g ${SHAREGRP} \
		-m 0644 ${NCURSES_DIR}/panel/panel.pc ${DESTDIR}/usr/lib/pkgconfig/


SHLIB_LDADD+=   -M${SRCDIR}/mapfile
realbuild: ${MAPFILE}


.ifdef DESTDIR
afterinstall: includes pc_files
.endif
.include <bsd.lib.mk>
