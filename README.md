How to build unleashed
======================

Install unleashed
-----------------

Building unleashed requires unleashed, so first, download and install the
latest snapshot from https://www.unleashed-os.org/snapshots/latest/

Get the source
--------------

Clone this repository:

```
$ git clone git://repo.or.cz/unleashed.git
$ cd unleashed
```

Build
-----

For a complete build, use 'tools/nightly.sh'. The user executing the build
needs to be able to write to `/usr/obj`, so make sure you can do that first.

```
# zfs create -o mountpoint=/usr/obj rpool/obj && install -m 1777 -d /usr/obj
$ ./tools/nightly.sh
```

On success, this results in installable packages in the `packages` directory.

Incremental build
-----------------

To build a component that is using the new build system (e.g., cat(1)), change
into the source directory and run make. For example:

```
$ cd bin/cat
$ make
# make install
```

The component will be built against the running system and installed to /. To
install to the proto area, pass DESTDIR=path to 'make install'. Building
against libraries and headers in the "proto area" or in object directories is
not currently supported.

To build a component under the legacy (dmake) build system, ie. things under
'usr/src', first complete a full nightly build so that prerequisite objects for
the component are made, and then:

```
$ ./tools/bldenv.sh tools/env.sh
$ cd usr/src/cmd/w
$ dmake install
```

The component will be built against the "proto area" and installed there.
