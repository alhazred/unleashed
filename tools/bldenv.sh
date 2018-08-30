#!/bin/ksh
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
# Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
# Copyright 2011 Nexenta Systems, Inc.  All rights reserved.
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
#
# Uses supplied "env" file, based on /opt/onbld/etc/env, to set shell variables
# before spawning a shell for doing a release-style builds interactively
# and incrementally.
#

function fatal_error
{
	print -u2 "${progname}: $*"
	exit 1
}

function usage
{
    print -u2 "usage: ${progname} [-cfd] env_file"
    exit 2
}

# boolean flags (true/false)
flags_c=false
flags_f=false
flags_d=false

progname="$(basename -- "${0}")"

OPTIND=1

while getopts cfd OPT ; do 
    case ${OPT} in
	  c)	flags_c=true  ;;
	  +c)	flags_c=false ;;
	  f)	flags_f=true  ;;
	  +f)	flags_f=false ;;
	  d)	flags_d=true  ;;
	  +d)	flags_d=false ;;
	  \?)	usage ;;
    esac
done
shift $((OPTIND-1))

# test that the path to the environment-setting file was given
if [ -z "$1" ] ; then
	usage
fi

# force locale to C
export \
	LANG=C \
	LC_ALL=C \
	LC_COLLATE=C \
	LC_CTYPE=C \
	LC_MESSAGES=C \
	LC_MONETARY=C \
	LC_NUMERIC=C \
	LC_TIME=C

# clear environment variables we know to be bad for the build
unset \
	LD_OPTIONS \
	LD_LIBRARY_PATH \
	LD_AUDIT \
	LD_BIND_NOW \
	LD_BREADTH \
	LD_CONFIG \
	LD_DEBUG \
	LD_FLAGS \
	LD_LIBRARY_PATH_64 \
	LD_NOVERSION \
	LD_ORIGIN \
	LD_LOADFLTR \
	LD_NOAUXFLTR \
	LD_NOCONFIG \
	LD_NODIRCONFIG \
	LD_NOOBJALTER \
	LD_PRELOAD \
	LD_PROFILE \
	CONFIG \
	GROUP \
	OWNER \
	REMOTE \
	ENV \
	ARCH \
	CLASSPATH

#
# Setup environment variables
#

if [[ -f "$1" ]]; then
	if [[ "$1" == */* ]]; then
		. "$1"
	else
		. "./$1"
	fi
else
	printf \
	    'Cannot find env file "%s"\n' "$1"
	exit 1
fi
shift

# Check if we have sufficient data to continue...
[[ -n "${SRCTOP}" ]] || fatal_error "Error: Variable SRCTOP not set."
[[ -d "${SRCTOP}" ]] || fatal_error "Error: ${SRCTOP} is not a directory."
[[ -f "${SRCTOP}/usr/src/Makefile" ]] || fatal_error "Error: ${SRCTOP}/usr/src/Makefile not found."

POUND_SIGN="#"
# have we set RELEASE_DATE in our env file?
if [ -z "$RELEASE_DATE" ]; then
	RELEASE_DATE=$(LC_ALL=C date +"%B %Y")
fi
BUILD_DATE=$(LC_ALL=C date +%Y-%b-%d)
BASEWSDIR=$(basename -- "${SRCTOP}")
export RELEASE_DATE POUND_SIGN

print 'Build type   is  \c'
if ${flags_d} ; then
	print 'DEBUG'
	unset RELEASE_BUILD
	unset EXTRA_OPTIONS
	unset EXTRA_CFLAGS
else
	# default is a non-DEBUG build
	print 'non-DEBUG'
	export RELEASE_BUILD=
	unset EXTRA_OPTIONS
	unset EXTRA_CFLAGS
fi

# update build-type variables
PKGARCHIVE="${PKGARCHIVE}"

# 	Set PATH for a build
PATH="/opt/onbld/bin:/opt/onbld/bin/${MACH}:/usr/bin:/usr/sbin:/usr/ucb:/usr/etc:/usr/openwin/bin:/usr/sfw/bin:/opt/sfw/bin:.:/opt/SUNWspro/bin"

if [[ -n "${MAKE}" ]]; then
	if [[ -x "${MAKE}" ]]; then
		export PATH="$(dirname -- "${MAKE}"):$PATH"
	else
		print "\$MAKE (${MAKE}) is not a valid executible"
		exit 1	
	fi
fi

TOOLS="${SRC}/tools"
TOOLS_PROTO="${TOOLS}/proto/root_${MACH}-nd" ; export TOOLS_PROTO

export ONBLD_TOOLS="${ONBLD_TOOLS:=${TOOLS_PROTO}/opt/onbld}"

export STABS="${TOOLS_PROTO}/opt/onbld/bin/${MACH}/stabs"
export CTFSTABS="${TOOLS_PROTO}/opt/onbld/bin/${MACH}/ctfstabs"
export GENOFFSETS="${TOOLS_PROTO}/opt/onbld/bin/genoffsets"

PATH="${TOOLS_PROTO}/opt/onbld/bin/${MACH}:${PATH}"
PATH="${TOOLS_PROTO}/opt/onbld/bin:${PATH}"
export PATH

export DMAKE_MODE=${DMAKE_MODE:-parallel}

DEF_STRIPFLAG="-s"

TMPDIR="/tmp"

export \
	PATH TMPDIR \
	POUND_SIGN \
	DEF_STRIPFLAG \
	RELEASE_DATE
unset \
	CFLAGS \
	LD_LIBRARY_PATH

# a la ws
ENVLDLIBS1=
ENVLDLIBS2=
ENVLDLIBS3=
ENVCPPFLAGS1=
ENVCPPFLAGS2=
ENVCPPFLAGS3=
ENVCPPFLAGS4=

ENVLDLIBS1="-L$ROOT/lib -L$ROOT/usr/lib"
ENVCPPFLAGS1="-I$ROOT/usr/include"
MAKEFLAGS=e

export \
        ENVLDLIBS1 \
        ENVLDLIBS2 \
        ENVLDLIBS3 \
	ENVCPPFLAGS1 \
        ENVCPPFLAGS2 \
        ENVCPPFLAGS3 \
	ENVCPPFLAGS4 \
        MAKEFLAGS

printf 'RELEASE      is %s\n'   "$RELEASE"
printf 'VERSION      is %s\n'   "$VERSION"
printf 'RELEASE_DATE is %s\n\n' "$RELEASE_DATE"

print "Use 'bmake gen-config' target to generate config makefiles/headers."
print "Use 'dmake setup' target to build legacy headers and tools."
print ""

#
# place ourselves in a new task, respecting BUILD_PROJECT if set.
#
/usr/bin/newtask -c $$ ${BUILD_PROJECT:+-p$BUILD_PROJECT}

SHELL=/bin/sh

if [[ "${flags_c}" == "false" && -x "$SHELL" && \
    "$(basename -- "${SHELL}")" != "csh" ]]; then
	# $SHELL is set, and it's not csh.

	if "${flags_f}" ; then
		print 'WARNING: -f is ignored when $SHELL is not csh'
	fi

	printf 'Using %s as shell.\n' "$SHELL"
	exec "$SHELL" ${@:+-c "$@"}

elif "${flags_f}" ; then
	print 'Using csh -f as shell.'
	exec csh -f ${@:+-c "$@"}

else
	print 'Using csh as shell.'
	exec csh ${@:+-c "$@"}
fi

# not reached
