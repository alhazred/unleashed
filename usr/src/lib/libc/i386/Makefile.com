#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License (the "License").
# You may not use this file except in compliance with the License.
#
# You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
# or http://www.opensolaris.org/os/licensing.
# See the License for the specific language governing permissions
# and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each
# file and include the License file at usr/src/OPENSOLARIS.LICENSE.
# If applicable, add the following below this CDDL HEADER, with the
# fields enclosed by brackets "[]" replaced with your own identifying
# information: Portions Copyright [yyyy] [name of copyright owner]
#
# CDDL HEADER END
#
#
# Copyright (c) 2004, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2016 Joyent, Inc.
# Copyright (c) 2013, OmniTI Computer Consulting, Inc. All rights reserved.
# Copyright 2013 Garrett D'Amore <garrett@damore.org>
#
# Copyright 2011 Nexenta Systems, Inc.  All rights reserved.
# Use is subject to license terms.
#

LIBCDIR=	$(SRC)/lib/libc
LIB_PIC=	libc_pic.a
VERS=		.1
TARGET_ARCH=	i386

# include comm page definitions
include $(SRC)/lib/commpage/Makefile.shared.com
include $(SRC)/lib/commpage/Makefile.shared.targ

VALUES=		values-Xa.o

# objects are grouped by source directory

# Symbol capabilities objects
EXTPICS=	\
	$(LIBCDIR)/capabilities/i386-sse/i386/pics/symcap.o \
	$(LIBCDIR)/capabilities/i386-sse2/i386/pics/symcap.o

# local objects
STRETS=

CRTOBJS=			\
	cerror.o		\
	cerror64.o

DYNOBJS=			\
	_rtbootld.o

FPOBJS=				\
	_D_cplx_div.o		\
	_D_cplx_div_ix.o	\
	_D_cplx_div_rx.o	\
	_D_cplx_lr_div.o	\
	_D_cplx_lr_div_ix.o	\
	_D_cplx_lr_div_rx.o	\
	_D_cplx_mul.o		\
	_F_cplx_div.o		\
	_F_cplx_div_ix.o	\
	_F_cplx_div_rx.o	\
	_F_cplx_lr_div.o	\
	_F_cplx_lr_div_ix.o	\
	_F_cplx_lr_div_rx.o	\
	_F_cplx_mul.o		\
	_X_cplx_div.o		\
	_X_cplx_div_ix.o	\
	_X_cplx_div_rx.o	\
	_X_cplx_lr_div.o	\
	_X_cplx_lr_div_ix.o	\
	_X_cplx_lr_div_rx.o	\
	_X_cplx_mul.o		\
	fpgetmask.o		\
	fpgetround.o		\
	fpgetsticky.o		\
	fpsetmask.o		\
	fpsetround.o		\
	fpsetsticky.o		\
	fpstart.o		\
	ieee.o

FPASMOBJS=			\
	__xgetRD.o		\
	_base_il.o		\
	_xtoll.o		\
	_xtoull.o		\
	fpcw.o

ATOMICOBJS=			\
	atomic.o

CHACHAOBJS=			\
	chacha.o

XATTROBJS=			\
	xattr_common.o

COMOBJS=			\
	bcmp.o			\
	bcopy.o			\
	bsearch.o		\
	bzero.o			\
	qsort.o			\
	strtol.o		\
	strtoul.o		\
	strtoll.o		\
	strtoull.o

DTRACEOBJS=			\
	dtrace_data.o

SECFLAGSOBJS=			\
	secflags.o

GENOBJS=			\
	$(COMMPAGE_OBJS)	\
	_div64.o		\
	_divdi3.o		\
	_getsp.o		\
	_mul64.o		\
	abs.o			\
	alloca.o		\
	arc4random.o		\
	arc4random_uniform.o	\
	byteorder.o		\
	byteorder64.o		\
	cuexit.o		\
	ecvt.o			\
	endian.o		\
	errlst.o		\
	fts.o			\
	freezero.o		\
	i386_data.o		\
	ladd.o			\
	ldivide.o		\
	lmul.o			\
	lock.o			\
	lshiftl.o		\
	lsign.o			\
	lsub.o			\
	makectxt.o		\
	memccpy.o		\
	memchr.o		\
	memcmp.o		\
	memcpy.o		\
	memset.o		\
	new_list.o		\
	reallocarray.o		\
	recallocarray.o		\
	setjmp.o		\
	siginfolst.o		\
	siglongjmp.o		\
	strcat.o		\
	strchr.o		\
	strcmp.o		\
	strcpy.o		\
	strlen.o		\
	strncat.o		\
	strncmp.o		\
	strncpy.o		\
	strnlen.o		\
	strrchr.o		\
	sync_instruction_memory.o\
	unvis.o			\
	vis.o

# sysobjs that contain large-file interfaces
COMSYSOBJS64=			\
	fstatvfs64.o		\
	getdents64.o		\
	getrlimit64.o		\
	lseek64.o		\
	mmap64.o		\
	pread64.o		\
	preadv64.o		\
	pwrite64.o		\
	pwritev64.o		\
	setrlimit64.o		\
	statvfs64.o

SYSOBJS64=

