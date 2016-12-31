MAN = acl_check.3sec \
      acl_free.3sec \
      acl_get.3sec \
      acl_strip.3sec \
      acl_totext.3sec \
      acl_trivial.3sec \
      aclcheck.3sec \
      aclsort.3sec \
      acltomode.3sec \
      acltotext.3sec

MLINKS+=	acl_get.3sec acl_set.3sec
MLINKS+=	acl_get.3sec facl_get.3sec
MLINKS+=	acl_get.3sec facl_set.3sec
MLINKS+=	acl_totext.3sec acl_fromtext.3sec
MLINKS+=	acltomode.3sec aclfrommode.3sec
MLINKS+=	acltotext.3sec aclfromtext.3sec

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
