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
# Copyright (c) 2018 Joyent, Inc.
#

dir path=kernel group=sys
dir path=kernel/drv group=sys
dir path=usr/share/man
dir path=usr/share/man/man7d
driver name=qede clone_perms="qede 0666 root sys" perms="* 0666 root sys" \
    alias=pciex1077,1629 \
    alias=pciex1077,1630 \
    alias=pciex1077,1634 \
    alias=pciex1077,1654 \
    alias=pciex1077,1656 \
    alias=pciex1077,1666 \
    alias=pciex1077,8070 \
    alias=pciex1077,8071 \
    alias=pciex1077,8072 \
    alias=pciex1077,8073
file path=kernel/drv/qede group=sys
file path=kernel/drv/qede.conf group=sys
file path=usr/share/man/man7d/qede.7d
license lic_CDDL license=lic_CDDL
