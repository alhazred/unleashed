PROG=	mandoc

SRCS=	mandoc_aux.c mandoc_ohash.c mandoc.c mandoc_xr.c \
	chars.c preconv.c read.c \
	roff.c roff_validate.c tbl.c tbl_opts.c tbl_layout.c tbl_data.c eqn.c
SRCS+=	mdoc_macro.c mdoc.c \
	mdoc_argv.c mdoc_state.c mdoc_validate.c att.c msec.c st.c
SRCS+=	man_macro.c man.c man_validate.c
SRCS+=	main.c out.c tag.c tree.c
SRCS+=	term.c term_tab.c term_ascii.c term_ps.c
SRCS+=	roff_term.c mdoc_term.c man_term.c eqn_term.c tbl_term.c
SRCS+=	mdoc_man.c
SRCS+=	html.c roff_html.c mdoc_html.c man_html.c eqn_html.c tbl_html.c
SRCS+=	mdoc_markdown.c
SRCS+=	dbm_map.c dbm.c dba_write.c dba_array.c dba.c dba_read.c
SRCS+=	manpath.c mandocdb.c mansearch.c

SRCS+=	lib.c compat_ohash.c

LDADD+=	-lz
CFLAGS+=	-std=gnu99

MAN=	apropos.1 man.1 mandoc.1 man.conf.5 mandoc.db.5 eqn.7 man.7 \
	mandoc_char.7 mdoc.7 roff.7 tbl.7 makewhatis.8
LINKS=	${BINDIR}/mandoc ${BINDIR}/apropos \
	${BINDIR}/mandoc ${BINDIR}/man \
	${BINDIR}/mandoc ${BINDIR}/whatis \
	${BINDIR}/mandoc /usr/sbin/makewhatis

afterinstall:
	@mkdir -p ${DESTDIR}/lib/svc/manifest/system
	${INSTALL} -m 0444 ${.CURDIR}/makewhatis.xml ${DESTDIR}/lib/svc/manifest/system

.include <prog.mk>
