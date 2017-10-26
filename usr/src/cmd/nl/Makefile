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
# Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#
# Message catalog requires no special processing because messages
# only occur in ./nl.c, not in ../expr/compile.c.

PROG= nl

OBJS= $(PROG).o compile.o
SRCS = nl.c ../expr/compile.c

include ../Makefile.cmd

LDLIBS += -lgen
CERRWARN += -Wno-uninitialized
CERRWARN += -Wno-unused-variable
CPPFLAGS += -DXPG4

MAPFILE.INT =	../expr/mapfile-intf
LDFLAGS +=	$(MAPFILE.INT:%=-Wl,-M%)

.KEEP_STATE:

all: $(PROG)

install: all $(ROOTPROG)

$(PROG): $(OBJS) $(MAPFILE.INT)
	$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

%.o:   %.c
	$(COMPILE.c) -o $@ $<

%.o:   ../expr/%.c
	$(COMPILE.c) -o $@ $<

clean:
	$(RM) $(OBJS)


include ../Makefile.targ

