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
# ident	"%Z%%M%	%I%	%E% SMI"
#

LIBRARY= libvscan.a
VERS= .1

OBJECTS= libvscan.o

include ../../Makefile.lib

LIBS=	$(DYNLIB)
SRCDIR =	../common

# Reset the Makefile.lib macro ROOTLIBDIR to refer to usr/lib/vscan
ROOTLIBDIR = $(ROOT)/usr/lib/vscan

LDLIBS += -lc -lscf -lsecdb -lnsl -lm
CPPFLAGS += -I$(SRCDIR)

.KEEP_STATE:

all: $(LIBS)

include ../../Makefile.targ
