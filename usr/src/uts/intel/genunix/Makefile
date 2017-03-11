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

#
#	This makefile drives the production of the generic
#	unix kernel module.
#
#	x86 implementation architecture dependent
#

#
#	Path to the base of the uts directory tree (usually /usr/src/uts).
#
UTSBASE	= ../..

#
#	Define the module and object file sets.
#
MODULE		= genunix
GENUNIX		= $(OBJS_DIR)/$(MODULE)

OBJECTS		= $(GENUNIX_OBJS:%=$(OBJS_DIR)/%) \
		  $(NOT_YET_KMODS:%=$(OBJS_DIR)/%)

ROOTMODULE	= $(ROOT_KERN_DIR)/$(MODULE)

LIBGEN		= $(OBJS_DIR)/libgenunix.so
LIBSTUBS	= $(GENSTUBS_OBJS:%=$(OBJS_DIR)/%)

#
#	Include common rules.
#
include $(UTSBASE)/intel/Makefile.intel

#
#	Define targets
#
ALL_TARGET	= $(LIBGEN) $(GENUNIX)
INSTALL_TARGET	= $(LIBGEN) $(GENUNIX) $(ROOTMODULE)

#
#	Overrides
#
CLOBBERFILES	+= $(GENUNIX)
CLEANFILES	+= $(LIBSTUBS) $(LIBGEN)
BINARY		=

CPPFLAGS	+= -I$(CODEMGR_WS)/include
CPPFLAGS	+= -I$(SRC)/common
CPPFLAGS	+= -I$(CODEMGR_WS)/kernel/fs/zfs

CPPFLAGS	+= -I$(CODEMGR_WS)/arch/x86/kernel/platform/i86pc/include

#
# For now, disable these compiler warnings; maintainers should endeavor to
# investigate and remove these for maximum coverage.  Please do not carry
# these forward to new Makefiles.
#

CERRWARN	+= -Wno-unused-label
CERRWARN	+= -Wno-unused-variable
CERRWARN	+= -Wno-unused-value
CERRWARN	+= -Wno-unused-function
CERRWARN	+= -Wno-parentheses
CERRWARN	+= -Wno-switch
CERRWARN	+= -Wno-type-limits
CERRWARN	+= -Wno-uninitialized
CERRWARN	+= -Wno-clobbered
CERRWARN	+= -Wno-empty-body

#
#	Default build targets.
#
.KEEP_STATE:

def:		$(DEF_DEPS)

all:		$(ALL_DEPS)

clean:		$(CLEAN_DEPS)

clobber:	$(CLOBBER_DEPS)

install:	$(INSTALL_DEPS)

$(LIBGEN):	$(GENUNIX) $(LIBSTUBS)
	$(BUILD.SO) $(GENUNIX) $(LIBSTUBS)

$(GENUNIX): $(OBJECTS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJECTS)
	$(CTFMERGE_MODULE)
	$(POST_PROCESS)

#
#	Include common targets.
#
include $(UTSBASE)/intel/Makefile.targ

#
#	Software workarounds for hardware "features".
#
include	$(UTSBASE)/i86pc/Makefile.workarounds

ALL_DEFS += $(WORKAROUND_DEFS)
