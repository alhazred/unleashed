MAN=	accept.2 bind.2 connect.2 getpeername.2 getsockname.2 getsockopt.2 \
	listen.2 recv.2 send.2 shutdown.2 socket.2 socketpair.2 sockatmark.3 \
	CMSG_DATA.3 readdir.3

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

# lib.mk doesn't allow empty LIB, so use prog.mk to install just the manpages
# until we actually build code from here
.include <prog.mk>
