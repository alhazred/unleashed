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
# Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

include		../Makefile.cmd

PROG1 =		refer addbib lookbib sortbib
PROG2 =		roffbib indxbib
PROG3 =		mkey inv hunt
PROG =		$(PROG1) $(PROG2)

OBJS =		addbib.o deliv2.o glue1.o glue2.o glue3.o glue4.o	\
		glue5.o hunt1.o hunt2.o hunt3.o hunt5.o hunt6.o		\
		hunt7.o hunt8.o inv1.o inv2.o inv3.o		\
		inv5.o inv6.o lookbib.o mkey1.o mkey2.o mkey3.o		\
		refer..o refer0.o refer1.o refer2.o refer3.o refer4.o	\
		refer5.o refer6.o refer7.o refer8.o shell.o sortbib.o	\
		tick.o types.o

#
# for messaging catalog
#
POFILES= $(OBJS:%.o=%.po)
POFILE= refer.po

CERRWARN += -Wno-implicit-function-declaration
CERRWARN += -Wno-parentheses
CERRWARN += -Wno-unused-variable
CERRWARN += -Wno-uninitialized

CLOBBERFILES += $(PROG3)

addbib.po :=	XGETFLAGS += -a -x addbib.xcl


OTXTS =		tmac.bib

REFDIR =	$(ROOTLIB)/refer
TMACDIR =	$(ROOTSHLIB)/tmac
#Don't re-install dir already installed by Targetdirs
#ROOTDIRS =	$(REFDIR) $(TMACDIR)
ROOTDIRS =     $(REFDIR)

SUBDIRS =	papers

REFDIRPROG =	$(PROG3:%=$(REFDIR)/%)
IOTXTS =	$(OTXTS:%=$(TMACDIR)/%)

$(IOTXTS):=	FILEMODE = 0644

all:=		TARGET=	all
install:=	TARGET=	install
clean:=		TARGET=	clean
clobber:=	TARGET=	clobber

mkey:=		POBJS=  mkey1.o mkey2.o mkey3.o deliv2.o
inv:=		POBJS= inv1.o inv2.o inv3.o inv5.o inv6.o deliv2.o
hunt:=		POBJS= hunt1.o hunt2.o hunt3.o hunt5.o hunt6.o		\
			hunt7.o glue5.o refer3.o shell.o	\
			deliv2.o hunt8.o glue4.o tick.o
refer:=		POBJS= glue1.o refer1.o refer2.o refer4.o refer5.o	\
			refer6.o mkey3.o refer7.o refer8.o hunt2.o	\
			hunt3.o deliv2.o hunt5.o hunt6.o hunt8.o	\
			glue3.o hunt7.o glue2.o glue4.o		\
			glue5.o refer0.o shell.o
addbib:=	POBJS= addbib.o
lookbib:=	POBJS= lookbib.o
sortbib:=	POBJS= sortbib.o

.KEEP_STATE :

.PARALLEL: $(OBJS)

all:		$(PROG) $(PROG3) $(OTXTS)

$(PROG1) $(PROG3): $$(POBJS)
		$(LINK.c) -o $@ $(POBJS) $(LDFLAGS) $(LDLIBS)
		$(POST_PROCESS)

$(POFILE):      $(POFILES)
	$(RM)	$@
	cat     $(POFILES)      > $@

types.po:
	touch types.po


install:	all .WAIT $(ROOTDIRS) \
		$(ROOTPROG) $(REFDIRPROG) $(IOTXTS) $(SUBDIRS)

$(REFDIR)/% :	%
		$(INS.file)

$(TMACDIR)/% :	%
		$(INS.file)

clean :		$(SUBDIRS)
		$(RM) $(OBJS)

clobber :	$(SUBDIRS) local_clobber

local_clobber:
		$(RM) $(PROG) $(OBJS) $(CLOBBERFILES)

strip:

$(SUBDIRS):	FRC
		@cd $@; pwd; $(MAKE) $(TARGET)

$(ROOTDIRS) :
		$(INS.dir)

FRC:

include ../Makefile.targ