COMSYSOBJS=			\
	__clock_timer.o		\
	__getloadavg.o		\
	__rusagesys.o		\
	__signotify.o		\
	__sigrt.o		\
	__time.o		\
	_lgrp_home_fast.o	\
	_lgrpsys.o		\
	_nfssys.o		\
	_portfs.o		\
	_pset.o			\
	_rpcsys.o		\
	_sigaction.o		\
	_so_accept.o		\
	_so_bind.o		\
	_so_connect.o		\
	_so_getpeername.o	\
	_so_getsockname.o	\
	_so_getsockopt.o	\
	_so_listen.o		\
	_so_recv.o		\
	_so_recvfrom.o		\
	_so_recvmsg.o		\
	_so_send.o		\
	_so_sendmsg.o		\
	_so_sendto.o		\
	_so_setsockopt.o	\
	_so_shutdown.o		\
	_so_socket.o		\
	_so_socketpair.o	\
	_sockconfig.o		\
	acct.o			\
	acl.o			\
	adjtime.o		\
	alarm.o			\
	brk.o			\
	chdir.o			\
	chroot.o		\
	close.o			\
	execve.o		\
	exit.o			\
	facl.o			\
	fchdir.o		\
	fchroot.o		\
	fdsync.o		\
	fpathconf.o		\
	fstatfs.o		\
	fstatvfs.o		\
	getcpuid.o		\
	getdents.o		\
	getegid.o		\
	geteuid.o		\
	getgid.o		\
	getgroups.o		\
	gethrtime.o		\
	getitimer.o		\
	getmsg.o		\
	getpid.o		\
	getpmsg.o		\
	getppid.o		\
	getrandom.o		\
	getrlimit.o		\
	getuid.o		\
	gtty.o			\
	install_utrap.o		\
	ioctl.o			\
	kaio.o			\
	kill.o			\
	llseek.o		\
	lseek.o			\
	mmapobjsys.o		\
	memcntl.o		\
	mincore.o		\
	mmap.o			\
	modctl.o		\
	mount.o			\
	mprotect.o		\
	munmap.o		\
	nice.o			\
	ntp_adjtime.o		\
	ntp_gettime.o		\
	p_online.o		\
	pathconf.o		\
	pause.o			\
	pcsample.o		\
	pipe2.o			\
	pollsys.o		\
	pread.o			\
	preadv.o		\
	priocntlset.o		\
	processor_bind.o	\
	processor_info.o	\
	profil.o		\
	psecflagsset.o		\
	putmsg.o		\
	putpmsg.o		\
	pwrite.o		\
	pwritev.o		\
	read.o			\
	readv.o			\
	resolvepath.o		\
	seteguid.o		\
	setgid.o		\
	setgroups.o		\
	setitimer.o		\
	setreid.o		\
	setrlimit.o		\
	setuid.o		\
	sigaltstk.o		\
	sigprocmsk.o		\
	sigsendset.o		\
	sigsuspend.o		\
	statfs.o		\
	statvfs.o		\
	stty.o			\
	sync.o			\
	sysconfig.o		\
	sysfs.o			\
	sysinfo.o		\
	syslwp.o		\
	times.o			\
	ulimit.o		\
	umask.o			\
	umount2.o		\
	uname.o			\
	utssys.o		\
	uucopy.o		\
	vhangup.o		\
	waitid.o		\
	write.o			\
	writev.o		\
	yield.o

SYSOBJS=			\
	__clock_gettime.o	\
	__clock_gettime_sys.o	\
	__getcontext.o		\
	__uadmin.o		\
	_lwp_mutex_unlock.o	\
	_stack_grow.o		\
	door.o			\
	forkx.o			\
	forkallx.o		\
	getcontext.o		\
	gettimeofday.o		\
	lwp_private.o		\
	ptrace.o		\
	syscall.o		\
	sysi86.o		\
	tls_get_addr.o		\
	uadmin.o		\
	umount.o		\
	vforkx.o		\
	xstat.o

# objects under $(LIBCDIR)/port which contain transitional large file interfaces
PORTGEN64=			\
	_xftw64.o		\
	attropen64.o		\
	ftw64.o			\
	mkstemp64.o		\
	nftw64.o		\
	tell64.o		\
	truncate64.o

# objects from source under $(LIBCDIR)/port
PORTFP=				\
	__flt_decim.o		\
	__flt_rounds.o		\
	__tbl_10_b.o		\
	__tbl_10_h.o		\
	__tbl_10_s.o		\
	__tbl_2_b.o		\
	__tbl_2_h.o		\
	__tbl_2_s.o		\
	__tbl_fdq.o		\
	__tbl_tens.o		\
	__x_power.o		\
	_base_sup.o		\
	aconvert.o		\
	decimal_bin.o		\
	double_decim.o		\
	econvert.o		\
	fconvert.o		\
	file_decim.o		\
	finite.o		\
	fp_data.o		\
	func_decim.o		\
	gconvert.o		\
	hex_bin.o		\
	ieee_globals.o		\
	pack_float.o		\
	sigfpe.o		\
	string_decim.o

