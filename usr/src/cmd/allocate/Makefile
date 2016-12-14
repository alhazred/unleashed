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
# Copyright (c) 1992, 2010, Oracle and/or its affiliates. All rights reserved.
#

include		../Makefile.cmd

CERRWARN +=	-Wno-unused-value
CERRWARN +=	-Wno-uninitialized
CERRWARN +=	-Wno-parentheses
CERRWARN +=	-Wno-clobbered

ROOTSEC =	$(ROOTETC)/security
ROOTSECDEV =	$(ROOTSEC)/dev
ROOTSECLIB =	$(ROOTSEC)/lib
ROOTDIRS =	$(ROOTSECDEV) $(ROOTSECLIB)

ROOTMANIFESTDIR=	$(ROOTSVCSYSTEMDEVICE)
SVCMETHOD =	svc-allocate	
MANIFEST =	allocate.xml

RTLCKS =	audio fd0 sr0 st0 st1
CLEANfd =	fd_clean
CLEANsr =	sr_clean
CLEANst =	st_clean
CLEANaudio =	audio_clean
CLEAN_SCRIPTS =	$(CLEANfd) $(CLEANsr) $(CLEANst) $(CLEANaudio)
SCRIPTS =	$(CLEAN_SCRIPTS)
ALLSCRIPTS =	allscripts.sh

PROGalloc =	allocate
PROGmkdevalloc =	mkdevalloc
PROGdminfo =	dminfo
PROG =		$(PROGalloc) $(PROGmkdevalloc) $(PROGdminfo)

LINKPROGalloc =	deallocate list_devices
LINKPROGmkdevalloc = mkdevmaps

POFILE =	allocate_all.po
POFILES =	$(OBJS:%.o=%.po) $(ALLSCRIPTS:%.sh=%.po)

DFLAGS +=	-D_REENTRANT
CPPFLAGS +=	$(DFLAGS)

ROOTLOCKS =	$(RTLCKS:%=$(ROOTSECDEV)/%)
ROOTSCRIPTS =	$(SCRIPTS:%=$(ROOTSECLIB)/%)

ROOTPROG =	$(PROGallocate:%=$(ROOTUSRSBIN)/%) \
		$(PROGmkdevalloc:%=$(ROOTUSRSBIN)/%) \
		$(PROGdminfo:%=$(ROOTUSRSBIN)/%) \
		$(PROGaddaloc:%=$(ROOTUSRSBIN)/%)
ROOTLINKalloc =	$(LINKPROGalloc:%=$(ROOTUSRSBIN)/%)
ROOTLINKmkdevalloc = $(LINKPROGmkdevalloc:%=$(ROOTUSRSBIN)/%)
ROOTLINKS =	$(ROOTLINKalloc) $(ROOTLINKmkdevalloc)

PROGallocOBJS =		allocate.o allocate3.o
PROGmkdevallocOBJS =	mkdevalloc.o
PROGdminfoOBJS =	dminfo.o

OBJS =		$(PROGallocOBJS) \
		$(PROGmkdevallocOBJS) \
		$(PROGdminfoOBJS)

SRCS =		$(OBJS:%.o=%.c)

$(ROOTUSRSBIN)/% :=	FILEMODE = 555
$(ROOTUSRSBIN)/allocate :=	FILEMODE = 4555
$(ROOTSECDEV)/% :=	FILEMODE = 0400
$(ROOTSECLIB)/% :=	FILEMODE = 0555


$(PROGalloc) :=		LDLIBS += -lbsm -lsec -lsecdb
$(PROGmkdevalloc) :=	LDLIBS += -lbsm
$(PROGdminfo) :=	LDLIBS += -lbsm

CLOBBERFILES +=	$(SCRIPTS)

.KEEP_STATE:

all :		$(PROG) $(RTLCKS) $(SCRIPTS)

install :	$(PROG) $(ROOTDIRS) $(ROOTPROG) $(ROOTLOCKS) \
		$(ROOTSCRIPTS) $(ROOTLINKS) $(ROOTMANIFEST) $(ROOTSVCMETHOD)
$(RTLCKS):
		$(TOUCH) $@

$(ROOTSECLIB)/%: %.sh
		$(INS.rename)

$(PROGalloc) :	$(PROGallocOBJS)
		$(LINK.c) $(PROGallocOBJS) -o $@ $(LDLIBS)
		$(POST_PROCESS)

$(PROGmkdevalloc) :	$(PROGmkdevallocOBJS)
			$(LINK.c) $(PROGmkdevallocOBJS) -o $@ $(LDLIBS)
			$(POST_PROCESS)

$(PROGdminfo) :	$(PROGdminfoOBJS)
		$(LINK.c) $(PROGdminfoOBJS) -o $@ $(LDLIBS)
		$(POST_PROCESS)

$(ROOTDIRS) :
		$(INS.dir)

$(ROOTSECDEV)/%: %
		$(INS.file)

$(ROOTSECLIB)/%: %
		$(RM) $@
		$(INS.file)

$(ROOTLINKalloc) :	$(PROGalloc:%=$(ROOTUSRSBIN)/%)
			$(RM) $@
			$(LN) $(PROGalloc:%=$(ROOTUSRSBIN)/%) $@

$(ROOTLINKmkdevalloc) :	$(PROGmkdevalloc:%=$(ROOTUSRSBIN)/%)
			$(RM) $@
			$(LN) $(PROGmkdevalloc:%=$(ROOTUSRSBIN)/%) $@


$(ROOTETCSECLIB)/%.windowing: %
	$(RM) $@
	$(SYMLINK) $< $@

$(POFILE):      $(POFILES)
		$(RM) $@
		$(CAT) $(POFILES) > $@

#
# Concatenate all the scripts into one before we build the catalogue.
# Done to shrink the catalogue since some messages are shared between
# the various scripts.
#
$(ALLSCRIPTS):	$(SCRIPTS:%=%.sh)
		$(CAT) $(SCRIPTS:%=%.sh) > $@

clean :
		$(RM) $(PROG) $(RTLCKS) $(OBJS) \
		$(SCRIPTS) $(ALLSCRIPTS) $(POFILE) $(POFILES)

strip :
		$(STRIP) $(PROG)

include		../Makefile.targ
