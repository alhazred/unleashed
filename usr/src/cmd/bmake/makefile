# $Id: makefile.in,v 1.1 2012/12/28 21:28:19 sjg Exp $

# a simple makefile for those who don't like anything beyond:
# ./configure; make; make install
#

all: build

clobber: clean

build clean install test:
	./boot-strap --prefix=/usr --install-destdir=$(ROOT) -o obj op=$@
