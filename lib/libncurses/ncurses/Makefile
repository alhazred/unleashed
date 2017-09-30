.include "${.CURDIR:H}/config.mk"

.NOTPARALLEL:	make_hash comp_captab.c

LIB=	ncurses

GENHDRS= \
	curses.h \
	hashsize.h \
	init_keytry.h \
	ncurses_def.h \
	parametrized.h

GENSRCS= \
	codes.c \
	comp_captab.c \
	expanded.c \
	fallback.c \
	lib_gen.c \
	lib_keyname.c \
	names.c \
	unctrl.c

HEADERS=	curses.h termcap.h unctrl.h ncurses_dll.h term.h

.PATH:  ${.CURDIR}

curses.h: Caps \
		curses.head \
		curses.tail \
		MKkey_defs.sh
	cat ${NCURSES_INC}/curses.head >$@
	AWK=${AWK} ${SHELL} ${NCURSES_INC}/MKkey_defs.sh ${TERMINFO_CAPS} >> $@
	${SHELL} -c 'if test "cchar_t" = "cchar_t" ; then cat ${NCURSES_INC}/curses.wide >> $@ ; fi'
	cat ${NCURSES_INC}/curses.tail >> $@
	#mv -f $@ ${NCURSES_INC}

hashsize.h: MKhashsize.sh Caps
	${SHELL} ${NCURSES_INC}/MKhashsize.sh ${TERMINFO_CAPS} > $@

ncurses_def.h: ncurses_defs MKncurses_def.sh
	AWK=${AWK} ${SHELL} ${NCURSES_INC}/MKncurses_def.sh ${NCURSES_INC}/ncurses_defs > $@

parametrized.h: MKparametrized.sh Caps
	AWK=${AWK} ${SHELL} ${NCURSES_INC}/MKparametrized.sh ${TERMINFO_CAPS} > $@

names.c: MKnames.awk Caps 
	${AWK} -f ${NCURSES_TINFO}/MKnames.awk bigstrings=${USE_BIG_STRINGS} ${TERMINFO_CAPS} >$@

codes.c: MKcodes.awk Caps 
	${AWK} -f ${NCURSES_TINFO}/MKcodes.awk bigstrings=${USE_BIG_STRINGS} ${TERMINFO_CAPS} >$@

fallback.c : MKfallback.sh Caps
	${SHELL} -e ${NCURSES_TINFO}/MKfallback.sh ${TERMINFO} ${TERMINFO_SRC} ${TIC_PATH} >$@

lib_gen.c : MKlib_gen.sh curses.h 
	${SHELL} -e ${NCURSES_BASE}/MKlib_gen.sh "${CPP} ${CFLAGS}" ${AWK} generated < ${NCURSES_INC}/curses.h >$@

init_keytry.h: make_keys keys.list
	env PATH=${BSDOBJDIR}/libncurses/ncurses:${CURDIR}:${PATH} make_keys ${NCURSES_SRC}/keys.list >$@

keys.list: MKkeys_list.sh Caps
	AWK=${AWK} ${SHELL} ${NCURSES_TINFO}/MKkeys_list.sh ${TERMINFO_CAPS} | LC_ALL=C sort >$@

expanded.c: curses.priv.h MKexpanded.sh
	${SHELL} -e ${NCURSES_SERIAL}/MKexpanded.sh "${CPP} ${CFLAGS}" >$@

make_keys: ${GENSRCS} make_keys.c ncurses_def.h
	${CC} -o $@ ${CFLAGS} ${NCURSES_TINFO}/make_keys.c

make_hash: ${HEADERS} \
	make_hash.c \
	hashsize.h \
	ncurses_def.h
	${CC} -o $@ ${CFLAGS} ${NCURSES_TINFO}/make_hash.c

