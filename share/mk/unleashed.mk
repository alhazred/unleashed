.ifndef SRCTOP
SRCTOP!=	git rev-parse --show-toplevel
.if empty(SRCTOP)
.error "cannot find top of source tree - set SRCTOP manually"
.endif
.endif
