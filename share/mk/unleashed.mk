.ifndef SRCTOP
.  if ${.CURDIR:M/usr/src*} == ${.CURDIR}
SRCTOP=		/usr/src
.  else
SRCTOP!=	cd ${.CURDIR} && git rev-parse --show-toplevel
.  endif
.  if empty(SRCTOP)
.  error cannot find top of source tree - set SRCTOP manually
.  endif
.endif
UNLEAHED_OBJ?=	/usr/obj/${MACHINE}
