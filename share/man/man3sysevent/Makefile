MAN = sysevent_bind_handle.3sysevent \
      sysevent_free.3sysevent \
      sysevent_get_attr_list.3sysevent \
      sysevent_get_class_name.3sysevent \
      sysevent_get_vendor_name.3sysevent \
      sysevent_post_event.3sysevent \
      sysevent_subscribe_event.3sysevent

MLINKS+=	sysevent_bind_handle.3sysevent sysevent_unbind_handle.3sysevent
MLINKS+=	sysevent_get_class_name.3sysevent sysevent_get_seq.3sysevent
MLINKS+=	sysevent_get_class_name.3sysevent sysevent_get_size.3sysevent
MLINKS+=	sysevent_get_class_name.3sysevent sysevent_get_subclass_name.3sysevent
MLINKS+=	sysevent_get_class_name.3sysevent sysevent_get_time.3sysevent
MLINKS+=	sysevent_get_vendor_name.3sysevent sysevent_get_pid.3sysevent
MLINKS+=	sysevent_get_vendor_name.3sysevent sysevent_get_pub_name.3sysevent
MLINKS+=	sysevent_subscribe_event.3sysevent sysevent_unsubscribe_event.3sysevent

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
