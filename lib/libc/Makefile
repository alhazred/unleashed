MAN=   accept.2 bind.2 connect.2 getpeername.2 getsockname.2 getsockopt.2 \
       listen.2 recv.2 send.2 shutdown.2 socket.2 socketpair.2 CMSG_DATA.3 \
       alloca.3 err.3 fts.3 malloc.3 readdir.3 sockatmark.3 posix_memalign.3 \
       valloc.3 vis.3

MLINKS=		accept.2 accept4.2
MLINKS+=	getsockopt.2 setsockopt.2
MLINKS+=	recv.2 recvfrom.2
MLINKS+=	recv.2 recvmsg.2
MLINKS+=	send.2 sendto.2
MLINKS+=	send.2 sendmsg.2
MLINKS+=	CMSG_DATA.3 CMSG_FIRSTHDR.3
MLINKS+=	CMSG_DATA.3 CMSG_LEN.3
MLINKS+=	CMSG_DATA.3 CMSG_NEXTHDR.3
MLINKS+=	CMSG_DATA.3 CMSG_SPACE.3
MLINKS+=	readdir.3 readdir_r.3
MLINKS+=	err.3 verr.3
MLINKS+=	err.3 errc.3
MLINKS+=	err.3 verrc.3
MLINKS+=	err.3 errx.3
MLINKS+=	err.3 verrx.3
MLINKS+=	err.3 warn.3
MLINKS+=	err.3 vwarn.3
MLINKS+=	err.3 warnc.3
MLINKS+=	err.3 vwarnc.3
MLINKS+=	err.3 warnx.3
MLINKS+=	err.3 vwarnx.3
MLINKS+=	fts.3 fts_children.3
MLINKS+=	fts.3 fts_close.3
MLINKS+=	fts.3 fts_open.3
MLINKS+=	fts.3 fts_read.3
MLINKS+=	fts.3 fts_set.3
MLINKS+=	malloc.3 calloc.3
MLINKS+=	malloc.3 reallocarray.3
MLINKS+=	malloc.3 realloc.3
MLINKS+=	malloc.3 free.3
MLINKS+=	posix_memalign.3 memalign.3
MLINKS+=	vis.3 strvis.3
MLINKS+=	vis.3 strnvis.3
MLINKS+=	vis.3 strvisx.3
MLINKS+=	vis.3 stravis.3

# lib.mk doesn't allow empty LIB, so use prog.mk to install just the manpages
# until we actually build code from here
.include <prog.mk>
