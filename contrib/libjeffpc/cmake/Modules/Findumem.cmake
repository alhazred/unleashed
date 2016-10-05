#
# Copyright (c) 2015 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

#
# Find the UMEM includes and library.
#
# This module defines:
#   UMEM_INCLUDE_DIR
#   UMEM_LIBRARY
#   UMEM_FOUND
#

find_path(UMEM_INCLUDE_DIR umem.h)
find_library(UMEM_LIBRARY NAMES umem)

#
# Handle the QUIETLY and REQUIRED arguments and set UMEM_FOUND to TRUE if
# all listed variables are TRUE.
#
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UMEM DEFAULT_MSG UMEM_LIBRARY
	UMEM_INCLUDE_DIR)
