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
dir path=kernel/dtrace group=sys
dir path=usr/share/man
dir path=usr/share/man/man7d
driver name=dcpc
driver name=dtrace perms="* 0666 root sys"
driver name=fasttrap perms="fasttrap 0666 root sys"
driver name=fbt perms="fbt 0644 root sys"
driver name=lockstat perms="* 0644 root sys"
driver name=profile perms="profile 0644 root sys"
driver name=sdt perms="sdt 0644 root sys"
driver name=systrace perms="systrace 0644 root sys"
file path=kernel/drv/dcpc group=sys
file path=kernel/drv/dcpc.conf group=sys
file path=kernel/drv/dtrace group=sys
file path=kernel/drv/dtrace.conf group=sys
file path=kernel/drv/fasttrap group=sys
file path=kernel/drv/fasttrap.conf group=sys
file path=kernel/drv/fbt group=sys
file path=kernel/drv/fbt.conf group=sys
file path=kernel/drv/lockstat group=sys
file path=kernel/drv/lockstat.conf group=sys
file path=kernel/drv/profile group=sys
file path=kernel/drv/profile.conf group=sys
file path=kernel/drv/sdt group=sys
file path=kernel/drv/sdt.conf group=sys
file path=kernel/drv/systrace group=sys
file path=kernel/drv/systrace.conf group=sys
file path=usr/share/man/man7d/dtrace.7d
file path=usr/share/man/man7d/fasttrap.7d
file path=usr/share/man/man7d/fbt.7d
file path=usr/share/man/man7d/lockstat.7d
file path=usr/share/man/man7d/profile.7d
file path=usr/share/man/man7d/sdt.7d
file path=usr/share/man/man7d/systrace.7d
hardlink path=kernel/dtrace/dcpc target=../../kernel/drv/dcpc
hardlink path=kernel/dtrace/fasttrap target=../../kernel/drv/fasttrap
hardlink path=kernel/dtrace/fbt target=../../kernel/drv/fbt
hardlink path=kernel/dtrace/lockstat target=../../kernel/drv/lockstat
hardlink path=kernel/dtrace/profile target=../../kernel/drv/profile
hardlink path=kernel/dtrace/sdt target=../../kernel/drv/sdt
hardlink path=kernel/dtrace/systrace target=../../kernel/drv/systrace
license cr_Sun license=cr_Sun
license lic_CDDL license=lic_CDDL
