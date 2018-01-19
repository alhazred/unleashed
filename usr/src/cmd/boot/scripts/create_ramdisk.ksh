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
dirsize=0

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
	ISA=i386
	ARCH64=amd64
	;;
i86pc)	ISA=i386
	ARCH64=amd64
	;;
*)	usage
	;;
esac

BOOT_ARCHIVE=platform/$PLATFORM/boot_archive
BOOT_ARCHIVE_64=platform/$PLATFORM/$ARCH64/boot_archive

function cleanup
{
	[ -n "$rddir" ] && rm -fr "$rddir" 2> /dev/null
	[ -n "$new_rddir" ] && rm -fr "$new_rddir" 2>/dev/null
}

function getsize
{
	# Estimate image size and add 10% overhead for ufs stuff.
	# Note, we can't use du here in case we're on a filesystem, e.g. zfs,
	# in which the disk usage is less than the sum of the file sizes.
	# The nawk code 
	#
	#	{t += ($5 % 1024) ? (int($5 / 1024) + 1) * 1024 : $5}
	#
	# below rounds up the size of a file/directory, in bytes, to the
	# next multiple of 1024.  This mimics the behavior of ufs especially
	# with directories.  This results in a total size that's slightly
	# bigger than if du was called on a ufs directory.
	size=$(cat "$list" | xargs -I {} ls -lLd "{}" 2> /dev/null |
		nawk '{t += ($5 % 1024) ? (int($5 / 1024) + 1) * 1024 : $5}
		END {print int(t * 1.10 / 1024)}')
	(( total_size = size + dirsize ))
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
		cat "$cpiofile" > "${archive}-new"
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
		mv "${archive}-new" "$archive"
		rm -f "$archive.hash"
		mv "$archive.hash-new" "$archive.hash" 2> /dev/null
	fi
}

function duplicate_archive
{
	src="$1"
	dst="$2"

	if ! cp "$src" "$dst-new" ; then
		ERROR=1
	elif ! cp "$src.hash" "$dst.hash-new" ; then
		ERROR=1
		rm "$dst-new"
	else
		mv "$dst-new" "$dst"
		mv "$dst.hash-new" "$dst.hash"
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
# We use /tmp/ for scratch space now.  This may be changed later if there
# is insufficient space in /tmp/.
#
rddir="/tmp/create_ramdisk.$$.tmp"
new_rddir=
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
find $filelist -print 2>/dev/null | while read path
do
	if [ -d "$path" ]; then
		size=`ls -lLd "$path" | nawk '
		    {print ($5 % 1024) ? (int($5 / 1024) + 1) * 1024 : $5}'`
		(( dirsize += size ))
	else
		print "$path"
	fi
done >"$list"

# calculate image size
getsize

# check to see if there is sufficient space in tmpfs 
#
tmp_free=`df -b /tmp | tail -1 | awk '{ printf ($2) }'`
(( tmp_free = tmp_free / 3 ))

if [ $total_size -gt $tmp_free  ] ; then
	# assumes we have enough scratch space on $ALT_ROOT
	new_rddir="/$ALT_ROOT/var/tmp/create_ramdisk.$$.tmp"
	rm -rf "$new_rddir"
	mkdir "$new_rddir" || fatal_error \
	    "Could not create temporary directory $new_rddir"

	# Save the file list
	mv "$list" "$new_rddir"/
	list="/$new_rddir/filelist"

	# Remove the old $rddir and set the new value of rddir
	rm -rf "$rddir"
	rddir="$new_rddir"
	new_rddir=
fi

cpiofile="$rddir/cpio.file"

create_archive "$ALT_ROOT/$BOOT_ARCHIVE"

if [ $ERROR = 1 ]; then
	cleanup
	exit 1
fi

#
# For now, use the same archive for both bare and $ARCH64 files.
#
duplicate_archive "$ALT_ROOT/$BOOT_ARCHIVE" "$ALT_ROOT/$BOOT_ARCHIVE_64"

[ -n "$rddir" ] && rm -rf "$rddir"
