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
# uts/intel/rdc/Makefile
#
#       This makefile drives the production of the rdc kernel module
#
#       intel implementation architecture dependent
#

#
#       Path to the base of the uts directory tree (usually /usr/src/uts).
#
#
UTSBASE = ../..

ARCHDIR:sh = cd ..; basename `pwd`

#
#       Define the module and object file sets.
#
MODULE		= rdc
OBJECTS		= $(RDC_OBJS:%=$(OBJS_DIR)/%)
ROOTMODULE      = $(USR_DRV_DIR)/$(MODULE)
INC_PATH	+= -I$(ROOT)/usr/include
CONF_SRCDIR	= $(UTSBASE)/common/avs/ns/rdc

#
#       Include common rules.
#
include ../Makefile.$(ARCHDIR)
include $(UTSBASE)/common/avs/Makefile.com

#
#	Define targets
#
ALL_TARGET	= $(BINARY) $(SRC_CONFFILE)
INSTALL_TARGET	= $(BINARY) $(ROOTMODULE) $(ROOT_CONFFILE)

CFLAGS += -D_SD_8K_BLKSIZE -D_SYSCALL32
LDFLAGS += -dy -N"drv/nsctl" -N"drv/nskern" -N"drv/ncall" -N"misc/spuni" \
	-N"misc/rdcsrv" -N"strmod/rpcmod"

CERRWARN += -_gcc=-Wno-switch
CERRWARN += -_gcc=-Wno-unused-label
CERRWARN += -_gcc=-Wno-parentheses
CERRWARN += -_gcc=-Wno-uninitialized


.KEEP_STATE:

def:		$(DEF_DEPS)

all:		$(ALL_DEPS)

clean:		$(CLEAN_DEPS)

clobber:	$(CLOBBER_DEPS)
		-rm -f obj*/rdc_prot_xdr.c debug*/rdc_prot_xdr.c

install:	$(INSTALL_DEPS)

$(OBJS_DIR)/rdc_prot_xdr.c:	$(UTSBASE)/common/avs/ns/rdc/rdc_prot.x
	$(RPCGEN) -i 0 -C -c -o $@ \
		`/bin/pwd`/$(UTSBASE)/common/avs/ns/rdc/rdc_prot.x

$(OBJS_DIR)/rdc_prot_xdr.o:	$(OBJS_DIR)/rdc_prot_xdr.c
	$(COMPILE.c) -o $@ $(OBJS_DIR)/rdc_prot_xdr.c

#
#	Include common targets.
#
include ../Makefile.targ
