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

dir path=etc group=sys
dir path=etc/sock2path.d group=sys
dir path=kernel group=sys
dir path=kernel/socketmod group=sys
dir path=usr group=sys
dir path=usr/include
dir path=usr/include/net
dir path=usr/include/netpacket
dir path=usr/kernel group=sys
dir path=usr/kernel/drv group=sys
driver name=bpf perms="bpf 0666 root sys"
file path=etc/sock2path.d/driver%2Fnetwork%2Fbpf group=sys
file path=kernel/socketmod/sockpfp group=sys mode=0755
file path=usr/include/net/bpf.h mode=0644
file path=usr/include/net/bpfdesc.h mode=0644
file path=usr/include/net/dlt.h mode=0644
file path=usr/include/netpacket/packet.h
file path=usr/kernel/drv/bpf group=sys
file path=usr/kernel/drv/bpf.conf group=sys
license cr_Sun license=cr_Sun
license usr/src/cmd/ipf/tools/IPFILTER.LICENCE \
    license=usr/src/cmd/ipf/tools/IPFILTER.LICENCE
license usr/src/uts/common/io/bpf/BPF.LICENCE \
    license=usr/src/uts/common/io/bpf/BPF.LICENCE
