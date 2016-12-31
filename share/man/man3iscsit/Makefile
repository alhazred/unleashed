MAN = it_config_load.3iscsit \
      it_ini_create.3iscsit \
      it_portal_create.3iscsit \
      it_tgt_create.3iscsit \
      it_tpg_create.3iscsit

MLINKS+=	it_config_load.3iscsit it_config_commit.3iscsit
MLINKS+=	it_config_load.3iscsit it_config_free.3iscsit
MLINKS+=	it_config_load.3iscsit it_config_setprop.3iscsit
MLINKS+=	it_ini_create.3iscsit it_ini_delete.3iscsit
MLINKS+=	it_ini_create.3iscsit it_ini_free.3iscsit
MLINKS+=	it_ini_create.3iscsit it_ini_setprop.3iscsit
MLINKS+=	it_portal_create.3iscsit it_portal_delete.3iscsit
MLINKS+=	it_tgt_create.3iscsit it_tgt_delete.3iscsit
MLINKS+=	it_tgt_create.3iscsit it_tgt_free.3iscsit
MLINKS+=	it_tgt_create.3iscsit it_tgt_setprop.3iscsit
MLINKS+=	it_tgt_create.3iscsit it_tpgt_create.3iscsit
MLINKS+=	it_tgt_create.3iscsit it_tpgt_delete.3iscsit
MLINKS+=	it_tgt_create.3iscsit it_tpgt_free.3iscsit
MLINKS+=	it_tpg_create.3iscsit it_tpg_delete.3iscsit
MLINKS+=	it_tpg_create.3iscsit it_tpg_free.3iscsit

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
