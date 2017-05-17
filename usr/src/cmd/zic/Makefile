#
# Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
# Copyright 2016-2017 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
#

PROG= zic
KSHPROG= tzselect

OBJS= zic.o scheck.o ialloc.o
SRCS= $(OBJS:%.o=%.c)

CLOBBERFILES= $(KSHPROG)

include ../Makefile.cmd

CERRWARN +=	-Wno-unused-variable
CERRWARN +=	-Wno-uninitialized

ROOTKSHPROG=	$(ROOTBIN)/$(KSHPROG)

.KEEP_STATE:

all: $(PROG) $(KSHPROG)

install: all $(ROOTUSRSBINPROG) $(ROOTKSHPROG)

clean:
	$(RM) $(OBJS)

check:

$(PROG): $(OBJS)
	$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

include ../Makefile.targ
