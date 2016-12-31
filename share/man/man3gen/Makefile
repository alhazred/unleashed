MAN = bgets.3gen \
      bufsplit.3gen \
      copylist.3gen \
      gmatch.3gen \
      isencrypt.3gen \
      mkdirp.3gen \
      p2open.3gen \
      pathfind.3gen \
      regexpr.3gen \
      strccpy.3gen \
      strfind.3gen

MLINKS+=	mkdirp.3gen rmdirp.3gen
MLINKS+=	p2open.3gen p2close.3gen
MLINKS+=	regexpr.3gen advance.3gen
MLINKS+=	regexpr.3gen compile.3gen
MLINKS+=	regexpr.3gen step.3gen
MLINKS+=	strccpy.3gen strcadd.3gen
MLINKS+=	strccpy.3gen streadd.3gen
MLINKS+=	strccpy.3gen strecpy.3gen
MLINKS+=	strfind.3gen str.3gen
MLINKS+=	strfind.3gen strrspn.3gen
MLINKS+=	strfind.3gen strtrns.3gen

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
