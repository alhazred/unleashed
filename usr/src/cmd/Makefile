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
# Copyright (c) 1989, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2016 Joyent, Inc.
# Copyright (c) 2012 by Delphix. All rights reserved.
# Copyright (c) 2013 DEY Storage Systems, Inc. All rights reserved.
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
# Copyright 2016 Toomas Soome <tsoome@me.com>
# Copyright 2016 Nexenta Systems, Inc.
#

include ../Makefile.master

#
#	Note that the command 'perl' is first in the list, violating
#	alphabetical order.  This is because it is very long-running and
#	should be given the most wall-clock time for a parallel build.
#
#	Commands in the FIRST_SUBDIRS list are built before starting the build
#	of other commands.  Currently this includes only 'isaexec' and
#	'platexec'.  This is necessary because $(ROOT)/usr/lib/isaexec or
#	$(ROOT)/usr/lib/platexec must exist when some other commands are built
#	because their 'make install' creates a hard link to one of them.
#

FIRST_SUBDIRS=		\
	isaexec		\
	platexec

COMMON_SUBDIRS=		\
	allocate	\
	perl		\
	Adm		\
	abi		\
	adbgen		\
	acct		\
	acctadm		\
	asa		\
	ast		\
	audio		\
	auths		\
	autopush	\
	backup		\
	bart		\
	basename	\
	bc		\
	bdiff		\
	beadm		\
	bnu		\
	boot		\
	busstat		\
	cal		\
	cdrw		\
	cfgadm		\
	chgrp		\
	chmod		\
	chown		\
	chroot		\
	cmd-crypto	\
	cmd-inet	\
	col		\
	compress	\
	consadm		\
	coreadm		\
	cpio		\
	cpc		\
	cron		\
	crypt		\
	csh		\
	csplit		\
	ctfconvert	\
	ctfdiff		\
	ctfdump		\
	ctfmerge	\
	ctrun		\
	ctstat		\
	ctwatch		\
	datadm		\
	date		\
	dc		\
	dd		\
	devfsadm	\
	syseventd	\
	devctl		\
	devinfo		\
	devmgmt		\
	devprop		\
	dfs.cmds	\
	diff		\
	diff3		\
	dircmp		\
	dirname		\
	diskinfo	\
	diskmgtd	\
	dispadmin	\
	dladm		\
	dlstat		\
	dmesg		\
	dodatadm	\
	dtrace		\
	du		\
	dumpadm		\
	dumpcs		\
	ed		\
	eeprom		\
	eject		\
	emul64ioctl	\
	enhance		\
	expand		\
	expr		\
	exstr		\
	factor		\
	fcinfo		\
	fcoesvc		\
	fdetach		\
	fdformat	\
	fdisk		\
	ficl		\
	filesync	\
	find		\
	flowadm		\
	flowstat	\
	fm		\
	fmt		\
	fmthard		\
	fmtmsg		\
	fold		\
	format		\
	fs.d		\
	fstyp		\
	fuser		\
	fwflash		\
	gcore		\
	gencat		\
	geniconvtbl	\
	genmsg		\
	getconf		\
	getdevpolicy	\
	getent		\
	getfacl		\
	getmajor	\
	getopt		\
	gettext		\
	gettxt		\
	groups		\
	growfs		\
	grpck		\
	gss		\
	hal		\
	halt		\
	head		\
	hostid		\
	hostname	\
	hotplug		\
	hotplugd	\
	ibd_upgrade	\
	iconv		\
	idmap		\
	init		\
	initpkg		\
	intrd		\
	intrstat	\
	ipcrm		\
	ipcs		\
	ipdadm		\
	ipf		\
	isainfo		\
	isalist		\
	itutools	\
	iscsiadm	\
	iscsid		\
	iscsitsvc	\
	isns		\
	itadm		\
	kbd		\
	keyserv		\
	killall		\
	krb5		\
	ksh		\
	kvmstat		\
	last		\
	lastcomm	\
	latencytop	\
	ldap		\
	ldapcachemgr	\
	lgrpinfo	\
	line		\
	link		\
	dlmgmtd		\
	listen		\
	loadkeys	\
	locale		\
	lockstat	\
	locator		\
	lofiadm		\
	logadm		\
	logger		\
	login		\
	logins		\
	look		\
	ls		\
	luxadm		\
	mail		\
	mailwrapper	\
	mailx		\
	make		\
	mdb		\
	mesg		\
	mkdir		\
	mkfifo		\
	mkfile		\
	mkmsgs		\
	mknod		\
	mkpwdict	\
	mktemp		\
	modload		\
	more		\
	mpathadm	\
	msgfmt		\
	msgid		\
	mt		\
	mv		\
	mvdir		\
	ndmpadm		\
	ndmpd		\
	ndmpstat	\
	netadm		\
	netfiles	\
	newform		\
	newgrp		\
	news		\
	newtask		\
	nice		\
	nl		\
	nlsadmin	\
	nohup		\
	nsadmin		\
	nscd		\
	oamuser		\
	od		\
	pack		\
	pagesize	\
	passmgmt	\
	passwd		\
	pathchk		\
	pbind		\
	pcidr		\
	pcitool		\
	pfexec		\
	pfexecd		\
	pginfo		\
	pgstat		\
	pgrep		\
	picl		\
	plimit		\
	policykit	\
	pools		\
	power		\
	powertop	\
	ppgsz		\
	pg		\
	plockstat	\
	pr		\
	prctl		\
	printenv	\
	printf		\
	priocntl	\
	profiles	\
	projadd		\
	projects	\
	prstat		\
	prtconf		\
	prtdiag		\
	prtvtoc		\
	ps		\
	psradm		\
	psrinfo		\
	psrset		\
	ptools		\
	pwck		\
	pyzfs		\
	raidctl		\
	ramdiskadm	\
	rcap		\
	rcm_daemon	\
	rctladm		\
	refer		\
	regcmp		\
	renice		\
	rm		\
	rmdir		\
	rmformat	\
	rmmount		\
	rmvolmgr	\
	roles		\
	rpcbind		\
	rpcgen		\
	rpcinfo		\
	rpcsvc		\
	runat		\
	sa		\
	saf		\
	sasinfo		\
	savecore	\
	sbdadm		\
	script		\
	scsi		\
	sdiff		\
	sdpadm		\
	setfacl		\
	setmnt		\
	setpgrp		\
	sgs		\
	shcomp		\
	smbios		\
	smbsrv		\
	smserverd	\
	sort		\
	spell		\
	split		\
	sqlite		\
	srchtxt		\
	srptadm		\
	srptsvc		\
	stat		\
	stmfadm		\
	stmfsvc		\
	stmsboot	\
	streams		\
	strings		\
	su		\
	sulogin		\
	sunpc		\
	svc		\
	swap		\
	sync		\
	sysdef		\
	syseventadm	\
	syslogd		\
	tail		\
	tar		\
	tcpd		\
	th_tools	\
	tip		\
	terminfo	\
	touch		\
	tr		\
	trapstat	\
	truss		\
	tty		\
	ttymon		\
	tzreload	\
	uadmin		\
	units		\
	unlink		\
	unpack		\
	userattr	\
	users		\
	utmpd		\
	uuidgen		\
	volcheck	\
	volrmmount	\
	vrrpadm		\
	vscan		\
	vt		\
	w		\
	wall		\
	which		\
	who		\
	whodo		\
	wracct		\
	write		\
	xargs		\
	xhci		\
	xstr		\
	ypcmd		\
	yppasswd	\
	zdb		\
	zdump		\
	zfs		\
	zhack		\
	zinject		\
	zlogin		\
	zoneadm		\
	zoneadmd	\
	zonecfg		\
	zonename	\
	zpool		\
	zlook		\
	zonestat	\
	zstreamdump	\
	ztest

