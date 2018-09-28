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
dir path=kernel/misc group=sys
dir path=usr/share/man
dir path=usr/share/man/man4
dir path=usr/share/man/man7
dir path=usr/share/man/man7d
driver name=ib perms="* 0644 root sys"
file path=kernel/drv/ib group=sys
file path=kernel/drv/ib.conf group=sys preserve=renameold
file path=kernel/misc/ibcm group=sys mode=0755
file path=kernel/misc/ibdm group=sys mode=0755
file path=kernel/misc/ibmf group=sys mode=0755
file path=kernel/misc/ibtl group=sys mode=0755
file path=usr/share/man/man4/ib.4
file path=usr/share/man/man7/ibmf.7
file path=usr/share/man/man7d/ib.7d
file path=usr/share/man/man7d/ibcm.7d
file path=usr/share/man/man7d/ibdm.7d
file path=usr/share/man/man7d/ibtl.7d
license cr_Sun license=cr_Sun
license lic_CDDL license=lic_CDDL
