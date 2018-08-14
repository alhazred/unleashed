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
driver name=udmf clone_perms="udmf 0666 root sys" perms="* 0666 root sys" \
    alias=usb7aa,9601 \
    alias=usba46,268 \
    alias=usba46,6688 \
    alias=usba46,9601 \
    alias=usbfe6,8101
file path=kernel/drv/udmf group=sys
license usr/src/uts/common/io/udmf/THIRDPARTYLICENSE \
    license=usr/src/uts/common/io/udmf/THIRDPARTYLICENSE
