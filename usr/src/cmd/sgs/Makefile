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
# Copyright (c) 1991, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2016 RackTop Systems.
# Copyright 2017 Joyent, Inc.
#

include		$(SRC)/cmd/Makefile.cmd

# Note: Why SUBDIRS isn't sorted alphabetically
#
# The items under SGS are not independent of each other.
# They must be built in an order that ensures that
# all dependencies of an item have been built before the
# item itself.
#
SUBDIRS=	libconv		\
		.WAIT		\
		libdl		\
		libelf		\
		liblddbg	\
		.WAIT		\
		libld		\
		libldmake	\
		libldstab	\
		librtld		\
		libcrle		\
		.WAIT		\
		ld		\
		ldd		\
		lddstub		\
		rtld		\
		.WAIT		\
		librtld_db	\
		ldprof		\
		pvs		\
		crle		\
		ar		\
		dump		\
		elfdump		\
		elfedit		\
		elfwrap		\
		error		\
		gprof		\
		lari		\
		lex		\
		lorder		\
		m4		\
		mcs		\
		nm		\
		prof		\
		ranlib		\
		unifdef

# Messaging support
#
POSUBDIRS=	m4		nm
POFILE=		sgs.po
POFILES=	$(POSUBDIRS:%=%/%.po)

MSGSUBDIRS=	ld		ldd		libld		liblddbg \
		libldstab	librtld		rtld		libelf \
		ldprof		libcrle		pvs		elfdump	\
		elfedit		crle		lari \
		librtld_db	elfwrap		ar

MSGDIR=		messages


all :=		TARGET= all
install :=	TARGET= install
clean :=	TARGET= clean
clobber :=	TARGET= clobber
delete :=	TARGET= delete
_msg :=		TARGET= catalog
_msg_gettext :=	TARGET= catalog
_msg_sgsmsg :=	TARGET= catalog
chkmsg :=	TARGET= chkmsg


.KEEP_STATE:

.PARALLEL:	$(SUBDIRS)

all install:	.WAIT $(SUBDIRS)

include		$(SRC)/cmd/Makefile.targ

# Messaging support
#
_msg: _msg_gettext _msg_sgsmsg

_msg_gettext: $(MSGDOMAIN)/$(POFILE)

# $(MACH)/sgsmsg must be built before we can descend into $(MSGDIR)
_msg_sgsmsg: .WAIT $(MSGDIR)

$(MSGDOMAIN)/$(POFILE): \
		$(MSGDOMAIN) $(POFILE)

$(POFILE):	$(POSUBDIRS)
		$(RM) $(POFILE)
		cat $(POFILES) > $(POFILE)

$(MSGDIR):	$(MSGSUBDIRS) FRC
		@ cd $@; pwd; $(MAKE) $(TARGET)

chkmsg:		$(MSGSUBDIRS) FRC

check:		chkmsg

# built from lib/Makefile
install_lib:	FRC
		@ cd lex; pwd; $(MAKE) $@

delete \
clean clobber:	.WAIT $(SUBDIRS) $(MSGDIR)

$(SUBDIRS):	FRC
		@ cd $@; pwd; $(MAKE) $(TARGET)

FRC:
