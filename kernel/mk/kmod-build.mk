#
# Build and install a kernel module
#
# Inputs from user's Makefile:
#
#   MODULE		- name of the kernel module
#   MODULE_TYPE		- "fs", "drv", etc.
#   MODULE_TYPE_LINKS	- "fs", "drv", etc. which will be hardlinked to
#   			  MODULE_TYPE
#   MODULE_DEPS		- dependencies
#   MODULE_CONF		- name of .conf file to install
#   MODULE_FW		- firmware files to install
#   SRCS		- source files
#   SRCS32		- additional source files (32-bit build only)
#   SRCS64		- additional source files (64-bit build only)
#   SRCS_DIRS		- additional source directories to search in
#   INCS		- compiler include directives
#   DEFS		- compiler defines (e.g., -DFOO -UBAR)
#   CERRWARN		- compiler error warning args (e.g., -Wno-parentheses)
#
# Additionally, we get the following values from kmod.mk:
#
#   BITS		- should we build a 32-bit or a 64-bit binary
#   REPOROOT		- root of the repository
#

.if empty(REPOROOT)
.error "You must define REPOROOT to point to the top-level of the repository"
.endif

.include <${REPOROOT}/Makefile.cfgparam>

# prevent kmod.mk inclusion in user's Makefile from setting up confusing targets
_KMOD_BUILD=yes
.include <${.CURDIR}/Makefile>

KERNEL_CFLAGS = \
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

CFLAGS = \
	$(KERNEL_CFLAGS) \
	$(KERNEL_CFLAGS_$(BITS)) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH$(BITS))) \
	$(KERNEL_CFLAGS_$(CONFIG_MACH)) \
	$(KERNEL_INCLUDES) \
	$(KERNEL_INCLUDES_$(CONFIG_MACH)) \
	$(CERRWARN) \
	$(INCS:%=-I%) \
	$(DEFS)

KERNEL_LDFLAGS = \
	-r

LDFLAGS = \
	$(KERNEL_LDFLAGS)

.if defined(MODULE_DEPS) && ${MODULE_DEPS} != ""
LDFLAGS += -dy $(MODULE_DEPS:%=-N %)
.endif

# generate all the hard link names even though we may not use it all
LINKS=
.if !empty(MODULE_TYPE_LINKS)
.for type in ${MODULE_TYPE_LINKS}
.if !empty(BITS) && ${BITS} == 32
LINKS += "/kernel/${MODULE_TYPE}/${MODULE}" \
	 "/kernel/${type}/${MODULE}"
.else
LINKS += "/kernel/${MODULE_TYPE}/${CONFIG_MACH64}/${MODULE}" \
	 "/kernel/${type}/${CONFIG_MACH64}/${MODULE}"
.endif
.endfor
.endif

.OBJDIR: ${.CURDIR}/obj${BITS}

OBJS =	$(SRCS:%.c=%.o) \
	$(SRCS$(BITS):%.c=%.o)

.if !empty(SRCS_DIRS)
.PATH: ${SRCS_DIRS:%=%}
.endif

CC=/opt/gcc/4.4.4/bin/gcc
LD=/usr/bin/ld
INS=/usr/bin/install
CTFCONVERT=/opt/onbld/bin/i386/ctfconvert
CTFMERGE=/opt/onbld/bin/i386/ctfmerge

.if !empty(VERBOSE) && ${VERBOSE} != "0" && ${VERBOSE} != "no"
QCC=
QLD=
QCTFCVT=
QCTFMRG=
.else
QCC=@echo "  CC (${BITS})  ${.IMPSRC}";
QLD=@echo "  LD (${BITS})  ${.TARGET}";
QCTFCVT=@
QCTFMRG=@
.endif

all: $(MODULE)

clean cleandir:

.include <links.mk>

install: $(MODULE)
.if !empty(BITS) && ${BITS} == 32
	$(INS) -d -m 755 "$(DESTDIR)/kernel/${MODULE_TYPE}"
	$(INS) -m 755 ${.ALLSRC} "$(DESTDIR)/kernel/${MODULE_TYPE}/${MODULE}"
.else
	$(INS) -d -m 755 "$(DESTDIR)/kernel/${MODULE_TYPE}/${CONFIG_MACH64}"
	$(INS) -m 755 ${.ALLSRC} "$(DESTDIR)/kernel/${MODULE_TYPE}/${CONFIG_MACH64}/${MODULE}"
.endif
.if !empty(LINKS)
	@set ${LINKS}; ${_LINKS_SCRIPT}
.endif

.PHONY: all clean cleandir install

install-misc: install-conf install-fw

install-conf: ${MODULE_CONF}
.if !empty(MODULE_CONF)
	$(INS) -d -m 755 "$(DESTDIR)/kernel/${MODULE_TYPE}"
	$(INS) -m 644 ${MODULE_CONF} "$(DESTDIR)/kernel/${MODULE_TYPE}/${MODULE}.conf"
.endif

install-fw: ${MODULE_FW}
.if !empty(MODULE_FW)
	$(INS) -d -m 755 "$(DESTDIR)/kernel/firmware/${MODULE}"
.for x in ${MODULE_FW}
	$(INS) -m 644 ${x} "$(DESTDIR)/kernel/firmware/${MODULE}"
.endfor
.endif

.PHONY: install-misc install-conf install-fw

$(MODULE): $(OBJS)
	${QLD}$(LD) $(LDFLAGS) -o ${.TARGET} ${.ALLSRC}
	${QCTFCVT}$(CTFCONVERT) -L VERSION ${.TARGET}

.SUFFIXES: .o

.c.o:
	@mkdir -p ${.TARGET:H}
	${QCC}$(CC) $(CFLAGS) -c -o ${.TARGET} ${.IMPSRC}
