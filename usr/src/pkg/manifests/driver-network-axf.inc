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
# Copyright 2016 Joyent Inc.
#

dir path=kernel group=sys
dir path=kernel/drv group=sys
driver name=axf clone_perms="axf 0666 root sys" perms="* 0666 root sys" \
    alias=usb1189,893 \
    alias=usb13b1,18 \
    alias=usb1557,7720 \
    alias=usb1631,6200 \
    alias=usb2001,1a00 \
    alias=usb2001,3c05 \
    alias=usb411,3d \
    alias=usb557,2009 \
    alias=usb5ac,1402 \
    alias=usb6189,182d \
    alias=usb77b,2226 \
    alias=usb7aa,17 \
    alias=usb7b8,420a \
    alias=usb7d1,3c05 \
    alias=usb846,1040 \
    alias=usb8dd,90ff \
    alias=usbb95,1720 \
    alias=usbb95,7720 \
    alias=usbb95,772a
file path=kernel/drv/axf group=sys
license usr/src/uts/common/io/axf/THIRDPARTYLICENSE \
    license=usr/src/uts/common/io/axf/THIRDPARTYLICENSE
