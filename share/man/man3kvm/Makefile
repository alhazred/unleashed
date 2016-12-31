MAN = kvm_getu.3kvm \
      kvm_kread.3kvm \
      kvm_nextproc.3kvm \
      kvm_nlist.3kvm \
      kvm_open.3kvm \
      kvm_read.3kvm

MLINKS+=	kvm_getu.3kvm kvm_getcmd.3kvm
MLINKS+=	kvm_kread.3kvm kvm_kwrite.3kvm
MLINKS+=	kvm_kread.3kvm kvm_uread.3kvm
MLINKS+=	kvm_kread.3kvm kvm_uwrite.3kvm
MLINKS+=	kvm_nextproc.3kvm kvm_getproc.3kvm
MLINKS+=	kvm_nextproc.3kvm kvm_setproc.3kvm
MLINKS+=	kvm_open.3kvm kvm_close.3kvm
MLINKS+=	kvm_read.3kvm kvm_write.3kvm

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
