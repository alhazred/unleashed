MAN = cpc.3cpc \
      cpc_access.3cpc \
      cpc_bind_curlwp.3cpc \
      cpc_bind_event.3cpc \
      cpc_buf_create.3cpc \
      cpc_count_usr_events.3cpc \
      cpc_enable.3cpc \
      cpc_event.3cpc \
      cpc_event_diff.3cpc \
      cpc_getcpuver.3cpc \
      cpc_npic.3cpc \
      cpc_open.3cpc \
      cpc_pctx_bind_event.3cpc \
      cpc_set_create.3cpc \
      cpc_seterrfn.3cpc \
      cpc_seterrhndlr.3cpc \
      cpc_shared_open.3cpc \
      cpc_strtoevent.3cpc \
      cpc_version.3cpc \
      generic_events.3cpc \
      pctx_capture.3cpc \
      pctx_set_events.3cpc

MLINKS+=	cpc_bind_curlwp.3cpc cpc_bind_cpu.3cpc
MLINKS+=	cpc_bind_curlwp.3cpc cpc_bind_pctx.3cpc
MLINKS+=	cpc_bind_curlwp.3cpc cpc_request_preset.3cpc
MLINKS+=	cpc_bind_curlwp.3cpc cpc_set_restart.3cpc
MLINKS+=	cpc_bind_curlwp.3cpc cpc_unbind.3cpc
MLINKS+=	cpc_bind_event.3cpc cpc_rele.3cpc
MLINKS+=	cpc_bind_event.3cpc cpc_take_sample.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_add.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_copy.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_destroy.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_get.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_hrtime.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_set.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_sub.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_tick.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_buf_zero.3cpc
MLINKS+=	cpc_buf_create.3cpc cpc_set_sample.3cpc
MLINKS+=	cpc_count_usr_events.3cpc cpc_count_sys_events.3cpc
MLINKS+=	cpc_enable.3cpc cpc_disable.3cpc
MLINKS+=	cpc_event_diff.3cpc cpc_event_accum.3cpc
MLINKS+=	cpc_getcpuver.3cpc cpc_getcciname.3cpc
MLINKS+=	cpc_getcpuver.3cpc cpc_getcpuref.3cpc
MLINKS+=	cpc_getcpuver.3cpc cpc_getnpic.3cpc
MLINKS+=	cpc_getcpuver.3cpc cpc_getusage.3cpc
MLINKS+=	cpc_getcpuver.3cpc cpc_walk_names.3cpc
MLINKS+=	cpc_npic.3cpc cpc_caps.3cpc
MLINKS+=	cpc_npic.3cpc cpc_cciname.3cpc
MLINKS+=	cpc_npic.3cpc cpc_cpuref.3cpc
MLINKS+=	cpc_npic.3cpc cpc_walk_attrs.3cpc
MLINKS+=	cpc_npic.3cpc cpc_walk_events_all.3cpc
MLINKS+=	cpc_npic.3cpc cpc_walk_events_pic.3cpc
MLINKS+=	cpc_npic.3cpc cpc_walk_generic_events_all.3cpc
MLINKS+=	cpc_npic.3cpc cpc_walk_generic_events_pic.3cpc
MLINKS+=	cpc_open.3cpc cpc_close.3cpc
MLINKS+=	cpc_pctx_bind_event.3cpc cpc_pctx_invalidate.3cpc
MLINKS+=	cpc_pctx_bind_event.3cpc cpc_pctx_rele.3cpc
MLINKS+=	cpc_pctx_bind_event.3cpc cpc_pctx_take_sample.3cpc
MLINKS+=	cpc_set_create.3cpc cpc_set_add_request.3cpc
MLINKS+=	cpc_set_create.3cpc cpc_set_destroy.3cpc
MLINKS+=	cpc_set_create.3cpc cpc_walk_requests.3cpc
MLINKS+=	cpc_shared_open.3cpc cpc_shared_bind_event.3cpc
MLINKS+=	cpc_shared_open.3cpc cpc_shared_close.3cpc
MLINKS+=	cpc_shared_open.3cpc cpc_shared_rele.3cpc
MLINKS+=	cpc_shared_open.3cpc cpc_shared_take_sample.3cpc
MLINKS+=	cpc_strtoevent.3cpc cpc_eventtostr.3cpc
MLINKS+=	pctx_capture.3cpc pctx_create.3cpc
MLINKS+=	pctx_capture.3cpc pctx_release.3cpc
MLINKS+=	pctx_capture.3cpc pctx_run.3cpc

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