PORTGEN=			\
	_env_data.o		\
	_xftw.o			\
	a64l.o			\
	abort.o			\
	addsev.o		\
	ascii_strcasecmp.o	\
	ascii_strncasecmp.o	\
	assert.o		\
	atof.o			\
	atoi.o			\
	atol.o			\
	atoll.o			\
	attrat.o		\
	attropen.o		\
	atexit.o		\
	atfork.o		\
	base64.o		\
	basename.o		\
	calloc.o		\
	catgets.o		\
	catopen.o		\
	cfgetispeed.o		\
	cfgetospeed.o		\
	cfree.o			\
	cfsetispeed.o		\
	cfsetospeed.o		\
	cftime.o		\
	clock.o			\
	closedir.o		\
	closefrom.o		\
	confstr.o		\
	crypt.o			\
	csetlen.o		\
	ctime.o			\
	daemon.o		\
	deflt.o			\
	directio.o		\
	dirname.o		\
	div.o			\
	drand48.o		\
	dup.o			\
	env_data.o		\
	err.o			\
	errno.o			\
	euclen.o		\
	event_port.o		\
	execvp.o		\
	explicit_bzero.o	\
	fattach.o		\
	fdetach.o		\
	fdopendir.o		\
	ffs.o			\
	flock.o			\
	fls.o			\
	fmtmsg.o		\
	ftime.o			\
	ftok.o			\
	ftw.o			\
	gcvt.o			\
	getauxv.o		\
	getcwd.o		\
	getdate_err.o		\
	getdtblsize.o		\
	getentropy.o		\
	getenv.o		\
	getexecname.o		\
	getgrnam.o		\
	getgrnam_r.o		\
	gethostid.o		\
	gethostname.o		\
	gethz.o			\
	getisax.o		\
	getloadavg.o		\
	getlogin.o		\
	getmntent.o		\
	getnetgrent.o		\
	get_nprocs.o		\
	getopt.o		\
	getopt_long.o		\
	getpagesize.o		\
	getpw.o			\
	getpwnam.o		\
	getpwnam_r.o		\
	getrusage.o		\
	getspent.o		\
	getspent_r.o		\
	getsubopt.o		\
	gettxt.o		\
	getusershell.o		\
	getut.o			\
	getutx.o		\
	getvfsent.o		\
	getwd.o			\
	getwidth.o		\
	getxby_door.o		\
	gtxt.o			\
	hsearch.o		\
	iconv.o			\
	imaxabs.o		\
	imaxdiv.o		\
	index.o			\
	initgroups.o		\
	insque.o		\
	isaexec.o		\
	isastream.o		\
	isatty.o		\
	killpg.o		\
	klpdlib.o		\
	l64a.o			\
	lckpwdf.o		\
	lconstants.o		\
	lexp10.o		\
	lfind.o			\
	lfmt.o			\
	lfmt_log.o		\
	llabs.o			\
	lldiv.o			\
	llog10.o		\
	lltostr.o		\
	localtime.o		\
	lsearch.o		\
	madvise.o		\
	malloc.o		\
	memalign.o		\
	memmem.o		\
	mkdev.o			\
	mkdtemp.o		\
	mkfifo.o		\
	mkstemp.o		\
	mktemp.o		\
	mlock.o			\
	mlockall.o		\
	mon.o			\
	msync.o			\
	munlock.o		\
	munlockall.o		\
	ndbm.o			\
	nftw.o			\
	nlspath_checks.o	\
	nsparse.o		\
	nss_common.o		\
	nss_dbdefs.o		\
	nss_deffinder.o		\
	opendir.o		\
	opt_data.o		\
	perror.o		\
	pfmt.o			\
	pfmt_data.o		\
	pfmt_print.o		\
	pipe.o			\
	plock.o			\
	poll.o			\
	posix_fadvise.o		\
	posix_fallocate.o	\
	posix_madvise.o		\
	posix_memalign.o	\
	priocntl.o		\
	privlib.o		\
	priv_str_xlate.o	\
	psecflags.o		\
	psiginfo.o		\
	psignal.o		\
	pt.o			\
	putpwent.o		\
	putspent.o		\
	raise.o			\
	rand.o			\
	random.o		\
	rctlops.o		\
	readdir.o		\
	readdir_r.o		\
	realpath.o		\
	reboot.o		\
	regexpr.o		\
	remove.o		\
	rewinddir.o		\
	rindex.o		\
	scandir.o		\
	seekdir.o		\
	select.o		\
	setlabel.o		\
	setmode.o		\
	setpriority.o		\
	settimeofday.o		\
	sh_locks.o		\
	sigflag.o		\
	siglist.o		\
	sigsend.o		\
	sigsetops.o		\
	ssignal.o		\
	stack.o			\
	stpcpy.o		\
	stpncpy.o		\
	str2sig.o		\
	strcase_charmap.o	\
	strchrnul.o		\
	strcspn.o		\
	strdup.o		\
	strerror.o		\
	strlcat.o		\
	strlcpy.o		\
	strndup.o		\
	strpbrk.o		\
	strsep.o		\
	strsignal.o		\
	strspn.o		\
	strstr.o		\
	strtod.o		\
	strtoimax.o		\
	strtok.o		\
	strtok_r.o		\
	strtoumax.o		\
	swab.o			\
	swapctl.o		\
	sysconf.o		\
	syslog.o		\
	tcdrain.o		\
	tcflow.o		\
	tcflush.o		\
	tcgetattr.o		\
	tcgetpgrp.o		\
	tcgetsid.o		\
	tcsendbreak.o		\
	tcsetattr.o		\
	tcsetpgrp.o		\
	tell.o			\
	telldir.o		\
	tfind.o			\
	time_data.o		\
	time_gdata.o		\
	timespec_get.o		\
	tls_data.o		\
	truncate.o		\
	tsdalloc.o		\
	tsearch.o		\
	ttyname.o		\
	ttyslot.o		\
	ualarm.o		\
	ucred.o			\
	valloc.o		\
	vlfmt.o			\
	vpfmt.o			\
	waitpid.o		\
	walkstack.o		\
	wdata.o			\
	xgetwidth.o		\
	xpg4.o			\
	xpg6.o

