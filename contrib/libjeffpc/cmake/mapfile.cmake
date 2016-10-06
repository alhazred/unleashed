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

macro(target_apply_mapfile tgt mapfile)
	if (HAVE_GNU_LD)
		set(ld_script "${CMAKE_CURRENT_BINARY_DIR}/${mapfile}.gnu")
		set(ld_flag "'-Wl,${ld_script}'")
		add_custom_command(
			OUTPUT "${ld_script}"
			COMMAND echo "VERSION {" > "${ld_script}"
			COMMAND cat "${CMAKE_CURRENT_SOURCE_DIR}/${mapfile}" >> "${ld_script}"
			COMMAND echo "'};'" >> "${ld_script}"
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${mapfile}"
			COMMENT "Generating ${mapfile}.gnu for ${tgt}"
		)
		add_custom_target("${tgt}-${mapfile}" DEPENDS "${ld_script}")
		add_dependencies("${tgt}" "${tgt}-${mapfile}")
	else()
		# This is assuming an illumos-style -M linker option &
		# mapfile format
		set(ld_script "${CMAKE_CURRENT_SOURCE_DIR}/${mapfile}")
		set(ld_flag "-Wl,-M '${ld_script}'")
	endif()
	set_target_properties("${tgt}" PROPERTIES
		LINK_FLAGS "${ld_flag}"
		LINK_DEPENDS "${ld_script}"
	)
endmacro()
