#!/bin/ksh -p
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

# Copyright 2016 Toomas Soome <tsoome@me.com>
# Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#

#
# Copyright (c) 2014 by Delphix. All rights reserved.
#

ALT_ROOT=
EXTRACT_ARGS=
ERROR=0

usage() {
	echo "This utility is a component of the bootadm(1M) implementation"
	echo "and it is not recommended for stand-alone use."
	echo "Please use bootadm(1M) instead."
	echo ""
	echo "Usage: ${0##*/}: [-R \<root\>] [-p \<platform\>]"
	echo "where \<platform\> is one of: i86pc"
	exit
}

# default platform is what we're running on
PLATFORM=`uname -m`

export PATH=/usr/sbin:/usr/bin:/sbin
export GZIP_CMD=/usr/bin/gzip

EXTRACT_FILELIST="/boot/solaris/bin/extract_boot_filelist"

#
# Parse options
#
while [ "$1" != "" ]
do
        case $1 in
        -R)	shift
		ALT_ROOT="$1"
		if [ "$ALT_ROOT" != "/" ]; then
			echo "Creating boot_archive for $ALT_ROOT"
			EXTRACT_ARGS="${EXTRACT_ARGS} -R ${ALT_ROOT}"
			EXTRACT_FILELIST="${ALT_ROOT}${EXTRACT_FILELIST}"
		fi
		;;
	-p)	shift
		PLATFORM="$1"
		EXTRACT_ARGS="${EXTRACT_ARGS} -p ${PLATFORM}"
		;;
        *)      usage
		;;
        esac
	shift
done

shift `expr $OPTIND - 1`

if [ $# -eq 1 ]; then
	ALT_ROOT="$1"
	echo "Creating boot_archive for $ALT_ROOT"
fi

case $PLATFORM in
i386)	PLATFORM=i86pc
	;;
i86pc)	;;
*)	usage
	;;
esac

BOOT_ARCHIVE=platform/$PLATFORM/boot_archive

function cleanup
{
	[ -n "$rddir" ] && rm -fr "$rddir" 2> /dev/null
}

function create_cpio
{
	archive=$1

	cpio -o -H odc < "$list" > "$cpiofile"

	[ -x /usr/bin/digest ] && /usr/bin/digest -a sha1 "$cpiofile" > "$archive.hash-new"

	#
	# Check if gzip exists in /usr/bin, so we only try to run gzip
	# on systems that have gzip.
	#
	if [ -x $GZIP_CMD ] ; then
		gzip -c "$cpiofile" > "${archive}-new"
	else
		mv "$cpiofile" "${archive}-new"
	fi
	
	if [ $? -ne 0 ] ; then
		rm -f "${archive}-new" "$achive.hash-new"
	fi
}

function create_archive
{
	archive=$1

	echo "updating $archive"

	create_cpio "$archive"

	if [ ! -e "${archive}-new" ] ; then
		#
		# Two of these functions may be run in parallel.  We
		# need to allow the other to clean up, so we can't
		# exit immediately.  Instead, we set a flag.
		#
		echo "update of $archive failed"
		ERROR=1
	else
		# remove the hash first, it's better to have a boot archive
		# without a hash, than one with a hash for its predecessor
		rm -f "$archive.hash"
		mv "${archive}-new" "$archive"
		mv "$archive.hash-new" "$archive.hash" 2> /dev/null
	fi
}

function fatal_error
{
	print -u2 $*
	exit 1
}

#
# get filelist
#
if [ ! -f "$ALT_ROOT/boot/solaris/filelist.ramdisk" ] &&
    [ ! -f "$ALT_ROOT/etc/boot/solaris/filelist.ramdisk" ]
then
	print -u2 "Can't find filelist.ramdisk"
	exit 1
fi
filelist=$($EXTRACT_FILELIST $EXTRACT_ARGS \
	/boot/solaris/filelist.ramdisk \
	/etc/boot/solaris/filelist.ramdisk \
		2>/dev/null | sort -u)

#
# We assume there is enough space on $ALT_ROOT.  We create the temporary
# file in /platform because then it is just a local filesystem move (instead
# of a cross-mountpoint move).  So, if we successufully create the boot
# archive temp file, we will likely succeed in moving it into place.
#
rddir="/$ALT_ROOT/platform/create_ramdisk.$$.tmp"
rm -rf "$rddir"
mkdir "$rddir" || fatal_error "Could not create temporary directory $rddir"

# Clean up upon exit.
trap 'cleanup' EXIT

list="$rddir/filelist"

touch $list

#
# This loop creates the lists of files.  The list is written to stdout,
# which is redirected at the end of the loop.
#
cd "/$ALT_ROOT"
find $filelist -print 2>/dev/null > "$list"

cpiofile="$rddir/cpio.file"

create_archive "$ALT_ROOT/$BOOT_ARCHIVE"

if [ $ERROR = 1 ]; then
	cleanup
	exit 1
fi

[ -n "$rddir" ] && rm -rf "$rddir"
