MODULE=		ctfs
MODULE_TYPE=	fs
SRCS=		ctfs_all.c \
		ctfs_cdir.c \
		ctfs_ctl.c \
		ctfs_event.c \
		ctfs_latest.c \
		ctfs_root.c \
		ctfs_sym.c \
		ctfs_tdir.c \
		ctfs_tmpl.c

CERRWARN = -Wno-parentheses

REPOROOT = ${.CURDIR:H:H:H}
.include <kmod.mk>