comp_captab.c: ${HEADERS} \
		hashsize.h \
		MKcaptab.sh \
		MKcaptab.awk \
		make_hash
	env PATH=${BSDOBJDIR}/libncurses/ncurses:${CURDIR}:${PATH} ${SHELL} -e ${NCURSES_TINFO}/MKcaptab.sh ${AWK} ${USE_BIG_STRINGS} ${NCURSES_TINFO}/MKcaptab.awk ${TERMINFO_CAPS} >$@

lib_keyname.c: keys.list MKkeyname.awk
	${AWK} -f ${NCURSES_BASE}/MKkeyname.awk bigstrings=${USE_BIG_STRINGS} ${NCURSES_SRC}/keys.list >$@

unctrl.c: MKunctrl.awk
	echo | ${AWK} -f ${NCURSES_BASE}/MKunctrl.awk bigstrings=${USE_BIG_STRINGS} >$@

SRCS=	${GENHDRS} ${GENSRCS}

# Base
SRCS+= \
	define_key.c \
	key_defined.c \
	keybound.c \
	keyok.c \
	legacy_coding.c \
	lib_addch.c \
	lib_addstr.c \
	lib_beep.c \
	lib_bkgd.c \
	lib_box.c \
	lib_chgat.c \
	lib_clear.c \
	lib_clearok.c \
	lib_clrbot.c \
	lib_clreol.c \
	lib_color.c \
	lib_colorset.c \
	lib_delch.c \
	lib_delwin.c \
	lib_dft_fgbg.c \
	lib_echo.c \
	lib_endwin.c \
	lib_erase.c \
	lib_flash.c \
	lib_freeall.c \
	lib_getch.c \
	lib_getstr.c \
	lib_hline.c \
	lib_immedok.c \
	lib_inchstr.c \
	lib_initscr.c \
	lib_insch.c \
	lib_insdel.c \
	lib_insnstr.c \
	lib_instr.c \
	lib_isendwin.c \
	lib_keyname.c \
	lib_leaveok.c \
	lib_mouse.c \
	lib_move.c \
	lib_mvwin.c \
	lib_newterm.c \
	lib_newwin.c \
	lib_nl.c \
	lib_overlay.c \
	lib_pad.c \
	lib_printw.c \
	lib_redrawln.c \
	lib_refresh.c \
	lib_restart.c \
	lib_scanw.c \
	lib_screen.c \
	lib_scroll.c \
	lib_scrollok.c \
	lib_scrreg.c \
	lib_set_term.c \
	lib_slk.c \
	lib_slkatr_set.c \
	lib_slkatrof.c \
	lib_slkatron.c \
	lib_slkatrset.c \
	lib_slkattr.c \
	lib_slkclear.c \
	lib_slkcolor.c \
	lib_slkinit.c \
	lib_slklab.c \
	lib_slkrefr.c \
	lib_slkset.c \
	lib_slktouch.c \
	lib_touch.c \
	lib_ungetch.c \
	lib_vline.c \
	lib_wattroff.c \
	lib_wattron.c \
	lib_winch.c \
	lib_window.c \
	nc_panel.c \
	new_pair.c \
	resizeterm.c \
	safe_sprintf.c \
	wresize.c \
	tries.c \
	use_window.c \
	version.c \
	vsscanf.c \
	wresize.c

# Serial
SRCS+= \
	hardscroll.c \
	hashmap.c \
	lib_mvcur.c \
	lib_tstp.c \
	lib_twait.c \
	lib_vidattr.c \
	tty_update.c

# Tinfo
SRCS+=	access.c \
	add_tries.c \
	alloc_entry.c \
	alloc_ttype.c \
	captoinfo.c \
	comp_error.c \
	comp_expand.c \
	comp_hash.c \
	comp_parse.c \
	comp_scan.c \
	db_iterator.c \
	doalloc.c \
	entries.c \
	free_ttype.c \
	getenv_num.c \
	hashed_db.c \
	home_terminfo.c \
	init_keytry.c \
	lib_acs.c \
	lib_baudrate.c \
	lib_cur_term.c \
	lib_data.c \
	lib_has_cap.c \
	lib_kernel.c \
	lib_longname.c \
	lib_napms.c \
	lib_options.c \
	lib_print.c \
	lib_raw.c \
	lib_setup.c \
	lib_termcap.c \
	lib_termname.c \
	lib_tgoto.c \
	lib_ti.c \
	lib_tparm.c \
	lib_tputs.c \
	lib_ttyflags.c \
	name_match.c \
	obsolete.c \
	parse_entry.c \
	read_entry.c \
	read_termcap.c \
	strings.c \
	trim_sgr0.c \
	use_screen.c \
	write_entry.c

