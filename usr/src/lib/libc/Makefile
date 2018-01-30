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
# Copyright (c) 1989, 2010, Oracle and/or its affiliates. All rights reserved.
#
# Copyright 2010 Nexenta Systems, Inc.  All rights reserved.
# Use is subject to license terms.
#
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
#

LIBBASENAME=	libc
LIBRARY=	$(LIBBASENAME:%=%.a)
VERS=		.1

# Note that we build libc_pic.a for the benefit of building
# ld.so.1, but we do not install it.  Only ld.so.1 needs it
# and it must be built in the same workspace as libc.
LIB_PIC= libc_pic.a

ROOTLIB=	$(ROOT)/usr/lib
ROOTFS_LIB=	$(ROOT)/lib
ROOTLIB64=	$(ROOTLIB)/$(MACH64)
ROOTFS_LIB64=	$(ROOTFS_LIB)/$(MACH64)

# include common library definitions
include ../Makefile.lib

i386_COMP= capabilities
sparc_COMP=	capabilities

MACH_COMP=	$($(MACH)_COMP)

i386_ETC=	$(MACH)/etc
sparc_ETC=

SUBDIRS=	$(MACH_CAP) $(MACH) $(MACH_ETC)
$(BUILD64)SUBDIRS += $(MACH64)

LIBS =		$(DYNLIB)

ROOTLIBS64=	$(ROOTLIBDIR64)/$(DYNLIB)
MACHLIBS64=	$(MACH64)/$(DYNLIB)

# definitions for install_h target
BASEHDRS=	getxby_door.h 
CHECKHDRS=	$(BASEHDRS:%.h=port/gen/%.check)
HDRS=		$(BASEHDRS)

CLOBBERFILES += THIRDPARTYLICENSE extract-copyright

# install rules for install_h target
$(ROOTHDRDIR)/%: port/gen/%
	$(INS.file)

$(ROOTLIBDIR)/$(DYNLIB)	:=	FILEMODE = 755
$(ROOTLIBDIR64)/$(DYNLIB) :=	FILEMODE = 755
$(ROOTFS_LIBDIR)/$(DYNLIB)	:=	FILEMODE = 755
$(ROOTFS_LIBDIR64)/$(DYNLIB)	:=	FILEMODE = 755

.KEEP_STATE:

all:	all_h lib32 $(BUILD64) .WAIT lib64 .WAIT etc THIRDPARTYLICENSE

THIRDPARTYLICENSE: extract-copyright
	$(RM) $@
	./extract-copyright . > $@

etc:	$($(MACH)_ETC)

lib32:	$(MACH_COMP) $(MACHLIBS) $(MACH)/$(LIB_PIC)
	@if $(ELFDUMP) -r $(MACH)/$(DYNLIB) | $(GREP) -w environ ; then \
		$(ECHO) "Error: Invalid reference to environ" ; \
		$(ECHO) "Error: See comments in port/gen/env_data.c" ; \
		exit 1; \
	fi

lib64:	$(MACH_COMP) $(MACHLIBS64) $(MACH64)/$(LIB_PIC)
	@if $(ELFDUMP) -r $(MACH64)/$(DYNLIB) | $(GREP) -w environ ; then \
		$(ECHO) "Error: Invalid reference to environ" ; \
		$(ECHO) "Error: See comments in port/gen/env_data.c" ; \
		exit 1; \
	fi

$(MACH)/assym.h $(MACH64)/assym.h:
	@cd $(@D); $(MAKE) assym.h

$($(MACH)_ETC): FRC
	@cd $(@); pwd; $(MAKE) $(TARGET)

$($(MACH)_COMP): FRC
	@cd $(@); pwd; $(MAKE) $(TARGET)

install:	all			\
		etc			\
		inslib32		\
		$(BUILD64) inslib64

inslib32:	$(ROOTFS_LIBS) $(ROOTFS_LINKS)

inslib64:	$(ROOTFS_LIBS64) $(ROOTFS_LINKS64)

install_h: all_h $(ROOTHDRS)

all_h:	$(MACH)/assym.h $(MACH64)/assym.h

check:	$(CHECKHDRS)

$(ROOTFS_LIB)/%: $(MACH)/%
	$(INS.file)
$(ROOTFS_LIBDIR)/$(LIBLINKS): $(ROOTFS_LIBDIR)/$(LIBLINKS)$(VERS)
	$(INS.liblink)
$(ROOTFS_LIB64)/%: $(MACH64)/%
	$(INS.file)
$(ROOTFS_LIBDIR64)/$(LIBLINKS): $(ROOTFS_LIBDIR64)/$(LIBLINKS)$(VERS)
	$(INS.liblink)

$(MACH)/$(LIB_PIC): FRC
	@cd $(MACH); pwd; VERSION='$(VERSION)' $(MAKE) $(LIB_PIC)
$(MACH64)/$(LIB_PIC): FRC
	@cd $(MACH64); pwd; VERSION='$(VERSION)' $(MAKE) $(LIB_PIC)

all	:=	TARGET= all
install	:=	TARGET= install
clean	:=	TARGET= clean
clobber	:=	TARGET= clobber

.PARALLEL:	$(SUBDIRS)

$(SUBDIRS): FRC
	@cd $@; pwd; VERSION='$(VERSION)' $(MAKE) $(TARGET)

clean: $(SUBDIRS)
clobber: $(SUBDIRS) etc clobber_local
clobber_local:
	$(RM) $(CLOBBERFILES)

_msg:	$(MSGDOMAIN) catalog

catalog:
	sh ./makelibccatalog.sh $(MSGDOMAIN)

$(MSGDOMAIN):
	$(INS.dir)

FRC:
# include MACH-specific library targets
include ../Makefile.mach
