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
#

#
# The default for payload-bearing actions in this package is to appear in the
# global zone only.  See the include file for greater detail, as well as
# information about overriding the defaults.
#
dir path=lib
dir path=lib/svc
dir path=lib/svc/manifest group=sys
dir path=lib/svc/manifest/platform group=sys
dir path=lib/svc/manifest/platform/i86pc group=sys
dir path=lib/svc/method
dir path=platform group=sys
dir path=platform/i86pc group=sys
dir path=platform/i86pc/kernel group=sys
dir path=platform/i86pc/kernel/drv group=sys
dir path=platform/i86pc/kernel/misc group=sys
dir path=usr group=sys
dir path=usr/platform group=sys
dir path=usr/platform/i86pc group=sys
dir path=usr/platform/i86pc/lib
dir path=usr/platform/i86pc/lib/cfgadm
dir path=usr/platform/i86pc/lib/cfgadm/$(ARCH64)
driver name=dr alias=acpidr_sbd
file path=lib/svc/manifest/platform/i86pc/acpihpd.xml group=sys mode=0444
file path=lib/svc/method/svc-acpihpd mode=0555
file path=platform/i86pc/kernel/drv/dr group=sys
file path=platform/i86pc/kernel/drv/dr.conf group=sys mode=0644
file path=platform/i86pc/kernel/misc/drmach_acpi group=sys mode=0755
file path=usr/platform/i86pc/lib/acpihpd group=sys mode=0755
file path=usr/platform/i86pc/lib/cfgadm/$(ARCH64)/sbd.so.1
file path=usr/platform/i86pc/lib/cfgadm/sbd.so.1
license cr_Sun license=cr_Sun
license lic_CDDL license=lic_CDDL
license usr/src/cmd/acpihpd/THIRDPARTYLICENSE \
    license=usr/src/cmd/acpihpd/THIRDPARTYLICENSE
link path=usr/platform/i86pc/lib/cfgadm/$(ARCH64)/sbd.so target=./sbd.so.1
link path=usr/platform/i86pc/lib/cfgadm/sbd.so target=./sbd.so.1
