# $Id: makefile.in,v 1.1 2012/12/28 21:28:19 sjg Exp $

# a simple makefile for those who don't like anything beyond:
# ./configure; make; make install
#

include ../Makefile.cmd

all: build

clobber: clean
clean:
	$(RM) -r obj

CC= $(GCC_ROOT)/bin/gcc
# bmake uses ${CC} as its linker, so prefix all options with -Wl,
LDFLAGS= $(LDFLAGS.cmd:%=-Wl,%)

build install test:
	LDFLAGS="$(LDFLAGS)" CC="$(CC)" ./boot-strap --prefix=/usr --install-destdir=$(ROOT) -o obj op=$@
