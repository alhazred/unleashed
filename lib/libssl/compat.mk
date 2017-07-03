.include <unleashed.mk>
LCRYPTO_SRC=	${SRCTOP}/lib/libcrypto
CPPFLAGS+=	-D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS=
CPPFLAGS+=	-I${LCRYPTO_SRC}/compat/include
SRCS+=		timingsafe_memcmp.c
.PATH:		${LCRYPTO_SRC}/compat

BSDOBJDIR?=     ${.OBJDIR:tA:H:H}
SHLIB_LDADD?=   ${LDADD}

INSTALL_COPY?=	${COPY}
SHAREMODE?=	444

# we don't use MAPFILE_VERS here, because that would cause us to ignore
# shlib_version
SHLIB_LDADD+=	-M${.OBJDIR}/mapfile
mapfile: ${VERSION_SCRIPT}
	{ echo '$$mapfile_version 2'; \
	    printf 'SYMBOL_SCOPE '; \
	    cat ${VERSION_SCRIPT}; } > $@
CLEANFILES+=	mapfile
all: mapfile

# XXX <openssl/*> includes need to be available in DESTDIR if DESTDIR is
# specified since the mk files set -isysroot in that case
.ifdef DESTDIR
beforebuild: includes
.endif
