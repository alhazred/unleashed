PROG=	mailwrapper
SRCS=	fgetln.c fparseln.c mailwrapper.c
MAN=	mailer.conf.4 mailwrapper.8

SYMLINKS=\
	${BINDIR}/mailwrapper /usr/bin/mailq \
	${BINDIR}/mailwrapper /usr/lib/sendmail \
	${BINDIR}/mailwrapper /usr/sbin/sendmail \
	${BINDIR}/mailwrapper /usr/sbin/newaliases

FILES=		mailer.conf
FILESDIR=	/etc
FILESMODE=	644

.include <prog.mk>
