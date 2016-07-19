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
# Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#
#

DYNPROG = \
	ncalladm

include ../../Makefile.cmd

SUBDIRS=
PROG=	$(DYNPROG)

ncalladm	:= POBJS = ncalladm.o

OBJS=	ncalladm.o
SRCS=	$(OBJS:%.o=%.c)
POFILE=	$(OBJS:%.o=%.po)

LDLIBS += -ldscfg
CFLAGS += $(CCVERBOSE) -D_SYSCALL32 -D__NCALL__
CERRWARN += -_gcc=-Wno-uninitialized
ROOTLINK = $(ROOTLIB)/$(PROG)

all	:=	TARGET= all
install	:=	TARGET= install
clean	:=	TARGET= clean
clobber	:=	TARGET= clobber

.KEEP_STATE:

.PARALLEL: $(OBJS)

all:	$(SUBDIRS)$(PROG) $(POFILE)

install: $(SUBDIRS) all $(ROOTPROG) $(ROOTLINK)


clean:	$(SUBDIRS)
	$(RM) *.o $(POFILE)

$(PROG):	$$(POBJS)
	$(LINK.c) $(POBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

$(SUBDIRS):	FRC
	@cd $@; pwd; $(MAKE) $(TARGET)

$(ROOTLINK): $(ROOTLIB) $(ROOTPROG)
	-$(RM) $@; $(LN) $(ROOTPROG) $@

FRC:

include ../../Makefile.targ
