.include <../../Makefile.cfgparam>

SUBDIR_${CONFIG_FS_AUTOFS}	=  autofs
SUBDIR_${CONFIG_FS_HSFS}	+= hsfs
SUBDIR_${CONFIG_FS_LOFS}	+= lofs
SUBDIR_${CONFIG_FS_NAMEFS}	+= namefs
SUBDIR_${CONFIG_FS_PCFS}	+= pcfs
SUBDIR_${CONFIG_FS_UDFS}	+= udfs

SUBDIR = ${SUBDIR_y} \
	 bootfs \
	 ctfs \
	 dcfs \
	 dev \
	 devfs \
	 doorfs \
	 fifofs \
	 mntfs \
	 objfs \
	 portfs \
	 sockfs \
	 specfs \
	 tmpfs \
	 ufs

.include <bsd.subdir.mk>
