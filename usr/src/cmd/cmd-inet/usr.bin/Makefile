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
# Copyright 2011 Nexenta Systems, Inc. All rights reserved.
# Copyright 2016 Toomas Soome <tsoome@me.com>
#

PROG=		finger rdate ruptime rwho whois
ALL=		$(PROG)
SRCS=		$(ALL:%=%.c)

SUBDIRS=	chat dns-sd ftp netstat \
		pppd pppdump pppstats talk telnet tftp
SUBDIR1=	talk 
MSGSUBDIRS=	talk

include ../../Makefile.cmd
include ../Makefile.cmd-inet

CLOBBERFILES +=	$(ALL)

all:=		TARGET= all
install:=	TARGET= install
clean:=		TARGET= clean
clobber:=	TARGET= clobber

CPPFLAGS +=	-DSYSV -DSTRNET -DBSD_COMP -I$(CMDINETCOMMONDIR)
CERRWARN +=	-Wno-parentheses
CERRWARN +=	-Wno-uninitialized
CERRWARN +=	-Wno-unused-function

# Enable large file support for reading the lastlog file.
finger :=	CPPFLAGS += -D_FILE_OFFSET_BITS=64 \
		-I$(ROOT)/usr/include/ncurses

finger :=		LDLIBS += -lncurses

.KEEP_STATE:

all: $(ALL) $(SUBDIRS)

install: all .WAIT $(ROOTPROG) $(SUBDIRS)

$(SUBDIRS): FRC
	@cd $@; pwd; $(MAKE) $(MFLAGS) $(TARGET)

FRC:

clean: $(SUBDIRS) clean_local
clean_local:
	$(RM) $(CLEANFILES)

clobber: $(SUBDIRS) clobber_local
clobber_local: clean_local
	$(RM) $(CLOBBERFILES) 
