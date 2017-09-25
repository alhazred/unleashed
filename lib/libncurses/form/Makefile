.include "${.CURDIR:H}/config.mk"

SRCDIR=	${NCURSES_DIR}/form

LIB=	form

.PATH: ${SRCDIR}
SRCS=	\
	ncurses_def.h \
	fld_arg.c \
	fld_attr.c \
	fld_current.c \
	fld_def.c \
	fld_dup.c \
	fld_ftchoice.c \
	fld_ftlink.c \
	fld_info.c \
	fld_just.c \
	fld_link.c \
	fld_max.c \
	fld_move.c \
	fld_newftyp.c \
	fld_opts.c \
	fld_pad.c \
	fld_page.c \
	fld_stat.c \
	fld_type.c \
	fld_user.c \
	frm_cursor.c \
	frm_data.c \
	frm_def.c \
	frm_driver.c \
	frm_hook.c \
	frm_opts.c \
	frm_page.c \
	frm_post.c \
	frm_req_name.c \
	frm_scale.c \
	frm_sub.c \
	frm_user.c \
	frm_win.c \
	fty_alnum.c \
	fty_alpha.c \
	fty_enum.c \
	fty_generic.c \
	fty_int.c \
	fty_ipv4.c \
	fty_num.c \
	fty_regex.c

CLEANFILES=	ncurses_def.h

BSDOBJDIR?=     ${.OBJDIR:tA:H:H}
CFLAGS+=	-I${NCURSES_SRC}
CFLAGS+=	-I${NCURSES_DIR}/menu
CFLAGS+=	-I${.CURDIR:H}/ncurses
CFLAGS+=	-I${BSDOBJDIR}/ncurses/ncurses

LDADD+= -L${BSDOBJDIR}/ncurses/ncurses -lncurses
SHLIB_LDADD+=  ${LDADD}

HEADERS=	form.h

includes:
	@test -d ${DESTDIR}/usr/include/ncurses || \
		mkdir ${DESTDIR}/usr/include/ncurses
	@for i in $(HEADERS); do \
	j="${INSTALL} ${INSTALL_COPY} ${INC_INSTALL_OWN} -m 0644 ${NCURSES_DIR}/form/$$i \
		${DESTDIR}/usr/include/ncurses"; \
	echo $$j; \
	eval "$$j"; \
	done;

pc_files:
	${INSTALL} ${INSTALL_COPY} -o ${SHAREOWN} -g ${SHAREGRP} \
		-m 0644 ${NCURSES_DIR}/form/form.pc ${DESTDIR}/usr/lib/pkgconfig/


SHLIB_LDADD+=   -M${SRCDIR}/mapfile
realbuild: ${MAPFILE}

.ifdef DESTDIR
afterinstall: includes pc_files
.endif
.include <bsd.lib.mk>
