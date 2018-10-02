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
# Copyright 2014 Gary Mills
#

#
# The default for payload-bearing actions in this package is to appear in the
# global zone only.  See the include file for greater detail, as well as
# information about overriding the defaults.
#
dir path=platform group=sys
dir path=platform/$(CONFIG_PLATFORM) group=sys
dir path=platform/$(CONFIG_PLATFORM)/$(ARCH64) group=sys
dir path=platform/$(CONFIG_PLATFORM)/kernel group=sys
dir path=platform/$(CONFIG_PLATFORM)/kernel/cpu group=sys
dir path=platform/$(CONFIG_PLATFORM)/kernel/dacf group=sys
dir path=platform/$(CONFIG_PLATFORM)/kernel/drv group=sys
dir path=platform/$(CONFIG_PLATFORM)/kernel/mach group=sys
dir path=platform/$(CONFIG_PLATFORM)/kernel/misc group=sys
dir path=platform/$(CONFIG_PLATFORM)/ucode group=sys
dir path=usr/share/man/man4
driver name=acpinex alias=acpivirtnex
driver name=acpippm
driver name=amd_iommu perms="* 0644 root sys" \
    alias=pci1002,5a23 \
    alias=pci1022,11ff
driver name=balloon perms="* 0444 root sys"
driver name=cpudrv alias=cpu
driver name=domcaps perms="* 0444 root sys"
driver name=evtchn perms="* 0666 root sys"
driver name=isa alias=pciclass,060100 class=sysbus
driver name=npe alias=pciex_root_complex
driver name=pci class=pci
driver name=pit_beep alias=SUNW,pit_beep
driver name=ppm
driver name=privcmd perms="* 0666 root sys"
driver name=rootnex
driver name=xdb
driver name=xdf
driver name=xenbus perms="* 0666 root sys"
driver name=xencons
driver name=xnbe alias=xnb,ioemu
driver name=xnbo \
    alias=xnb \
    alias=xnb,SUNW_mac
driver name=xnbu alias=xnb,netfront
driver name=xnf
file path=platform/$(CONFIG_PLATFORM)/kernel/unix group=sys \
    mode=0755 pkg.depend.bypass-generate=dtracestubs
file path=platform/i86pc/kernel/cpu/cpu.generic group=sys \
    mode=0755
file path=platform/i86pc/kernel/cpu/cpu_ms.AuthenticAMD group=sys \
    mode=0755
file path=platform/i86pc/kernel/cpu/cpu_ms.AuthenticAMD.15 \
    group=sys mode=0755
file path=platform/i86pc/kernel/cpu/cpu_ms.GenuineIntel group=sys \
    mode=0755
file path=platform/i86pc/kernel/dacf/consconfig_dacf group=sys \
    mode=0755
file path=platform/i86pc/kernel/drv/acpinex group=sys
file path=platform/i86pc/kernel/drv/acpippm group=sys
file path=platform/i86pc/kernel/drv/acpippm.conf group=sys
file path=platform/i86pc/kernel/drv/amd_iommu group=sys
file path=platform/i86pc/kernel/drv/amd_iommu.conf group=sys
file path=platform/i86pc/kernel/drv/cpudrv group=sys
file path=platform/i86pc/kernel/drv/isa group=sys
file path=platform/i86pc/kernel/drv/npe group=sys
file path=platform/i86pc/kernel/drv/pci group=sys
file path=platform/i86pc/kernel/drv/pit_beep group=sys
file path=platform/i86pc/kernel/drv/pit_beep.conf group=sys
file path=platform/i86pc/kernel/drv/ppm group=sys
file path=platform/i86pc/kernel/drv/ppm.conf group=sys
file path=platform/i86pc/kernel/drv/rootnex group=sys
file path=platform/i86pc/kernel/drv/rootnex.conf group=sys
file path=platform/i86pc/kernel/mach/apix group=sys mode=0755
file path=platform/i86pc/kernel/mach/pcplusmp group=sys mode=0755
file path=platform/i86pc/kernel/mach/uppc group=sys mode=0755
file path=platform/i86pc/kernel/misc/acpidev group=sys mode=0755
file path=platform/i86pc/kernel/misc/gfx_private group=sys \
    mode=0755
file path=platform/i86pc/ucode/amd-ucode.bin group=sys mode=0444 \
    preserve=true reboot-needed=true
file path=platform/i86pc/ucode/intel-ucode.txt group=sys mode=0444 \
    preserve=true reboot-needed=true
file path=usr/share/man/man4/sysbus.4
file path=usr/share/man/man7d/npe.7d
license cr_Sun license=cr_Sun
license include/sys/THIRDPARTYLICENSE.unicode \
    license=include/sys/THIRDPARTYLICENSE.unicode
license kernel/zmod/THIRDPARTYLICENSE license=kernel/zmod/THIRDPARTYLICENSE
license lic_CDDL license=lic_CDDL
license usr/src/cmd/mdb/common/libstand/THIRDPARTYLICENSE \
    license=usr/src/cmd/mdb/common/libstand/THIRDPARTYLICENSE
license usr/src/common/bzip2/LICENSE license=usr/src/common/bzip2/LICENSE
license usr/src/uts/intel/THIRDPARTYLICENSE \
    license=usr/src/uts/intel/THIRDPARTYLICENSE
