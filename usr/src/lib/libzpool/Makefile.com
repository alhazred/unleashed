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
# Copyright (c) 2005, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright (c) 2013, 2015 by Delphix. All rights reserved.
#

LIBRARY= libzpool.a
VERS= .1

# include the list of ZFS sources
include ../../../uts/common/Makefile.files
KERNEL_OBJS = kernel.o taskq.o util.o
DTRACE_OBJS = zfs.o

OBJECTS=$(ZFS_COMMON_OBJS) $(ZFS_SHARED_OBJS) $(KERNEL_OBJS)

# include library definitions
include ../../Makefile.lib

ZFS_COMMON_SRCS=	$(ZFS_COMMON_OBJS:%.o=../../../uts/common/fs/zfs/%.c)
ZFS_SHARED_SRCS=	$(ZFS_SHARED_OBJS:%.o=../../../common/zfs/%.c)
KERNEL_SRCS=		$(KERNEL_OBJS:%.o=../common/%.c)

SRCS=$(ZFS_COMMON_SRCS) $(ZFS_SHARED_SRCS) $(KERNEL_SRCS)
SRCDIR=		../common

# There should be a mapfile here
MAPFILES =

INCS += -I../common
INCS += -I../../../uts/common/fs/zfs
INCS += -I../../../common/zfs
INCS += -I../../../common

CLEANFILES += ../common/zfs.h
CLEANFILES += $(EXTPICS)

C99MODE=	$(C99_ENABLE)

CFLAGS +=	-g
CFLAGS64 +=	-g
LDLIBS +=	-lcmdutils -lumem -lavl -lnvpair -lz -lc -lsysevent -lmd
CPPFLAGS +=	$(INCS)	-DDEBUG -D__UNLEASHED_VISIBLE

CERRWARN +=	-Wno-parentheses
CERRWARN +=	-Wno-switch
CERRWARN +=	-Wno-type-limits
CERRWARN +=	-Wno-unused-variable
CERRWARN +=	-Wno-empty-body
CERRWARN +=	-Wno-unused-function
CERRWARN +=	-Wno-unused-label

.KEEP_STATE:

all: $(LIBS)

include ../../Makefile.targ

EXTPICS= $(DTRACE_OBJS:%=pics/%)

pics/%.o: ../../../uts/common/fs/zfs/%.c ../common/zfs.h
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

pics/%.o: ../../../common/zfs/%.c ../common/zfs.h
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

pics/%.o: ../common/%.d $(PICS)
	$(COMPILE.d) -C -s $< -o $@ $(PICS)
	$(POST_PROCESS_O)

../common/%.h: ../common/%.d
	$(DTRACE) -xnolibs -h -s $< -o $@