PORTPRINT_W=			\
	doprnt_w.o

PORTPRINT=			\
	asprintf.o		\
	doprnt.o		\
	fprintf.o		\
	printf.o		\
	snprintf.o		\
	sprintf.o		\
	vfprintf.o		\
	vprintf.o		\
	vsnprintf.o		\
	vsprintf.o		\
	vwprintf.o		\
	wprintf.o

# c89 variants to support 32-bit size of c89 u/intmax_t (32-bit libc only)
PORTPRINT_C89=			\
	vfprintf_c89.o		\
	vprintf_c89.o		\
	vsnprintf_c89.o		\
	vsprintf_c89.o		\
	vwprintf_c89.o

PORTSTDIO_C89=			\
	vscanf_c89.o		\
	vwscanf_c89.o		\

# portable stdio objects that contain large file interfaces.
# Note: fopen64 is a special case, as we build it small.
PORTSTDIO64=			\
	fopen64.o		\
	fpos64.o

PORTSTDIO_W=			\
	doscan_w.o

PORTSTDIO=			\
	__extensions.o		\
	_endopen.o		\
	_filbuf.o		\
	_findbuf.o		\
	_flsbuf.o		\
	_wrtchk.o		\
	clearerr.o		\
	ctermid.o		\
	ctermid_r.o		\
	cuserid.o		\
	data.o			\
	doscan.o		\
	fdopen.o		\
	feof.o			\
	ferror.o		\
	fgetc.o			\
	fgets.o			\
	fileno.o		\
	flockf.o		\
	flush.o			\
	fopen.o			\
	fpos.o			\
	fputc.o			\
	fputs.o			\
	fread.o			\
	fseek.o			\
	fseeko.o		\
	ftell.o			\
	ftello.o		\
	fwrite.o		\
	getc.o			\
	getchar.o		\
	getline.o		\
	getpass.o		\
	gets.o			\
	getw.o			\
	mse.o			\
	popen.o			\
	putc.o			\
	putchar.o		\
	puts.o			\
	putw.o			\
	rewind.o		\
	scanf.o			\
	setbuf.o		\
	setbuffer.o		\
	setvbuf.o		\
	system.o		\
	tempnam.o		\
	tmpfile.o		\
	tmpnam_r.o		\
	ungetc.o		\
	vscanf.o		\
	vwscanf.o		\
	wscanf.o

PORTI18N=			\
	getwchar.o		\
	putwchar.o		\
	putws.o			\
	strtows.o		\
	wcsnlen.o		\
	wcsstr.o		\
	wcstoimax.o		\
	wcstol.o		\
	wcstoul.o		\
	wcswcs.o		\
	wmemchr.o		\
	wmemcmp.o		\
	wmemcpy.o		\
	wmemmove.o		\
	wmemset.o		\
	wscat.o			\
	wschr.o			\
	wscmp.o			\
	wscpy.o			\
	wscspn.o		\
	wsdup.o			\
	wslen.o			\
	wsncat.o		\
	wsncmp.o		\
	wsncpy.o		\
	wspbrk.o		\
	wsprintf.o		\
	wsrchr.o		\
	wsscanf.o		\
	wsspn.o			\
	wstod.o			\
	wstok.o			\
	wstol.o			\
	wstoll.o		\
	wsxfrm.o		\
	gettext.o		\
	gettext_gnu.o		\
	gettext_real.o		\
	gettext_util.o		\
	plural_parser.o		\
	wdresolve.o		\
	_ctype.o		\
	isascii.o		\
	toascii.o

PORTI18N_COND=			\
	wcstol_longlong.o	\
	wcstoul_longlong.o

PORTINET= bindresvport.o bootparams_getbyname.o ether_addr.o \
	  getaddrinfo.o getnameinfo.o getnetent.o getnetent_r.o \
	  getprotoent.o getprotoent_r.o getservbyname_r.o getservent.o \
	  getservent_r.o inet_lnaof.o inet_mkaddr.o inet_network.o \
	  inet6_opt.o inet6_rthdr.o interface_id.o link_addr.o \
	  netmasks.o ruserpass.o sourcefilter.o \
	  getifaddrs.o

