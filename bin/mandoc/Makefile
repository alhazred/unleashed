PROG=	mandoc

SRCS=	mandoc.c mandoc_aux.c mandoc_ohash.c preconv.c read.c \
	roff.c tbl.c tbl_opts.c tbl_layout.c tbl_data.c eqn.c
SRCS+=	mdoc_macro.c mdoc.c mdoc_hash.c \
        mdoc_argv.c mdoc_state.c mdoc_validate.c att.c msec.c st.c
SRCS+=	man_macro.c man.c man_hash.c man_validate.c
SRCS+=	main.c mdoc_term.c tag.c chars.c term.c tree.c man_term.c eqn_term.c
SRCS+=	mdoc_man.c
SRCS+=	html.c mdoc_html.c man_html.c out.c eqn_html.c
SRCS+=	term_ps.c term_ascii.c tbl_term.c tbl_html.c
SRCS+=	dbm_map.c dbm.c dba_write.c dba_array.c dba.c dba_read.c
SRCS+=	manpath.c mandocdb.c mansearch.c

SRCS+=	lib.c compat_ohash.c compat_strtonum.c

LDADD+=	-lz
CPPFLAGS+=	-DOSNAME='"unleashed"'
CFLAGS+=	-std=gnu99

# FIXME shouldn't need these hacks
CPPFLAGS+=	-D_TS_ERRNO -I/usr/include

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
