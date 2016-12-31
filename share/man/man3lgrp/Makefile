MAN = lgrp_affinity_get.3lgrp \
      lgrp_children.3lgrp \
      lgrp_cookie_stale.3lgrp \
      lgrp_cpus.3lgrp \
      lgrp_fini.3lgrp \
      lgrp_home.3lgrp \
      lgrp_init.3lgrp \
      lgrp_latency.3lgrp \
      lgrp_mem_size.3lgrp \
      lgrp_nlgrps.3lgrp \
      lgrp_parents.3lgrp \
      lgrp_resources.3lgrp \
      lgrp_root.3lgrp \
      lgrp_version.3lgrp \
      lgrp_view.3lgrp

MLINKS+=	lgrp_affinity_get.3lgrp lgrp_affinity_set.3lgrp
MLINKS+=	lgrp_latency.3lgrp lgrp_latency_cookie.3lgrp

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