PORTLOCALE=			\
	big5.o			\
	btowc.o			\
	collate.o		\
	collcmp.o		\
	euc.o			\
	fnmatch.o		\
	fgetwc.o		\
	fgetws.o		\
	fix_grouping.o		\
	fputwc.o		\
	fputws.o		\
	fwide.o			\
	gb18030.o		\
	gb2312.o		\
	gbk.o			\
	getdate.o		\
	isdigit.o		\
	iswctype.o		\
	ldpart.o		\
	lmessages.o		\
	lnumeric.o		\
	lmonetary.o		\
	localeconv.o		\
	localeimpl.o		\
	mbftowc.o		\
	mblen.o			\
	mbrlen.o		\
	mbrtowc.o		\
	mbsinit.o		\
	mbsnrtowcs.o		\
	mbsrtowcs.o		\
	mbstowcs.o		\
	mbtowc.o		\
	mskanji.o		\
	nextwctype.o		\
	nl_langinfo.o		\
	none.o			\
	rune.o			\
	runetype.o		\
	setlocale.o		\
	setrunelocale.o		\
	strcasecmp.o		\
	strcasestr.o		\
	strcoll.o		\
	strfmon.o		\
	strftime.o		\
	strncasecmp.o		\
	strptime.o		\
	strxfrm.o		\
	table.o			\
	timelocal.o		\
	tolower.o		\
	towlower.o		\
	ungetwc.o		\
	utf8.o			\
	wcrtomb.o		\
	wcscasecmp.o		\
	wcscoll.o		\
	wcsftime.o		\
	wcsnrtombs.o		\
	wcsrtombs.o		\
	wcswidth.o		\
	wcstombs.o		\
	wcsxfrm.o		\
	wctob.o			\
	wctomb.o		\
	wctrans.o		\
	wctype.o		\
	wcwidth.o		\
	wscol.o

PORTNSL= _conn_util.o _data2.o _errlst.o _utility.o algs.o auth_des.o \
         auth_none.o auth_sys.o auth_time.o authdes_prot.o authsys_prot.o \
         can_use_af.o checkver.o clnt_bcast.o clnt_dg.o clnt_door.o \
         clnt_generic.o clnt_perror.o clnt_raw.o clnt_simple.o clnt_vc.o \
         daemon_utils.o dbm.o des_crypt.o des_soft.o doconfig.o getauthattr.o \
         getauuser.o getdname.o getexecattr.o gethostby_door.o \
         gethostbyname_r.o gethostent.o gethostent6.o gethostent_r.o \
         getipnodeby.o getipnodeby_door.o getprofattr.o getrpcent.o \
         getrpcent_r.o getuserattr.o inet_matchaddr.o inet_ntoa.o inet_ntop.o \
         inet_pton.o key_call.o key_prot.o mt_misc.o netdir.o netdir_inet.o \
         netdir_inet_sundry.o netname.o netnamer.o netselect.o nis_misc.o \
         nis_misc_proc.o nis_sec_mechs.o nis_subr.o nis_xdr.o parse.o \
         pmap_clnt.o pmap_prot.o publickey.o rpc_callmsg.o rpc_comdata.o \
         rpc_fdsync.o rpc_generic.o rpc_prot.o rpc_sel2poll.o \
         rpc_soc.o rpc_td.o rpcb_clnt.o rpcb_prot.o rpcb_st_xdr.o rpcdname.o \
         rpcsec_gss_if.o rtime_tli.o svc.o svc_auth.o svc_auth_loopb.o \
         svc_auth_sys.o svc_dg.o svc_door.o svc_generic.o svc_raw.o svc_run.o \
         svc_simple.o svc_vc.o svcauth_des.o svid_funcs.o t_accept.o \
         t_alloc.o t_bind.o t_close.o t_connect.o t_error.o t_free.o \
         t_getinfo.o t_getname.o t_getstate.o t_listen.o t_look.o t_open.o \
         t_optmgmt.o t_rcv.o t_rcvconnect.o t_rcvdis.o t_rcvrel.o \
         t_rcvreldata.o t_rcvudata.o t_rcvuderr.o t_rcvv.o t_rcvvudata.o \
         t_snd.o t_snddis.o t_sndrel.o t_sndreldata.o t_sndudata.o t_sndv.o \
         t_sndvudata.o t_strerror.o t_sync.o t_sysconf.o t_unbind.o \
         thr_get_storage.o ti_opts.o tli_wrappers.o xdr.o xdr_array.o \
         xdr_float.o xdr_mem.o xdr_rec.o xdr_refer.o xdr_sizeof.o xdr_stdio.o \
         xti_wrappers.o yp_all.o yp_b_clnt.o yp_b_xdr.o yp_bind.o yp_enum.o \
         yp_master.o yp_match.o yp_order.o yp_rsvd.o yp_update.o yp_xdr.o \
         yperr_string.o yppasswd_xdr.o ypprot_err.o ypupd.o

AIOOBJS=			\
	aio.o			\
	aio_alloc.o		\
	posix_aio.o

RTOBJS=				\
	clock_timer.o		\
	mqueue.o		\
	pos4obj.o		\
	sched.o			\
	sem.o			\
	shm.o			\
	sigev_thread.o

TPOOLOBJS=			\
	thread_pool.o

THREADSOBJS=			\
	alloc.o			\
	assfail.o		\
	cancel.o		\
	c11_thr.o		\
	door_calls.o		\
	tmem.o			\
	pthr_attr.o		\
	pthr_barrier.o		\
	pthr_cond.o		\
	pthr_mutex.o		\
	pthr_rwlock.o		\
	pthread.o		\
	rwlock.o		\
	scalls.o		\
	sema.o			\
	sigaction.o		\
	spawn.o			\
	synch.o			\
	tdb_agent.o		\
	thr.o			\
	thread_interface.o	\
	tls.o			\
	tsd.o

