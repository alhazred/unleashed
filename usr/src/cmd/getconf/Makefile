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
# Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

PROG= getconf
XPG4PROG= getconf
XPG6PROG= getconf
X4= objs.xpg4
X6= objs.xpg6
OBJS= getconf.o
XPG4EXOBJS= $(X4)/getconf.o
XPG6EXOBJS= $(X6)/getconf.o

include ../Makefile.cmd

XPG4EXOBJS += values-xpg4.o
# XPG4
$(XPG4) := CFLAGS += -D_XOPEN_SOURCE -I$(SRC)/lib/libc/inc

XPG6MODE = $(C99_ENABLE)
$(XPG6) := C99MODE = $(XPG6MODE)
$(XPG6) := CFLAGS += -D_XOPEN_SOURCE=600 -I$(SRC)/lib/libc/inc

.KEEP_STATE:

all: $(PROG) $(XPG4) $(XPG6)

$(PROG):	$(OBJS)
	$(LINK.c) -o $@ $(OBJS) $(LDLIBS)
	$(POST_PROCESS)

$(XPG4):	$(X4) $(XPG4EXOBJS)
	$(LINK.c) -o $@ $(XPG4EXOBJS) $(LDLIBS)
	$(POST_PROCESS)

$(XPG6):	$(X6) $(XPG6EXOBJS)
	$(LINK.c) -o $@ $(XPG6EXOBJS) $(LDLIBS)
	$(POST_PROCESS)


$(XPG4EXOBJS):	$(X4)


$(X4)/%.o:	%.c
	$(COMPILE.c) -o $@ $<


$(X4):
	-@mkdir -p $@


$(XPG6EXOBJS):	$(X6)


$(X6)/%.o:	%.c
	$(COMPILE.c) -o $@ $<


$(X6):
	-@mkdir -p $@


install: all $(ROOTPROG) $(ROOTXPG4PROG) $(ROOTXPG6PROG)

values-xpg4.o: ../../lib/crt/common/values-xpg4.c
	$(COMPILE.c) -o $@ ../../lib/crt/common/values-xpg4.c

clean:
	-@rm -rf $(OBJS) $(XPG4EXOBJS) $(XPG6EXOBJS) $(X4) $(X6) \
		$(PROG) $(XPG4) $(XPG6)


include ../Makefile.targ
