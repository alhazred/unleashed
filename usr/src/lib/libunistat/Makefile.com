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
# lib/libunistat/Makefile.com
#
# include global definitions
include ../../../Makefile.master

LIBRARY= libunistat.a
VERS= .1

OBJECTS= spcs_s_u.o spcs_log.o

# include library definitions
include ../../Makefile.lib

SRCS=		$(OBJECTS:%.o=../common/%.c)
SRCDIR=		../common

LIBS 	+=	$(DYNLIB)

CERRWARN	+= -_gcc=-Wno-parentheses
CERRWARN	+= -_gcc=-Wno-unused-variable

all:=	  TARGET= all
clean:=   TARGET= clean
clobber:= TARGET= clobber
install:= TARGET= install

MODS=	dsw \
	rdc \
	spcs \
	sdbc \
	solaris \
	sv

ERRS=	$(MODS:%=$(SRCDIR)/%/%.err)
MSGS=	$(MODS:%=$(SRCDIR)/%/%.msg)
EDEFS=	$(MODS:%=$(SRCDIR)/%/%.edef)
TRNKS=	$(MODS:%=$(SRCDIR)/%/%.trnk)
DTRNKS=	$(MODS:%=$(SRCDIR)/%/%.dtrnk)

ERRGEN_DIR=	$(SRC)/cmd/avs/errgen
ERRGEN=		$(ERRGEN_DIR)/errgen

# production (non-debug)
DFLAGS =	-DISSTATIC=static

# development (debug) - cstyle prohibits use of "STATIC"
DFLAGS =	-g -DISSTATIC=" "

CFLAGS +=	$(DFLAGS) -I. -DLIBSPCS_CLIENT\
		-I$(JAVAINC) -I$(JAVAINCSOL)\
		-DLIBUNISTAT_LOCALE=\"/usr/install/unistat/locale\"
CFLAGS64 +=	$(DFLAGS) -I. -DLIBSPCS_CLIENT\
		-I$(JAVAINC) -I$(JAVAINCSOL)\
		-DLIBUNISTAT_LOCALE=\"/usr/install/unistat/locale\"
LDLIBS +=	-lc

COMMENT=	"/* THIS FILE IS AUTOMATICALLY GENERATED: DO NOT EDIT */"

CLEANFILES +=	*.h *.po\
		$(SRCDIR)/*/*.msg\
		$(SRCDIR)/*/*.properties\
		$(SRCDIR)/*/*.exc\
		$(SRCDIR)/*/*.edef\
		$(SRCDIR)/*/*.trnk\
		$(SRCDIR)/*/*.dtrnk

# note that the properties files are generated in ../libspcs/java

.SUFFIXES: .err .exc .properties .edef .msg .trnk .dtrnk

.err.msg: 
	$(ERRGEN) -m `basename $*` <$*.err >$*.msg

.err.edef:
	$(ERRGEN) -c `basename $*` <$*.err >$*.edef

.err.trnk:
	$(ERRGEN) -t `basename $*` <$*.err >$*.trnk

.err.dtrnk:
	$(ERRGEN) -x `basename $*` <$*.err >$*.dtrnk

all:	spcs_etext.h spcs_errors.h spcs_etrinkets.h spcs_dtrinkets.h $(LIB)

spcs_dtrinkets.h: $(ERRGEN) $(DTRNKS)
	@echo $(COMMENT) > $@
	cat $(DTRNKS) >>spcs_dtrinkets.h

spcs_etrinkets.h: $(ERRGEN) $(TRNKS)
	@echo $(COMMENT) > $@
	cat $(TRNKS) $(SRCDIR)/spcs_etrinkets.stub >> $@

spcs_etext.h: $(ERRGEN) $(MSGS)
	@echo $(COMMENT) > $@
	$(CAT) $(MSGS) $(SRCDIR)/spcs_etext.stub >> $@
	$(SED) "s/	\"/	gettext(\"/" < $@ |\
		 sed "s/\",/\"),/" > temp 
	xgettext -d unistat temp ; rm temp

spcs_errors.h: $(ERRGEN) $(EDEFS)
	@echo $(COMMENT) > $@
	$(CAT) $(EDEFS) $(SRCDIR)/spcs_errors.stub >> $@

$(ERRGEN):
	@cd $(ERRGEN_DIR); pwd; $(MAKE) install


.KEEP_STATE:

FRC:

# include library targets
include ../../Makefile.targ

objs/%.o pics/%.o: ../common/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)
