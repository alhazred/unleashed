.include <../../../Makefile.cfgparam>

MAN_i386 = agpgart_io.7i

MAN = audio.7i \
      cdio.7i \
      dkio.7i \
      dsp.7i \
      fbio.7i \
      fdio.7i \
      hdio.7i \
      iec61883.7i \
      ipnat.7i \
      mhd.7i \
      mixer.7i \
      mtio.7i \
      prnio.7i \
      quotactl.7i \
      sesio.7i \
      sockio.7i \
      streamio.7i \
      termio.7i \
      termiox.7i \
      uscsi.7i \
      visual_io.7i \
      vt.7i \
      $(MAN_$(CONFIG_MACH))

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
