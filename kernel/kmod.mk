#
# Build an install a kernel module
#
# Inputs:
#
#   MODULE		- name of the kernel module
#   MODULE_TYPE		- "fs", "drv", etc.
#   MODULE_TYPE_LINKS	- "fs", "drv", etc. which will be hardlinked to
#   			  MODULE_TYPE
#   MODULE_DEPS		- dependencies
#   SRCS		- source files
#   SRCS32		- additional source files (32-bit build only)
#   SRCS64		- additional source files (64-bit build only)
#   INCS		- compiler include directives
#   CERRWARN		- compiler error warning args (e.g., -Wno-parentheses)
#
# The config system tells us:
#
#   (1) whether to build this module at all
#   (2) if we're supposed to build it, should we build 32? 64? both?
#
# The following two config vars address #2 above:
#
#   CONFIG_BUILD_KMOD_32 = y
#   CONFIG_BUILD_KMOD_64 = y
#
# and the following config var tells us whether to bother with this
# particular module:
#
#   CONFIG_FS_FOOFS = y
#
# If we are not supposed to care about this module, we do not even recurse
# into the module's build directory.
#
# This way, we separate the concerns of defining a module and how it is
# (generically) built, from which platforms/ISA/whatever we're supposed to
# build the module for.
#

.include <../../../Makefile.cfgparam>

KERNEL_CFLAGS = \
	-fident \
	-finline \
	-fno-inline-functions \
	-fno-builtin \
	-fno-asm \
	-fdiagnostics-show-option \
	-nodefaultlibs \
	-D_ASM_INLINES \
	-ffreestanding \
	-std=gnu99 \
	-g \
	-Wall \
	-Wextra \
	-Werror \
	-Wno-missing-braces \
	-Wno-sign-compare \
	-Wno-unknown-pragmas \
	-Wno-unused-parameter \
	-Wno-missing-field-initializers \
	-fno-inline-small-functions \
	-fno-inline-functions-called-once \
	-fno-ipa-cp \
	-fstack-protector \
	-D_KERNEL \
	-D_SYSCALL32 \
	-D_DDI_STRICT \
	-D__sun \
	-nostdinc

# TODO: support for debug builds
# KERNEL_CFLAGS += -DDEBUG

KERNEL_CFLAGS_32 = \
	-m32

KERNEL_CFLAGS_64 = \
	-m64 \
	-D_ELF64

KERNEL_CFLAGS_i386 = \
	-mno-mmx \
	-mno-sse

KERNEL_CFLAGS_i86 = \
	-O \
	-march=pentiumpro

KERNEL_CFLAGS_amd64 = \
	-O2 \
	-Dsun \
	-D__SVR4 \
	-Ui386 \
	-U__i386 \
	-mtune=opteron \
	-msave-args \
	-mcmodel=kernel \
	-fno-strict-aliasing \
	-fno-unit-at-a-time \
	-fno-optimize-sibling-calls \
	-mno-red-zone \
	-D_SYSCALL32_IMPL

KERNEL_CFLAGS_sparc =
KERNEL_CFLAGS_sparcv7 =
KERNEL_CFLAGS_sparcv9 =

KERNEL_INCLUDES = \
	-I../../../usr/src/uts/common

KERNEL_INCLUDES_i386 = \
	-I../../../usr/src/uts/intel

KERNEL_INCLUDES_sparc =

CFLAGS32 = \
	$(KERNEL_CFLAGS) \
	$(KERNEL_CFLAGS_32) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH32)) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH)) \
	$(KERNEL_INCLUDES) \
	$(KERNEL_INCLUDES_$(CONFIG_MACH)) \
	$(CERRWARN) \
	$(INCS)

CFLAGS64 = \
	$(KERNEL_CFLAGS) \
	$(KERNEL_CFLAGS_64) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH64)) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH)) \
	$(KERNEL_INCLUDES) \
	$(KERNEL_INCLUDES_$(CONFIG_MACH)) \
	$(CERRWARN) \
	$(INCS)

