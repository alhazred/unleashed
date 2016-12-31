MAN = rsm_create_localmemory_handle.3rsm \
      rsm_get_controller.3rsm \
      rsm_get_interconnect_topology.3rsm \
      rsm_get_segmentid_range.3rsm \
      rsm_intr_signal_post.3rsm \
      rsm_intr_signal_wait_pollfd.3rsm \
      rsm_memseg_export_create.3rsm \
      rsm_memseg_export_publish.3rsm \
      rsm_memseg_get_pollfd.3rsm \
      rsm_memseg_import_connect.3rsm \
      rsm_memseg_import_get.3rsm \
      rsm_memseg_import_init_barrier.3rsm \
      rsm_memseg_import_map.3rsm \
      rsm_memseg_import_open_barrier.3rsm \
      rsm_memseg_import_put.3rsm \
      rsm_memseg_import_putv.3rsm \
      rsm_memseg_import_set_mode.3rsm

MLINKS+=	rsm_create_localmemory_handle.3rsm rsm_free_localmemory_handle.3rsm
MLINKS+=	rsm_get_controller.3rsm rsm_get_controller_attr.3rsm
MLINKS+=	rsm_get_controller.3rsm rsm_release_controller.3rsm
MLINKS+=	rsm_get_interconnect_topology.3rsm rsm_free_interconnect_topology.3rsm
MLINKS+=	rsm_intr_signal_post.3rsm rsm_intr_signal_wait.3rsm
MLINKS+=	rsm_memseg_export_create.3rsm rsm_memseg_export_destroy.3rsm
MLINKS+=	rsm_memseg_export_create.3rsm rsm_memseg_export_rebind.3rsm
MLINKS+=	rsm_memseg_export_publish.3rsm rsm_memseg_export_republish.3rsm
MLINKS+=	rsm_memseg_export_publish.3rsm rsm_memseg_export_unpublish.3rsm
MLINKS+=	rsm_memseg_get_pollfd.3rsm rsm_memseg_release_pollfd.3rsm
MLINKS+=	rsm_memseg_import_connect.3rsm rsm_memseg_import_disconnect.3rsm
MLINKS+=	rsm_memseg_import_get.3rsm rsm_memseg_import_get16.3rsm
MLINKS+=	rsm_memseg_import_get.3rsm rsm_memseg_import_get32.3rsm
MLINKS+=	rsm_memseg_import_get.3rsm rsm_memseg_import_get64.3rsm
MLINKS+=	rsm_memseg_import_get.3rsm rsm_memseg_import_get8.3rsm
MLINKS+=	rsm_memseg_import_init_barrier.3rsm rsm_memseg_import_destroy_barrier.3rsm
MLINKS+=	rsm_memseg_import_map.3rsm rsm_memseg_import_unmap.3rsm
MLINKS+=	rsm_memseg_import_open_barrier.3rsm rsm_memseg_import_close_barrier.3rsm
MLINKS+=	rsm_memseg_import_open_barrier.3rsm rsm_memseg_import_order_barrier.3rsm
MLINKS+=	rsm_memseg_import_put.3rsm rsm_memseg_import_put16.3rsm
MLINKS+=	rsm_memseg_import_put.3rsm rsm_memseg_import_put32.3rsm
MLINKS+=	rsm_memseg_import_put.3rsm rsm_memseg_import_put64.3rsm
MLINKS+=	rsm_memseg_import_put.3rsm rsm_memseg_import_put8.3rsm
MLINKS+=	rsm_memseg_import_putv.3rsm rsm_memseg_import_getv.3rsm
MLINKS+=	rsm_memseg_import_set_mode.3rsm rsm_memseg_import_get_mode.3rsm

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
