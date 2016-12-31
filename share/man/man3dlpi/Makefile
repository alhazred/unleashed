MAN = dlpi_arptype.3dlpi \
      dlpi_bind.3dlpi \
      dlpi_close.3dlpi \
      dlpi_disabnotify.3dlpi \
      dlpi_enabmulti.3dlpi \
      dlpi_enabnotify.3dlpi \
      dlpi_fd.3dlpi \
      dlpi_get_physaddr.3dlpi \
      dlpi_iftype.3dlpi \
      dlpi_info.3dlpi \
      dlpi_linkname.3dlpi \
      dlpi_mactype.3dlpi \
      dlpi_open.3dlpi \
      dlpi_promiscon.3dlpi \
      dlpi_recv.3dlpi \
      dlpi_send.3dlpi \
      dlpi_set_physaddr.3dlpi \
      dlpi_set_timeout.3dlpi \
      dlpi_strerror.3dlpi \
      dlpi_unbind.3dlpi \
      dlpi_walk.3dlpi

MLINKS+=	dlpi_enabmulti.3dlpi dlpi_disabmulti.3dlpi
MLINKS+=	dlpi_promiscon.3dlpi dlpi_promiscoff.3dlpi

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
