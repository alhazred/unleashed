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

SHELL=/usr/bin/ksh93

LIBRARY=	libpp.a
VERS=		.1

OBJECTS= \
	ppargs.o \
	ppbuiltin.o \
	ppcall.o \
	ppcomment.o \
	ppcontext.o \
	ppcontrol.o \
	ppcpp.o \
	ppdata.o \
	pperror.o \
	ppexpr.o \
	ppfsm.o \
	ppincref.o \
	ppinput.o \
	ppkey.o \
	pplex.o \
	ppline.o \
	ppmacref.o \
	ppmisc.o \
	ppop.o \
	pppragma.o \
	ppprintf.o \
	ppproto.o \
	ppsearch.o \
	pptrace.o

include ../../Makefile.astmsg

include ../../Makefile.lib

# mapfile-vers does not live with the sources in in common/ to make
# automated code updates easier.
MAPFILES=       ../mapfile-vers

# Set common AST build flags (e.g. C99/XPG6, needed to support the math stuff)
include ../../../Makefile.ast

LIBS =		$(DYNLIB)

LDLIBS += \
	-last \
	-lc

SRCDIR =	../common

# We use "=" here since using $(CPPFLAGS.master) is very tricky in our
# case - it MUST come as the last element but future changes in -D options
# may then cause silent breakage in the AST sources because the last -D
# option specified overrides previous -D options so we prefer the current
# way to explicitly list each single flag.
CPPFLAGS = \
	$(DTEXTDOM) $(DTS_ERRNO) \
	-I. \
	-I$(ROOT)/usr/include/ast \
	-I$(ROOT)/usr/include \
	-D_PACKAGE_ast \
	'-DUSAGE_LICENSE=\
		"[-author?Glenn Fowler <gsf@research.att.com>]"\
		"[-copyright?Copyright (c) 1986-2009 AT&T Intellectual Property]"\
		"[-license?http://www.opensource.org/licenses/cpl1.0.txt]"\
		"[--catalog?libpp]"'


CFLAGS += \
	$(ASTCFLAGS)
CFLAGS64 += \
	$(ASTCFLAGS64)

CERRWARN	+= -Wno-parentheses
CERRWARN	+= -Wno-uninitialized
CERRWARN	+= -Wno-char-subscripts
CERRWARN	+= -Wno-empty-body
CERRWARN	+= -Wno-unused-value

pics/ppsearch.o 	:= CERRWARN += -Wno-sequence-point

.KEEP_STATE:

all: $(LIBS)

include ../../Makefile.targ
