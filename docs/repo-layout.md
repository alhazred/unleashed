Repository Layout
=================

The following directory listing is *not* meant to be exhaustive.  Rather, it
is intended to demonstrate the general organization.  Deviations should be
avoided but are by no means forbidden.

```
arch				- architecture/platform specific code
arch/sparc			- sparc architecture/platform specific code
arch/x86			- x86 architecture/platform specific code
bin				- commands (/bin /usr/bin)
contrib				- 3rd party code
docs				- documentation
include				- headers (/usr/include)
kernel				- all arch/plat independent kernel code
kernel/comstar			- ... COMSTAR
kernel/cpr			- ... suspend & resume
kernel/disp			- ... dispatcher
kernel/drivers			- ... drivers
kernel/drivers/block		- ... block device drivers
kernel/drivers/net		- ... NIC drivers
kernel/drivers/net/bar		- ... the 'bar' NIC driver
kernel/fs			- ... file system code
kernel/fs/ufs			- ... UFS kernel code
kernel/krtld			- ... linker
kernel/mk			- kernel build makefiles
kernel/net			- ... network code
kernel/os			- ... kernel code
kernel/sched			- ... scheduler code
kernel/syscall			- ... syscall code
kernel/vm			- ... vm code
lib				- libraries
share				- misc files (/usr/share)
tools				- build tools
```

Architecture Subdirectories
---------------------------

In general, the structure of each subdirectory in `arch` mirrors the same
layout as the top level.  That is, there are sub-subdirectories such as
`include` and `kernel`.  In other words, the contents are:

```
arch/x86/bin			- x86 specific commands
arch/x86/include		- x86 specific headers
arch/x86/kernel			- x86 specific kernel code
arch/x86/lib			- x86 specific libs
```

In general, every effort should be made to avoid putting code in
architecture specific directories.  That is, if a feature is in theory
architecture independent, the code code should be put in the architecture
independent directories.  If only some architectures are not fully
supported, proper Sconfig based build-time guards should be used.

Example 1: a PCI device driver that hasn't been ported to sparc should live
in `kernel/drivers` even though it only builds on x86 since PCI devices are
standard between architectures.

Example 2: a CPU microcode updating driver should be placed in `arch/x86`
since microcode update procedures are architecture specific.
