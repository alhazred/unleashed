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
# Copyright (c) 1990, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2010 Nexenta Systems, Inc.  All rights reserved.
#

ETCFILES= ioctl.syscon passwd shadow motd release
FTPDFILES= ftpusers
SMBFILES= smbpasswd

include ../../Makefile.cmd

ROOTETCFILES= $(ETCFILES:%=$(ROOTETC)/%)
ROOTETCFTPDFILES= $(FTPDFILES:%=$(ROOTETCFTPD)/%)
ROOTVARSMBFILES= $(SMBFILES:%=$(ROOTVARSMB)/%)

FILEMODE= 0644

ROOTETCFTPUSERSLINK= $(ROOTETC)/ftpusers

$(ROOTETC)/shadow 	:=	FILEMODE =	400
$(ROOTETC)/release	:=	FILEMODE =	0444
$(ROOTVARSMB)/smbpasswd :=	FILEMODE =	0400


.KEEP_STATE:

$(ROOTETCFTPUSERSLINK):	$(ROOTETCFTPDFILES)
	$(RM) $@; $(SYMLINK) ftpd/ftpusers $@

all: $(ETCFILES) $(SMBFILES) $(FTPDFILES)

install: all $(ROOTETCFILES) $(ROOTETCFTPDFILES) $(ROOTVARSMBFILES) $(ROOTETCFTPUSERSLINK)

clean:
	$(RM) $(ROOTETCFTPUSERSLINK)


clobber:

motd:	FRC
	@-$(ECHO) "rebuilding motd"
	@-$(ECHO) "Unleashed\t$(VERSION)\t$(RELEASE_DATE)" > motd
	@-$(CAT) release_info >> motd

release: motd
	cp motd release

smbpasswd:
	$(TOUCH) smbpasswd

clean:


clobber:
	$(RM) motd smbpasswd

FRC:
