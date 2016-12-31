MAN = byteorder.3socket \
      ethers.3socket \
      getaddrinfo.3socket \
      getifaddrs.3socket \
      getipnodebyname.3socket \
      getnetbyname.3socket \
      getprotobyname.3socket \
      getservbyname.3socket \
      getsourcefilter.3socket \
      icmp6_filter.3socket \
      if_nametoindex.3socket \
      inet.3socket \
      inet6_opt.3socket \
      inet6_rth.3socket \
      rcmd.3socket \
      rexec.3socket \
      sctp_bindx.3socket \
      sctp_getladdrs.3socket \
      sctp_getpaddrs.3socket \
      sctp_opt_info.3socket \
      sctp_peeloff.3socket \
      sctp_recvmsg.3socket \
      sctp_send.3socket \
      sctp_sendmsg.3socket \
      sockaddr.3socket \
      spray.3socket

MLINKS+=	byteorder.3socket htonl.3socket
MLINKS+=	byteorder.3socket htonll.3socket
MLINKS+=	byteorder.3socket htons.3socket
MLINKS+=	byteorder.3socket ntohl.3socket
MLINKS+=	byteorder.3socket ntohll.3socket
MLINKS+=	byteorder.3socket ntohs.3socket
MLINKS+=	ethers.3socket ether_aton.3socket
MLINKS+=	ethers.3socket ether_aton_r.3socket
MLINKS+=	ethers.3socket ether_hostton.3socket
MLINKS+=	ethers.3socket ether_line.3socket
MLINKS+=	ethers.3socket ether_ntoa.3socket
MLINKS+=	ethers.3socket ether_ntoa_r.3socket
MLINKS+=	ethers.3socket ether_ntohost.3socket
MLINKS+=	getaddrinfo.3socket freeaddrinfo.3socket
MLINKS+=	getaddrinfo.3socket gai_strerror.3socket
MLINKS+=	getaddrinfo.3socket getnameinfo.3socket
MLINKS+=	getifaddrs.3socket freeifaddrs.3socket
MLINKS+=	getipnodebyname.3socket freehostent.3socket
MLINKS+=	getipnodebyname.3socket getipnodebyaddr.3socket
MLINKS+=	getnetbyname.3socket endnetent.3socket
MLINKS+=	getnetbyname.3socket getnetbyaddr.3socket
MLINKS+=	getnetbyname.3socket getnetbyaddr_r.3socket
MLINKS+=	getnetbyname.3socket getnetbyname_r.3socket
MLINKS+=	getnetbyname.3socket getnetent.3socket
MLINKS+=	getnetbyname.3socket getnetent_r.3socket
MLINKS+=	getnetbyname.3socket setnetent.3socket
MLINKS+=	getprotobyname.3socket endprotoent.3socket
MLINKS+=	getprotobyname.3socket getprotobyname_r.3socket
MLINKS+=	getprotobyname.3socket getprotobynumber.3socket
MLINKS+=	getprotobyname.3socket getprotobynumber_r.3socket
MLINKS+=	getprotobyname.3socket getprotoent.3socket
MLINKS+=	getprotobyname.3socket getprotoent_r.3socket
MLINKS+=	getprotobyname.3socket setprotoent.3socket
MLINKS+=	getservbyname.3socket endservent.3socket
MLINKS+=	getservbyname.3socket getservbyname_r.3socket
MLINKS+=	getservbyname.3socket getservbyport.3socket
MLINKS+=	getservbyname.3socket getservbyport_r.3socket
MLINKS+=	getservbyname.3socket getservent.3socket
MLINKS+=	getservbyname.3socket getservent_r.3socket
MLINKS+=	getservbyname.3socket setservent.3socket
MLINKS+=	getsourcefilter.3socket getipv4sourcefilter.3socket
MLINKS+=	getsourcefilter.3socket setipv4sourcefilter.3socket
MLINKS+=	getsourcefilter.3socket setsourcefilter.3socket
MLINKS+=	if_nametoindex.3socket if_freenameindex.3socket
MLINKS+=	if_nametoindex.3socket if_indextoname.3socket
MLINKS+=	if_nametoindex.3socket if_nameindex.3socket
MLINKS+=	inet.3socket inet6.3socket
MLINKS+=	inet.3socket inet_addr.3socket
MLINKS+=	inet.3socket inet_aton.3socket
MLINKS+=	inet.3socket inet_lnaof.3socket
MLINKS+=	inet.3socket inet_makeaddr.3socket
MLINKS+=	inet.3socket inet_netof.3socket
MLINKS+=	inet.3socket inet_network.3socket
MLINKS+=	inet.3socket inet_ntoa.3socket
MLINKS+=	inet.3socket inet_ntop.3socket
MLINKS+=	inet.3socket inet_pton.3socket
MLINKS+=	inet6_opt.3socket inet6_opt_append.3socket
MLINKS+=	inet6_opt.3socket inet6_opt_find.3socket
MLINKS+=	inet6_opt.3socket inet6_opt_finish.3socket
MLINKS+=	inet6_opt.3socket inet6_opt_get_val.3socket
MLINKS+=	inet6_opt.3socket inet6_opt_init.3socket
MLINKS+=	inet6_opt.3socket inet6_opt_next.3socket
MLINKS+=	inet6_opt.3socket inet6_opt_set_val.3socket
MLINKS+=	inet6_rth.3socket inet6_rth_add.3socket
MLINKS+=	inet6_rth.3socket inet6_rth_getaddr.3socket
MLINKS+=	inet6_rth.3socket inet6_rth_init.3socket
MLINKS+=	inet6_rth.3socket inet6_rth_reverse.3socket
MLINKS+=	inet6_rth.3socket inet6_rth_segments.3socket
MLINKS+=	inet6_rth.3socket inet6_rth_space.3socket
MLINKS+=	rcmd.3socket rcmd_af.3socket
MLINKS+=	rcmd.3socket rresvport.3socket
MLINKS+=	rcmd.3socket rresvport_af.3socket
MLINKS+=	rcmd.3socket ruserok.3socket
MLINKS+=	rexec.3socket rexec_af.3socket
MLINKS+=	sctp_getladdrs.3socket sctp_freeladdrs.3socket
MLINKS+=	sctp_getpaddrs.3socket sctp_freepaddrs.3socket
MLINKS+=	sockaddr.3socket sockaddr_dl.3socket
MLINKS+=	sockaddr.3socket sockaddr_in.3socket
MLINKS+=	sockaddr.3socket sockaddr_in6.3socket
MLINKS+=	sockaddr.3socket sockaddr_ll.3socket
MLINKS+=	sockaddr.3socket sockaddr_storage.3socket
MLINKS+=	sockaddr.3socket sockaddr_un.3socket

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
