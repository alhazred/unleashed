MAN = libMPAPI.3lib \
      libSMHBAAPI.3lib \
      libadm.3lib \
      libaio.3lib \
      libavl.3lib \
      libbsdmalloc.3lib \
      libbsm.3lib \
      libc.3lib \
      libc_db.3lib \
      libcfgadm.3lib \
      libcommputil.3lib \
      libcontract.3lib \
      libcpc.3lib \
      libcrypt.3lib \
      libcurses.3lib \
      libdat.3lib \
      libdevid.3lib \
      libdevinfo.3lib \
      libdl.3lib \
      libdlpi.3lib \
      libdns_sd.3lib \
      libdoor.3lib \
      libdtrace.3lib \
      libefi.3lib \
      libelf.3lib \
      libexacct.3lib \
      libfcoe.3lib \
      libform.3lib \
      libfstyp.3lib \
      libgen.3lib \
      libgss.3lib \
      libintl.3lib \
      libiscsit.3lib \
      libkmf.3lib \
      libkrb5.3lib \
      libkstat.3lib \
      libkvm.3lib \
      libl.3lib \
      liblgrp.3lib \
      libmail.3lib \
      libmalloc.3lib \
      libmapmalloc.3lib \
      libmd.3lib \
      libmd5.3lib \
      libmenu.3lib \
      libmp.3lib \
      libmtmalloc.3lib \
      libnls.3lib \
      libnvpair.3lib \
      libpam.3lib \
      libpanel.3lib \
      libpapi.3lib \
      libpctx.3lib \
      libpicl.3lib \
      libpicltree.3lib \
      libpkcs11.3lib \
      libpool.3lib \
      libproc.3lib \
      libproject.3lib \
      libpthread.3lib \
      libresolv.3lib \
      librpcsvc.3lib \
      librsm.3lib \
      librt.3lib \
      librtld_db.3lib \
      libsasl.3lib \
      libscf.3lib \
      libsctp.3lib \
      libsec.3lib \
      libsecdb.3lib \
      libsendfile.3lib \
      libsip.3lib \
      libstmf.3lib \
      libsys.3lib \
      libsysevent.3lib \
      libtecla.3lib \
      libthread.3lib \
      libtsalarm.3lib \
      libumem.3lib \
      libuuid.3lib \
      libvolmgt.3lib \
      libw.3lib \
      liby.3lib

MLINKS+=	libMPAPI.3lib libmpapi.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetAdapterAttributes.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetAdapterPortAttributes.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetBindingCapability.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetBindingSupport.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetDiscoveredPortAttributes.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetFCPhyAttributes.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetLUNStatistics.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetNumberofPorts.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetPersistentBinding.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetPhyStatistics.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetPortAttributesByWWN.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetPortType.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetProtocolStatistics.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetSASPhyAttributes.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetTargetMapping.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetVendorLibraryAttributes.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetVersion.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_GetWrapperLibraryAttributes.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RegisterForAdapterAddEvents.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RegisterForAdapterEvents.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RegisterForAdapterPhyStatEvents.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RegisterForAdapterPortEvents.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RegisterForAdapterPortStatEvents.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RegisterForTargetEvents.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RegisterLibrary.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RemoveAllPersistentBindings.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_RemovePersistentBinding.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_ScsiInquiry.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_ScsiReadCapacity.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_ScsiReportLuns.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_SendECHO.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_SendSMPPassThru.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_SendTEST.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_SetBindingSupport.3lib
MLINKS+=	libSMHBAAPI.3lib SMHBA_SetPersistentBinding.3lib
MLINKS+=	libSMHBAAPI.3lib libsmhbaapi.3lib
MLINKS+=	libcurses.3lib libtermcap.3lib
MLINKS+=	libcurses.3lib libtermlib.3lib
MLINKS+=	librt.3lib libposix4.3lib

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
