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
# Copyright (c) 1990, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
#

SYNCPROG=	syncinit syncloop syncstat

PROG=		6to4relay arp if_mpadm \
		in.comsat in.fingerd in.rarpd \
		in.rwhod in.telnetd in.tftpd ipaddrsel \
		ndp $(SYNCPROG)

MANIFEST=	rarp.xml telnet.xml comsat.xml finger.xml
SVCMETHOD=	svc-sockfilter

ROOTFS_PROG=	hostconfig route soconfig
SBINLINKS=	hostconfig route

RPCSVCPROG=	hostconfig
AUDITPROG=	in.telnetd
PAMPROG=	in.telnetd
SOCKETPROG=	6to4relay arp hostconfig if_mpadm in.comsat \
		in.fingerd in.rarpd \
		in.rwhod in.telnetd in.tftpd ipaddrsel ndp route
NSLPROG=	6to4relay arp hostconfig in.comsat in.rarpd \
		in.rwhod in.telnetd \
		in.tftpd ipaddrsel ndp route
CMDPROG=	in.telnetd
K5PROGS=	in.telnetd
DLADMPROG=	6to4relay
DEFAULTFILES=	telnetd.dfl

PROGSRCS=	$(PROG:%=%.c)
TFTPDOBJS=	in.tftpd.o tftpsubs.o
OTHERSRC=	../usr.bin/tftp/tftpsubs.c
K5TELNETOBJS=	in.telnetd.o
SRCS=		$(PROGSRCS) $(OTHERSRC)

SUBDIRS=	ifconfig ilbadm in.rdisc in.routed \
		in.talkd inetadm inetconv ipadm ipmpstat ipqosconf ipsecutils \
		nwamadm nwamcfg ping \
		snoop sppptun traceroute wificonfig

MSGSUBDIRS=	ifconfig ilbadm in.routed in.talkd \
		inetadm inetconv ipadm ipmpstat ipqosconf ipsecutils \
		nwamadm nwamcfg sppptun snoop wificonfig

include ../../Makefile.cmd
ROOTMANIFESTDIR=	$(ROOTSVCNETWORK)
$(ROOTMANIFEST)		:= FILEMODE= 444
include ../Makefile.cmd-inet

ROOTSBINPROG = $(ROOTFS_PROG:%=$(ROOTSBIN)/%)
ROOTUSRSBINLINKS = $(SBINLINKS:%=$(ROOTUSRSBIN)/%)

CERRWARN += -Wno-implicit-function-declaration
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-char-subscripts
CERRWARN += -Wno-extra
CERRWARN += -Wno-address

#
# Message catalog
#
POFILES=	6to4relay.po if_mpadm.po in.comsat.po ipaddrsel.po route.po \
		soconfig.po
POFILE=		usr.sbin.po

all:=		TARGET= all
install:=	TARGET= install
clean:=		TARGET= clean
clobber:=	TARGET= clobber
_msg:=		TARGET= _msg

CLOBBERFILES +=	$(ROOTFS_PROG) $(PROG)
CLEANFILES += $(TFTPDOBJS) $(K5TELNETOBJS)

CPPFLAGS +=	-DSYSV -DBSD_COMP -I$(CMDINETCOMMONDIR) -I

include $(SRC)/lib/gss_mechs/mech_krb5/Makefile.mech_krb5
K5LIBS=

$(SYNCPROG)		:=	LDLIBS += -ldlpi
$(AUDITPROG)		:=	LDLIBS += -lbsm
$(PAMPROG)		:=	LDLIBS += -lpam
$(RPCSVCPROG)		:=	LDLIBS += -lrpcsvc
$(K5PROGS)		:=	LDFLAGS += $(KRUNPATH) \
				-L$(ROOT)$(KLIBDIR_DO) -L$(ROOT)$(KLIBDIR_GL)
$(K5PROGS)		:=	K5LIBS=  -lmech_krb5
$(K5PROGS)		:=	CPPFLAGS += -I$(CODEMGR_WS)/include \
				-I$(SRC)/uts/common/ \
				-I$(SRC)/uts/common/gssapi/mechs/krb5/include \
				-I$(SRC)/lib/gss_mechs/mech_krb5/include \
				-I$(SRC)/lib/pam_modules/krb5
LDLIBS +=	$(K5LIBS)
$(DLADMPROG)		:=	LDLIBS += -ldladm

in.rarpd		:=	LDLIBS += -linetutil -ldlpi
if_mpadm		:=	LDLIBS += -linetutil -lipmp
if_mpadm.po		:=	XGETFLAGS += -a
route			:=	CPPFLAGS += -DNDEBUG
in.comsat		:=	LDFLAGS += $(MAPFILE.NGB:%=-Wl,-M%)

.KEEP_STATE:

.PARALLEL:

all: $(PROG) $(ROOTFS_PROG) $(SUBDIRS) THIRDPARTYLICENSE.arp

#
# message catalog
#
_msg: $(MSGSUBDIRS) $(POFILE)

syncutil: $(SYNCPROG)

$(POFILE): $(POFILES)
	$(RM) $@
	cat $(POFILES) > $@

%.o: $(CMDINETCOMMONDIR)/%.c
	$(COMPILE.c) -o $@ $<

in.telnetd: $(K5TELNETOBJS)
	$(LINK.c) $(K5TELNETOBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

in.tftpd: $(TFTPDOBJS)
	$(LINK.c) $(TFTPDOBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

tftpsubs.o: $(OTHERSRC)
	$(COMPILE.c) $(OTHERSRC) -o $@
	$(POST_PROCESS_O)

$(ROOTUSRSBINLINKS):
	-$(RM) $@; $(SYMLINK) ../../sbin/$(@F) $@

install: $(PROG) $(ROOTFS_PROG) $(SUBDIRS) .WAIT $(ROOTUSRSBINPROG) \
	$(ROOTSBINPROG) $(ROOTUSRSBINLINKS) $(ROOTETCDEFAULTFILES) \
	$(ROOTMANIFEST) $(ROOTSVCMETHOD) THIRDPARTYLICENSE.arp

THIRDPARTYLICENSE.arp: arp.c
	$(SED) -n '/University of California/,/SUCH DAMAGE/p' arp.c > $@

CLOBBERFILES += THIRDPARTYLICENSE.arp

#
# The reason this rule checks for the existence of the
# Makefile is that some of the directories do not exist
# in our exportable source builds.
#
$(SUBDIRS): FRC
	@if [ -f $@/Makefile  ]; then \
		cd $@; pwd; $(MAKE) $(TARGET); \
	else \
		true; \
	fi

FRC:

check:	$(CHKMANIFEST)

clean: $(SUBDIRS) clean_local
clean_local:
	$(RM) $(CLEANFILES)

clobber: $(SUBDIRS) clobber_local
clobber_local: clean_local
	$(RM) $(CLOBBERFILES)
