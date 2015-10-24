#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet at
# http://www.illumos.org/license/CDDL.
#

#
# Copyright 2016 Toomas Soome <tsoome@me.com>
#

include ../Makefile.tools

.KEEP_STATE:

VPATH=	.:$(SRC)/cmd/boot/common

PROG=	btxld
MAN=	btxld.8
SRCS=	btxld.c elfh.c version.c bblk_einfo.c mboot_extra.c boot_utils.c
OBJS=	btxld.o elfh.o version.o bblk_einfo.o mboot_extra.o boot_utils.o
LDLIBS += -lmd5
C99MODE=$(C99_ENABLE)
CPPFLAGS += -I$(SRC)/uts/common -I$(SRC)/cmd/boot/common
CERRWARN += -_gcc=-Wno-uninitialized

all: $(PROG)

$(PROG): $(OBJS)
	$(LINK.c) -o $@ $(OBJS) $(LDLIBS)
	$(POST_PROCESS)

install: all .WAIT $(ROOTONBLDMACHPROG)

clean:
	$(RM) $(OBJS)

include ../Makefile.targ
