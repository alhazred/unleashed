#
# Copyright (c) 2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
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

function(simple_c_test type section bin data)
	add_test(NAME "${type}:${section}:${data}"
		 COMMAND "${CMAKE_BINARY_DIR}/test_${bin}"
			 "${CMAKE_CURRENT_SOURCE_DIR}/${data}"
	)
endfunction()

macro(build_test_bin name)
	add_executable("test_${name}"
		"test_${name}.c"
	)

	target_link_libraries("test_${name}"
		jeffpc
	)
endmacro()

macro(build_test_bin_and_run name)
	build_test_bin(${name})
	add_test(NAME "${name}"
		 COMMAND "${CMAKE_BINARY_DIR}/test_${name}"
	)
endmacro()