i386_SUBDIRS=		\
	acpi		\
	acpihpd		\
	addbadsec	\
	biosdev		\
	diskscan	\
	nvmeadm		\
	rtc		\
	ucodeadm	\

sparc_SUBDIRS=		\
	cvcd		\
	dcs		\
	device_remap	\
	drd		\
	fruadm		\
	ldmad		\
	oplhpd		\
	prtdscp		\
	prtfru		\
	scadm		\
	sckmd		\
	sf880drd	\
	virtinfo	\
	vntsd

#
# Commands that are messaged.
#
MSGSUBDIRS=		\
	abi		\
	acctadm		\
	allocate	\
	asa		\
	audio		\
	audit		\
	auditconfig	\
	auditd		\
	auditrecord	\
	auditset	\
	auths		\
	autopush	\
	backup		\
	bart		\
	basename	\
	beadm		\
	bnu		\
	busstat		\
	cal		\
	cdrw		\
	cfgadm		\
	chgrp		\
	chmod		\
	chown		\
	cmd-crypto	\
	cmd-inet	\
	col		\
	compress	\
	consadm		\
	coreadm		\
	cpio		\
	cpc		\
	cron		\
	csh		\
	csplit		\
	ctrun		\
	ctstat		\
	ctwatch		\
	datadm		\
	date		\
	dc		\
	dcs		\
	dd		\
	devfsadm	\
	dfs.cmds	\
	diff		\
	dladm		\
	dlstat		\
	du		\
	dumpcs		\
	ed		\
	eject		\
	expand		\
	expr		\
	fcinfo		\
	filesync	\
	find		\
	flowadm		\
	flowstat	\
	fm		\
	fold		\
	fs.d		\
	fwflash		\
	geniconvtbl	\
	genmsg		\
	getconf		\
	getent		\
	gettext		\
	gettxt		\
	grpck		\
	gss		\
	halt		\
	head		\
	hostname	\
	hotplug		\
	iconv		\
	idmap		\
	isaexec		\
	iscsiadm	\
	iscsid		\
	isns		\
	itadm		\
	kbd		\
	krb5		\
	ksh		\
	last		\
	ldap		\
	ldapcachemgr	\
	lgrpinfo	\
	locale		\
	lofiadm		\
	logadm		\
	logger		\
	logins		\
	ls		\
	luxadm		\
	mailx		\
	make		\
	mesg		\
	mkdir		\
	mkpwdict	\
	mktemp		\
	more		\
	mpathadm	\
	msgfmt		\
	mv		\
	ndmpadm		\
	ndmpstat	\
	newgrp		\
	newtask		\
	nice		\
	nohup		\
	pack		\
	passwd		\
	passmgmt	\
	pathchk		\
	pfexec		\
	pg		\
	pgrep		\
	picl            \
	pools		\
	power		\
	pr		\
	praudit		\
	profiles	\
	projadd		\
	projects	\
	prstat		\
	prtdiag		\
	ps		\
	psrinfo		\
	ptools		\
	raidctl		\
	ramdiskadm	\
	rcap		\
	rcm_daemon	\
	refer		\
	regcmp		\
	renice		\
	roles		\
	rm		\
	rmdir		\
	rmformat	\
	rmmount		\
	rmvolmgr	\
	sasinfo		\
	sbdadm		\
	scadm		\
	script		\
	scsi		\
	sdiff		\
	sdpadm		\
	sgs		\
	shcomp		\
	smbsrv		\
	sort		\
	split		\
	srptadm		\
	stat		\
	stmfadm		\
	stmsboot	\
	strings		\
	su		\
	svc		\
	swap		\
	syseventadm	\
	syseventd	\
	tar		\
	touch		\
	tty		\
	ttymon		\
	tzreload	\
	units		\
	unlink		\
	unpack		\
	userattr	\
	volcheck	\
	volrmmount	\
	vrrpadm		\
	vscan		\
	w		\
	who		\
	whodo		\
	wracct		\
	write		\
	xargs		\
	yppasswd	\
	zdump		\
	zfs		\
	zlogin		\
	zoneadm		\
	zoneadmd	\
	zonecfg		\
	zonename	\
	zpool		\
	zonestat

