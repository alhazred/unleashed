INCDIR=	/usr/include/rpc
INCS=

# FIXME: we shouldn't actually install rpc_sztypes.x, only the .h file

# the .x files
INCS+=	rpc_sztypes.x

# headers generated from .x files
GENERATED_INCS= \
	rpc_sztypes.h

INCS+=	${GENERATED_INCS}
CLEANFILES+= ${GENERATED_INCS}

RPCGEN?=	/usr/bin/rpcgen
SED?=		/usr/bin/sed

RPCGENFLAGS = -C -h

.PHONY: all

all: ${GENERATED_INCS}

.SUFFIXES: .x

.x.h:
	${RPCGEN} $(RPCGENFLAGS) $< -o $@

.include <prog.mk>
.include <inc.mk>
