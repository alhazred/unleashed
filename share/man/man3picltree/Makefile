MAN = libpicltree.3picltree \
      picld_log.3picltree \
      picld_plugin_register.3picltree \
      ptree_add_node.3picltree \
      ptree_add_prop.3picltree \
      ptree_create_and_add_node.3picltree \
      ptree_create_and_add_prop.3picltree \
      ptree_create_node.3picltree \
      ptree_create_prop.3picltree \
      ptree_create_table.3picltree \
      ptree_find_node.3picltree \
      ptree_get_first_prop.3picltree \
      ptree_get_frutree_parent.3picltree \
      ptree_get_next_by_row.3picltree \
      ptree_get_node_by_path.3picltree \
      ptree_get_prop_by_name.3picltree \
      ptree_get_propinfo.3picltree \
      ptree_get_propinfo_by_name.3picltree \
      ptree_get_propval.3picltree \
      ptree_get_root.3picltree \
      ptree_init_propinfo.3picltree \
      ptree_post_event.3picltree \
      ptree_register_handler.3picltree \
      ptree_unregister_handler.3picltree \
      ptree_update_propval.3picltree \
      ptree_walk_tree_by_class.3picltree

MLINKS+=	ptree_add_node.3picltree ptree_delete_node.3picltree
MLINKS+=	ptree_add_prop.3picltree ptree_delete_prop.3picltree
MLINKS+=	ptree_create_node.3picltree ptree_destroy_node.3picltree
MLINKS+=	ptree_create_prop.3picltree ptree_destroy_prop.3picltree
MLINKS+=	ptree_create_table.3picltree ptree_add_row_to_table.3picltree
MLINKS+=	ptree_get_first_prop.3picltree ptree_get_next_prop.3picltree
MLINKS+=	ptree_get_next_by_row.3picltree ptree_get_next_by_col.3picltree
MLINKS+=	ptree_get_propval.3picltree ptree_get_propval_by_name.3picltree
MLINKS+=	ptree_update_propval.3picltree ptree_update_propval_by_name.3picltree

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
