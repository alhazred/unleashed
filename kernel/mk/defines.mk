#
# This is a makefile fragment that's supposed to set the following
# variables:
#
#   KERNEL_CFLAGS		- CFLAGS for kernel code
#   KERNEL_LDFLAGS		- LDFLAGS for kernel code
#
# The bit-ness (32- or 64-bit) is selected by setting BITS to 32 or 64.
#
# The following can be tweaked to change up the KERNEL_CFLAGS variables:
#
#   KERNEL_CFLAGS		- CFLAGS used by both 32- & 64-bit
#   KERNEL_CFLAGS_$(BITS)
#   KERNEL_CFLAGS_$(CONFIG_MACH)
#   KERNEL_CFLAGS_$(CONFIG_MACH32)
#   KERNEL_CFLAGS_$(CONFIG_MACH64)
#   				- CFLAGS used by specific builds
#   KERNEL_INCLUDES		- -Ifoo directives used by all builds
#   KERNEL_INCLUDES_$(CONFIG_MACH)
#   				- -Ifoo directives for specific build
#   KERNEL_LDFLAGS		- LDFLAGS used by both 32- & 64-bit
#

#
# CFLAGS
#
# TODO: -fstack-protector will fail for 'unix'
KERNEL_CFLAGS_ALL = \
	-pipe \
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
	-I${REPOROOT}/usr/src/uts/common \
	-I${REPOROOT}/kernel/arch/${CONFIG_MACH}/include \
	-I${REPOROOT}/include

KERNEL_INCLUDES_i386 = \
	-I${REPOROOT}/usr/src/uts/intel

KERNEL_INCLUDES_sparc =

KERNEL_CFLAGS = \
	$(KERNEL_CFLAGS_ALL) \
	$(KERNEL_CFLAGS_$(BITS)) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH$(BITS))) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH)) \
	$(KERNEL_INCLUDES) \
	$(KERNEL_INCLUDES_$(CONFIG_MACH))

#
# LDFLAGS
#
KERNEL_LDFLAGS = \
	-r

#
# Programs
#
CC=/opt/gcc/4.4.4/bin/gcc
LD=/usr/bin/ld
INS=/usr/bin/install
CTFCONVERT=/opt/onbld/bin/i386/ctfconvert
CTFMERGE=/opt/onbld/bin/i386/ctfmerge

#
# Verbosity
#
.if !empty(VERBOSE) && ${VERBOSE} != "0" && ${VERBOSE} != "no"
QCC =
QLD =
QCTFCVT =
QCTFMRG =
.else
QCC = @echo "  CC (${BITS})   ${.IMPSRC}";
QLD = @echo "  LD (${BITS})   ${.TARGET}";
QCTFCVT = @
QCTFMRG = @
.endif
