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

PROG= ed

EDOBJ= ed.o compile.o
SRCS= ./ed.c ../expr/compile.c 
OBJS = $(EDOBJ) values-xpg6.o
POFILES= $(EDOBJ:%.o=%.po)

include ../Makefile.cmd

CERRWARN += -Wno-parentheses
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-unused-function
CERRWARN += -Wno-uninitialized
CFLAGS += -DXPG4 -DXPG6 -I$(SRC)/lib/libc/inc
LDLIBS += -lmapmalloc -lgen -lcrypt_i
XGETFLAGS += -a -x ed.xcl

MAPFILE.INT =	../expr/mapfile-intf
LDFLAGS +=	$(MAPFILE.INT:%=-Wl,-M%)

POFILE= ed_cmd.po

%.po: ../expr/%.c
	$(COMPILE.cpp) $< > $*.c.i
	$(XGETTEXT) $(XGETFLAGS) $*.c.i
	$(MV) messages.po $@

.KEEP_STATE:

all:	$(PROG)

$(PROG): $(OBJS) $(MAPFILE.INT)
	$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

compile.o:	../expr/compile.c 
	$(COMPILE.c) -o $@ ../expr/compile.c

$(POFILE): $(POFILES)
	$(RM) $@
	cat $(POFILES) > $@

install: all $(ROOTPROG)
	$(RM) $(ROOTBIN)/red
	$(LN) $(ROOTBIN)/ed $(ROOTBIN)/red

values-xpg6.o: ../../lib/crt/common/values-xpg6.c
	$(COMPILE.c) -o $@ ../../lib/crt/common/values-xpg6.c

clean:
	-@rm -rf $(OBJS) \
		compile.c.i


include ../Makefile.targ
