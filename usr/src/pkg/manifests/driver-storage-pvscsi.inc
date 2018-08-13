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
# Copyright 2016 Nexenta Systems, Inc.
#

#
# The default for payload-bearing actions in this package is to appear in the
# global zone only.  See the include file for greater detail, as well as
# information about overriding the defaults.
#
dir path=kernel group=sys
dir path=kernel/drv group=sys
driver name=pvscsi alias=pci15ad,7c0 class=scsi-self-identifying
file path=kernel/drv/pvscsi group=sys
license lic_CDDL license=lic_CDDL
license usr/src/uts/intel/io/scsi/adapters/pvscsi/THIRDPARTYLICENSE \
    license=usr/src/uts/intel/io/scsi/adapters/pvscsi/THIRDPARTYLICENSE
