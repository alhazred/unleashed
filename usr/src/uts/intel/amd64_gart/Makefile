#
# Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#
# uts/intel/amd64_gart/Makefile
#
#
#ident	"%Z%%M%	%I%	%E% SMI"
#
#	This makefile drives the framework of agp protocol
#	(amd64_gart) kernel module.
#
#
#	Path to the base of the uts directory tree (usually /usr/src/uts).
#
UTSBASE = ../..

#
#	Define the module and object file sets.
#
MODULE		= amd64_gart
OBJECTS		= $(AMD64GART_OBJS:%=$(OBJS_DIR)/%)
ROOTMODULE	= $(ROOT_DRV_DIR)/$(MODULE)

#
#	Include common rules.
#
include $(UTSBASE)/intel/Makefile.intel

#
#	Define targets
#
ALL_TARGET	= $(BINARY)
INSTALL_TARGET	= $(BINARY) $(ROOTMODULE)

#
#	Default build targets.
#
.KEEP_STATE:

def:		$(DEF_DEPS)

all:		$(ALL_DEPS)

clean:		$(CLEAN_DEPS)

clobber:	$(CLOBBER_DEPS)

install:	$(INSTALL_DEPS)

#
#	Include common targets.
#
include $(UTSBASE)/intel/Makefile.targ
