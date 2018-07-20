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
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
# Copyright 2016 Toomas Soome <tsoome@me.com>
# Copyright (c) 2016, Chris Fraire <cfraire@me.com>.
#

include ../Makefile.master

COMMON_SUBDIRS= \
	codereview \
	ctf \
	ndrgen \
	onbld \
	protocmp \
	protolist \
	scripts

sparc_SUBDIRS= \
	chk4ubin \
	stabs \
	tokenize

i386_SUBDIRS=		\
	elfextract	\
	btxld

SUBDIRS= \
	$($(MACH)_SUBDIRS) \
	$(COMMON_SUBDIRS)

include Makefile.tools

ROOTDIRS= \
	$(ROOTOPT) \
	$(ROOTONBLD) \
	$(ROOTONBLD)/bin \
	$(ROOTONBLD)/bin/$(MACH) \
	$(ROOTONBLD)/lib \
	$(ROOTONBLD)/lib/$(MACH) \
	$(ROOTONBLD)/lib/perl \
	$(ROOTONBLD)/lib/python$(PYTHON_VERSION) \
	$(ROOTONBLD)/lib/python$(PYTHON_VERSION)/onbld \
	$(ROOTONBLD)/lib/python$(PYTHON_VERSION)/onbld/Checks \
	$(ROOTONBLD)/lib/python$(PYTHON_VERSION)/onbld/Scm \
	$(ROOTONBLD)/etc \
	$(ROOTONBLD)/etc/exception_lists \
	$(ROOTONBLD)/man \
	$(ROOTONBLD)/man/man1onbld

all :=		TARGET= install
install :=	TARGET= install
clean :=	TARGET= clean
clobber :=	TARGET= clobber
_msg :=		TARGET= _msg

.KEEP_STATE:

#
# Only create directories in the tools proto area when doing an actual
# build, not a clean or clobber.
#
DOROOTDIRS= $(ROOTDIRS)
clobber:= DOROOTDIRS=
clean:= DOROOTDIRS=

DOROOTONBLDLIBPY= $(ROOTONBLDLIBPY)
clobber:= DOROOTONBLDLIBPY=
clean:= DOROOTONBLDLIBPY=

all install: $(SUBDIRS)

clean: $(SUBDIRS)

clobber: $(SUBDIRS)
	$(RM) -rf $(TOOLS_PROTO)

_msg: $(MSGSUBDIRS)

.PARALLEL: $(SUBDIRS)

$(BOOT_SUBDIRS) $(SUBDIRS): $$(DOROOTDIRS) $$(DOROOTONBLDLIBPY) FRC
	@cd $@; pwd; $(MAKE) $(TARGET)

# Assume we don't have the install.bin available yet
$(ROOTDIRS):
	$(MKDIR) -p -m $(DIRMODE) $@

$(ROOTONBLDLIBPY): $(ROOTDIRS)
	$(RM) -r $@; $(SYMLINK) python$(PYTHON_VERSION) $@

FRC:
