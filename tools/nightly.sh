#!/bin/ksh -p
# vim: noet sw=8 ts=8
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
# Copyright 2008, 2010, Richard Lowe
# Copyright 2011 Nexenta Systems, Inc.  All rights reserved.
# Copyright 2012 Joshua M. Clulow <josh@sysmgr.org>
# Copyright 2014 Garrett D'Amore <garrett@damore.org>
#
# Based on the nightly script from the integration folks,
# Mostly modified and owned by mike_s.
# Changes also by kjc, dmk.
#
# -i on the command line, means fast options, so when it's on the
# command line (only), check builds are skipped no matter what
# the setting of their individual flags are in NIGHTLY_OPTIONS.
#
# OPTHOME  may be set in the environment to override /opt
#

#
# The CDPATH variable causes ksh's `cd' builtin to emit messages to stdout
# under certain circumstances, which can really screw things up; unset it.
#
unset CDPATH

# Get the absolute path of the nightly script that the user invoked.  This
# may be a relative path, and we need to do this before changing directory.
nightly_path=`whence $0`

#
# Keep track of where we found nightly so we can invoke the matching
# which_scm script.  If that doesn't work, don't go guessing, just rely
# on the $PATH settings, which will generally give us either /opt/onbld
# or the user's workspace.
#
WHICH_SCM=$(dirname $nightly_path)/which_scm
if [[ ! -x $WHICH_SCM ]]; then
	WHICH_SCM=which_scm
fi

function fatal_error
{
	print -u2 "nightly: $*"
	exit 1
}

#
# Function to do a DEBUG and non-DEBUG build. Needed because we might
# need to do another for the source build, and since we only deliver DEBUG or
# non-DEBUG packages.
#
# usage: normal_build
#
function normal_build {

	typeset orig_p_FLAG="$p_FLAG"

	if [ "$D_FLAG" = "n" ]; then
		set_non_debug_build_flags
		build "non-DEBUG"
	else
		set_debug_build_flags
		build "DEBUG"
	fi

	p_FLAG="$orig_p_FLAG"
}

#
# usage: run_hook HOOKNAME ARGS...
#
# If variable "$HOOKNAME" is defined, insert a section header into 
# our logs and then run the command with ARGS
#
function run_hook {
	HOOKNAME=$1
    	eval HOOKCMD=\$$HOOKNAME
	shift

	if [ -n "$HOOKCMD" ]; then 
	    	(
			echo "\n==== Running $HOOKNAME command: $HOOKCMD ====\n"
		    	( $HOOKCMD "$@" 2>&1 )
			if [ "$?" -ne 0 ]; then
			    	# Let exit status propagate up
			    	touch $TMPDIR/abort
			fi
		)

		if [ -f $TMPDIR/abort ]; then
			build_ok=n
			echo "\nAborting at request of $HOOKNAME"
			exit 1
		fi
	fi
}

# Return library search directive as function of given root.
function myldlibs {
	echo "-L$1/lib -L$1/usr/lib"
}

# Return header search directive as function of given root.
function myheaders {
	echo "-I$1/usr/include"
}

#
# Function to do the build, including package generation.
# usage: build LABEL
# - LABEL is used to tag build output.
#
function build {
	LABEL=$1
	INSTALLOG=install-${MACH}
	NOISE=noise-${MACH}

	export ROOT

	export ENVLDLIBS1=`myldlibs $ROOT`
	export ENVCPPFLAGS1=`myheaders $ROOT`

	this_build_ok=y

	#
	#	Before we build anything via dmake, we need to install
	#	bmake-ified headers to the proto area
	#
	bmake_build_step_user_dir $CODEMGR_WS/include all
	bmake_build_step_user_dir $CODEMGR_WS/include install

	#
	#	Build the legacy part of the source
	#
	echo "\n==== Building legacy source at `date` ($LABEL) ====\n"

	cd $SRC
	echo "\n==== dmake install ====\n" >&2
	if ! /bin/time $MAKE -e install; then
		build_ok=n
		this_build_ok=n
	fi

	#
	#	Build the new part of the source
	#
	bmake_build_step_user build

	echo "\n==== Ended OS-Net source build at `date` ($LABEL) ====\n"

	#
	#	Building Packages
	#
	if [ "$p_FLAG" = "y" -a "$this_build_ok" = "y" ]; then
		if [ -d $SRC/pkg ]; then
			echo "\n==== Creating $LABEL packages at `date` ====\n"
			echo "Clearing out $PKGARCHIVE ..."
			rm -rf $PKGARCHIVE
			mkdir -p $PKGARCHIVE

			rm -f $SRC/pkg/${INSTALLOG}.out
			cd $SRC/pkg
			echo "\n==== package build errors ($LABEL) ====\n" >&2
			if ! /bin/time $MAKE -e install; then
				build_extras_ok=n
				this_build_ok=n
			fi
		else
			#
			# Handle it gracefully if -p was set but there so
			# no pkg directory.
			#
			echo "\n==== No $LABEL packages to build ====\n"
		fi
	else
		echo "\n==== Not creating $LABEL packages ====\n"
	fi
}

