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

dir path=etc group=sys variant.opensolaris.zone=global
dir path=etc/brand group=sys variant.opensolaris.zone=global
dir path=etc/zones group=sys variant.opensolaris.zone=global
dir path=lib variant.opensolaris.zone=global
dir path=lib/svc variant.opensolaris.zone=global
dir path=lib/svc/manifest group=sys variant.opensolaris.zone=global
dir path=lib/svc/manifest/system group=sys variant.opensolaris.zone=global
dir path=lib/svc/method variant.opensolaris.zone=global
dir path=usr group=sys
dir path=usr/bin
dir path=usr/kernel group=sys
dir path=usr/kernel/drv group=sys
dir path=usr/lib
dir path=usr/lib/$(ARCH64)
dir path=usr/lib/brand
dir path=usr/lib/brand/ipkg group=bin
dir path=usr/lib/brand/shared group=sys
dir path=usr/lib/zones
dir path=usr/sbin
dir path=usr/share
dir path=usr/share/lib
dir path=usr/share/lib/xml group=sys
dir path=usr/share/lib/xml/dtd group=sys
dir path=usr/share/man
dir path=usr/share/man/man1
dir path=usr/share/man/man8
dir path=usr/share/man/man5
dir path=usr/share/man/man7d
driver name=zcons
file path=etc/zones/SUNWblank.xml mode=0444 variant.opensolaris.zone=global
file path=etc/zones/SUNWdefault.xml mode=0444 variant.opensolaris.zone=global
file path=etc/zones/index group=sys preserve=true \
    variant.opensolaris.zone=global
file path=lib/svc/manifest/system/resource-mgmt.xml group=sys mode=0444 \
    variant.opensolaris.zone=global
file path=lib/svc/manifest/system/zones.xml group=sys mode=0444 \
    variant.opensolaris.zone=global
file path=lib/svc/manifest/system/zonestat.xml group=sys mode=0444 \
    variant.opensolaris.zone=global
file path=lib/svc/method/svc-resource-mgmt mode=0555 \
    variant.opensolaris.zone=global
file path=lib/svc/method/svc-zones mode=0555 variant.opensolaris.zone=global
file path=lib/svc/method/svc-zonestat mode=0555 \
    variant.opensolaris.zone=global
file path=usr/bin/zonestat mode=0555
file path=usr/kernel/drv/zcons group=sys
file path=usr/lib/$(ARCH64)/libbrand.so.1
file path=usr/lib/$(ARCH64)/libzonecfg.so.1
file path=usr/lib/$(ARCH64)/libzonestat.so.1
file path=usr/lib/brand/ipkg/config.xml mode=0444
file path=usr/lib/brand/ipkg/platform.xml mode=0444
file path=usr/lib/brand/shared/common.ksh mode=0444
file path=usr/lib/brand/shared/query mode=0755
file path=usr/lib/brand/shared/uninstall.ksh mode=0444
file path=usr/lib/libbrand.so.1
file path=usr/lib/libzonecfg.so.1
file path=usr/lib/libzonestat.so.1
file path=usr/lib/zones/zoneadmd mode=0555
file path=usr/lib/zones/zonestatd mode=0555 variant.opensolaris.zone=global
file path=usr/sbin/zlogin mode=0555
file path=usr/sbin/zoneadm mode=0555
file path=usr/sbin/zonecfg mode=0555
file path=usr/share/lib/xml/dtd/brand.dtd.1 mode=0644
file path=usr/share/lib/xml/dtd/zone_platform.dtd.1 mode=0644
file path=usr/share/lib/xml/dtd/zonecfg.dtd.1
file path=usr/share/man/man1/zlogin.1
file path=usr/share/man/man8/zoneadm.8
file path=usr/share/man/man8/zoneadmd.8
file path=usr/share/man/man8/zonecfg.8
file path=usr/share/man/man5/brands.5
file path=usr/share/man/man5/zones.5
file path=usr/share/man/man7d/zcons.7d
license cr_Sun license=cr_Sun
license lic_CDDL license=lic_CDDL
link path=usr/bin/zonename target=../../sbin/zonename
