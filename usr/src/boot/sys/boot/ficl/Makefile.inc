#

CC=		$(GCC_ROOT)/bin/gcc
FICLDIR=	$(SRC)/common/ficl

all: lib

CPPFLAGS= -DSTAND -I. -I.. -I../../../../include -I../../../../lib/libstand
CPPFLAGS += -I../../..  -I$(FICLDIR) -I../../common
CFLAGS= -O2 -Wall -nostdinc

CFLAGS +=	-ffreestanding
CFLAGS +=	-mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-sse3 -msoft-float
CFLAGS +=	-std=gnu99

OBJECTS= dictionary.o system.o fileaccess.o float.o double.o prefix.o search.o
OBJECTS += softcore.o stack.o tools.o vm.o primitives.o unix.o utility.o
OBJECTS += hash.o callback.o word.o loader.o
HEADERS= $(FICLDIR)/ficl.h $(FICLDIR)/ficlplatform/unix.h ../ficllocal.h
#

MAJOR = 4
MINOR = 1.0

lib: machine x86 libficl.a

# static library build
libficl.a: $(OBJECTS)
	$(AR) $(ARFLAGS) libficl.a $(OBJECTS)

machine:
	ln -sf ../../../$(MACHINE)/include machine

x86:
	ln -sf ../../../x86/include x86

%.o:	../softcore/%.c $(HEADERS)
	$(COMPILE.c) $<

%.o:	$(FICLDIR)/%.c $(HEADERS)
	$(COMPILE.c) $<

%.o:	$(FICLDIR)/ficlplatform/%.c $(HEADERS)
	$(COMPILE.c) $<

#
#       generic cleanup code
#
clobber clean:	FRC
	$(RM) *.o *.a libficl.* ficl machine x86
