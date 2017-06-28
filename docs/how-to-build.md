How to Build Unleashed
======================

Building unleashed is not as simple as we would like it to be.  Currently,
it is in a transitional period where parts of the codebase still rely on the
legacy build system while other parts of the codebase use the new
bmake-based build.

Install Prerequisites
---------------------

NOTE: You must be running a reasonably new version of unleashed in order to
build unleashed.

You will need these packages:

- pkg:/developer/astdev
- pkg:/developer/build/onbld
- pkg:/developer/debug/ctf
- pkg:/developer/illumos-gcc
- pkg:/developer/lexer/flex
- pkg:/developer/parser/bison
- pkg:/developer/versioning/git
- pkg:/library/nspr/header-nspr
- pkg:/system/library/mozilla-nss/header-nss
- pkg:/system/mozilla-nss

Get the Source
--------------

The code is maintained in a git repository.  You can clone it from either of
these URLs (depending on if you want to use the git protocol or http):

* git://repo.or.cz/illumos-gate/unleashed.git
* http://repo.or.cz/illumos-gate/unleashed.git

For example:

```
$ git clone git://repo.or.cz/illumos-gate/unleashed.git
```

Build
-----

To build everything, you want to use the nightly shell script.  For now it
still requires an "env" file which defines the environment.  (A sample env
file can be found in the tools directory.)  To start the build, simply run:

```
$ ./tools/nightly.sh <env file>
```

This results in installable packages in the packages directory.

Incremental Build
-----------------

Rebuilding a component is the easiest after a full nightly build.  (TODO:
document a no-nightly component building)

Building a specific component can be either easy or somewhat arcane -
depending on whether the component is under the legacy build system or the
new one.

To build a component that is using the new build system (e.g., cat(1)),
simply change into the source directory and run bmake.  For example:

```
$ cd bin/cat
$ bmake DESTDIR=.../proto/root_i386
$ bmake install DESTDIR=.../proto/root_i386
```

Note: Omitting DESTDIR will result in the component being built against the
running system and installed to /.

To build a component under the legacy build system, more steps are needed:

```
$ ./tools/bldenv.sh <env file>
$ cd usr/src/cmd/w
$ dmake install
```

Install Packages
----------------

TODO
