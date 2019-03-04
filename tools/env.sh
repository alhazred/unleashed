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
# Copyright (c) 2005, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2015 Nexenta Systems, Inc.  All rights reserved.
# Copyright 2012 Joshua M. Clulow <josh@sysmgr.org>
# Copyright 2015, OmniTI Computer Consulting, Inc. All rights reserved.
#

# SRCTOP - where is your workspace at
#export SRCTOP="$HOME/ws/illumos-gate"
[ -z "$SRCTOP" ] && SRCTOP="`git rev-parse --show-toplevel`"
export SRCTOP

# Maximum number of dmake jobs.  The recommended number is 2 + NCPUS,
# where NCPUS is the number of logical CPUs on your build system.
maxjobs()
{
	njobs=1
	min_mem_per_job=512 # minimum amount of memory for a job

	ncpu=$(/usr/bin/getconf 'NPROCESSORS_ONLN')
	njobs=$(( ncpu + 2 ))
	
	# Throttle number of parallel jobs launched by dmake to a value which
	# gurantees that all jobs have enough memory. This was added to avoid
	# excessive paging/swapping in cases of virtual machine installations
	# which have lots of CPUs but not enough memory assigned to handle
	# that many parallel jobs
        /usr/sbin/prtconf 2>/dev/null | awk "BEGIN { mem_per_job = $min_mem_per_job; njobs = $njobs; } "'
/^Memory size: .* Megabytes/ {
	mem = $3;
        njobs_mem = int(mem/mem_per_job);
        if (njobs < njobs_mem) print(njobs);
        else print(njobs_mem);
}'
}

export DMAKE_MAX_JOBS=$(maxjobs)

export MACH="$(uname -p)"
if [ "$MACH" = "amd64" ]; then
    MACH=i386
fi

export ROOT="$SRCTOP/proto/root_${MACH}"
export SRC="$SRCTOP/usr/src"

#
#	build environment variables, including version info for mcs, motd,
# motd, uname and boot messages. Mostly you shouldn't change this except
# when the release slips (nah) or you move an environment file to a new
# release
#
[ -z "$VERSION" ] && VERSION="`git describe --dirty`"
export VERSION

# Package creation variables.  You probably shouldn't change these,
# either.
#
# PKGARCHIVE determines where the repository will be created.
#
# PKGPUBLISHER controls the publisher setting for the repository.
#
export PKGARCHIVE="${SRCTOP}/packages/${MACH}/nightly"
#export PKGPUBLISHER='unleashed'

# Package manifest format version.
export PKGFMT_OUTPUT='v1'

# Don't build python 3 versions of libs.
export BUILDPY3='#'
