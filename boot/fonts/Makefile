#
# Copyright 2018 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

.include <unleashed.mk>

FONTSDIR=	${BOOTDIR}/fonts
CONSFONTDIR=	${SRCTOP}/contrib/consfonts

FONTS=		Gallant19.fnt \
		6x12.fnt \
		8x14.fnt \
		8x16.fnt \
		10x18.fnt \
		10x20.fnt \
		11x22.fnt \
		12x24.fnt \
		14x28.fnt \
		16x32.fnt

SYMLINKS=	../../../boot/fonts /usr/share/lib/fonts

VTFONTCVT=	vtfontcvt

all: ${FONTS}

proginstall:
	[ -d ${DESTDIR}${FONTSDIR} ] || \
	    ${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}${FONTSDIR}
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m 444 \
	    ${CONSFONTDIR}/fonts.dir ${DESTDIR}${FONTSDIR}
.for f in ${FONTS}
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m 444 \
	    ${f} ${DESTDIR}${FONTSDIR}
.endfor

.PATH: ${CONSFONTDIR}

Gallant19.fnt: Gallant19.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

6x12.fnt: ter-u12n.bdf ter-u12b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

8x14.fnt: ter-u14n.bdf ter-u14b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

8x16.fnt: ter-u16n.bdf ter-u16b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

10x18.fnt: ter-u18n.bdf ter-u18b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

10x20.fnt: ter-u20n.bdf ter-u20b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

11x22.fnt: ter-u22n.bdf ter-u22b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

12x24.fnt: ter-u24n.bdf ter-u24b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

14x28.fnt: ter-u28n.bdf ter-u28b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

16x32.fnt: ter-u32n.bdf ter-u32b.bdf
	${VTFONTCVT} -o ${.TARGET} ${.ALLSRC}

.include <prog.mk>
