#
# Build and install a kernel module helper.
#
# See kernel/mk/kmod-build.mk for description of inputs used during the
# build.  This makefile is responsible for making and cleaning up object
# subdirectories.  All other tasks are handled by secondary makefile -
# kmod-build.mk which we invoke for each of the bit-nesses.
#
# The config system tells us:
#
#   (1) whether to build this module at all
#   (2) if we're supposed to build it, should we build 32-bit? 64-bit? both?
#
# The following two config vars address #2 above:
#
#   CONFIG_BUILD_KMOD_32 = y
#   CONFIG_BUILD_KMOD_64 = y
#
# and the following config var tells us whether to bother with this
# particular module:
#
#   CONFIG_FOOBAR = y
#
# If we are not supposed to care about this module, we do not even recurse
# into the module's build directory.
#
# This way, we separate the concerns of defining a module and how it is
# (generically) built, from which platforms/ISA/whatever we're supposed to
# build the module for.
#
# To avoid recursively including kmod-build.mk, this makefile turns into a
# giant no-op if _KMOD_BUILD is set.
#

.if empty(_KMOD_BUILD)

.include <unleashed.mk>
.include <${SRCTOP}/Makefile.cfgparam>

BUILD=
.if defined(CONFIG_BUILD_KMOD_32) && ${CONFIG_BUILD_KMOD_32} == "y"
BUILD += 32
.endif
.if defined(CONFIG_BUILD_KMOD_64) && ${CONFIG_BUILD_KMOD_64} == "y"
BUILD += 64
.endif

all: ${BUILD:%=all-%}

# we don't use a for loop to allow for more parallelism
all-32:
	@mkdir -p obj32
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk all \
		BITS=32 SRCTOP=${SRCTOP}

all-64:
	@mkdir -p obj64
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk all \
		BITS=64 SRCTOP=${SRCTOP}

clean cleandir:
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk clean \
		SRCTOP=${SRCTOP}
	@rm -rf obj32 obj64

install:
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk install-misc \
		SRCTOP=${SRCTOP}
.for bits in ${BUILD}
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk install \
		BITS=${bits} SRCTOP=${SRCTOP}
.endfor

.PHONY: all all-32 all-64 clean cleandir install

.endif
