LIB=	uutil
SRCS=	uu_alloc.c uu_avl.c uu_dprintf.c uu_ident.c uu_list.c uu_misc.c \
	uu_open.c uu_pname.c uu_string.c uu_strtoint.c

# FIXME: libuutil_impl.h shouldn't be installed, but some dmake components
# depend on reaching it from the proto area
INCS=	libuutil.h libuutil_impl.h
includes: incinstall

.include <unleashed.mk>
.PATH:	${SRCTOP}/usr/src/common/avl
SRCS+=	avl.c

.include <lib.mk>
