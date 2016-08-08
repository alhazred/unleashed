#	$OpenBSD: Makefile,v 1.7 2011/11/08 10:37:10 guenther Exp $

PROG=		xinstall
PROG_NAME=	install
SRCS=		xinstall.c compat.c
MAN=		install.1
# XXX: omnios gnu-coreutils ships install(1) into /usr/bin, and for now we
# don't wish to conflict
BINDIR=		/usr/sbin

.include <bsd.prog.mk>