# Trace
SRCS+=	lib_trace.c \
	lib_traceatr.c \
	lib_tracebits.c \
	lib_tracechr.c \
	lib_tracedmp.c \
	lib_tracemse.c \
	trace_buf.c \
	trace_tries.c \
	trace_xnames.c \
	varargs.c \
	visbuf.c \

# Widechar
SRCS+=	charable.c \
	lib_add_wch.c \
	lib_box_set.c \
	lib_cchar.c \
	lib_erasewchar.c \
	lib_get_wch.c \
	lib_get_wstr.c \
	lib_hline_set.c \
	lib_in_wch.c \
	lib_in_wchnstr.c \
	lib_ins_wch.c \
	lib_inwstr.c \
	lib_key_name.c \
	lib_pecho_wchar.c \
	lib_slk_wset.c \
	lib_unget_wch.c \
	lib_vid_attr.c \
	lib_vline_set.c \
	lib_wacs.c \
	lib_wunctrl.c \
	widechars.c


includes: curses.h
	@test -f ${NCURSES_INC}/curses.h || \
		mv -f curses.h ${NCURSES_INC}
	@test -d ${DESTDIR}/usr/include/ncurses || \
		mkdir -p ${DESTDIR}/usr/include/ncurses
	@for i in ${HEADERS}; do \
	j="${INSTALL} ${INSTALL_COPY} ${INC_INSTALL_OWN} -m 0644 ${NCURSES_INC}/$$i \
		${DESTDIR}/usr/include/ncurses"; \
	echo $$j; \
	eval "$$j"; \
	done;
	cd ${DESTDIR}/usr/include/ncurses && ln -fs curses.h ncurses.h

pc_files:
	@test -d ${DESTDIR}/usr/lib/pkgconfig || \
		mkdir -p ${DESTDIR}/usr/lib/pkgconfig
	${INSTALL} ${INSTALL_COPY} -o ${SHAREOWN} -g ${SHAREGRP} \
		-m 0644 ${NCURSES_SRC}/ncurses.pc ${DESTDIR}/usr/lib/pkgconfig/


tabset= std stdcrt vt100 vt300

misc_files:
	${INSTALL} ${INSTALL_COPY} -o ${BINOWN} -g ${BINOWN} \
		-m 555 ${NCURSES_DIR}/misc/ncurses-config ${DESTDIR}/usr/bin/
	@test -d ${DESTDIR}/usr/share/tabset || \
		mkdir -p ${DESTDIR}/usr/share/tabset
	@for i in ${tabset}; do \
	j="${INSTALL} ${INSTALL_COPY} ${INC_INSTALL_OWN} -m 0644 ${NCURSES_DIR}/misc/tabset/$$i \
		${DESTDIR}/usr/share/tabset"; \
	echo $$j; \
	eval "$$j"; \
	done;


CLEANFILES=	${GENSRCS} ${GENHDRS} fallback.c ${NCURSES_DIR}/comp_captab.c lib_gen.c init_keytry.h keys.list make_hash \
		make_keys make_hash.dep make_keys.dep ${NCURSES_INC}/curses.h 
		

SHLIB_LDADD+=  -M${NCURSES_SRC}/mapfile
realbuild: ${MAPFILE}

beforebuild: clean ${GENHDRS}

.ifdef DESTDIR
afterinstall: includes pc_files misc_files
.endif
.include <bsd.lib.mk>