sparc_MSGSUBDIRS=	\
	fruadm		\
	prtdscp		\
	prtfru		\
	virtinfo	\
	vntsd

i386_MSGSUBDIRS=	\
	ucodeadm

#
# commands that use dcgettext for localized time, LC_TIME
#
DCSUBDIRS=		\
	cal		\
	cfgadm		\
	diff		\
	ls		\
	pr		\
	ps		\
	tar		\
	w		\
	who		\
	whodo		\
	write

#
# commands that belong only to audit.
#
AUDITSUBDIRS=		\
	amt		\
	audit		\
	audit_warn	\
	auditconfig	\
	auditd		\
	auditrecord	\
	auditreduce	\
	auditset	\
	auditstat	\
	praudit		

#
# commands not owned by the systems group
#
BWOSDIRS=


all :=		TARGET = all
install :=	TARGET = install
clean :=	TARGET = clean
clobber :=	TARGET = clobber
_msg :=		TARGET = _msg
_dc :=		TARGET = _dc

.KEEP_STATE:

SUBDIRS = $(COMMON_SUBDIRS) $($(MACH)_SUBDIRS)

.PARALLEL:	$(BWOSDIRS) $(SUBDIRS) $(MSGSUBDIRS) $(AUDITSUBDIRS)

all install clean clobber: $(FIRST_SUBDIRS) .WAIT $(SUBDIRS) \
	$(AUDITSUBDIRS)

#
# Manifests cannot be checked in parallel, because we are using
# the global repository that is in $(SRC)/cmd/svc/seed/global.db.
# For this reason, to avoid .PARALLEL and .NO_PARALLEL conflicts,
# we spawn off a sub-make to perform the non-parallel 'make check'
#
check:
	$(MAKE) -f Makefile.check check

#
# The .WAIT directive works around an apparent bug in parallel make.
# Evidently make was getting the target _msg vs. _dc confused under
# some level of parallelization, causing some of the _dc objects
# not to be built.
#
_msg: $(MSGSUBDIRS) $($(MACH)_MSGSUBDIRS) .WAIT _dc

_dc: $(DCSUBDIRS)

#
# Dependencies
#
fs.d: fstyp
ksh:	shcomp isaexec
mdb: 	terminfo
fmt:	mailx

$(FIRST_SUBDIRS) $(BWOSDIRS) $(SUBDIRS) $(AUDITSUBDIRS): FRC
	@if [ -f $@/Makefile  ]; then \
		cd $@; pwd; $(MAKE) $(TARGET); \
	else \
		true; \
	fi
FRC:
