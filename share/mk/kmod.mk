#
# Build and install a kernel module helper.
#
# See kernel/mk/kmod-build.mk for description of inputs used during the
# build.  This makefile is responsible for making and cleaning up object
# subdirectories.  All other tasks are handled by secondary makefile -
# kmod-build.mk.
#
# The config system tells us:
#
#   (1) whether to build this module at all
#   (2) if we're supposed to build it, should we build 32-bit or 64-bit?
#
# The following config variable addresses #2 above:
#
#   CONFIG_KERNEL_BITS = 64
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
.include <${SRCTOP}/cfgparam.mk>

all:
	@mkdir -p obj${CONFIG_KERNEL_BITS}
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk all \
		BITS=${CONFIG_KERNEL_BITS} SRCTOP=${SRCTOP}

clean cleandir:
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk clean \
		SRCTOP=${SRCTOP}
	@rm -rf obj${CONFIG_KERNEL_BITS}

install:
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk install-misc \
		SRCTOP=${SRCTOP}
	@${MAKE} -f ${SRCTOP}/kernel/mk/kmod-build.mk install \
		BITS=${CONFIG_KERNEL_BITS} SRCTOP=${SRCTOP}

.PHONY: all clean cleandir install

.endif
