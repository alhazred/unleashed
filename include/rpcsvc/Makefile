INCDIR=	/usr/include/rpcsvc
INCS=	bootparam.h	\
	daemon_utils.h	\
	dbm.h		\
	nis_db.h	\
	nislib.h	\
	svc_dg_priv.h	\
	yp_prot.h	\
	ypclnt.h	\
	yppasswd.h	\
	ypupd.h		\
	rpc_sztypes.h

# the .x files
INCS+=	bootparam_prot.x \
	mount.x		\
	nfs4_prot.x	\
	nfs_acl.x	\
	nfs_prot.x	\
	nis.x		\
	nis_object.x	\
	rex.x		\
	rquota.x	\
	rstat.x		\
	rusers.x	\
	rwall.x		\
	spray.x		\
	ufs_prot.x	\
	yp.x

# headers generated from .x files
GENERATED_INCS= \
	bootparam_prot.h \
	mount.h		\
	nfs_prot.h	\
	nfs4_prot.h	\
	nis.h		\
	rex.h		\
	rquota.h	\
	rstat.h		\
	rusers.h	\
	rwall.h		\
	spray.h		\
	ufs_prot.h	\
	nfs_acl.h

INCS+=	${GENERATED_INCS}
CLEANFILES+= ${GENERATED_INCS}

RPCGEN?=	/usr/bin/rpcgen
SED?=		/usr/bin/sed

RPCGENFLAGS = -C -h

# rpcsvc rpcgen source (.x files)
#
# yp.x is an attempt at codifying what was hand coded in RPCL.
# Unfortunately it doesn't quite work. (The handcoded stuff isn't
# expressable in RPCL) this is due to the fact that YP was written
# before rpcgen existed.  Hence, yp_prot.h cannot be derived from yp.x
#
# There is no `.h' for nis_object.x because it is included by nis.x and
# the resulting .h is nis.h.

.PHONY: all

all: ${GENERATED_INCS}

.SUFFIXES: .x

.x.h:
	${RPCGEN} $(RPCGENFLAGS) $< -o $@

nis.h: nis.x
	$(RPCGEN) $(RPCGENFLAGS) nis.x |\
	$(SED) -e '/EDIT_START/,$$ d' > $@

rwall.h: rwall.x
	$(RPCGEN) $(RPCGENFLAGS) -M rwall.x -o $@

.include <prog.mk>
.include <inc.mk>
