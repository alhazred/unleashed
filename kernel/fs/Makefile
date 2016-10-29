.include <../../Makefile.cfgparam>

SUBDIR_${CONFIG_FS_AUTOFS}	=  autofs
SUBDIR_${CONFIG_FS_HSFS}	+= hsfs
SUBDIR_${CONFIG_FS_LOFS}	+= lofs
SUBDIR_${CONFIG_FS_NAMEFS}	+= namefs
SUBDIR_${CONFIG_FS_PCFS}	+= pcfs

SUBDIR = ${SUBDIR_y} \
	 tmpfs

.include <bsd.subdir.mk>
