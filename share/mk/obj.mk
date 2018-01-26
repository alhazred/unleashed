# $Id: obj.mk,v 1.15 2012/11/11 22:37:02 sjg Exp $
#
#	@(#) Copyright (c) 1999-2010, Simon J. Gerraty
#
#	This file is provided in the hope that it will
#	be of use.  There is absolutely NO WARRANTY.
#	Permission to copy, redistribute or otherwise
#	use this file is hereby granted provided that 
#	the above copyright notice and this notice are
#	left intact. 
#      
#	Please send copies of changes and bug-fixes to:
#	sjg@crufty.net
#

.if !target(__${.PARSEFILE:S,bsd.,,}__)
__${.PARSEFILE:S,bsd.,,}__:

.if defined(NOOBJ)
obj:
.else

# this has to match how make behaves
.if defined(MAKEOBJDIRPREFIX) || defined(MAKEOBJDIR)
.if defined(MAKEOBJDIRPREFIX)
__objdir:=	${MAKEOBJDIRPREFIX}${.CURDIR}
.else
__objdir:=	${MAKEOBJDIR}
.endif
.else
__objdir=	obj
.endif

_SUBDIRUSE:

obj! _SUBDIRUSE
	@cd ${.CURDIR}; \
	here=`pwd`; \
	if [ -n "${UNLEASHED_OBJ}" ]; then \
		dest="${UNLEASHED_OBJ}$$here"; \
		echo "$$here/${__objdir} -> $$dest"; \
		if [ ! -L ${__objdir} -o "`readlink ${__objdir}`" != "$$dest" ]; then \
			[ -e ${__objdir} ] && rm -rf ${__objdir}; \
			ln -sf $$dest ${__objdir}; \
		fi; \
		mkdir -p $$dest; \
	else \
		dest=$$here/${__objdir} ; \
		if [ ! -d ${__objdir} ]; then \
			echo "making $$dest"; \
			mkdir -p $$dest; \
		fi; \
	fi
.endif
.endif
