Guide for illumos-gate Refugees
===============================

This guide is meant for those that are familiar with the illumos process and
repository.  If you are not familiar with illumos and illumos-gate, your
time will likely be better spent looking at the top-level README as this
guide only highlights the differences between the two communities.

Repository
----------

* The source tree layout is different.  It attempts to be wider (rather than
  deep) and better subdivided.  It is loosly based on the Linux kernel and
  BSD repositories.  The description of the repository layout can be found
  in `docs/repo-layout.md`.
* Sun Studio and lint are *not* supported
* `nightly`
  - `nightly` is not shipped, just use `usr/src/tools/scripts/nightly.sh`
    directly
    - many of the nightly options were removed
  - support for multi-proto and multiple builds was removed
* `bldenv` is not shipped, just use `usr/src/tools/scripts/bldenv.sh`
  directly

Contribution Process
--------------------

TODO

See Also
--------

* The Code of Conduct (docs/code-of-conduct.md)
* Community Organization (docs/organization.md)
* Repository Layout (docs/repo-layout.md)
