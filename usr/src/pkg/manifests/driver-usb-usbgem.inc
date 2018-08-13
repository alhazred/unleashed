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
dir path=kernel/misc group=sys
file path=kernel/misc/usbgem group=sys mode=0755
license usr/src/uts/common/io/usbgem/THIRDPARTYLICENSE \
    license=usr/src/uts/common/io/usbgem/THIRDPARTYLICENSE
depend fmri=driver/usb type=require