#
# Build and install the onbld tools.
#
# usage: build_tools DESTROOT
#
# returns non-zero status if the build was successful.
#
function build_tools {
	DESTROOT=$1

	INSTALLOG=install-${MACH}

	echo "\n==== Building tools at `date` ====\n" \

	rm -f ${TOOLS}/${INSTALLOG}.out
	cd ${TOOLS}
	echo "\n==== Tools build errors ====\n" >&2
	if ! /bin/time $MAKE TOOLS_PROTO=${DESTROOT} -e install; then
		return 1
	fi
	return 0
}

#
# Set up to use locally installed tools.
#
# usage: use_tools TOOLSROOT
#
function use_tools {
	TOOLSROOT=$1

	#
	# If we're not building ON workspace, then the TOOLSROOT
	# settings here are clearly ignored by the workspace
	# makefiles, prepending nonexistent directories to PATH is
	# harmless, and we clearly do not wish to override
	# ONBLD_TOOLS.
	#
	# If we're building an ON workspace, then the prepended PATH
	# elements should supercede the preexisting ONBLD_TOOLS paths,
	# and we want to override ONBLD_TOOLS to catch the tools that
	# don't have specific path env vars here.
	#
	# So the only conditional behavior is overriding ONBLD_TOOLS,
	# and we check for "an ON workspace" by looking for
	# ${TOOLSROOT}/opt/onbld.
	#

	STABS=${TOOLSROOT}/opt/onbld/bin/${MACH}/stabs
	export STABS
	CTFSTABS=${TOOLSROOT}/opt/onbld/bin/${MACH}/ctfstabs
	export CTFSTABS
	GENOFFSETS=${TOOLSROOT}/opt/onbld/bin/genoffsets
	export GENOFFSETS


	PATH="${TOOLSROOT}/opt/onbld/bin/${MACH}:${PATH}"
	PATH="${TOOLSROOT}/opt/onbld/bin:${PATH}"
	export PATH

	if [ -d "${TOOLSROOT}/opt/onbld" ]; then
		ONBLD_TOOLS=${TOOLSROOT}/opt/onbld
		export ONBLD_TOOLS
	fi

	echo "\n==== New environment settings. ====\n"
	echo "STABS=${STABS}"
	echo "CTFSTABS=${CTFSTABS}"
	echo "PATH=${PATH}"
	echo "ONBLD_TOOLS=${ONBLD_TOOLS}"
}

#
# wrapper over wsdiff.
# usage: do_wsdiff LABEL OLDPROTO NEWPROTO
#
function do_wsdiff {
	label=$1
	oldproto=$2
	newproto=$3

	wsdiff="wsdiff -t"

	echo "\n==== Getting object changes since last build at `date`" \
	    "($label) ====\n" >&2
	$wsdiff -s -r ${TMPDIR}/wsdiff.results $oldproto $newproto >&2
	echo "\n==== Object changes determined at `date` ($label) ====\n" >&2
}

#
# Functions for setting build flags (DEBUG/non-DEBUG).  Keep them
# together.
#

function set_non_debug_build_flags {
	export RELEASE_BUILD ; RELEASE_BUILD=
	unset EXTRA_OPTIONS
	unset EXTRA_CFLAGS
}

function set_debug_build_flags {
	unset RELEASE_BUILD
	unset EXTRA_OPTIONS
	unset EXTRA_CFLAGS
}


MACH=`uname -p`

if [ "$OPTHOME" = "" ]; then
	OPTHOME=/opt
	export OPTHOME
fi

USAGE='Usage: nightly [-in] [-V VERS ] <env_file>

Where:
	-i	Fast incremental options (no clobber, check)
	-V VERS set the build version string to VERS

	<env_file>  file in Bourne shell syntax that sets and exports
	variables that configure the operation of this script and many of
	the scripts this one calls.

non-DEBUG is the default build type. Build options can be set in the
NIGHTLY_OPTIONS variable in the <env_file> as follows:

	-A	check for ABI differences in .so files
	-C	check for cstyle/hdrchk errors
	-D	do a build with DEBUG on
	-G	gate keeper default group of options (-au)
	-I	integration engineer default group of options (-ampu)
	-M	do not run pmodes (safe file permission checker)
	-N	do not run protocmp
	-R	default group of options for building a release (-mp)
	-U	update proto area in the parent
	-V VERS set the build version string to VERS
	-i	do an incremental build (no "make clobber")
	-m	send mail to $MAILTO at end of build
	-p	create packages
	-r	check ELF runtime attributes in the proto area
	-u	update proto_list_$MACH and friends in the parent workspace
	-w	report on differences between previous and current proto areas
'
#
#	A log file will be generated under the name $LOGFILE
#	for partially completed build and log.`date '+%F'`
#	in the same directory for fully completed builds.
#

# default values for low-level FLAGS; G I R are group FLAGS
A_FLAG=n
C_FLAG=n
D_FLAG=n
i_FLAG=n; i_CMD_LINE_FLAG=n
M_FLAG=n
m_FLAG=n
N_FLAG=n
p_FLAG=n
r_FLAG=n
V_FLAG=n
w_FLAG=n
W_FLAG=n
#
build_ok=y
build_extras_ok=y

#
# examine arguments
#

OPTIND=1
while getopts +iV:W FLAG
do
	case $FLAG in
	  i )	i_FLAG=y; i_CMD_LINE_FLAG=y
		;;
	  V )	V_FLAG=y
		V_ARG="$OPTARG"
		;;
	  W )   W_FLAG=y
		;;
	 \? )	echo "$USAGE"
		exit 1
		;;
	esac
done

# correct argument count after options
shift `expr $OPTIND - 1`

# test that the path to the environment-setting file was given
if [ $# -ne 1 ]; then
	echo "$USAGE"
	exit 1
fi

#
# force locale to C
LANG=C;		export LANG
LC_ALL=C;	export LC_ALL
LC_COLLATE=C;	export LC_COLLATE
LC_CTYPE=C;	export LC_CTYPE
LC_MESSAGES=C;	export LC_MESSAGES
LC_MONETARY=C;	export LC_MONETARY
LC_NUMERIC=C;	export LC_NUMERIC
LC_TIME=C;	export LC_TIME

# clear environment variables we know to be bad for the build
unset LD_OPTIONS
unset LD_AUDIT		LD_AUDIT_32		LD_AUDIT_64
unset LD_BIND_NOW	LD_BIND_NOW_32		LD_BIND_NOW_64
unset LD_BREADTH	LD_BREADTH_32		LD_BREADTH_64
unset LD_CONFIG		LD_CONFIG_32		LD_CONFIG_64
unset LD_DEBUG		LD_DEBUG_32		LD_DEBUG_64
unset LD_DEMANGLE	LD_DEMANGLE_32		LD_DEMANGLE_64
unset LD_FLAGS		LD_FLAGS_32		LD_FLAGS_64
unset LD_LIBRARY_PATH	LD_LIBRARY_PATH_32	LD_LIBRARY_PATH_64
unset LD_LOADFLTR	LD_LOADFLTR_32		LD_LOADFLTR_64
unset LD_NOAUDIT	LD_NOAUDIT_32		LD_NOAUDIT_64
unset LD_NOAUXFLTR	LD_NOAUXFLTR_32		LD_NOAUXFLTR_64
unset LD_NOCONFIG	LD_NOCONFIG_32		LD_NOCONFIG_64
unset LD_NODIRCONFIG	LD_NODIRCONFIG_32	LD_NODIRCONFIG_64
unset LD_NODIRECT	LD_NODIRECT_32		LD_NODIRECT_64
unset LD_NOLAZYLOAD	LD_NOLAZYLOAD_32	LD_NOLAZYLOAD_64
unset LD_NOOBJALTER	LD_NOOBJALTER_32	LD_NOOBJALTER_64
unset LD_NOVERSION	LD_NOVERSION_32		LD_NOVERSION_64
unset LD_ORIGIN		LD_ORIGIN_32		LD_ORIGIN_64
unset LD_PRELOAD	LD_PRELOAD_32		LD_PRELOAD_64
unset LD_PROFILE	LD_PROFILE_32		LD_PROFILE_64

unset CONFIG
unset GROUP
unset OWNER
unset REMOTE
unset ENV
unset ARCH
unset CLASSPATH
unset NAME

#
# To get ONBLD_TOOLS from the environment, it must come from the env file.
# If it comes interactively, it is generally TOOLS_PROTO, which will be
# clobbered before the compiler version checks, which will therefore fail.
#
unset ONBLD_TOOLS

#
#	Setup environmental variables
#
if [ -f $1 ]; then
	if [[ $1 = */* ]]; then
		. $1
	else
		. ./$1
	fi
else
	echo "Cannot find env file as either $1"
	exit 1
fi

# Check if we have sufficient data to continue...
[[ -v CODEMGR_WS ]] || \
	fatal_error "Error: Variable CODEMGR_WS not set."
[[ -d "${CODEMGR_WS}" ]] || \
	fatal_error "Error: ${CODEMGR_WS} is not a directory."
[[ -f "${CODEMGR_WS}/usr/src/Makefile" ]] || \
	fatal_error "Error: ${CODEMGR_WS}/usr/src/Makefile not found."

#
# place ourselves in a new task, respecting BUILD_PROJECT if set.
#
if [ -z "$BUILD_PROJECT" ]; then
	/usr/bin/newtask -c $$
else
	/usr/bin/newtask -c $$ -p $BUILD_PROJECT
fi

ps -o taskid= -p $$ | read build_taskid
ps -o project= -p $$ | read build_project

#
# See if NIGHTLY_OPTIONS is set
#
if [ "$NIGHTLY_OPTIONS" = "" ]; then
	NIGHTLY_OPTIONS="-aBm"
fi

#
# Note: changes to the option letters here should also be applied to the
#	bldenv script.  `d' is listed for backward compatibility.
#
NIGHTLY_OPTIONS=-${NIGHTLY_OPTIONS#-}
OPTIND=1
while getopts +ABCDdfGIiMmNpRrwW FLAG $NIGHTLY_OPTIONS
do
	case $FLAG in
	  A )	A_FLAG=y
		;;
	  B )	D_FLAG=y
		;; # old version of D
	  C )	C_FLAG=y
		;;
	  D )	D_FLAG=y
		;;
	  G )   ;;
	  I )	m_FLAG=y
		p_FLAG=y
		;;
	  i )	i_FLAG=y
		;;
	  M )	M_FLAG=y
		;;
	  m )	m_FLAG=y
		;;
	  N )	N_FLAG=y
		;;
	  p )	p_FLAG=y
		;;
	  R )	m_FLAG=y
		p_FLAG=y
		;;
	  r )	r_FLAG=y
		;;
	  w )	w_FLAG=y
		;;
	  W )   W_FLAG=y
		;;
	 \? )	echo "$USAGE"
		exit 1
		;;
	esac
done

if [ -z "$MAILTO" -o "$MAILTO" = "nobody" ]; then
	MAILTO=`/usr/bin/id -un`
	export MAILTO
fi

PATH="$OPTHOME/onbld/bin:$OPTHOME/onbld/bin/${MACH}:/usr/ccs/bin"
PATH="$PATH:/usr/bin:/usr/sbin:/usr/ucb"
PATH="$PATH:/usr/openwin/bin:/usr/sfw/bin:/opt/sfw/bin:.:$OPTHOME/SUNWspro/bin"
export PATH

# roots of source trees, both relative to $SRC and absolute.
relsrcdirs="."
abssrcdirs="$SRC"

PROTOCMPTERSE="protocmp.terse -gu"
POUND_SIGN="#"
# have we set RELEASE_DATE in our env file?
if [ -z "$RELEASE_DATE" ]; then
	RELEASE_DATE=$(LC_ALL=C date +"%B %Y")
fi
BUILD_DATE=$(LC_ALL=C date +%Y-%b-%d)
BASEWSDIR=$(basename $CODEMGR_WS)
DEV_CM="\"@(#)illumos Development: $LOGNAME $BUILD_DATE [$BASEWSDIR]\""
RELEASE_MICRO=$(( ($(date +%Y) * 12 + $(date +%m) - 1) - (2010 * 12 + 8 - 1) ))

# we export POUND_SIGN, RELEASE_DATE and DEV_CM to speed up the build process
# by avoiding repeated shell invocations to evaluate Makefile.master
# definitions.
export POUND_SIGN RELEASE_DATE DEV_CM RELEASE_MICRO

maketype="distributed"
if [[ -z "$MAKE" ]]; then
	MAKE=dmake
elif [[ ! -x "$MAKE" ]]; then
	echo "\$MAKE is set to garbage in the environment"
	exit 1	
fi
export PATH
export MAKE

hostname=$(uname -n)
if [[ $DMAKE_MAX_JOBS != +([0-9]) || $DMAKE_MAX_JOBS -eq 0 ]]
then
	maxjobs=
	if [[ -f $HOME/.make.machines ]]
	then
		# Note: there is a hard tab and space character in the []s
		# below.
		egrep -i "^[ 	]*$hostname[ 	\.]" \
			$HOME/.make.machines | read host jobs
		maxjobs=${jobs##*=}
	fi

	if [[ $maxjobs != +([0-9]) || $maxjobs -eq 0 ]]
	then
		# default
		maxjobs=4
	fi

	export DMAKE_MAX_JOBS=$maxjobs
fi

DMAKE_MODE=parallel;
export DMAKE_MODE

if [ -z "${ROOT}" ]; then
	echo "ROOT must be set."
	exit 1
fi

#
# if -V flag was given, reset VERSION to V_ARG
#
if [ "$V_FLAG" = "y" ]; then
	VERSION=$V_ARG
fi

TMPDIR="/tmp/nightly.tmpdir.$$"
export TMPDIR
rm -rf ${TMPDIR}
mkdir -p $TMPDIR || exit 1
#
# Tools should only be built non-DEBUG.  Keep track of the tools proto
# area path relative to $TOOLS, because the latter changes in an
# export build.
#
# TOOLS_PROTO is included below for builds other than usr/src/tools
# that look for this location.  For usr/src/tools, this will be
# overridden on the $MAKE command line in build_tools().
#
TOOLS=${SRC}/tools
TOOLS_PROTO_REL=proto/root_${MACH}-nd
TOOLS_PROTO=${TOOLS}/${TOOLS_PROTO_REL};	export TOOLS_PROTO

unset   CFLAGS LD_LIBRARY_PATH LDFLAGS

# create directories that are automatically removed if the nightly script
# fails to start correctly
function newdir {
	dir=$1
	toadd=
	while [ ! -d $dir ]; do
		toadd="$dir $toadd"
		dir=`dirname $dir`
	done
	torm=
	newlist=
	for dir in $toadd; do
		if mkdir $dir; then
			newlist="$dir $newlist"
			torm="$dir $torm"
		else
			[ -z "$torm" ] || rmdir $torm
			return 1
		fi
	done
	newdirlist="$newlist $newdirlist"
	return 0
}
newdirlist=

[ -d $CODEMGR_WS ] || newdir $CODEMGR_WS || exit 1

# since this script assumes the build is from full source, it nullifies
# variables likely to have been set by a "ws" script; nullification
# confines the search space for headers and libraries to the proto area
# built from this immediate source.
ENVLDLIBS1=
ENVLDLIBS2=
ENVLDLIBS3=
ENVCPPFLAGS1=
ENVCPPFLAGS2=
ENVCPPFLAGS3=
ENVCPPFLAGS4=

export ENVLDLIBS3 ENVCPPFLAGS1 ENVCPPFLAGS2 ENVCPPFLAGS3 ENVCPPFLAGS4 \
	ENVLDLIBS1 ENVLDLIBS2

#
# Juggle the logs and optionally send mail on completion.
#

function logshuffle {
    	LLOG="$ATLOG/log.`date '+%F.%H:%M'`"
	if [ -f $LLOG -o -d $LLOG ]; then
	    	LLOG=$LLOG.$$
	fi
	mkdir $LLOG
	export LLOG

	if [ "$build_ok" = "y" ]; then
		mv $ATLOG/proto_list_${MACH} $LLOG

		if [ -f $ATLOG/proto_list_tools_${MACH} ]; then
			mv $ATLOG/proto_list_tools_${MACH} $LLOG
	        fi

		if [ -f $TMPDIR/wsdiff.results ]; then
		    	mv $TMPDIR/wsdiff.results $LLOG
		fi

		if [ -f $TMPDIR/wsdiff-nd.results ]; then
			mv $TMPDIR/wsdiff-nd.results $LLOG
		fi
	fi

	kill $TEE
	case "$build_ok" in
		y)
			state=Completed
			;;
		i)
			state=Interrupted
			;;
		*)
	    		state=Failed
			;;
	esac

	if [[ $state != "Interrupted" && $build_extras_ok != "y" ]]; then
		state=Failed
	fi

	NIGHTLY_STATUS=$state
	export NIGHTLY_STATUS

	run_hook POST_NIGHTLY $state
	run_hook SYS_POST_NIGHTLY $state

	echo "Subject: Nightly ${MACH} Build of `basename ${CODEMGR_WS}` ${state}." \
		> ${LLOG}/mail_msg
	cat $build_time_file $mail_msg_file \
		>> ${LLOG}/mail_msg
	if [ "$m_FLAG" = "y" ]; then
		/usr/bin/mail ${MAILTO} < ${LLOG}/mail_msg
	fi

	mv $LOGFILE $LLOG
}

#
#	Remove the temporary files on any exit
#
function cleanup {
    	logshuffle

	set -- $newdirlist
	while [ $# -gt 0 ]; do
		rmdir $1
		shift; shift
	done
	rm -rf $TMPDIR
}

function cleanup_signal {
    	build_ok=i
	# this will trigger cleanup(), above.
	exit 1
}

trap cleanup 0
trap cleanup_signal 1 2 3 15

newdirlist=

#
# Create mail_msg_file
#
mail_msg_file="${TMPDIR}/mail_msg"
touch $mail_msg_file
build_time_file="${TMPDIR}/build_time"

mkdir -p "$ATLOG"
#
#	Move old LOGFILE aside
#
if [ -f $LOGFILE ]; then
	mv -f $LOGFILE ${LOGFILE}-
fi

mkfifo ${TMPDIR}/err.fifo ${TMPDIR}/out.fifo
tee $mail_msg_file < ${TMPDIR}/err.fifo >> $LOGFILE &
TEE=$!
tee ${TMPDIR}/stdout.txt < ${TMPDIR}/out.fifo >> $LOGFILE &
TEE="$TEE $!"
exec > ${TMPDIR}/out.fifo
exec 2> ${TMPDIR}/err.fifo

#
#	Build OsNet source
#
START_DATE=`date`
SECONDS=0
echo "\n==== Nightly $maketype build started:   $START_DATE ====" | \
    tee -a $build_time_file

echo "\nBuild project:  $build_project\nBuild taskid:   $build_taskid" >&2

run_hook SYS_PRE_NIGHTLY
run_hook PRE_NIGHTLY

echo "\n==== list of environment variables ====\n"
env

echo "\n==== Nightly argument issues ====\n" >&2

if [ "$N_FLAG" = "y" ]; then
	if [ "$p_FLAG" = "y" ]; then
		cat <<EOF >&2
WARNING: the p option (create packages) is set, but so is the N option (do
         not run protocmp); this is dangerous; you should unset the N option
EOF
	else
		cat <<EOF >&2
Warning: the N option (do not run protocmp) is set; it probably shouldn't be
EOF
	fi
	echo "" >&2
fi

if [ "$w_FLAG" = "y" -a ! -d $ROOT ]; then
	echo "WARNING: -w specified, but $ROOT does not exist;" \
	    "ignoring -w\n" >&2
	w_FLAG=n
fi

echo "\n==== Build version ====\n" >&2
echo $VERSION >&2

# Save the current proto area if we're comparing against the last build
if [ "$w_FLAG" = "y" -a -d "$ROOT" ]; then
    if [ -d "$ROOT.prev" ]; then
	rm -rf $ROOT.prev
    fi
    mv $ROOT $ROOT.prev
fi

# Echo the SCM types of $CODEMGR_WS
function child_wstype {
	typeset scm_type junk

	# Probe CODEMGR_WS to determine its type
	if [[ -d $CODEMGR_WS ]]; then
		$WHICH_SCM | read scm_type junk || exit 1
	fi

	case "$scm_type" in
	none|git|mercurial)
		;;
	*)	scm_type=none
		;;
	esac

	echo $scm_type
}

function run_bmake {
	echo "\n==== bmake $@ ====\n" >&2
	/bin/time env -i PATH=${GCC_ROOT}/bin:/usr/bin \
		SRCTOP=$CODEMGR_WS \
		bmake -j $DMAKE_MAX_JOBS \
			VERBOSE=yes \
			"$@"
}

# usage: bmake_build_step_args <dir> <target> <args...>
function bmake_build_step_args {
	D=$1
	shift

	echo "\n==== \`bmake -C $D $@\` at `date` ($LABEL) ====\n"

	if ! run_bmake -C $D "$@"; then
		build_ok=n
		this_build_ok=n
		return 1
	fi

	return 0
}

# usage: bmake_build_step_user <target>
function bmake_build_step_user {
	bmake_build_step_args $CODEMGR_WS $1 \
		DESTDIR=$ROOT
}

# usage: bmake_build_step_user_dir <dir> <target>
function bmake_build_step_user_dir {
	bmake_build_step_args $1 $2 \
		DESTDIR=$ROOT
}

SCM_TYPE=$(child_wstype)

# Safeguards
[[ -v CODEMGR_WS ]] || fatal_error "Error: Variable CODEMGR_WS not set."
[[ -d "${CODEMGR_WS}" ]] || fatal_error "Error: ${CODEMGR_WS} is not a directory."
[[ -f "${CODEMGR_WS}/usr/src/Makefile" ]] || fatal_error "Error: ${CODEMGR_WS}/usr/src/Makefile not found."

#
#	Generate the cfgparam files
#
# We have to do this before running *any* make commands.
#
bmake_build_step_args $CODEMGR_WS gen-config || build_extras_ok=n

#
#	Decide whether to clobber
#
if [ "$i_FLAG" = "n" -a -d "$SRC" ]; then
	echo "\n==== Make clobber at `date` ====\n"

	cd $SRC
	# remove old clobber file
	rm -f $SRC/clobber.out
	rm -f $SRC/clobber-${MACH}.out

	# Remove all .make.state* files, just in case we are restarting
	# the build after having interrupted a previous 'make clobber'.
	find . \( -name SCCS -o -name .hg -o -name .svn -o -name .git \
		-o -name 'interfaces.*' \) -prune \
		-o -name '.make.*' -print | xargs rm -f

	echo "\n==== Make clobber ERRORS ====\n" >&2
	if ! $MAKE -ek clobber; then
		build_extras_ok=n
	fi

	echo "\n==== Make tools clobber at `date` ====\n"
	cd ${TOOLS}
	rm -f ${TOOLS}/clobber-${MACH}.out

	echo "\n==== Make tools clobber ERRORS ====\n" >&2
	if ! $MAKE TOOLS_PROTO=$TOOLS_PROTO -ek clobber; then
		build_extras_ok=n
	fi
	rm -rf ${TOOLS_PROTO}
	mkdir -p ${TOOLS_PROTO}

	typeset roots=$ROOT
	echo "\n\nClearing $roots"
	rm -rf $roots

	# Get back to a clean workspace as much as possible to catch
	# problems that only occur on fresh workspaces.
	# Remove all .make.state* files, libraries, and .o's that may
	# have been omitted from clobber.  A couple of libraries are
	# under source code control, so leave them alone.
	# We should probably blow away temporary directories too.
	cd $SRC
	find $relsrcdirs \( -name SCCS -o -name .hg -o -name .svn \
	    -o -name .git -o -name 'interfaces.*' \) -prune -o \
	    \( -name '.make.*' -o -name 'lib*.a' -o -name 'lib*.so*' -o \
	       -name '*.o' \) -print | \
	    grep -v 'tools/ctf/dwarf/.*/libdwarf' | xargs rm -f
	echo "\n==== bmake cleandir ====\n"
	run_bmake -C $CODEMGR_WS cleandir
else
	echo "\n==== No clobber at `date` ====\n"
fi

echo "\n==== Build environment ====\n" >&2

# System
whence uname >&2
uname -a >&2
echo >&2

# make
whence $MAKE >&2
$MAKE -v >&2
echo "number of concurrent jobs = $DMAKE_MAX_JOBS" >&2

#
# Report the compiler versions.
#

if [[ ! -f $SRC/Makefile ]]; then
	build_ok=n
	echo "\nUnable to find \"Makefile\" in $SRC." >&2
	exit 1
fi

whence ld >&2
LDVER=`ld -V 2>&1`
echo $LDVER >&2
#
# Build and use the workspace's tools if requested
#
set_non_debug_build_flags

build_tools ${TOOLS_PROTO}
if (( $? != 0 )); then
	build_ok=n
else
	use_tools $TOOLS_PROTO
fi

normal_build

ORIG_SRC=$SRC
BINARCHIVE=${CODEMGR_WS}/bin-${MACH}.cpio.Z

if [ "$build_ok" = "y" ]; then
	echo "\n==== Creating protolist system file at `date` ====" \
	protolist $ROOT > $ATLOG/proto_list_${MACH}
	echo "==== protolist system file created at `date` ====\n" \

	if [ "$N_FLAG" != "y" ]; then

		E1=
		f1=
		for f in $f1; do
			if [ -f "$f" ]; then
				E1="$E1 -e $f"
			fi
		done

		E2=
		f2=
		if [ -d "$SRC/pkg" ]; then
			f2="$f2 exceptions/packaging"
		fi

		for f in $f2; do
			if [ -f "$f" ]; then
				E2="$E2 -e $f"
			fi
		done
	fi

	if [ "$N_FLAG" != "y" -a -d $SRC/pkg ]; then
		echo "\n==== Validating manifests against proto area ====\n" >&2
		if ! ( cd $SRC/pkg ; $MAKE -e protocmp ROOT="$ROOT" ) >&2; then
			build_extras_ok=n
		fi
	fi
fi

#
# ELF verification: ABI (-A) and runtime (-r) checks
#
if [[ ($build_ok = y) && (($A_FLAG = y) || ($r_FLAG = y)) ]]; then
	# Directory ELF-data.$MACH holds the files produced by these tests.
	elf_ddir=$SRC/ELF-data.$MACH

	# If there is a previous ELF-data backup directory, remove it. Then,
	# rotate current ELF-data directory into its place and create a new
	# empty directory
	rm -rf $elf_ddir.ref
	if [[ -d $elf_ddir ]]; then
		mv $elf_ddir $elf_ddir.ref
	fi
	mkdir -p $elf_ddir

	# Call find_elf to produce a list of the ELF objects in the proto area.
	# This list is passed to check_rtime and interface_check, preventing
	# them from separately calling find_elf to do the same work twice.
	find_elf -fr $ROOT > $elf_ddir/object_list

	if [[ $A_FLAG = y ]]; then
	       	echo "\n==== Check versioning and ABI information ====\n" >&2

		# Produce interface description for the proto. Report errors.
		interface_check -o -w $elf_ddir -f object_list \
			-i interface -E interface.err
		if [[ -s $elf_ddir/interface.err ]]; then
			cat $elf_ddir/interface.err >&2
			build_extras_ok=n
		fi

		# If ELF_DATA_BASELINE_DIR is defined, compare the new interface
		# description file to that from the baseline gate. Issue a
		# warning if the baseline is not present, and keep going.
		if [[ "$ELF_DATA_BASELINE_DIR" != '' ]]; then
			base_ifile="$ELF_DATA_BASELINE_DIR/interface"

		       	echo "\n==== Compare versioning and ABI information" \
			    "to baseline ====\n" >&2
		       	echo "Baseline:  $base_ifile\n"

			if [[ -f $base_ifile ]]; then
				interface_cmp -d -o $base_ifile \
				    $elf_ddir/interface > $elf_ddir/interface.cmp
				if [[ -s $elf_ddir/interface.cmp ]]; then
					echo >&2
					cat $elf_ddir/interface.cmp >&2
					build_extras_ok=n
				fi
			else
			       	echo "baseline not available. comparison" \
                                    "skipped" >&2
			fi

		fi
	fi

	if [[ $r_FLAG = y ]]; then
		echo "\n==== Check ELF runtime attributes ====\n" >&2

		# If we're doing a DEBUG build the proto area will be left
		# with debuggable objects, thus don't assert -s.
		if [[ $D_FLAG = y ]]; then
			rtime_sflag=""
		else
			rtime_sflag="-s"
		fi
		check_rtime -i -m -v $rtime_sflag -o -w $elf_ddir \
			-D object_list  -f object_list -E runtime.err \
			-I runtime.attr.raw
		if (( $? != 0 )); then
			build_extras_ok=n
		fi

		# check_rtime -I output needs to be sorted in order to 
		# compare it to that from previous builds.
		sort $elf_ddir/runtime.attr.raw > $elf_ddir/runtime.attr
		rm $elf_ddir/runtime.attr.raw

		# Report errors
		if [[ -s $elf_ddir/runtime.err ]]; then
			cat $elf_ddir/runtime.err >&2
			build_extras_ok=n
		fi

		# If there is an ELF-data directory from a previous build,
		# then diff the attr files. These files contain information
		# about dependencies, versioning, and runpaths. There is some
		# overlap with the ABI checking done above, but this also
		# flushes out non-ABI interface differences along with the
		# other information.
		echo "\n==== Diff ELF runtime attributes" \
		    "(since last build) ====\n" >&2

		if [[ -f $elf_ddir.ref/runtime.attr ]]; then
			diff $elf_ddir.ref/runtime.attr \
				$elf_ddir/runtime.attr >&2
		fi
	fi
fi

# "make check" begins

if [ "$i_CMD_LINE_FLAG" = "n" -a "$C_FLAG" = "y" ]; then
	# remove old check.out
	rm -f $SRC/check.out

	rm -f $SRC/check-${MACH}.out
	cd $SRC
	echo "\n==== cstyle/hdrchk errors ====\n" >&2
	if ! $MAKE -ek check ROOT="$ROOT"; then
		build_extras_ok=n
	fi
else
	echo "\n==== No '$MAKE check' ====\n"
fi

echo "\n==== Find core files ====\n" >&2

find $abssrcdirs -name core -a -type f -exec file {} \; >&2

# Verify that the usual lists of files, such as exception lists,
# contain only valid references to files.  If the build has failed,
# then don't check the proto area.
CHECK_PATHS=${CHECK_PATHS:-y}
if [ "$CHECK_PATHS" = y -a "$N_FLAG" != y ]; then
	echo "\n==== Check lists of files ====\n" >&2
	arg=-b
	[ "$build_ok" = y ] && arg=
	checkpaths $arg $ROOT > $SRC/check-paths.out 2>&1
	if [[ -s $SRC/check-paths.out ]]; then
		cat $SRC/check-paths.out >&2
		build_extras_ok=n
	fi
fi

if [ "$M_FLAG" != "y" -a "$build_ok" = y ]; then
	echo "\n==== Impact on file permissions ====\n" >&2

	abspkg=
	for d in $abssrcdirs; do
		if [ -d "$d/pkg" ]; then
			abspkg="$abspkg $d"
		fi
	done

	if [ -n "$abspkg" ]; then
		for d in "$abspkg"; do
			( cd $d/pkg ; $MAKE -e pmodes ) >&2
		done
	fi
fi

if [ "$w_FLAG" = "y" -a "$build_ok" = "y" ]; then
	if [[ "$D_FLAG" = y ]]; then
		do_wsdiff DEBUG $ROOT.prev $ROOT
	else
		do_wsdiff non-DEBUG $ROOT.prev $ROOT
	fi
fi

END_DATE=`date`
echo "==== Nightly $maketype build completed: $END_DATE ====" | \
    tee -a $build_time_file

typeset -i10 hours
typeset -Z2 minutes
typeset -Z2 seconds

elapsed_time=$SECONDS
((hours = elapsed_time / 3600 ))
((minutes = elapsed_time / 60  % 60))
((seconds = elapsed_time % 60))

echo "\n==== Total build time ====" | \
    tee -a $build_time_file
echo "\nreal    ${hours}:${minutes}:${seconds}" | \
    tee -a $build_time_file

#
# All done save for the sweeping up.
# (whichever exit we hit here will trigger the "cleanup" trap which
# optionally sends mail on completion).
#
if [[ "$build_ok" == "y" ]]; then
	if [[ "$W_FLAG" == "y" || "$build_extras_ok" == "y" ]]; then
		exit 0
	fi
fi

exit 1
