# this makefile handles all the post-build ops that were done serially in
# nightly.sh before.
#
# because we run dmake, this makefile is special and should only be run from
# bldenv or nightly.sh, which put in env the stuff that the dmake dirs expect.

all: dmake_check check_rtime interface_check packages protocmp
.PHONY: dmake_check check_rtime interface_check packages protocmp clean

.if !defined(SRC) || !defined(ROOT) || !defined(DMAKE_MODE) || !defined(PKGARCHIVE)
.error postbuild Makefile must be run from bldenv
.endif

clean:
	rm -f object_list

object_list:
	${SRCTOP}/usr/src/tools/scripts/find_elf -fr ${ROOT} > $@

dmake_check:
	cd ${SRCTOP}/usr/src && MAKEFLAGS= dmake -ek check ROOT=${ROOT}

check_rtime: object_list
	${SRCTOP}/usr/src/tools/scripts/check_rtime -i -v -w ${.OBJDIR}\
		-D object_list -f object_list -I runtime.attr.raw

interface_check: object_list
	${SRCTOP}/usr/src/tools/scripts/interface_check -o -w ${.OBJDIR}\
		-f object_list -i interface

protocmp:
	cd ${SRCTOP}/usr/src/pkg && MAKEFLAGS= dmake -e protocmp ROOT=${ROOT}

packages: protocmp
	rm -rf ${PKGARCHIVE}
	mkdir -p ${PKGARCHIVE}
	cd ${SRCTOP}/usr/src/pkg && MAKEFLAGS= dmake -e install

UNLEASHED_OBJ?=		/usr/obj/${MACHINE}
.include <unleashed.mk>
.include <obj.mk>
