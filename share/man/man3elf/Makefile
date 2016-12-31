MAN = elf.3elf \
      elf32_checksum.3elf \
      elf32_fsize.3elf \
      elf32_getehdr.3elf \
      elf32_getphdr.3elf \
      elf32_getshdr.3elf \
      elf32_xlatetof.3elf \
      elf_begin.3elf \
      elf_cntl.3elf \
      elf_errmsg.3elf \
      elf_fill.3elf \
      elf_flagdata.3elf \
      elf_getarhdr.3elf \
      elf_getarsym.3elf \
      elf_getbase.3elf \
      elf_getdata.3elf \
      elf_getident.3elf \
      elf_getscn.3elf \
      elf_hash.3elf \
      elf_kind.3elf \
      elf_rawfile.3elf \
      elf_strptr.3elf \
      elf_update.3elf \
      elf_version.3elf \
      gelf.3elf \
      nlist.3elf

MLINKS+=	elf32_checksum.3elf elf64_checksum.3elf
MLINKS+=	elf32_fsize.3elf elf64_fsize.3elf
MLINKS+=	elf32_getehdr.3elf elf32_newehdr.3elf
MLINKS+=	elf32_getehdr.3elf elf64_getehdr.3elf
MLINKS+=	elf32_getehdr.3elf elf64_newehdr.3elf
MLINKS+=	elf32_getphdr.3elf elf32_newphdr.3elf
MLINKS+=	elf32_getphdr.3elf elf64_getphdr.3elf
MLINKS+=	elf32_getphdr.3elf elf64_newphdr.3elf
MLINKS+=	elf32_getshdr.3elf elf64_getshdr.3elf
MLINKS+=	elf32_xlatetof.3elf elf32_xlatetom.3elf
MLINKS+=	elf32_xlatetof.3elf elf64_xlatetof.3elf
MLINKS+=	elf32_xlatetof.3elf elf64_xlatetom.3elf
MLINKS+=	elf_begin.3elf elf_end.3elf
MLINKS+=	elf_begin.3elf elf_memory.3elf
MLINKS+=	elf_begin.3elf elf_next.3elf
MLINKS+=	elf_begin.3elf elf_rand.3elf
MLINKS+=	elf_errmsg.3elf elf_errno.3elf
MLINKS+=	elf_flagdata.3elf elf_flagehdr.3elf
MLINKS+=	elf_flagdata.3elf elf_flagelf.3elf
MLINKS+=	elf_flagdata.3elf elf_flagphdr.3elf
MLINKS+=	elf_flagdata.3elf elf_flagscn.3elf
MLINKS+=	elf_flagdata.3elf elf_flagshdr.3elf
MLINKS+=	elf_getdata.3elf elf_newdata.3elf
MLINKS+=	elf_getdata.3elf elf_rawdata.3elf
MLINKS+=	elf_getident.3elf elf_getphdrnum.3elf
MLINKS+=	elf_getident.3elf elf_getphnum.3elf
MLINKS+=	elf_getident.3elf elf_getshdrnum.3elf
MLINKS+=	elf_getident.3elf elf_getshdrstrndx.3elf
MLINKS+=	elf_getident.3elf elf_getshnum.3elf
MLINKS+=	elf_getident.3elf elf_getshstrndx.3elf
MLINKS+=	elf_getscn.3elf elf_ndxscn.3elf
MLINKS+=	elf_getscn.3elf elf_newscn.3elf
MLINKS+=	elf_getscn.3elf elf_nextscn.3elf
MLINKS+=	gelf.3elf gelf_checksum.3elf
MLINKS+=	gelf.3elf gelf_fsize.3elf
MLINKS+=	gelf.3elf gelf_getcap.3elf
MLINKS+=	gelf.3elf gelf_getclass.3elf
MLINKS+=	gelf.3elf gelf_getdyn.3elf
MLINKS+=	gelf.3elf gelf_getehdr.3elf
MLINKS+=	gelf.3elf gelf_getmove.3elf
MLINKS+=	gelf.3elf gelf_getphdr.3elf
MLINKS+=	gelf.3elf gelf_getrel.3elf
MLINKS+=	gelf.3elf gelf_getrela.3elf
MLINKS+=	gelf.3elf gelf_getshdr.3elf
MLINKS+=	gelf.3elf gelf_getsym.3elf
MLINKS+=	gelf.3elf gelf_getsyminfo.3elf
MLINKS+=	gelf.3elf gelf_getsymshndx.3elf
MLINKS+=	gelf.3elf gelf_newehdr.3elf
MLINKS+=	gelf.3elf gelf_newphdr.3elf
MLINKS+=	gelf.3elf gelf_update_cap.3elf
MLINKS+=	gelf.3elf gelf_update_dyn.3elf
MLINKS+=	gelf.3elf gelf_update_ehdr.3elf
MLINKS+=	gelf.3elf gelf_update_getmove.3elf
MLINKS+=	gelf.3elf gelf_update_move.3elf
MLINKS+=	gelf.3elf gelf_update_phdr.3elf
MLINKS+=	gelf.3elf gelf_update_rel.3elf
MLINKS+=	gelf.3elf gelf_update_rela.3elf
MLINKS+=	gelf.3elf gelf_update_shdr.3elf
MLINKS+=	gelf.3elf gelf_update_sym.3elf
MLINKS+=	gelf.3elf gelf_update_syminfo.3elf
MLINKS+=	gelf.3elf gelf_update_symshndx.3elf
MLINKS+=	gelf.3elf gelf_xlatetof.3elf
MLINKS+=	gelf.3elf gelf_xlatetom.3elf

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
