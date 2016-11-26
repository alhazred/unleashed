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

.include <${REPOROOT}/kernel/mk/defines.mk>

CFLAGS = \
	$(KERNEL_CFLAGS) \
	$(CERRWARN) \
	$(INCS:%=-I%) \
	$(DEFS)

LDFLAGS = $(KERNEL_LDFLAGS)
.if !empty(MODULE_DEPS)
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
