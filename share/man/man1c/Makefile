MAN = ct.1c \
      cu.1c \
      rup.1c \
      uucp.1c \
      uuencode.1c \
      uuglist.1c \
      uustat.1c \
      uuto.1c \
      uux.1c

MLINKS+=	uucp.1c uulog.1c
MLINKS+=	uucp.1c uuname.1c
MLINKS+=	uuencode.1c uudecode.1c
MLINKS+=	uuto.1c uupick.1c

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
