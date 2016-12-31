MAN = Intro.3 \
      hosts_access.3

MLINKS+=	Intro.3 intro.3
MLINKS+=	hosts_access.3 hosts_ctl.3
MLINKS+=	hosts_access.3 libwrap.3
MLINKS+=	hosts_access.3 request_init.3
MLINKS+=	hosts_access.3 request_set.3

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
