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
# lib/librdc/Makefile.com

LIBRARY= librdc.a
VERS= .1

OBJECTS= netaddrs.o rdcerr.o rdcconfig.o rdc_ioctl.o rdcpersist.o rdcrules.o

# include library definitions
include ../../Makefile.lib

SRCS= ../common/*.c ../../../cmd/avs/rdc/rdc_ioctl.c 
SRCDIR= ../common

LIBS +=		$(DYNLIB)

CERRWARN	+= -Wno-parentheses
CERRWARN	+= -Wno-unused-variable
CERRWARN	+= -Wno-address

CPPFLAGS +=	-DBUILD_REV_STR='"5.11"'
CFLAGS +=	-I..
CFLAGS64 +=	-I..
LDLIBS +=	 -lnsctl -lc -lunistat -ldscfg

.KEEP_STATE:

# include library targets
include ../../Makefile.targ

objs/%.o pics/%.o: ../common/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

objs/rdc_ioctl.o pics/rdc_ioctl.o: ../../../cmd/avs/rdc/rdc_ioctl.c
	$(COMPILE.c) -o $@ ../../../cmd/avs/rdc/rdc_ioctl.c
	$(POST_PROCESS_O)
