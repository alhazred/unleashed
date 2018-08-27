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
# Copyright 2010 Nexenta Systems, Inc. All rights reserved.
# Copyright 2012 OmniTI Computer Consulting, Inc.  All rights reserved.
#

dir path=sbin group=sys
dir path=usr group=sys
dir path=usr/include
dir path=usr/lib
dir path=usr/lib/$(ARCH64)
dir path=usr/lib/python$(PYTHON_VERSION)
dir path=usr/lib/python$(PYTHON_VERSION)/vendor-packages
dir path=usr/lib/python$(PYTHON_VERSION)/vendor-packages/64
dir path=usr/lib/python$(PYTHON_VERSION)/vendor-packages/beadm
dir path=usr/sbin
dir path=usr/share
dir path=usr/share/man
dir path=usr/share/man/man8
file path=sbin/beadm mode=0555
file path=usr/include/libbe.h
file path=usr/lib/$(ARCH64)/libbe.so.1
file path=usr/lib/libbe.so.1
file path=usr/lib/python$(PYTHON_VERSION)/vendor-packages/64/libbe_py.so
file path=usr/lib/python$(PYTHON_VERSION)/vendor-packages/libbe_py.so
file path=usr/share/man/man8/beadm.8
license cr_Sun license=cr_Sun
license lic_CDDL license=lic_CDDL
link path=usr/lib/$(ARCH64)/libbe.so target=./libbe.so.1
link path=usr/lib/libbe.so target=./libbe.so.1