THREADSMACHOBJS=		\
	machdep.o

THREADSASMOBJS=			\
	asm_subr.o

UNICODEOBJS=			\
	u8_textprep.o		\
	uconv.o

UNWINDMACHOBJS=			\
	unwind.o

UNWINDASMOBJS=			\
	unwind_frame.o

# objects that implement the transitional large file API
PORTSYS64=			\
	lockf64.o		\
	stat64.o

PORTSYS=			\
	_autofssys.o		\
	access.o		\
	acctctl.o		\
	bsd_signal.o		\
	chmod.o			\
	chown.o			\
	corectl.o		\
	epoll.o			\
	eventfd.o		\
	exacctsys.o		\
	execl.o			\
	execle.o		\
	execv.o			\
	fcntl.o			\
	getpagesizes.o		\
	getpeerucred.o		\
	inst_sync.o		\
	issetugid.o		\
	link.o			\
	lockf.o			\
	lwp.o			\
	lwp_cond.o		\
	lwp_rwlock.o		\
	lwp_sigmask.o		\
	meminfosys.o		\
	mkdir.o			\
	mknod.o			\
	msgsys.o		\
	nfssys.o		\
	open.o			\
	pgrpsys.o		\
	posix_sigwait.o		\
	ppriv.o			\
	psetsys.o		\
	rctlsys.o		\
	readlink.o		\
	rename.o		\
	sbrk.o			\
	semsys.o		\
	set_errno.o		\
	sharefs.o		\
	shmsys.o		\
	sidsys.o		\
	siginterrupt.o		\
	signal.o		\
	signalfd.o		\
	sigpending.o		\
	sigstack.o		\
	stat.o			\
	symlink.o		\
	tasksys.o		\
	time.o			\
	time_util.o		\
	timerfd.o		\
	ucontext.o		\
	unlink.o		\
	ustat.o			\
	utimesys.o		\
	zone.o

PORTREGEX=			\
	glob.o			\
	regcmp.o		\
	regcomp.o		\
	regerror.o		\
	regex.o			\
	regexec.o		\
	regfree.o		\
	wordexp.o

PORTREGEX64=			\
	glob64.o

PORTSOCKET= _soutil.o sockatmark.o socket.o socketpair.o weaks.o

MOSTOBJS=			\
	$(STRETS)		\
	$(CRTOBJS)		\
	$(DYNOBJS)		\
	$(FPOBJS)		\
	$(FPASMOBJS)		\
	$(ATOMICOBJS)		\
	$(CHACHAOBJS)		\
	$(XATTROBJS)		\
	$(COMOBJS)		\
	$(DTRACEOBJS)		\
	$(GENOBJS)		\
	$(PORTFP)		\
	$(PORTGEN)		\
	$(PORTGEN64)		\
	$(PORTI18N)		\
	$(PORTI18N_COND)	\
	$(PORTINET)		\
	$(PORTLOCALE)		\
	$(PORTNSL)		\
	$(PORTPRINT)		\
	$(PORTPRINT_C89)	\
	$(PORTPRINT_W)		\
	$(PORTREGEX)		\
	$(PORTREGEX64)		\
	$(PORTSOCKET)		\
	$(PORTSTDIO)		\
	$(PORTSTDIO64)		\
	$(PORTSTDIO_C89)	\
	$(PORTSTDIO_W)		\
	$(PORTSYS)		\
	$(PORTSYS64)		\
	$(AIOOBJS)		\
	$(RTOBJS)		\
	$(SECFLAGSOBJS)		\
	$(TPOOLOBJS)		\
	$(THREADSOBJS)		\
	$(THREADSMACHOBJS)	\
	$(THREADSASMOBJS)	\
	$(UNICODEOBJS)		\
	$(UNWINDMACHOBJS)	\
	$(UNWINDASMOBJS)	\
	$(COMSYSOBJS)		\
	$(SYSOBJS)		\
	$(COMSYSOBJS64)		\
	$(SYSOBJS64)		\
	$(VALUES)

TRACEOBJS=			\
	plockstat.o

# NOTE:	libc.so.1 must be linked with the minimal crti.o and crtn.o
# modules whose source is provided in the $(SRC)/lib/crt directory.
# This must be done because otherwise the Sun C compiler would insert
# its own versions of these modules and those versions contain code
# to call out to C++ initialization functions.  Such C++ initialization
# functions can call back into libc before thread initialization is
# complete and this leads to segmentation violations and other problems.
# Since libc contains no C++ code, linking with the minimal crti.o and
# crtn.o modules is safe and avoids the problems described above.
OBJECTS= $(CRTI) $(MOSTOBJS) $(CRTN)
CRTSRCS= ../../crt/i86

LDPASS_OFF=	$(POUND_SIGN)

# include common library definitions
include ../../Makefile.lib

# we need to override the default SONAME here because we might
# be building a variant object (still libc.so.1, but different filename)
SONAME = libc.so.1

CFLAGS += $(CTF_FLAGS)

CERRWARN += -Wno-parentheses
CERRWARN += -Wno-switch
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-value
CERRWARN += -Wno-unused-label
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-type-limits
CERRWARN += -Wno-char-subscripts
CERRWARN += -Wno-clobbered
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-address

