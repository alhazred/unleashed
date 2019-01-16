# make to dmake-in-bldenv wrapper

# dmake build doesn't really work with separate "all", and has baked-in
# assumptions in many places that "install" on any dependencies has been made
# previously. so just do "install".
all install:
	cd src && ${SRCTOP}/tools/bldenv.sh dmake -j ${.MAKE.JOBS:U1} install

clean cleandir:
	cd src && ${SRCTOP}/tools/bldenv.sh dmake -j ${.MAKE.JOBS:U1} clobber

includes obj:

.include <unleashed.mk>
