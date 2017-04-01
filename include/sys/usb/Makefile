INCDIR=	/usr/include/sys/usb
INCS=	usba.h	\
	usbai.h

GENERATED_INCS= \
	usbdevs.h

INCS+=	${GENERATED_INCS}
CLEANFILES+=	${GENERATED_INCS}

AWK?=	/usr/bin/awk
USBDEVS_AWK = ../../../tools/usbdevs2h.awk
USBDEVS_DATA = ../../../kernel/drivers/usb/usbdevs

.PHONY: all

all: ${GENERATED_INCS}

usbdevs.h: ${USBDEVS_AWK} ${USBDEVS_DATA}
	$(AWK) -f ${USBDEVS_AWK} ${USBDEVS_DATA} -H > $@

SUBDIR=	clients \
	hubd

.include <prog.mk>
.include <inc.mk>
