Guide for illumos-gate Refugees
===============================

This guide is meant for those that are familiar with the illumos process and
repository.  If you are not familiar with illumos and illumos-gate, your
time will likely be better spent looking at the top-level README as this
guide only highlights the differences between the two communities.

Repository
----------

* Building with Sun Studio and lint is *not* supported
* `nightly`
  - `nightly` is not shipped, just use `tools/nightly.sh` directly
    - many of the nightly options were removed
  - support for multi-proto and multiple builds was removed
* `bldenv` is not shipped, just use `tools/bldenv.sh` directly

### illumos-gate to unleashed directory mapping

The source tree layout is different.  It attempts to be wider (rather than
deep) and better subdivided.  It is loosly based on the Linux kernel and
BSD repositories.  The description of the repository layout can be found in
`docs/repo-layout.md`.

The following are *rough* mappings between the two repositories to give you
a vague idea where files ended up.

* `usr/src/cmd` -> `bin`
* `usr/src/lib` -> `lib`
* `usr/src/head` -> `include`
* `usr/src/uts/common/os` -> `kernel/os`
* `usr/src/uts/common/vm` -> `kernel/vm`
* `usr/src/uts/common/sys` -> `include/sys`
* `usr/src/uts/intel` -> `arch/x86`

Contribution Process
--------------------

It's pretty simple:

1. make a patch
2. send the patch to the mailing list for review (there's no webrev, just
   send a diff) and apply any feedback you get
3. one of the committers will take your patch and commit it for you

Note, there is no special illumos like RTI step.

See Also
--------

* The Code of Conduct (docs/code-of-conduct.md)
* Community Organization (docs/organization.md)
* Repository Layout (docs/repo-layout.md)