KERNEL_LDFLAGS = \
	-r

LDFLAGS = \
	$(KERNEL_LDFLAGS)

.if defined(MODULE_DEPS) && ${MODULE_DEPS} != ""
LDFLAGS += -dy $(MODULE_DEPS:%=-N %)
.endif

# generate all the hard link names even though we may not use it all
LINKS32=
LINKS64=
.if !empty(MODULE_TYPE_LINKS)
.for type in ${MODULE_TYPE_LINKS}
LINKS32+="/kernel/${MODULE_TYPE}/${MODULE}" \
	 "/kernel/${type}/${MODULE}"
LINKS64+="/kernel/${MODULE_TYPE}/${CONFIG_MACH64}/${MODULE}" \
	 "/kernel/${type}/${CONFIG_MACH64}/${MODULE}"
.endfor
.endif

OBJS32=$(SRCS:%.c=%-32.o) $(SRCS32:%.c=%-32.o)
OBJS64=$(SRCS:%.c=%-64.o) $(SRCS64:%.c=%-64.o)

MODULES=
INSTALLTGTS=
.if defined(CONFIG_FS_PCFS) && ${CONFIG_FS_PCFS} == "y"
.if defined(CONFIG_BUILD_KMOD_32) && ${CONFIG_BUILD_KMOD_32} == "y"
MODULES+=$(MODULE)-32
INSTALLTGTS+=install-32
.endif
.if defined(CONFIG_BUILD_KMOD_64) && ${CONFIG_BUILD_KMOD_64} == "y"
MODULES+=$(MODULE)-64
INSTALLTGTS+=install-64
.endif
.endif

CC=/opt/gcc/4.4.4/bin/gcc
LD=/usr/bin/ld
INS=/usr/bin/install
CTFCONVERT=/opt/onbld/bin/i386/ctfconvert
CTFMERGE=/opt/onbld/bin/i386/ctfmerge

all: $(MODULES)

clean cleandir:
	rm -f $(MODULE)-32 $(MODULE)-64 $(OBJS32) $(OBJS64)

install: $(INSTALLTGTS)

.include <links.mk>

install-32: $(MODULE)-32
	$(INS) -d -m 755 "$(DESTDIR)/kernel/${MODULE_TYPE}"
	$(INS) -m 755 ${.ALLSRC} "$(DESTDIR)/kernel/${MODULE_TYPE}/${MODULE}"
.if !empty(LINKS32)
	@set ${LINKS32}; ${_LINKS_SCRIPT}
.endif
	
install-64: $(MODULE)-64
	$(INS) -d -m 755 "$(DESTDIR)/kernel/${MODULE_TYPE}/${CONFIG_MACH64}"
	$(INS) -m 755 ${.ALLSRC} "$(DESTDIR)/kernel/${MODULE_TYPE}/${CONFIG_MACH64}/${MODULE}"
.if !empty(LINKS64)
	@set ${LINKS64}; ${_LINKS_SCRIPT}
.endif

.PHONY: all clean install-32 install-64

$(MODULE)-32: $(OBJS32)
	$(LD) $(LDFLAGS) -o ${.TARGET} ${.ALLSRC}
	$(CTFMERGE) -L VERSION -o ${.TARGET} ${.ALLSRC}

$(MODULE)-64: $(OBJS64)
	$(LD) $(LDFLAGS) -o ${.TARGET} ${.ALLSRC}
	$(CTFMERGE) -L VERSION -o ${.TARGET} ${.ALLSRC}

.SUFFIXES: -32.o -64.o

.c-32.o:
	$(CC) $(CFLAGS32) -c -o ${.TARGET} ${.IMPSRC}
	$(CTFCONVERT) -i -L VERSION ${.TARGET}

.c-64.o:
	$(CC) $(CFLAGS64) -c -o ${.TARGET} ${.IMPSRC}
	$(CTFCONVERT) -i -L VERSION ${.TARGET}
