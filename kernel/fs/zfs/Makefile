MODULE=		zfs
MODULE_TYPE=	drv
MODULE_TYPE_LINKS=fs
MODULE_DEPS=	fs/specfs \
		crypto/swrand \
		misc/idmap \
		misc/sha2 \
		misc/skein \
		misc/edonr \
		sched/SDC
MODULE_CONF=	zfs.conf
SRCS=		arc.c \
		blkptr.c \
		bplist.c \
		bpobj.c \
		bptree.c \
		bqueue.c \
		common/zfeature_common.c \
		common/zfs_comutil.c \
		common/zfs_deleg.c \
		common/zfs_fletcher.c \
		common/zfs_namecheck.c \
		common/zfs_prop.c \
		common/zpool_prop.c \
		common/zprop_common.c \
		dbuf.c \
		ddt.c \
		ddt_zap.c \
		dmu.c \
		dmu_diff.c \
		dmu_object.c \
		dmu_objset.c \
		dmu_send.c \
		dmu_traverse.c \
		dmu_tx.c \
		dmu_zfetch.c \
		dnode.c \
		dnode_sync.c \
		dsl_bookmark.c \
		dsl_dataset.c \
		dsl_deadlist.c \
		dsl_deleg.c \
		dsl_destroy.c \
		dsl_dir.c \
		dsl_pool.c \
		dsl_prop.c \
		dsl_scan.c \
		dsl_synctask.c \
		dsl_userhold.c \
		edonr_zfs.c \
		gzip.c \
		lz4.c \
		lzjb.c \
		metaslab.c \
		multilist.c \
		range_tree.c \
		refcount.c \
		rrwlock.c \
		sa.c \
		sha256.c \
		skein_zfs.c \
		spa.c \
		spa_boot_${CONFIG_MACH}.c \
		spa_config.c \
		spa_errlog.c \
		spa_history.c \
		spa_misc.c \
		space_map.c \
		space_reftree.c \
		txg.c \
		uberblock.c \
		unique.c \
		vdev.c \
		vdev_cache.c \
		vdev_disk.c \
		vdev_file.c \
		vdev_label.c \
		vdev_mirror.c \
		vdev_missing.c \
		vdev_queue.c \
		vdev_raidz.c \
		vdev_root.c \
		zap.c \
		zap_leaf.c \
		zap_micro.c \
		zfeature.c \
		zfs_acl.c \
		zfs_byteswap.c \
		zfs_ctldir.c \
		zfs_debug.c \
		zfs_dir.c \
		zfs_fm.c \
		zfs_fuid.c \
		zfs_ioctl.c \
		zfs_log.c \
		zfs_onexit.c \
		zfs_replay.c \
		zfs_rlock.c \
		zfs_sa.c \
		zfs_vfsops.c \
		zfs_vnops.c \
		zfs_znode.c \
		zil.c \
		zio.c \
		zio_checksum.c \
		zio_compress.c \
		zio_inject.c \
		zle.c \
		zrlock.c \
		zvol.c
INCS=		${.CURDIR} \
		${.CURDIR}/common \
		${REPOROOT}/usr/src/common

CERRWARN	+= -Wno-type-limits
CERRWARN	+= -Wno-switch
CERRWARN	+= -Wno-parentheses
CERRWARN	+= -Wno-unused-variable
CERRWARN	+= -Wno-unused-function
CERRWARN	+= -Wno-unused-label

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
