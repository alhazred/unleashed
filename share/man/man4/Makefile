.include <../../../Makefile.cfgparam>

MAN_sparc = sbus.4

MAN_i386 = sysbus.4

MAN = Intro.4 \
      NISLDAPmapping.4 \
      a.out.4 \
      admin.4 \
      au.4 \
      audit.log.4 \
      audit_class.4 \
      audit_control.4 \
      audit_event.4 \
      audit_user.4 \
      auth_attr.4 \
      autofs.4 \
      bart_manifest.4 \
      bart_rules.4 \
      bootparams.4 \
      compver.4 \
      contents.4 \
      contract.4 \
      copyright.4 \
      core.4 \
      crypt.conf.4 \
      crypto_certs.4 \
      ctf.4 \
      d_passwd.4 \
      dacf.conf.4 \
      dat.conf.4 \
      default_fs.4 \
      defaultdomain.4 \
      defaultrouter.4 \
      depend.4 \
      device_allocate.4 \
      device_contract.4 \
      device_maps.4 \
      devices.4 \
      dfstab.4 \
      dhcp_inittab.4 \
      dialups.4 \
      dir_ufs.4 \
      driver.conf.4 \
      ethers.4 \
      exec_attr.4 \
      fdi.4 \
      format.dat.4 \
      fspec.4 \
      fstypes.4 \
      ftp.4 \
      ftpusers.4 \
      fx_dptbl.4 \
      gateways.4 \
      group.4 \
      gsscred.conf.4 \
      hba.conf.4 \
      holidays.4 \
      hosts.4 \
      hosts.equiv.4 \
      hosts_access.4 \
      hosts_options.4 \
      ib.4 \
      ipf.4 \
      ipmon.4 \
      ipnat.4 \
      ippool.4 \
      inet_type.4 \
      inetd.conf.4 \
      init.4 \
      init.d.4 \
      inittab.4 \
      ipaddrsel.conf.4 \
      ipnodes.4 \
      issue.4 \
      kadm5.acl.4 \
      kdc.conf.4 \
      keytables.4 \
      krb5.conf.4 \
      ldapfilter.conf.4 \
      ldapsearchprefs.conf.4 \
      ldaptemplates.conf.4 \
      logadm.conf.4 \
      logindevperm.4 \
      loginlog.4 \
      mailer.conf.4 \
      mech.4 \
      mnttab.4 \
      mpapi.conf.4 \
      ndmp.4 \
      ndpd.conf.4 \
      netconfig.4 \
      netgroup.4 \
      netid.4 \
      netmasks.4 \
      netrc.4 \
      networks.4 \
      nfs.4 \
      nfslog.conf.4 \
      nfssec.conf.4 \
      nodename.4 \
      nologin.4 \
      note.4 \
      notrouter.4 \
      nscd.conf.4 \
      nsmbrc.4 \
      nss.4 \
      nsswitch.conf.4 \
      packingrules.4 \
      pam.conf.4 \
      passwd.4 \
      path_to_inst.4 \
      pci.4 \
      phones.4 \
      pkginfo.4 \
      pkgmap.4 \
      policy.conf.4 \
      power.conf.4 \
      printers.4 \
      printers.conf.4 \
      priv_names.4 \
      proc.4 \
      process.4 \
      prof_attr.4 \
      profile.4 \
      project.4 \
      protocols.4 \
      prototype.4 \
      pseudo.4 \
      publickey.4 \
      queuedefs.4 \
      rcmscript.4 \
      remote.4 \
      resolv.conf.4 \
      rmtab.4 \
      rpc.4 \
      rt_dptbl.4 \
      sasl_appname.conf.4 \
      scsi.4 \
      securenets.4 \
      service_bundle.4 \
      service_provider.conf.4 \
      services.4 \
      shadow.4 \
      sharetab.4 \
      shells.4 \
      smb.4 \
      smbautohome.4 \
      smhba.conf.4 \
      sock2path.d.4 \
      space.4 \
      sulog.4 \
      syslog.conf.4 \
      system.4 \
      term.4 \
      terminfo.4 \
      timezone.4 \
      ts_dptbl.4 \
      ttydefs.4 \
      ttysrch.4 \
      ufsdump.4 \
      updaters.4 \
      user_attr.4 \
      utmp.4 \
      utmpx.4 \
      vfstab.4 \
      warn.conf.4 \
      ypfiles.4 \
      yppasswdd.4 \
      ypserv.4 \
      zoneinfo.4 \
      $(MAN_$(CONFIG_MACH))

MLINKS+=	Intro.4 intro.4
MLINKS+=	default_fs.4 fs.4
MLINKS+=	devices.4 devid_cache.4
MLINKS+=	devices.4 devname_cache.4
MLINKS+=	devices.4 mdi_ib_cache.4
MLINKS+=	devices.4 mdi_scsi_vhci_cache.4
MLINKS+=	devices.4 pci_unitaddr_persistent.4
MLINKS+=	devices.4 snapshot_cache.4
MLINKS+=	dir_ufs.4 dir.4
MLINKS+=	hosts.equiv.4 rhosts.4
MLINKS+=	hosts_access.4 hosts.allow.4
MLINKS+=	hosts_access.4 hosts.deny.4
MLINKS+=	ipf.4 ipf.conf.4
MLINKS+=	ipf.4 ipf6.conf.4
MLINKS+=	ipmon.4 ipmon.conf.4
MLINKS+=	ipnat.4 ipnat.conf.4
MLINKS+=	ippool.4 ippool.conf.4
MLINKS+=	logindevperm.4 fbtab.4
MLINKS+=	mech.4 qop.4
MLINKS+=	pam.conf.4 pam.d.4
MLINKS+=	pci.4 pcie.4
MLINKS+=	ufsdump.4 dumpdates.4
MLINKS+=	utmp.4 wtmp.4
MLINKS+=	utmpx.4 wtmpx.4

.if $(CONFIG_MACH) == i386
MLINKS+=	sysbus.4 isa.4
.endif

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
