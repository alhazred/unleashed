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

# Configuration variables for the runtime environment of the nightly
# build script and other tools for construction and packaging of
# releases.
# This example is suitable for building an illumos workspace, which
# will contain the resulting archives. It is based off the onnv
# release. It sets NIGHTLY_OPTIONS to make nightly do:
#       DEBUG build instead of non-DEBUG (-D)
#       runs 'make check' (-C)
#       checks for new interfaces in libraries (-A)
#       sends mail on completion (-m and the MAILTO variable)
#       creates packages for PIT/RE (-p)
#       checks for changes in ELF runpaths (-r)
#
# - This file is sourced by "bldenv.sh" and "nightly.sh" and should not 
#   be executed directly.
# - This script is only interpreted by ksh93 and explicitly allows the
#   use of ksh93 language extensions.
#
export NIGHTLY_OPTIONS='-CAmpr'

# SRCTOP - where is your workspace at
#export SRCTOP="$HOME/ws/illumos-gate"
export SRCTOP="`git rev-parse --show-toplevel`"

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

# Some scripts optionally send mail messages to MAILTO.
export MAILTO="$LOGNAME"

# The project (see project(4)) under which to run this build.  If not
# specified, the build is simply run in a new task in the current project.
export BUILD_PROJECT=''

# You should not need to change the next three lines
export ATLOG="$SRCTOP/log"
export LOGFILE="$ATLOG/nightly.log"
export MACH="$(uname -p)"

export ROOT="$SRCTOP/proto/root_${MACH}"
export SRC="$SRCTOP/usr/src"

#
#	build environment variables, including version info for mcs, motd,
# motd, uname and boot messages. Mostly you shouldn't change this except
# when the release slips (nah) or you move an environment file to a new
# release
#
export VERSION="`git describe HEAD`"

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

# Set this flag to 'n' to disable the use of 'checkpaths'.  The default,
# if the 'N' option is not specified, is to run this test.
#CHECK_PATHS='y'

# POST_NIGHTLY can be any command to be run at the end of nightly.  See
# nightly(1) for interactions between environment variables and this command.
#POST_NIGHTLY=
