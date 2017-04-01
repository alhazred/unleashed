MODULE=		ctf
MODULE_TYPE=	misc
SRCS=		ctf_mod.c \
		ctf_subr.c \
		ctf_create.c \
		ctf_decl.c \
		ctf_error.c \
		ctf_hash.c \
		ctf_labels.c \
		ctf_lookup.c \
		ctf_open.c \
		ctf_types.c \
		ctf_util.c
DEFS=		-DCTF_OLD_VERSIONS
INCS=		${SRCTOP}/usr/src/common/ctf

.PATH: ${SRCTOP}/usr/src/common/ctf

.include <kmod.mk>
