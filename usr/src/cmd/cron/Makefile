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
# Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

DEFAULTFILES =	cron.dfl

include		../Makefile.cmd

MANIFEST =	cron.xml

ROOTMANIFESTDIR = $(ROOTSVCSYSTEM)
ROOTMETHOD =	$(ROOTLIBSVCMETHOD)/svc-cron

CPPFLAGS +=	-D_FILE_OFFSET_BITS=64 -I $(SRC)/common/util
CERRWARN +=	-Wno-parentheses
CERRWARN +=	-Wno-unused-label
CERRWARN +=	-Wno-uninitialized
CERRWARN +=	-Wno-unused-variable
CERRWARN +=	-Wno-unused-value

CFLAGS += -DXPG4

ROOTVAR =	$(ROOT)/var

ROOTSPCRON =	$(ROOTVAR)/spool/cron
ROOTCROND =	$(ROOTETC)/cron.d
ROOTCRONTABS =	$(ROOTSPCRON)/crontabs
ROOTATJOBS =	$(ROOTSPCRON)/atjobs
ROOTLIBCRON =	$(ROOTLIB)/cron

PROG1 =		cron
PROG2 =		at atq atrm crontab
PROG =		$(PROG1) $(PROG2)

SCRIPT =	batch

POFILE=		$(PROG1)_cmd.po
POFILES1=	at_cmd.po crontab.po funcs.po batch.po
POFILES=	$(POFILES1) atrm.po
$(POFILES1) :=	XGETFLAGS= -a -x $(PROG1).xcl

ROOTDIRS =	$(ROOTSPCRON) $(ROOTCROND) \
		$(ROOTCRONTABS) $(ROOTATJOBS)

ROOTPROG =	$(PROG1:%=$(ROOTUSRSBIN)/%) $(PROG2:%=$(ROOTBIN)/%)	\
		$(SCRIPT:%=$(ROOTBIN)/%)

ROOTSYMLINK =	$(ROOTLIBCRON) $(ROOTETC)/cron

GETRESPSRC=	$(SRC)/common/util/getresponse.c
GETRESPOBJ=	getresponse.o
COMMONOBJ1=	permit.o
COMMONOBJ2=	funcs.o
COMMONOBJS=	$(COMMONOBJ1) $(COMMONOBJ2)
CRONOBJS=	cron.o elm.o
ATOBJS=		at_cmd.o att1.o att2.o values-xpg4.o
ATRMOBJS1=	atrm.o
ATRMOBJS=	$(ATRMOBJS1) $(GETRESPOBJ)
ATQOBJS=	atq.o
CRONTABOBJS1=	crontab.o
CRONTABOBJS=	$(CRONTABOBJS1) $(GETRESPOBJ)

cron :=		POBJS = $(CRONOBJS) $(COMMONOBJ2)
at :=		POBJS = $(ATOBJS) $(COMMONOBJS)
atrm :=		POBJS = $(ATRMOBJS) $(COMMONOBJS)
atq :=		POBJS = $(ATQOBJS) $(COMMONOBJS)
crontab :=	POBJS = $(CRONTABOBJS) $(COMMONOBJS)


NOBJS=		$(CRONOBJS) $(ATOBJS) $(ATRMOBJS1) $(ATQOBJS) $(CRONTABOBJS1) \
		$(COMMONOBJS)
OBJS =		$(NOBJS) $(GETRESPOBJ)

SRCS =		$(NOBJS:%.o=%.c) $(GETRESPSRC)

CLOBBERFILES +=	$(SCRIPT)

$(ROOTLIBCRON) :=		SYMLNKDEST = ../../etc/cron.d
$(ROOTETC)/cron :=		SYMLNKDEST = ../usr/sbin/cron

$(ROOTBIN)/at :=		FILEMODE = 04755
$(ROOTBIN)/atrm :=		FILEMODE = 04755
$(ROOTBIN)/atq :=		FILEMODE = 04755
$(ROOTBIN)/crontab :=		FILEMODE = 04555
$(ROOTUSRSBIN)/cron :=		FILEMODE = 0555

LDLIBS +=	-lbsm

at :=		LDLIBS += -lproject -lsecdb
atq :=		LDLIBS += -lsecdb
atrm :=		LDLIBS += -lsecdb
cron :=		LDLIBS += -lpam -lproject -lcontract
crontab :=	LDLIBS += -lsecdb -lpam 


$(ROOTSVCSYSTEM)/cron.xml := FILEMODE = 0444
$(ROOTLIBSVCMETHOD)/svc-cron := FILEMODE = 0555


.KEEP_STATE:

all :		$(PROG) $(SCRIPT) $(FILES)

install :	all $(ROOTPROG) $(ROOTETCDEFAULTFILES) $(ROOTSYMLINK) \
		$(ROOTMANIFEST) $(ROOTMETHOD)

$(PROG) :	$$(POBJS)
		$(LINK.c) $(POBJS) -o $@ $(LDLIBS)
		$(POST_PROCESS)

values-xpg4.o:	../../lib/crt/common/values-xpg4.c
	$(COMPILE.c) -o $@ ../../lib/crt/common/values-xpg4.c

%.o:			$(SRC)/common/util/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POST_PROCESS_O)

att1.c :	att1.y
		$(YACC.y) -d att1.y
		$(MV) y.tab.c att1.c
		$(MV) y.tab.h att1.h

att2.c :	att2.l att2.ed att1.c
		$(LEX) att2.l
		ed - lex.yy.c < att2.ed
		$(MV) lex.yy.c att2.c

$(ROOTSYMLINK) :
		$(RM) $@; $(SYMLINK) $(SYMLNKDEST) $@

check:		$(CHKMANIFEST)

$(POFILE):	$(POFILES)
	$(RM) $@; cat $(POFILES) > $@

clean :
		$(RM) $(OBJS) att1.h att1.c att2.c

strip :
		$(STRIP) $(PROG)

include		../Makefile.targ
