#	$OpenBSD: Makefile,v 1.12 2016/08/23 06:00:28 guenther Exp $

PROG=   pax
SRCS=	ar_io.c ar_subs.c buf_subs.c cache.c cpio.c file_subs.c ftree.c\
	gen_subs.c getoldopt.c options.c pat_rep.c pax.c sel_subs.c tables.c\
	tar.c tty_subs.c
# compat
SRCS+=	pwcache.c strtonum.c strmode.c dprintf.c

# XXX for the time being, we need this to get the standard versions of
# reentrant functions
CPPFLAGS+=	-D_POSIX_PTHREAD_SEMANTICS
# XXX for the time being we also need to specify we want to work with 64-bit
# files.
CPPFLAGS+=	-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

# For now, our tar(1) and cpio(1) don't come from here.
#MAN=	pax.1 tar.1 cpio.1
#LINKS=	${BINDIR}/pax ${BINDIR}/tar ${BINDIR}/pax ${BINDIR}/cpio

.include <prog.mk>
