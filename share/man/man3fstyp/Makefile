MAN = fstyp_get_attr.3fstyp \
      fstyp_ident.3fstyp \
      fstyp_init.3fstyp \
      fstyp_mod_init.3fstyp \
      fstyp_strerror.3fstyp

MLINKS+=	fstyp_init.3fstyp fstyp_fini.3fstyp
MLINKS+=	fstyp_mod_init.3fstyp fstyp_mod_dump.3fstyp
MLINKS+=	fstyp_mod_init.3fstyp fstyp_mod_fini.3fstyp
MLINKS+=	fstyp_mod_init.3fstyp fstyp_mod_get_attr.3fstyp
MLINKS+=	fstyp_mod_init.3fstyp fstyp_mod_ident.3fstyp

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
