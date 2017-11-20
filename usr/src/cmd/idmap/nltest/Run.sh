#!/bin/sh

#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet at
# http://www.illumos.org/license/CDDL.
#

#
# Copyright 2014 Nexenta Systems, Inc.  All rights reserved.
#

# Helper program to run nltest


[ -n "$SRCTOP" ] || {
  echo "Need a buildenv to set SRCTOP=..."
  exit 1;
}


ROOT=${SRCTOP}/proto/root_i386
LD_LIBRARY_PATH=$ROOT/usr/lib:$ROOT/lib
export LD_LIBRARY_PATH

$ROOT/usr/sbin/nltest "$@"
