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
# Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

PROG= bc
EXOBJS= bc.o

LIBB= lib.b
CLOBBERFILES= $(LIBB)

include ../Makefile.cmd

CERRWARN += -Wno-implicit-function-declaration
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-unused-label

CFLAGS += -DXPG6
ROOTLIBB= $(LIBB:%=$(ROOTLIB)/%)
$(ROOTLIBB) :=	FILEMODE =	$(LIBFILEMODE)

.KEEP_STATE:

all: $(PROG) $(LIBB) 

$(PROG): $(EXOBJS)
	$(LINK.c) -o $@ $(EXOBJS) $(LDLIBS)
	$(POST_PROCESS)


$(LIBB): $(LIBB).data
	-$(RM) $(LIBB)
	$(CP) $(LIBB).data $(LIBB)

install: all $(ROOTPROG) $(ROOTLIBB)

clean:
	-@rm -rf $(PROG).c $(PROG).x $(EXOBJS) $(PROG)

include ../Makefile.targ
