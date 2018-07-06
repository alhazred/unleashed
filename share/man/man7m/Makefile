.include <../../../cfgparam.mk>

MAN_sparc = kb.7m

MAN = bufmod.7m \
      connld.7m \
      ldterm.7m \
      pckt.7m \
      pfmod.7m \
      pipemod.7m \
      ptem.7m \
      sppptun.7m \
      timod.7m \
      tirdwr.7m \
      ttcompat.7m \
      usb_ah.7m \
      usbkbm.7m \
      usbms.7m \
      vuidmice.7m \
      $(MAN_$(CONFIG_MACH))


# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
