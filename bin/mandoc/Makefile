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
SRCS+=	manpath.c

SRCS+=	lib.c compat_ohash.c compat_strtonum.c

LDADD+=	-lz
CPPFLAGS+=	-DOSNAME='"illumos"'
CFLAGS+=	-std=gnu99

# FIXME shouldn't need these hacks
CPPFLAGS+=	-D_TS_ERRNO -I/usr/include

MAN=	mandoc.1 eqn.5 mandoc_char.5 mandoc_roff.5 mdoc.5 tbl.5

.include <prog.mk>
