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
# Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2012 Nexenta Systems, Inc. All rights reserved.
#

dir path=lib variant.opensolaris.zone=global
dir path=lib/svc variant.opensolaris.zone=global
dir path=lib/svc/manifest group=sys variant.opensolaris.zone=global
dir path=usr group=sys
dir path=usr/lib
dir path=usr/lib/$(ARCH64)
dir path=usr/lib/picl group=sys
dir path=usr/lib/picl/plugins group=sys
dir path=usr/lib/sysevent
dir path=usr/lib/sysevent/modules
dir path=usr/sbin
file path=lib/svc/manifest/system/picl.xml group=sys mode=0444 \
    variant.opensolaris.zone=global
file path=usr/lib/$(ARCH64)/libpicl.so.1
file path=usr/lib/libpicl.so.1
file path=usr/lib/libpicld_pluginutil.so.1
file path=usr/lib/libpicltree.so.1
file path=usr/lib/picl/picld mode=0555
file path=usr/lib/picl/plugins/libpicldevtree.so.1 group=sys
file path=usr/lib/picl/plugins/libpiclevent.so.1 group=sys
file path=usr/lib/picl/plugins/libpiclfrutree.so.1 group=sys
file path=usr/lib/picl/plugins/libpiclmemcfg.so.1 group=sys
file path=usr/lib/sysevent/modules/picl_slm.so
file path=usr/sbin/prtpicl mode=0755
license cr_Sun license=cr_Sun
license lic_CDDL license=lic_CDDL
link path=usr/lib/$(ARCH64)/libpicl.so target=./libpicl.so.1
link path=usr/lib/libpicl.so target=./libpicl.so.1
link path=usr/lib/libpicld_pluginutil.so target=./libpicld_pluginutil.so.1
link path=usr/lib/libpicltree.so target=./libpicltree.so.1
link path=usr/lib/picl/plugins/libpicldevtree.so target=./libpicldevtree.so.1
link path=usr/lib/picl/plugins/libpiclevent.so target=./libpiclevent.so.1
link path=usr/lib/picl/plugins/libpiclfrutree.so target=./libpiclfrutree.so.1
link path=usr/lib/picl/plugins/libpiclmemcfg.so target=./libpiclmemcfg.so.1