# Setting THREAD_DEBUG = -DTHREAD_DEBUG (make THREAD_DEBUG=-DTHREAD_DEBUG ...)
# enables ASSERT() checking in the threads portion of the library.
# This is automatically enabled for DEBUG builds, not for non-debug builds.
THREAD_DEBUG =
$(NOT_RELEASE_BUILD)THREAD_DEBUG = -DTHREAD_DEBUG

ALTPICS= $(TRACEOBJS:%=pics/%)

# The use of sed is a gross hack needed because the current build system
# assumed that the compiler accepted linker flags (-Bfoo -zfoo and -Mfoo)
# directly.  Here, since we're calling the linker directly, we have to
# discard the prefixes.  Ideally, we would be using the LD_Z* and LD_B*
# variables instead, but that would require a lot of mucking with makefiles.
# So for now, we do this.
REMOVE_GCC_PREFIX=echo $(DYNFLAGS) | $(SED) -e 's/-Wl,//g'
$(DYNLIB) := BUILD.SO = $(LD) -o $@ -G $(REMOVE_GCC_PREFIX:sh) $(PICS) $(ALTPICS) \
		$(EXTPICS) $(LDLIBS)

MAPFILES =	$(LIBCDIR)/port/mapfile-vers

#
# EXTN_CPPFLAGS and EXTN_CFLAGS set in enclosing Makefile
#
CFLAGS +=	-march=pentiumpro
CPPFLAGS=	-Di386 $(EXTN_CPPFLAGS) $(THREAD_DEBUG) \
		-I$(LIBCBASE)/inc -I$(LIBCDIR)/inc $(CPPFLAGS.master)
ASFLAGS=	$(AS_PICFLAGS) -D_ASM $(CPPFLAGS) $(i386_AS_XARCH)

# Inform the run-time linker about libc specialized initialization
RTLDINFO =	-z rtldinfo=tls_rtldinfo
DYNFLAGS +=	$(RTLDINFO)

# Force libc's internal references to be resolved immediately upon loading
# in order to avoid critical region problems.  Since almost all libc symbols
# are marked 'protected' in the mapfiles, this is a minimal set (15 to 20).
DYNFLAGS +=	-znow

DYNFLAGS +=	-e __rtboot
DYNFLAGS +=	$(EXTN_DYNFLAGS)

# Inform the kernel about the initial DTrace area (in case
# libc is being used as the interpreter / runtime linker).
DTRACE_DATA =	-zdtrace=dtrace_data
DYNFLAGS +=	$(DTRACE_DATA)

# DTrace needs an executable data segment.
DYNFLAGS += -M$(SRC)/common/mapfiles/common/map.execdata

BUILD.s=	$(AS) $(ASFLAGS) $< -o $@

# Override this top level flag so the compiler builds in its native
# C99 mode.  This has been enabled to support the complex arithmetic
# added to libc.
C99MODE=	$(C99_ENABLE)

# libc method of building an archive
# The "$(GREP) -v ' L '" part is necessary only until
# lorder is fixed to ignore thread-local variables.
BUILD.AR= $(RM) $@ ; \
	$(AR) qc $@ `$(LORDER) $(MOSTOBJS:%=$(DIR)/%) | $(GREP) -v ' L ' | $(TSORT)`

# extra files for the clean target
CLEANFILES +=			\
	$(LIBCDIR)/port/gen/errlst.c	\
	$(LIBCDIR)/port/gen/new_list.c	\
	assym.h			\
	genassym		\
	crt/_rtld.s		\
	crt/_rtbootld.s		\
	pics/_rtbootld.o	\
	pics/crti.o		\
	pics/crtn.o		\
	$(ALTPICS)

CLOBBERFILES +=	$(LIB_PIC)

# list of C source formerly for lint
SRCS=							\
	$(ATOMICOBJS:%.o=$(SRC)/common/atomic/%.c)	\
	$(XATTROBJS:%.o=$(SRC)/common/xattr/%.c)	\
	$(COMOBJS:%.o=$(SRC)/common/util/%.c)		\
	$(DTRACEOBJS:%.o=$(SRC)/common/dtrace/%.c)	\
	$(PORTFP:%.o=$(LIBCDIR)/port/fp/%.c)			\
	$(PORTGEN:%.o=$(LIBCDIR)/port/gen/%.c)			\
	$(PORTI18N:%.o=$(LIBCDIR)/port/i18n/%.c)		\
	$(PORTLOCALE:%.o=$(LIBCDIR)/port/locale/%.c)		\
	$(PORTPRINT:%.o=$(LIBCDIR)/port/print/%.c)		\
	$(PORTREGEX:%.o=$(LIBCDIR)/port/regex/%.c)		\
	$(PORTSTDIO:%.o=$(LIBCDIR)/port/stdio/%.c)		\
	$(PORTSYS:%.o=$(LIBCDIR)/port/sys/%.c)			\
	$(AIOOBJS:%.o=$(LIBCDIR)/port/aio/%.c)			\
	$(RTOBJS:%.o=$(LIBCDIR)/port/rt/%.c)			\
	$(SECFLAGSOBJS:%.o=$(SRC)/common/secflags/%.c)		\
	$(TPOOLOBJS:%.o=$(LIBCDIR)/port/tpool/%.c)		\
	$(THREADSOBJS:%.o=$(LIBCDIR)/port/threads/%.c)		\
	$(THREADSMACHOBJS:%.o=$(LIBCDIR)/$(MACH)/threads/%.c)	\
	$(UNICODEOBJS:%.o=$(SRC)/common/unicode/%.c)	\
	$(UNWINDMACHOBJS:%.o=$(LIBCDIR)/port/unwind/%.c)	\
	$(FPOBJS:%.o=$(LIBCDIR)/$(MACH)/fp/%.c)			\
	$(LIBCBASE)/gen/ecvt.c				\
	$(LIBCBASE)/gen/makectxt.c			\
	$(LIBCBASE)/gen/siginfolst.c			\
	$(LIBCBASE)/gen/siglongjmp.c			\
	$(LIBCBASE)/gen/strcmp.c			\
	$(LIBCBASE)/gen/sync_instruction_memory.c	\
	$(LIBCBASE)/sys/ptrace.c			\
	$(LIBCBASE)/sys/uadmin.c

