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
# Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

include $(SRC)/cmd/Makefile.cmd

.KEEP_STATE:

ROOTOPTPKG = $(ROOT)/opt/SUNWdtrt
ROOTTST = $(ROOTOPTPKG)/tst
SUBDIR :sh= basename `pwd`
TSTDIR = $(ROOTTST)/$(SUBDIR)
DSTYLE = $(ROOTOPTPKG)/bin/dstyle

EXES += $(CSRCS:%.c=%.exe)
EXES += $(SSRCS:%.s=%.exe)

ROOT_TSTS = $(TSTS:%=$(TSTDIR)/%)
ROOT_EXES = $(EXES:%=$(TSTDIR)/%)

$(ROOT_TSTS) := FILEMODE = 0444
$(ROOT_EXES) := FILEMODE = 0555

# The DTrace tests rely on "normal" behaviour from the compiler which
# agressive optimization of small, simple, one compilation-unit programs may
# utterly subvert.  We force the compiler to not optimize rather than engage
# in an arms race with increasingly belligerent optimizers.
COPTFLAG=	-O0

CERRWARN +=	-Wno-switch
CERRWARN +=	-Wno-unused-variable
CERRWARN +=	-Wno-implicit-function-declaration
CERRWARN +=	-Wno-unused-function
CERRWARN +=	-Wno-unused-variable

all: $(EXES)

clean:

clobber: FRC
	-$(RM) $(CSRCS:%.c=%.exe) $(CSRCS:%.c=%.o)
	-$(RM) $(SSRCS:%.s=%.exe) $(SSRCS:%.s=%.o)
	-$(RM) $(DSRCS:%.d=%.o)
	-$(RM) $(CLOBBERFILES)

install: $(ROOT_TSTS) $(ROOT_EXES)

$(ROOT_TSTS): $(TSTDIR)

$(ROOT_EXES): $(TSTDIR)

$(TSTDIR):
	$(INS.dir)

$(TSTDIR)/%: %
	$(INS) -d -m $(DIRMODE) $(@D)
	$(INS.file)

%.exe: %.c
	$(LINK.c) -o $@ $< $(LDLIBS)
	$(POST_PROCESS) ; $(STRIP_STABS)

%.exe: %.o
	$(LINK.c) -o $@ $< $(LDLIBS)
	$(POST_PROCESS) ; $(STRIP_STABS)

%.o: %.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

%.o: %.s
	$(COMPILE.s) -o $@ $<
	$(POST_PROCESS_O)

scripts: FRC
	@cd ../cmd/scripts; pwd; $(MAKE) install

dstyle: FRC
	@if [ -n "$(DSRCS)" ]; then $(DSTYLE) $(DSRCS); fi

FRC:
