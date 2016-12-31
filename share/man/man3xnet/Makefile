MAN = endhostent.3xnet \
      endnetent.3xnet \
      endprotoent.3xnet \
      endservent.3xnet \
      freeaddrinfo.3xnet \
      gai_strerror.3xnet \
      gethostname.3xnet \
      getnameinfo.3xnet \
      htonl.3xnet \
      if_nametoindex.3xnet \
      inet_addr.3xnet \
      inet_ntop.3xnet

MLINKS+=	freeaddrinfo.3xnet getaddrinfo.3xnet
MLINKS+=	endhostent.3xnet gethostbyaddr.3xnet
MLINKS+=	endhostent.3xnet gethostbyname.3xnet
MLINKS+=	endhostent.3xnet gethostent.3xnet
MLINKS+=	endnetent.3xnet getnetbyaddr.3xnet
MLINKS+=	endnetent.3xnet getnetbyname.3xnet
MLINKS+=	endnetent.3xnet getnetent.3xnet
MLINKS+=	endprotoent.3xnet getprotobyname.3xnet
MLINKS+=	endprotoent.3xnet getprotobynumber.3xnet
MLINKS+=	endprotoent.3xnet getprotoent.3xnet
MLINKS+=	endservent.3xnet getservbyname.3xnet
MLINKS+=	endservent.3xnet getservbyport.3xnet
MLINKS+=	endservent.3xnet getservent.3xnet
MLINKS+=	htonl.3xnet htons.3xnet
MLINKS+=	if_nametoindex.3xnet if_freenameindex.3xnet
MLINKS+=	if_nametoindex.3xnet if_indextoname.3xnet
MLINKS+=	if_nametoindex.3xnet if_nameindex.3xnet
MLINKS+=	inet_addr.3xnet inet_lnaof.3xnet
MLINKS+=	inet_addr.3xnet inet_makeaddr.3xnet
MLINKS+=	inet_addr.3xnet inet_netof.3xnet
MLINKS+=	inet_addr.3xnet inet_network.3xnet
MLINKS+=	inet_addr.3xnet inet_ntoa.3xnet
MLINKS+=	inet_ntop.3xnet inet_pton.3xnet
MLINKS+=	htonl.3xnet ntohl.3xnet
MLINKS+=	htonl.3xnet ntohs.3xnet
MLINKS+=	endhostent.3xnet sethostent.3xnet
MLINKS+=	endnetent.3xnet setnetent.3xnet
MLINKS+=	endprotoent.3xnet setprotoent.3xnet
MLINKS+=	endservent.3xnet setservent.3xnet

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
