MAN = Intro.9s \
      aio_req.9s \
      buf.9s \
      cb_ops.9s \
      copyreq.9s \
      copyresp.9s \
      datab.9s \
      ddi_device_acc_attr.9s \
      ddi_dma_attr.9s \
      ddi_dma_cookie.9s \
      ddi_dmae_req.9s \
      ddi_fm_error.9s \
      ddi_idevice_cookie.9s \
      dev_ops.9s \
      devmap_callback_ctl.9s \
      fmodsw.9s \
      free_rtn.9s \
      gld_mac_info.9s \
      gld_stats.9s \
      hook_nic_event.9s \
      hook_pkt_event.9s \
      hook_t.9s \
      iocblk.9s \
      iovec.9s \
      kstat.9s \
      kstat_intr.9s \
      kstat_io.9s \
      kstat_named.9s \
      linkblk.9s \
      mac_callbacks.9s \
      mac_register.9s \
      modldrv.9s \
      modlinkage.9s \
      modlmisc.9s \
      modlstrmod.9s \
      module_info.9s \
      msgb.9s \
      net_inject_t.9s \
      net_instance_t.9s \
      qband.9s \
      qinit.9s \
      queclass.9s \
      queue.9s \
      scsi_address.9s \
      scsi_arq_status.9s \
      scsi_asc_key_strings.9s \
      scsi_device.9s \
      scsi_extended_sense.9s \
      scsi_hba_tran.9s \
      scsi_inquiry.9s \
      scsi_pkt.9s \
      scsi_status.9s \
      streamtab.9s \
      stroptions.9s \
      tuple.9s \
      uio.9s \
      usb_bulk_request.9s \
      usb_callback_flags.9s \
      usb_cfg_descr.9s \
      usb_client_dev_data.9s \
      usb_completion_reason.9s \
      usb_ctrl_request.9s \
      usb_dev_descr.9s \
      usb_dev_qlf_descr.9s \
      usb_ep_descr.9s \
      usb_if_descr.9s \
      usb_intr_request.9s \
      usb_isoc_request.9s \
      usb_other_speed_cfg_descr.9s \
      usb_request_attributes.9s \
      usb_string_descr.9s

MLINKS+=	Intro.9s intro.9s
MLINKS+=	datab.9s dblk.9s
MLINKS+=	mac_callbacks.9s mac_callbacks_t.9s
MLINKS+=	mac_register.9s mac_register_t.9s
MLINKS+=	msgb.9s mblk.9s

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
