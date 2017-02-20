#
# Copyright (c) 2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

CONTRDIR=${.CURDIR}/../../contrib/libjeffpc
.PATH: ${CONTRDIR}

CONFIG_SRCS=main.c

LIBJEFFPC_SRCS=error.c \
	       hexdump.c \
	       init.c \
	       io.c \
	       padding.c \
	       rand.c \
	       sexpr.c \
	       sexpr.lex.c \
	       sexpr.tab.c \
	       sexpr_eval.c \
	       sock.c \
	       str.c \
	       synch.c \
	       uuid.c \
	       val.c

PROG=mkconfig
SRCS=$(CONFIG_SRCS) $(LIBJEFFPC_SRCS)

CLEANFILES+=sexpr.lex.c \
	    sexpr.tab.c \
	    sexpr.tab.h

LEX=flex
YACC=bison
CFLAGS=-D_GNU_SOURCE -D_POSIX_C_SOURCE=200112L -D__EXTENSIONS__ \
       -Wall -O2 -g -std=gnu99 \
       -march=pentiumpro \
       -fno-omit-frame-pointer \
       -I${CONTRDIR}/include \
       -I${CONTRDIR}
LFLAGS=-P sexpr_reader_
YFLAGS=-p sexpr_reader_ -d
LDFLAGS=-lumem -lavl -luuid -lsocket /usr/lib/libcmdutils.so.1

.include <bsd.prog.mk>

sexpr.lex.c: sexpr.l
	$(LEX) -P sexpr_reader_ -o ${.TARGET} ${.ALLSRC}

sexpr.tab.c sexpr.tab.h: sexpr.y
	$(YACC) -p sexpr_reader_ -d -o sexpr.tab.c ${.ALLSRC}

sexpr.lex.o: sexpr.tab.h
