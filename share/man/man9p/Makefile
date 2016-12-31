MAN = ddi-forceattach.9p \
      inquiry-device-type.9p \
      no-involuntary-power-cycles.9p \
      pm-components.9p \
      pm.9p \
      removable-media.9p \
      size.9p

MLINKS+=	ddi-forceattach.9p ddi-no-autodetach.9p
MLINKS+=	inquiry-device-type.9p inquiry-product-id.9p
MLINKS+=	inquiry-device-type.9p inquiry-revision-id.9p
MLINKS+=	inquiry-device-type.9p inquiry-serial-no.9p
MLINKS+=	inquiry-device-type.9p inquiry-vendor-id.9p
MLINKS+=	size.9p Nblock.9p
MLINKS+=	size.9p blksize.9p
MLINKS+=	size.9p device-blksize.9p
MLINKS+=	size.9p device-nblocks.9p

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
