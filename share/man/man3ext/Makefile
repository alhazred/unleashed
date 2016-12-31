MAN = NOTE.3ext \
      SUNW_C_GetMechSession.3ext \
      auto_ef.3ext \
      crypt.3ext \
      ecb_crypt.3ext \
      efi_alloc_and_init.3ext \
      ld_support.3ext \
      md4.3ext \
      md5.3ext \
      read_vtoc.3ext \
      rtld_audit.3ext \
      rtld_db.3ext \
      sendfile.3ext \
      sendfilev.3ext \
      sha1.3ext \
      sha2.3ext \
      stdarg.3ext \
      tsalarm_get.3ext \
      varargs.3ext

MLINKS+=	NOTE.3ext _NOTE.3ext
MLINKS+=	SUNW_C_GetMechSession.3ext SUNW_C_KeyToObject.3ext
MLINKS+=	auto_ef.3ext auto_ef_file.3ext
MLINKS+=	auto_ef.3ext auto_ef_free.3ext
MLINKS+=	auto_ef.3ext auto_ef_get_encoding.3ext
MLINKS+=	auto_ef.3ext auto_ef_get_score.3ext
MLINKS+=	auto_ef.3ext auto_ef_str.3ext
MLINKS+=	crypt.3ext crypt_close.3ext
MLINKS+=	crypt.3ext des_crypt.3ext
MLINKS+=	crypt.3ext des_encrypt.3ext
MLINKS+=	crypt.3ext des_setkey.3ext
MLINKS+=	crypt.3ext encrypt.3ext
MLINKS+=	crypt.3ext run_crypt.3ext
MLINKS+=	crypt.3ext run_setkey.3ext
MLINKS+=	crypt.3ext setkey.3ext
MLINKS+=	ecb_crypt.3ext DES_FAILED.3ext
MLINKS+=	ecb_crypt.3ext cbc_crypt.3ext
MLINKS+=	ecb_crypt.3ext des_setparity.3ext
MLINKS+=	efi_alloc_and_init.3ext efi_alloc_and_read.3ext
MLINKS+=	efi_alloc_and_init.3ext efi_free.3ext
MLINKS+=	efi_alloc_and_init.3ext efi_use_whole_disk.3ext
MLINKS+=	efi_alloc_and_init.3ext efi_write.3ext
MLINKS+=	ld_support.3ext ld_atexit.3ext
MLINKS+=	ld_support.3ext ld_atexit64.3ext
MLINKS+=	ld_support.3ext ld_file.3ext
MLINKS+=	ld_support.3ext ld_file64.3ext
MLINKS+=	ld_support.3ext ld_input_done.3ext
MLINKS+=	ld_support.3ext ld_input_section.3ext
MLINKS+=	ld_support.3ext ld_input_section64.3ext
MLINKS+=	ld_support.3ext ld_open.3ext
MLINKS+=	ld_support.3ext ld_open64.3ext
MLINKS+=	ld_support.3ext ld_section.3ext
MLINKS+=	ld_support.3ext ld_section64.3ext
MLINKS+=	ld_support.3ext ld_start.3ext
MLINKS+=	ld_support.3ext ld_start64.3ext
MLINKS+=	ld_support.3ext ld_version.3ext
MLINKS+=	md4.3ext MD4Final.3ext
MLINKS+=	md4.3ext MD4Init.3ext
MLINKS+=	md4.3ext MD4Update.3ext
MLINKS+=	md5.3ext MD5Final.3ext
MLINKS+=	md5.3ext MD5Init.3ext
MLINKS+=	md5.3ext MD5Update.3ext
MLINKS+=	md5.3ext md5_calc.3ext
MLINKS+=	read_vtoc.3ext write_vtoc.3ext
MLINKS+=	rtld_audit.3ext la_activity.3ext
MLINKS+=	rtld_audit.3ext la_amd64_pltenter.3ext
MLINKS+=	rtld_audit.3ext la_i86_pltenter.3ext
MLINKS+=	rtld_audit.3ext la_objfilter.3ext
MLINKS+=	rtld_audit.3ext la_objopen.3ext
MLINKS+=	rtld_audit.3ext la_objsearch.3ext
MLINKS+=	rtld_audit.3ext la_pltexit.3ext
MLINKS+=	rtld_audit.3ext la_pltexit64.3ext
MLINKS+=	rtld_audit.3ext la_preinit.3ext
MLINKS+=	rtld_audit.3ext la_sparcv8_pltenter.3ext
MLINKS+=	rtld_audit.3ext la_sparcv9_pltenter.3ext
MLINKS+=	rtld_audit.3ext la_symbind32.3ext
MLINKS+=	rtld_audit.3ext la_symbind64.3ext
MLINKS+=	rtld_audit.3ext la_version.3ext
MLINKS+=	rtld_db.3ext rd_delete.3ext
MLINKS+=	rtld_db.3ext rd_errstr.3ext
MLINKS+=	rtld_db.3ext rd_event_addr.3ext
MLINKS+=	rtld_db.3ext rd_event_enable.3ext
MLINKS+=	rtld_db.3ext rd_event_getmsg.3ext
MLINKS+=	rtld_db.3ext rd_init.3ext
MLINKS+=	rtld_db.3ext rd_loadobj_iter.3ext
MLINKS+=	rtld_db.3ext rd_log.3ext
MLINKS+=	rtld_db.3ext rd_new.3ext
MLINKS+=	rtld_db.3ext rd_objpad_enable.3ext
MLINKS+=	rtld_db.3ext rd_plt_resolution.3ext
MLINKS+=	rtld_db.3ext rd_reset.3ext
MLINKS+=	sha1.3ext SHA1Final.3ext
MLINKS+=	sha1.3ext SHA1Init.3ext
MLINKS+=	sha1.3ext SHA1Update.3ext
MLINKS+=	sha2.3ext SHA256Final.3ext
MLINKS+=	sha2.3ext SHA256Init.3ext
MLINKS+=	sha2.3ext SHA256Update.3ext
MLINKS+=	sha2.3ext SHA2Final.3ext
MLINKS+=	sha2.3ext SHA2Init.3ext
MLINKS+=	sha2.3ext SHA2Update.3ext
MLINKS+=	sha2.3ext SHA384Final.3ext
MLINKS+=	sha2.3ext SHA384Init.3ext
MLINKS+=	sha2.3ext SHA384Update.3ext
MLINKS+=	sha2.3ext SHA512Final.3ext
MLINKS+=	sha2.3ext SHA512Init.3ext
MLINKS+=	sha2.3ext SHA512Update.3ext
MLINKS+=	tsalarm_get.3ext tsalarm_set.3ext

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
