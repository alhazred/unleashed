#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License, Version 1.0 only
# (the "License").  You may not use this file except in compliance
# with the License.
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
#ident	"%Z%%M%	%I%	%E% SMI"
#
# Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#
#	This makefile drives the production of the sun4u starfire platform
#	module.
#
#	sun4u starfire implementation architecture dependent
#

#
#	Path to the base of the uts directory tree (usually /usr/src/uts).
#
UTSBASE	= ../..

include $(UTSBASE)/sun4u/starfire/Makefile.starfire

def		:=	TARGET= def
all		:=	TARGET= all
install		:=	TARGET= install
install_h	:=	TARGET= install_h
clean		:=	TARGET= clean
clobber		:=	TARGET= clobber
modlist		:=	TARGET= modlist
modlist		:=	NO_STATE= -K $$MODSTATE$$$$
check		:=	TARGET= check

IMPLEMENTED_PLATFORM	= SUNW,Ultra-Enterprise-10000

#
#	Default build targets.
#
.KEEP_STATE:

.PARALLEL:	$(STARFIRE_KMODS)

.NO_PARALLEL:	$(STARFIRE_CPU_KMODS)

def all clean clobber: genassym unix .WAIT \
		$(STARFIRE_CPU_KMODS) $(STARFIRE_KMODS)

modlist:	unix $(STARFIRE_CPU_KMODS) $(STARFIRE_KMODS)

install: $(ROOT_STARFIRE_DIR) $(USR_STARFIRE_DIR) \
	$(USR_STARFIRE_INC_DIR) \
	$(USR_STARFIRE_SBIN_DIR) \
	$(USR_STARFIRE_LIB_DIR) \
	genassym unix .WAIT $(STARFIRE_CPU_KMODS) $(STARFIRE_KMODS)

genassym unix $(STARFIRE_CPU_KMODS) $(STARFIRE_KMODS): FRC
	@cd $@; pwd; $(MAKE) $(NO_STATE) $(TARGET)

install_h check:	FRC
	@cd sys; pwd; $(MAKE) $(TARGET)

#
#	Include common targets.
#
include $(UTSBASE)/sun4u/starfire/Makefile.targ
