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
dir path=kernel group=sys
dir path=kernel/drv group=sys
dir path=usr/share/man
dir path=usr/share/man/man7d
driver name=wpi clone_perms="wpi 0666 root sys" perms="* 0666 root sys" \
    alias=pciex8086,4222 \
    alias=pciex8086,4222.8086.1000 \
    alias=pciex8086,4222.8086.1001 \
    alias=pciex8086,4222.8086.1005 \
    alias=pciex8086,4222.8086.1034 \
    alias=pciex8086,4222.8086.1040 \
    alias=pciex8086,4222.8086.1041 \
    alias=pciex8086,4222.8086.1044 \
    alias=pciex8086,4222.8086.1050 \
    alias=pciex8086,4222.8086.1053 \
    alias=pciex8086,4227 \
    alias=pciex8086,4227.8086.1011 \
    alias=pciex8086,4227.8086.1012 \
    alias=pciex8086,4227.8086.1014
file path=kernel/drv/wpi group=sys
file path=usr/share/man/man7d/wpi.7d
license cr_Sun license=cr_Sun
license kernel/drivers/net/wifi/wpi/THIRDPARTYLICENSE \
    license=kernel/drivers/net/wifi/wpi/THIRDPARTYLICENSE
license kernel/drivers/net/wifi/wpi/fw-wpi/LICENSE \
    license=kernel/drivers/net/wifi/wpi/fw-wpi/LICENSE
