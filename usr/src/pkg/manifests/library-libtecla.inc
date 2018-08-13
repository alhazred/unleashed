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
# Copyright 2012 Nexenta Systems, Inc. All rights reserved.
#

dir path=usr group=sys
dir path=usr/bin
dir path=usr/include
dir path=usr/lib
dir path=usr/lib/$(ARCH64)
dir path=usr/share/man
dir path=usr/share/man/man1
dir path=usr/share/man/man3lib
dir path=usr/share/man/man3tecla
dir path=usr/share/man/man5
file path=usr/bin/enhance mode=0555
file path=usr/include/libtecla.h
file path=usr/lib/$(ARCH64)/libtecla.so.1
file path=usr/lib/libtecla.so.1
file path=usr/share/man/man1/enhance.1
file path=usr/share/man/man3lib/libtecla.3lib
file path=usr/share/man/man3tecla/cpl_complete_word.3tecla
file path=usr/share/man/man3tecla/ef_expand_file.3tecla
file path=usr/share/man/man3tecla/gl_get_line.3tecla
file path=usr/share/man/man3tecla/gl_io_mode.3tecla
file path=usr/share/man/man3tecla/libtecla_version.3tecla
file path=usr/share/man/man3tecla/pca_lookup_file.3tecla
file path=usr/share/man/man5/tecla.5
license usr/src/lib/libtecla/THIRDPARTYLICENSE \
    license=usr/src/lib/libtecla/THIRDPARTYLICENSE
link path=usr/lib/$(ARCH64)/libtecla.so target=libtecla.so.1
link path=usr/lib/libtecla.so target=libtecla.so.1
