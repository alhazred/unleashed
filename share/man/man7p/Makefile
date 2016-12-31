MAN = arp.7p \
      dlpi.7p \
      icmp.7p \
      icmp6.7p \
      if_tcp.7p \
      inet.7p \
      inet6.7p \
      ip.7p \
      ip6.7p \
      ipsec.7p \
      ipsecah.7p \
      ipsecesp.7p \
      ndp.7p \
      pf_key.7p \
      rarp.7p \
      route.7p \
      routing.7p \
      sctp.7p \
      sip.7p \
      tcp.7p \
      udp.7p

MLINKS+=	arp.7p ARP.7p
MLINKS+=	icmp.7p ICMP.7p
MLINKS+=	if_tcp.7p if.7p
MLINKS+=	ip.7p IP.7p
MLINKS+=	ipsecah.7p AH.7p
MLINKS+=	ipsecesp.7p ESP.7p
MLINKS+=	ndp.7p NDP.7p
MLINKS+=	rarp.7p RARP.7p
MLINKS+=	sctp.7p SCTP.7p
MLINKS+=	tcp.7p TCP.7p
MLINKS+=	udp.7p UDP.7p

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
