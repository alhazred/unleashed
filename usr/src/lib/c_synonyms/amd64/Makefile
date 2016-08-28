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

ASFLAGS=	-D_ASM $(amd64_AS_XARCH)

include ../Makefile.com
include ../../Makefile.lib.64

# The use of sed is a gross hack needed because the current build system
# assumed that the compiler accepted linker flags (-Bfoo -zfoo and -Mfoo)
# directly.  Here, since we're calling the linker directly, we have to
# discard the prefixes.  Ideally, we would be using the LD_Z* and LD_B*
# variables instead, but that would require a lot of mucking with makefiles.
# So for now, we do this.
REMOVE_GCC_PREFIX=echo $(DYNFLAGS) | $(SED) -e 's/-Wl,//g'
BUILD.SO=	$(LD) -o $@ -G -64 $(REMOVE_GCC_PREFIX:sh) $(PICS) $(LDLIBS)

install: all $(ROOTLIBS64)
