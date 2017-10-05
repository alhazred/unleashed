MACHINE_CPU?=	${MACHINE}

CPPFLAGS+=	-D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS=
CPPFLAGS+=	-I${LCRYPTO_SRC}/compat/include
# see libressl-portable/portable:m4/disable-compiler-warnings.m4 
CFLAGS+=	-Wno-pointer-sign

SRCS+=		compat/timingsafe_bcmp.c compat/timingsafe_memcmp.c
INSTALL_COPY?=	${COPY}
SHAREMODE?=	444

# we don't use MAPFILE_VERS here, because that would cause us to ignore
# shlib_version
SHLIB_LDADD+=	-Mmapfile
.NOPATH: mapfile ${VERSION_SCRIPT}
mapfile: ${VERSION_SCRIPT}
	{ echo '$$mapfile_version 2'; \
	    printf 'SYMBOL_SCOPE '; \
	    cat ${VERSION_SCRIPT}; } > $@
CLEANFILES+=	mapfile
BUILDFIRST+=	mapfile
# XXX OPENSSL_cpuid_setup needs this
SHLIB_LDADD+= -z textoff

# XXX <openssl/*> includes need to be available in DESTDIR if DESTDIR is
# specified since the mk files set -isysroot in that case
.ifdef DESTDIR
beforebuild: includes prereq
.endif
# install config files as well
afterinstall: distribution
distribution: mkdir_etc_ssl
mkdir_etc_ssl:
	${INSTALL} -d ${DESTDIR}/etc/ssl
