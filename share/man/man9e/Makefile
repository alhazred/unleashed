MAN = Intro.9e \
      _fini.9e \
      aread.9e \
      attach.9e \
      awrite.9e \
      chpoll.9e \
      close.9e \
      csx_event_handler.9e \
      detach.9e \
      devmap.9e \
      devmap_access.9e \
      devmap_contextmgt.9e \
      devmap_dup.9e \
      devmap_map.9e \
      devmap_unmap.9e \
      dump.9e \
      getinfo.9e \
      gld.9e \
      identify.9e \
      ioctl.9e \
      ks_snapshot.9e \
      ks_update.9e \
      mac.9e \
      mc_getcapab.9e \
      mc_getprop.9e \
      mc_getstat.9e \
      mc_ioctl.9e \
      mc_multicst.9e \
      mc_open.9e \
      mc_propinfo.9e \
      mc_setpromisc.9e \
      mc_setprop.9e \
      mc_start.9e \
      mc_tx.9e \
      mc_unicst.9e \
      mmap.9e \
      open.9e \
      power.9e \
      print.9e \
      probe.9e \
      prop_op.9e \
      put.9e \
      quiesce.9e \
      read.9e \
      segmap.9e \
      srv.9e \
      strategy.9e \
      tran_abort.9e \
      tran_bus_reset.9e \
      tran_dmafree.9e \
      tran_getcap.9e \
      tran_init_pkt.9e \
      tran_quiesce.9e \
      tran_reset.9e \
      tran_reset_notify.9e \
      tran_setup_pkt.9e \
      tran_start.9e \
      tran_sync_pkt.9e \
      tran_tgt_free.9e \
      tran_tgt_init.9e \
      tran_tgt_probe.9e \
      write.9e

MLINKS+=	Intro.9e intro.9e
MLINKS+=	_fini.9e _info.9e
MLINKS+=	_fini.9e _init.9e
MLINKS+=	mac.9e MAC.9e
MLINKS+=	mac.9e gldv3.9e
MLINKS+=	mac.9e GLDv3.9e
MLINKS+=	gld.9e gldm_get_stats.9e
MLINKS+=	gld.9e gldm_intr.9e
MLINKS+=	gld.9e gldm_ioctl.9e
MLINKS+=	gld.9e gldm_reset.9e
MLINKS+=	gld.9e gldm_send.9e
MLINKS+=	gld.9e gldm_set_mac_addr.9e
MLINKS+=	gld.9e gldm_set_multicast.9e
MLINKS+=	gld.9e gldm_set_promiscuous.9e
MLINKS+=	gld.9e gldm_start.9e
MLINKS+=	gld.9e gldm_stop.9e
MLINKS+=	mc_open.9e mc_close.9e
MLINKS+=	mc_start.9e mc_stop.9e
MLINKS+=	tran_getcap.9e tran_setcap.9e
MLINKS+=	tran_init_pkt.9e tran_destroy_pkt.9e
MLINKS+=	tran_quiesce.9e tran_unquiesce.9e
MLINKS+=	tran_setup_pkt.9e tran_pkt_constructor.9e
MLINKS+=	tran_setup_pkt.9e tran_pkt_destructor.9e
MLINKS+=	tran_setup_pkt.9e tran_teardown_pkt.9e

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
