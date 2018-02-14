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

PROG=		m4

include 	../../../Makefile.cmd
include		../../../sgs/Makefile.var

OBJS=		m4.o  m4ext.o  m4macs.o m4y_xpg4.o

INCLIST=	-I../common
DEFLIST=	-DELF
CPPFLAGS=	$(INCLIST) $(DEFLIST) $(CPPFLAGS.master)
C99MODE=	$(C99_ENABLE)

CERRWARN +=	-Wno-parentheses
CERRWARN +=	-Wno-unused-label
CERRWARN +=	-Wno-uninitialized

CPPFLAGS += -DXPG4

%.o: ../common/%.c
		$(COMPILE.c) -o $@ $<

.KEEP_STATE:

all:		$(PROG)

$(PROG):	$(OBJS)
		$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
		$(POST_PROCESS)


install:	all $(VAR_SGSBINPROG)

clean:
		$(RM) $(OBJS)

include		../../../Makefile.targ