# conditional assignments
$(DYNLIB) := CRTI = crti.o
$(DYNLIB) := CRTN = crtn.o

# large-file-aware components that should be built large

$(COMSYSOBJS64:%=pics/%) := \
	CPPFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

$(SYSOBJS64:%=pics/%) := \
	CPPFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

$(PORTGEN64:%=pics/%) := \
	CPPFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

$(PORTREGEX64:%=pics/%) := \
	CPPFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

$(PORTSTDIO64:%=pics/%) := \
	CPPFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

$(PORTSYS64:%=pics/%) := \
	CPPFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

$(PORTSTDIO_W:%=pics/%) := \
	CPPFLAGS += -D_WIDE

$(PORTPRINT_W:%=pics/%) := \
	CPPFLAGS += -D_WIDE

$(PORTPRINT_C89:%=pics/%) := \
	CPPFLAGS += -D_C89_INTMAX32

$(PORTSTDIO_C89:%=pics/%) := \
	CPPFLAGS += -D_C89_INTMAX32

$(PORTI18N_COND:%=pics/%) := \
	CPPFLAGS += -D_WCS_LONGLONG

pics/arc4random.o :=	CPPFLAGS += -I$(SRC)/common/crypto/chacha

pics/__clock_gettime.o := CPPFLAGS += $(COMMPAGE_CPPFLAGS)

.KEEP_STATE:

all: $(LIBS) $(LIB_PIC)

# include common libc targets
include $(LIBCDIR)/Makefile.targ

# We need to strip out all CTF and DOF data from the static library
$(LIB_PIC) := DIR = pics
$(LIB_PIC): pics $$(PICS)
	$(BUILD.AR)
	$(MCS) -d -n .SUNW_ctf $@ > /dev/null 2>&1
	$(MCS) -d -n .SUNW_dof $@ > /dev/null 2>&1
	$(AR) -ts $@ > /dev/null
	$(POST_PROCESS_A)

$(LIBCBASE)/crt/_rtbootld.s: $(LIBCBASE)/crt/_rtboot.s $(LIBCBASE)/crt/_rtld.c
	$(CC) $(CPPFLAGS) $(CTF_FLAGS) -O -S $(C_PICFLAGS) \
	    $(LIBCBASE)/crt/_rtld.c -o $(LIBCBASE)/crt/_rtld.s
	$(CAT) $(LIBCBASE)/crt/_rtboot.s $(LIBCBASE)/crt/_rtld.s > $@
	$(RM) $(LIBCBASE)/crt/_rtld.s

# partially built from C source
pics/_rtbootld.o: $(LIBCBASE)/crt/_rtbootld.s
	$(AS) $(ASFLAGS) $(LIBCBASE)/crt/_rtbootld.s -o $@
	$(CTFCONVERT_O)

ASSYMDEP_OBJS=			\
	_lwp_mutex_unlock.o	\
	_stack_grow.o		\
	getcontext.o		\
	setjmp.o		\
	tls_get_addr.o		\
	vforkx.o

$(ASSYMDEP_OBJS:%=pics/%)	:=	CPPFLAGS += -I.

$(ASSYMDEP_OBJS:%=pics/%): assym.h

# assym.h build rules

GENASSYM_C = $(LIBCDIR)/$(MACH)/genassym.c

genassym: $(GENASSYM_C)
	$(NATIVECC) $(NATIVE_CFLAGS) -I$(LIBCBASE)/inc -I$(LIBCDIR)/inc	\
		-D__EXTENSIONS__ $(CPPFLAGS.native) -o $@ $(GENASSYM_C)

OFFSETS = $(LIBCDIR)/$(MACH)/offsets.in

assym.h: $(OFFSETS) genassym
	$(OFFSETS_CREATE) <$(OFFSETS) >$@
	./genassym >>$@

# derived C source and related explicit dependencies
$(LIBCDIR)/port/gen/errlst.c + \
$(LIBCDIR)/port/gen/new_list.c: $(LIBCDIR)/port/gen/errlist $(LIBCDIR)/port/gen/errlist.awk
	cd $(LIBCDIR)/port/gen; pwd; $(AWK) -f errlist.awk < errlist

pics/errlst.o: $(LIBCDIR)/port/gen/errlst.c

pics/new_list.o: $(LIBCDIR)/port/gen/new_list.c
