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
driver name=urf clone_perms="urf 0666 root sys" perms="* 0666 root sys" \
    alias=usb1557,8150 \
    alias=usb3980,3 \
    alias=usb411,12 \
    alias=usb586,401a \
    alias=usb7b8,401a \
    alias=usbbda,8150
file path=kernel/drv/urf group=sys
license usr/src/uts/common/io/urf/THIRDPARTYLICENSE \
    license=usr/src/uts/common/io/urf/THIRDPARTYLICENSE
