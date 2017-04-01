.include <../../Makefile.cfgparam>

SUBDIR_${CONFIG_LOFI}		=  lofi
SUBDIR_${CONFIG_RAMDISK}	+= ramdisk
SUBDIR_${CONFIG_RANDOM}		+= random
SUBDIR_${CONFIG_VIRTIO}		+= virtio

SUBDIR = ${SUBDIR_y} \
	 net \
	 physmem

.include <bsd.subdir.mk>
