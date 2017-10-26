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
#

PROG= expr

OBJS= expr.o compile.o
SRCS= expr.c compile.c

include ../Makefile.cmd

CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-extra
CFLAGS += -DXPG4 -DXPG6

XGETFLAGS += -a -x expr.xcl

LDLIBS += -lgen

MAPFILES =	$(MAPFILE.INT) $(MAPFILE.NGB)
LDFLAGS +=	$(MAPFILES:%=-Wl,-M%)

.KEEP_STATE:

all: $(PROG)

install: all $(ROOTPROG)

$(PROG): $(OBJS) $(MAPFILES)
	$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

clean:
	$(RM) $(OBJS)


include ../Makefile.targ
