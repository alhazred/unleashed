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
# Copyright (c) 2012 by Delphix. All rights reserved.
# Copyright 2015, Joyent, Inc.
# Copyright (c) 2013 Gary Mills
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
# Copyright (c) 2015 Gary Mills
# Copyright 2016 Toomas Soome <tsoome@me.com>
# Copyright 2018 Nexenta Systems, Inc.
# Copyright (c) 2016, Chris Fraire <cfraire@me.com>.
#

include ../Makefile.master

#	Note that libcurses installs commands along with its library.
#	This is a minor bug which probably should be fixed.
#	Note also that a few extra libraries are kept in cmd source.
#
# Certain libraries are linked with, hence depend on, other libraries.
# Library dependencies are called out explicitly, see "Library
# interdependencies" below.
.PARALLEL:

# Build libc and its dependencies
SUBDIRS=			\
	crt			\
	../cmd/sgs/libconv	\
	../cmd/sgs/libdl	\
	libc			\
	.WAIT

# Build libraries with lots of dependents
SUBDIRS +=		\
	libm		\
	libmd		\
	libmp		\
	libnsl		\
	libnvpair	\
	.WAIT

# Build everything else. Aside from explicit dependencies, all libraries
# are built in parallel.
SUBDIRS +=				\
	../cmd/sgs/libelf	\
	../cmd/sgs/librtld_db	\
	abi		\
	auditd_plugins	\
	brand		\
	cfgadm_plugins	\
	crypt_modules	\
	extendedFILE	\
	fm		\
	getloginx	\
	gss_mechs/mech_dh	\
	gss_mechs/mech_dummy	\
	gss_mechs/mech_krb5	\
	gss_mechs/mech_spnego	\
	hal		\
	hbaapi		\
	krb5		\
	libadm		\
	libads		\
	libadutils	\
	libaio		\
	libast		\
	libavl		\
	libbe		\
	libbrand	\
	libbsdmalloc	\
	libbsm		\
	libc_db		\
	libcfgadm	\
	libcmd		\
	libcmdutils	\
	libcommputil	\
	libcontract	\
	libcpc		\
	libcrypt	\
	libcryptoutil	\
	libctf		\
	libcustr	\
	libdevice	\
	libdevid	\
	libdevinfo	\
	libdhcpagent	\
	libdhcputil	\
	libdisasm	\
	libdiskmgt	\
	libdladm	\
	libdll		\
	libdlpi		\
	libdns_sd	\
	libdoor		\
	libdtrace	\
	libdwarf	\
	libefi		\
	libelfsign	\
	libeti		\
	libexacct	\
	libexacct/demo	\
	libfcoe		\
	libficl		\
	libfru		\
	libfruutils	\
	libfsmgt	\
	libfstyp	\
	libgen		\
	libgss		\
	libhotplug	\
	libidmap	\
	libilb		\
	libima		\
	libinetsvc	\
	libinetutil	\
	libintl		\
	libipadm	\
	libipd		\
	libipmi		\
	libipmp		\
	libipp		\
	libipsecutil	\
	libiscsit	\
	libkmf		\
	libkrb5		\
	libkstat	\
	libkvm		\
	libldap5	\
	liblgrp		\
	liblm		\
	libmail		\
	libmalloc	\
	libmapid	\
	libmapmalloc	\
	libmd5		\
	libmlrpc	\
	libmtmalloc	\
	libmvec		\
	libndmp		\
	libnisdb	\
	libnls		\
	libnwam		\
	libofmt		\
	libpam		\
	libpcidb	\
	libpctx		\
	libpicl		\
	libpicltree	\
	libpool		\
	libpp		\
	libproc		\
	libproject	\
	libpthread	\
	libraidcfg	\
	librcm		\
	libreparse	\
	libresolv	\
	libresolv2	\
	librestart	\
	librpcsvc	\
	librsm		\
	librstp		\
	librt		\
	libsasl		\
	libscf		\
	libsched	\
	libsctp		\
	libsec		\
	libsecdb	\
	libsendfile	\
	libsff		\
	libshare	\
	libshell	\
	libsip		\
	libsldap	\
	libsmbfs	\
	libsmbios	\
	libsmedia	\
	libsqlite	\
	libsrpt		\
	libstmf		\
	libsum		\
	libsun_ima	\
	libsys		\
	libsysevent	\
	libtecla	\
	libthread	\
	libumem		\
	libuuid		\
	libvolmgt	\
	libvrrpadm	\
	libvscan	\
	libw		\
	libwrap		\
	libzfs		\
	libzfs_core	\
	libzonecfg	\
	libzonestat	\
	libzpool	\
	madv		\
	mpapi		\
	mpss		\
	nametoaddr	\
	ncad_addr	\
	nsswitch	\
	pam_modules	\
	passwdutil	\
	pkcs11		\
	policykit	\
	pylibbe		\
	raidcfg_plugins	\
	rpcsec_gss	\
	sasl_plugins	\
	scsi		\
	smbsrv		\
	smhba		\
	sun_fc		\
	sun_sas		\
	udapl		\
	watchmalloc	\
	$($(MACH)_SUBDIRS)

i386_SUBDIRS=		\
	libfdisk	\
	libsaveargs

#
# Create a special version of $(SUBDIRS) with no .WAIT's, for use with the
# clean and clobber targets (for more information, see those targets, below).
#
NOWAIT_SUBDIRS= $(SUBDIRS:.WAIT=)

DCSUBDIRS=

MSGSUBDIRS=		\
	abi		\
	auditd_plugins	\
	brand		\
	cfgadm_plugins	\
	gss_mechs/mech_dh	\
	gss_mechs/mech_krb5	\
	krb5		\
	libast		\
	libbsm		\
	libc		\
	libcfgadm	\
	libcmd		\
	libcontract	\
	libdhcputil	\
	libdiskmgt	\
	libdladm	\
	libdll		\
	libgss		\
	libidmap	\
	libilb		\
	libipadm	\
	libipmp		\
	libipsecutil	\
	libldap5	\
	libnwam		\
	libofmt		\
	libpam		\
	libpicl		\
	libpool		\
	libpp		\
	libreparse	\
	libsasl		\
	libscf		\
	libsecdb	\
	libshare	\
	libshell	\
	libsldap	\
	libsmbfs	\
	libsmedia	\
	libsum		\
	libvrrpadm	\
	libvscan	\
	libzfs		\
	libzonecfg	\
	madv		\
	mpss		\
	pam_modules	\
	rpcsec_gss	\
	$($(MACH)_MSGSUBDIRS)

i386_MSGSUBDIRS= libfdisk

HDRSUBDIRS=				\
	auditd_plugins	\
	fm		\
	hal		\
	hbaapi		\
	libads		\
	libast		\
	libbrand	\
	libbsm		\
	libc		\
	libcmd		\
	libcmdutils	\
	libcommputil	\
	libcontract	\
	libcpc		\
	libcryptoutil	\
	libctf		\
	libcustr	\
	libdevice	\
	libdevid	\
	libdevinfo	\
	libdhcpagent	\
	libdhcputil	\
	libdisasm	\
	libdiskmgt	\
	libdwarf	\
	libdladm	\
	libdll		\
	libdlpi		\
	libdns_sd	\
	libdtrace	\
	libelfsign	\
	libeti		\
	libfcoe		\
	libfru		\
	libfstyp	\
	libgen		\
	libidmap	\
	libilb		\
	libima		\
	libinetsvc	\
	libinetutil	\
	libipadm	\
	libipd		\
	libipmi		\
	libipmp		\
	libipp		\
	libipsecutil	\
	libiscsit	\
	libkrb5		\
	libkstat	\
	libkvm		\
	libmail		\
	libmapid	\
	libmd		\
	libmlrpc	\
	libmtmalloc	\
	libndmp		\
	libnvpair	\
	libnwam		\
	libofmt		\
	libpam		\
	libpcidb	\
	libpctx		\
	libpicl		\
	libpicltree	\
	libpool		\
	libpp		\
	libproc		\
	libraidcfg	\
	librcm		\
	libreparse	\
	librestart	\
	librpcsvc	\
	librsm		\
	librstp		\
	libsasl		\
	libscf		\
	libsec		\
	libsff		\
	libshare	\
	libshell	\
	libsip		\
	libsmbfs	\
	libsmbios	\
	libsmedia	\
	libsqlite	\
	libsrpt		\
	libstmf		\
	libsum		\
	libsun_ima	\
	libsysevent	\
	libtecla	\
	libumem		\
	libvolmgt	\
	libvrrpadm	\
	libvscan	\
	libwrap		\
	libzfs		\
	libzfs_core	\
	libzonestat	\
	mpapi		\
	passwdutil	\
	pkcs11		\
	policykit	\
	scsi		\
	smbsrv		\
	smhba		\
	udapl		\
	$($(MACH)_HDRSUBDIRS)

