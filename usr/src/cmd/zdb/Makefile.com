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
# Copyright (c) 2012 by Delphix. All rights reserved.
# Copyright 2017 Joyent, Inc.
#

PROG:sh=	cd ..; basename `pwd`
SRCS= ../$(PROG).c ../zdb_il.c
OBJS= $(PROG).o zdb_il.o

include ../../Makefile.cmd
include ../../Makefile.ctf

INCS += -I../../../lib/libzpool/common
INCS +=	-I$(SRCTOP)/kernel/fs/zfs
INCS +=	-I$(SRCTOP)/kernel/fs/zfs/common

LDLIBS += -lzpool -lumem -lnvpair -lzfs -lavl -lcmdutils

C99MODE=	$(C99_ENABLE)

CPPFLAGS += -D_LARGEFILE64_SOURCE=1 $(INCS) -DDEBUG

# re-enable warnings that we can tolerate, which are disabled by default
# in Makefile.master
CERRWARN += -Wmissing-braces
CERRWARN += -Wsign-compare
CERRWARN += -Wmissing-field-initializers

.KEEP_STATE:

all: $(PROG)

$(PROG): $(OBJS)
	$(LINK.c) -o $(PROG) $(OBJS) $(LDLIBS)
	$(POST_PROCESS)

clean:
	$(RM) $(OBJS)


include ../../Makefile.targ

%.o: ../%.c
	$(COMPILE.c) $<
	$(POST_PROCESS_O)
