MAN = DNSServiceBrowse.3dns_sd \
      DNSServiceConstructFullName.3dns_sd \
      DNSServiceCreateConnection.3dns_sd \
      DNSServiceEnumerateDomains.3dns_sd \
      DNSServiceProcessResult.3dns_sd \
      DNSServiceQueryRecord.3dns_sd \
      DNSServiceReconfirmRecord.3dns_sd \
      DNSServiceRefDeallocate.3dns_sd \
      DNSServiceRefSockFD.3dns_sd \
      DNSServiceRegister.3dns_sd \
      DNSServiceResolve.3dns_sd \
      TXTRecordCreate.3dns_sd

MLINKS+=	DNSServiceCreateConnection.3dns_sd DNSServiceAddRecord.3dns_sd
MLINKS+=	DNSServiceCreateConnection.3dns_sd DNSServiceRegisterRecord.3dns_sd
MLINKS+=	DNSServiceCreateConnection.3dns_sd DNSServiceRemoveRecord.3dns_sd
MLINKS+=	DNSServiceCreateConnection.3dns_sd DNSServiceUpdateRecord.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordContainsKey.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordDeallocate.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordGetBytesPtr.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordGetCount.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordGetItemAtIndex.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordGetLength.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordGetValuePtr.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordRemoveValue.3dns_sd
MLINKS+=	TXTRecordCreate.3dns_sd TXTRecordSetValue.3dns_sd

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
