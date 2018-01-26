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
# Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
#

include ../Makefile.lib

SUBDIRS =	help $(MACH)
$(BUILD64)SUBDIRS += $(MACH64)

MSGSUBDIRS=	help

all:=		TARGET= all
clean:=		TARGET= clean
clobber:=	TARGET= clobber
install:=	TARGET= install
_msg:=		TARGET= _msg

.KEEP_STATE:

ROOTETC = 		$(ROOT)/etc
ROOTETCSECURITY =	$(ROOTETC)/security

EFILES =	user_attr
EFILESRC =	$(EFILES:%=%)
ETCFILES =	$(EFILES:%=$(ROOTETC)/%)
ETCDFILES =	$(EFILES:%=$(ROOTETC)/%.d/core-os)

ESDFILES =	auth_attr exec_attr prof_attr
ESFILES =		$(ESDFILES) policy.conf
ESSRC =			$(ESFILES:%=%)
ETCSECURITYFILES =	$(ESFILES:%=$(ROOTETCSECURITY)/%)
ETCSECURITYDFILES =	$(ESDFILES:%=$(ROOTETCSECURITY)/%.d/core-os)

all clean clobber delete install package: $(SUBDIRS)
_msg: $(MSGSUBDIRS)

install:	install_data

install_data:	$(ETCSECURITYFILES) $(ETCSECURITYDFILES) $(ETCFILES) \
		$(ETCDFILES)
		

ATTR_FILES =	auth_attr exec_attr prof_attr user_attr
${ATTR_FILES}:	$$@.txt
	cp $@.txt $@

$(ETCSECURITYDFILES) $(ETCDFILES): $$(@D)

$(ETCSECURITYFILES) $(ETCFILES): $(ETCSECURITY) $(ROOTETCSECURITY)

$(ETCSECURITY)/%: %
	$(INS.file)

$(ROOTETCSECURITY):
	$(INS.dir)

$(ROOTETCSECURITY)/%: %
	$(INS.file)

$(ROOTETCSECURITY)/%.d/core-os: %
	$(INS.rename)

$(ROOTETC)/% : %
	$(INS.file)

$(ROOTETC)/%.d/core-os: %
	$(INS.rename)

$(INSTALL_DIR):
	$(INS.dir)

clobber clean:
	$(RM) $(ATTR_FILES)

$(SUBDIRS):	FRC
	@cd $@; pwd; $(MAKE) $(TARGET)

FRC:
