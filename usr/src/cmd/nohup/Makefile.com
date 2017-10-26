#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License, Version 1.0 only
# (the "License").  You may not use this file except in compliance
# with the License.
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
# Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#
#
# cmd/nohup/Makefile.com
# 

PROG = nohup

OBJS = nohup.o
SRCS = $(OBJS:%.o=../%.c)
LNTS = $(OBJS:%.o=%.ln)
CLEANFILES = $(OBJS) $(LNTS)

include ../../Makefile.cmd


CPPFLAGS += -DXPG4

TARGETS = $(PROG)

.KEEP_STATE:

.PARALLEL: $(OBJS) $(LNTS)

all: $(PROG)

clean:
	$(RM) $(CLEANFILES)

include ../../Makefile.targ

$(PROG): $(OBJS)
	$(LINK.c) -o $@ $(OBJS) $(LDLIBS)
	$(POST_PROCESS)

%.o: ../%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

