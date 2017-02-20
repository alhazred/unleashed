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
# Copyright (c) 2007, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2014 Nexenta Systems, Inc.  All rights reserved.
#

PROG =		test-getdc
OBJS =		getdc_main.o

SRCS =	$(OBJS:%.o=%.c)
POFILES = 	$(OBJS:.o=.po)

include ../../Makefile.cmd

POFILE = 	$(PROG)_all.po

LDLIBS +=	-ladutils -lumem
LDFLAGS +=	-R'$$ORIGIN/../lib'

INCS += -I. -I../../../lib/libadutils/common


$(OBJS) :=	CPPFLAGS += $(INCS)
$(POFILE) :=	CPPFLAGS += $(INCS)

.KEEP_STATE:

all: $(PROG)

$(PROG): $(OBJS) FRC
	$(LINK.c) -g -o $@ $(OBJS) $(LDLIBS)
	$(POST_PROCESS)

$(POFILE): $(POFILES)
	$(RM) $@
	cat $(POFILES) > $@

install: all $(ROOTPROG)

clean:
	$(RM) $(OBJS)

include ../../Makefile.targ

FRC:
