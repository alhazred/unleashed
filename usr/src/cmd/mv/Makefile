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

PROG=		mv
OBJS1=		mv.o
OBJS=           $(OBJS1) getresponse.o values-xpg4.o
CPFILE=		cp
LNFILE=		ln
SRCS=		$(OBJS1:%.o=%.c) $(SRC)/common/util/getresponse.c

ROOTLINKS= $(ROOTBIN)/$(CPFILE) $(ROOTBIN)/$(LNFILE)

include ../Makefile.cmd

CLOBBERFILES += $(CPFILE) $(LNFILE)
CERRWARN +=	-Wno-parentheses
CERRWARN +=	-Wno-unused-variable
CERRWARN +=	-Wno-uninitialized
CFLAGS += -DXPG4
XGETFLAGS += -a -x mv.xcl
CPPFLAGS += -D_FILE_OFFSET_BITS=64 -I $(SRC)/common/util

$(PROG) := LDLIBS += -lcmdutils -lavl -lsec -lnvpair

.KEEP_STATE:

all:	$(PROG) $(CPFILE) $(LNFILE)

$(PROG):	$$(OBJS)
	$(LINK.c) -o $@ $(OBJS) $(LDLIBS)
	$(POST_PROCESS)

%.o:    $(SRC)/common/util/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POST_PROCESS_O)

%values-xpg4.o:		../../lib/crt/common/values-xpg4.c
	$(COMPILE.c) -o $@ ../../lib/crt/common/values-xpg4.c

$(CPFILE):	$(PROG)
	@$(RM) $(CPFILE); $(LN) $(PROG) $(CPFILE)

$(LNFILE):	$(PROG)
	@$(RM) $(LNFILE); $(LN) $(PROG) $(LNFILE)

install: all $(ROOTLINKS)

$(ROOTLINKS):	$(ROOTPROG)
	$(RM) $@
	$(LN) $(ROOTPROG) $@

clean:
	$(RM) $(OBJS)


include ../Makefile.targ