i386_HDRSUBDIRS=	\
	libfdisk	\
	libsaveargs

all :=		TARGET= all
check :=	TARGET= check
clean :=	TARGET= clean
clobber :=	TARGET= clobber
install :=	TARGET= install
install_h :=	TARGET= install_h
_dc :=		TARGET= _dc
_msg :=		TARGET= _msg

.KEEP_STATE:

#
# For the all and install targets, we clearly must respect library
# dependencies so that the libraries link correctly.  However, for
# the remaining targets (check, clean, clobber, install_h, _dc, and
# _msg), libraries do not have any dependencies on one another and
# thus respecting dependencies just slows down the build.  As such,
# for these rules, we use pattern replacement to explicitly avoid
# triggering the dependency information.  Note that for clean and
# clobber, we must use $(NOWAIT_SUBDIRS) rather than $(SUBDIRS),
# to prevent `.WAIT' from expanding to `.WAIT-nodepend'.
#

all:			$(SUBDIRS)

install:		$(SUBDIRS) .WAIT install_extra

# extra libraries kept in other source areas
install_extra:
	@cd ../cmd/sgs; pwd; $(MAKE) install_lib
	@pwd

clean clobber:	$(NOWAIT_SUBDIRS:%=%-nodepend)

install_h check:	$(HDRSUBDIRS:%=%-nodepend)

_msg:			$(MSGSUBDIRS:%=%-nodepend) .WAIT _dc

_dc:			$(DCSUBDIRS:%=%-nodepend)

# Library interdependencies are called out explicitly here.
../cmd/sgs/libconv: crt
../cmd/sgs/libdl: crt
libc:		../cmd/sgs/libconv ../cmd/sgs/libdl

libm:		libc
libmd:		libc
libmp:		libc
libnsl:		libc libmd libmp
libnvpair:	libc libnsl

basedeps:	libc libm libmd libmp libnsl libnvpair
basedeps :=	TARGET=install

# The following dependencies are currently required by libdbus-1 and should
# be used for anything linking against it.
dbusdeps:	libsecdb libinetutil libscf libgen libsmbios

# The following dependencies don't need to be listed below as they are built
# before everything else:
# libc libm libmd libmp libnsl libnvpair
abi:		libctf libmapmalloc libproc
auditd_plugins: libbsm libsecdb libgss libmtmalloc
brand:		libzonecfg libmapmalloc
cfgadm_plugins:	libdevice libdevinfo libhotplug librcm hbaapi libkstat libscf
fm:		libexacct libipmi libzfs scsi libdevinfo libdevid libcfgadm \
		libcontract libsysevent ../cmd/sgs/libelf libdladm libsff \
		libpcidb
hal:		libbsm
gss_mechs/mech_dh: libgss
gss_mechs/mech_dummy: libgss
gss_mechs/mech_krb5: libgss libresolv2 pkcs11 libkstat
gss_mechs/mech_spnego: gss_mechs/mech_krb5
hal:		dbusdeps
krb5:		gss_mechs/mech_krb5 libtecla libldap5
libadutils:	libldap5 libresolv2
libbe:		libzfs libzonecfg libuuid libgen libdevinfo libefi libficl
libbsm:		libinetutil libscf libsecdb
libcfgadm:	libdevinfo
libcmd:		libsum libast
libcmdutils:	libavl
libcpc:		libpctx
libcrypt:	libgen
libctf:		libdwarf
libdevid:	libdevinfo
libdevinfo:	libsec libgen
libdhcpagent:	libdhcputil libuuid libdlpi libcontract
libdhcputil:	libgen libinetutil libdlpi
libdiskmgt:	libdevid libdevinfo libadm libefi libkstat libsysevent
$(INTEL_BLD)libdiskmgt: libfdisk
libdladm:	libdevinfo libinetutil libscf librcm libexacct libkstat \
		libpool
libdll:		libast
libdlpi:	libinetutil libdladm
libdtrace:	libproc libgen libctf libmapmalloc
libefi:		libuuid libsmbios
libelfsign:	libcryptoutil libkmf ../cmd/sgs/libelf
libexacct/demo:	libexacct libproject
libfcoe:	libdladm
libficl:	libuuid libumem
libfru:		libfruutils
libfsmgt:	libkstat
libidmap:	libavl
libinetsvc:	libscf
libipadm:	libinetutil libdlpi libdhcpagent libdladm libsecdb libdhcputil
libipmp:	libinetutil
libipsecutil:	libtecla
libiscsit:	libstmf libuuid
libkmf:		libcryptoutil pkcs11
libkvm:		../cmd/sgs/libelf
libldap5:	libsasl
libmapid:	libresolv2 libscf
libmlrpc:	libsmbfs libuuid
libndmp:	libscf
libnisdb:	libldap5
libnwam:	libscf libbsm libdladm libipadm
libpctx:	libproc
libpool:	libscf libexacct
libpp:		libast
libproc:	../cmd/sgs/librtld_db ../cmd/sgs/libelf libctf
$(INTEL_BLD)libproc: libsaveargs
libproject:	libpool libproc libsecdb
libraidcfg:	libdevinfo
librestart:	libscf libpool libproject libsecdb libsysevent
libsasl:	libgss pkcs11
libsaveargs:	libdisasm
libscf:		libgen libsmbios
libsec:		libavl libidmap
libsff:		libnvpair
libshare:	libscf libzfs libuuid libfsmgt libsecdb libumem libsmbfs
libshell:	libast libcmd libdll libsecdb
libsip:		libmd5
libsldap:	libldap5 libscf
libsmbfs:	libkrb5 libsec libidmap pkcs11
libsmbios:	libdevinfo
libsrpt:	libstmf
libstmf:	libscf
libsum:		libast
libsun_ima:	libdevinfo libsysevent
libsysevent:	libsecdb
libuuid:	libdlpi
libvolmgt:	libadm
libvrrpadm:	libdladm libscf
libvscan:	libscf libsecdb
libzfs:		libdevid libgen libadm libavl libefi libidmap \
		libumem libzfs_core
libzonecfg:	libuuid libsysevent libsec libbrand libpool libscf libproc \
		libbsm libsecdb
libzonestat:	libcmdutils libumem
libzpool:	libavl libumem libcmdutils libsysevent
madv:		libgen
mpapi:		libpthread libdevinfo libsysevent
mpss:		libgen
nsswitch:	libadutils libidmap libdns_sd libscf libldap5 libsldap
pam_modules:	libproject passwdutil smbsrv libpam libbsm libsecdb
passwdutil:	libsldap
pkcs11:		libcryptoutil libgen libuuid
policykit:	dbusdeps
pylibbe:	libbe libzfs
raidcfg_plugins: libraidcfg librcm libcfgadm libpicl libpicltree
rpcsec_gss:	libgss
sasl_plugins:	pkcs11 libgss libsasl
scsi:		libfru libumem libdevid libdevinfo
smbsrv:		libpthread librt libshare libidmap pkcs11 libsqlite \
		libcryptoutil libreparse libcmdutils libresolv2 libsmbfs \
		libuuid libads libgss libldap5 krb5 libmlrpc
sun_fc:		libdevinfo libsysevent
sun_sas:	libdevinfo libsysevent libkstat libdevid
udapl:		libdevinfo libdladm

#
# The reason this rule checks for the existence of the
# Makefile is that some of the directories do not exist
# in certain situations (e.g., exportable source builds,
# OpenSolaris).
#
$(SUBDIRS): FRC
	@if [ -f $@/Makefile  ]; then \
		cd $@; pwd; $(MAKE) $(TARGET); \
	else \
		true; \
	fi

$(SUBDIRS:%=%-nodepend):
	@if [ -f $(@:%-nodepend=%)/Makefile  ]; then \
		cd $(@:%-nodepend=%); pwd; $(MAKE) $(TARGET); \
	else \
		true; \
	fi

FRC:
