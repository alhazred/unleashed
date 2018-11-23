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
# Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
# Use is subject to license terms.
#
# ident	"%Z%%M%	%I%	%E% SMI"

#
# set path, but inherit /tmp/bfubin if it is sane
#
if [ "`echo $PATH | cut -f 1 -d :`" = /tmp/bfubin ] && \
    [ -O /tmp/bfubin ] ; then
	export PATH=/tmp/bfubin:/usr/sbin:/usr/bin:/sbin
else
	export PATH=/usr/sbin:/usr/bin:/sbin
fi

usage() {
	echo "This utility is a component of the bootadm(8) implementation"
	echo "and it is not recommended for stand-alone use."
	echo "Please use bootadm(8) instead."
	echo ""
	echo "Usage: ${0##*/}: [-R <root>] <filelist> ..."
	exit 2
}

build_platform() {

	altroot=$1

	(
		cd $altroot/
		if [ -z "$STRIP" ] ; then
			ls -d platform/*/kernel
		else
			ls -d platform/*/kernel | grep -v $STRIP
		fi
	)
}

altroot=""
filelists=
platform_provided=no

OPTIND=1
while getopts R:p: FLAG
do
        case $FLAG in
        R)	if [ "$OPTARG" != "/" ]; then
			altroot="$OPTARG"
		fi
		;;
        *)      usage
		;;
        esac
done

shift `expr $OPTIND - 1`
if [ $# -eq 0 ]; then
	usage
fi

filelists=$*

#
# If the target platform is provided, as is the case for diskless,
# or we're building an archive for this machine, we can build
# a smaller archive by not including unnecessary components.
#
filtering=no

for list in $filelists
do
	if [ -f $altroot/$list ]; then
		grep ^platform$ $altroot/$list > /dev/null
		if [ $? = 0 ] ; then
			build_platform $altroot
			if [ -z "$STRIP" ] ; then
				cat $altroot/$list | grep -v ^platform$
			else
				cat $altroot/$list | grep -v ^platform$ | \
				    grep -v $STRIP
			fi
		else
			if [ -z "$STRIP" ] ; then
				cat $altroot/$list
			else
				cat $altroot/$list | grep -v $STRIP
			fi
		fi

	fi
done

exit 0
