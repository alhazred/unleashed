# this makefile handles all the post-build ops that were done serially in
# nightly.sh before.

all: dmake_check check_rtime interface_check packages

.if !defined(DESTDIR)
.error DESTDIR must be defined
.endif

clean::
	rm -f object_list

object_list:
	${SRCTOP}/usr/src/tools/scripts/find_elf -fr ${DESTDIR} > $@

dmake_check::
	cd ${SRCTOP}/usr/src && ${SRCTOP}/tools/bldenv.sh dmake -ek check

check_rtime:: object_list
	${SRCTOP}/tools/bldenv.sh ${SRCTOP}/usr/src/tools/scripts/check_rtime \
		-i -v -w ${.OBJDIR} -D object_list -f object_list \
		-I runtime.attr.raw

interface_check:: object_list
	${SRCTOP}/tools/bldenv.sh ${SRCTOP}/usr/src/tools/scripts/interface_check \
		-o -w ${.OBJDIR} -f object_list -i interface

pkg_check:: protocmp pmodes
stage-licenses::
	cd ${SRCTOP}/usr/src/pkg && ${SRCTOP}/tools/bldenv.sh dmake -e stage-licenses
protocmp:: stage-licenses
	cd ${SRCTOP}/usr/src/pkg && ${SRCTOP}/tools/bldenv.sh dmake -e protocmp >&2
pmodes:: stage-licenses
	cd ${SRCTOP}/usr/src/pkg && ${SRCTOP}/tools/bldenv.sh dmake -e pmodes >&2

packages:: pkg_check
	rm -rf ${SRCTOP}/packages/i386/nightly/repo.redist
	mkdir -p ${SRCTOP}/packages/i386/nightly/repo.redist
	cd ${SRCTOP}/usr/src/pkg && ${SRCTOP}/tools/bldenv.sh dmake -e install

UNLEASHED_OBJ?=		/usr/obj/${MACHINE}
.include <unleashed.mk>
.include <obj.mk>
