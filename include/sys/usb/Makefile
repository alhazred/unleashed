INCDIR=	/usr/include/sys/usb
INCS=	usba.h	\
	usbai.h

GENERATED_INCS= \
	usbdevs.h

INCS+=	${GENERATED_INCS}
CLEANFILES+=	${GENERATED_INCS}

AWK?=	/usr/bin/awk
USBDEVS_AWK = ../../../usr/src/uts/common/io/usb/usbdevs2h.awk
USBDEVS_DATA = ../../../usr/src/uts/common/io/usb/usbdevs

.PHONY: all

all: ${GENERATED_INCS}

usbdevs.h: ${USBDEVS_AWK} ${USBDEVS_DATA}
	$(AWK) -f ${USBDEVS_AWK} ${USBDEVS_DATA} -H > $@

SUBDIR=	clients \
	hubd

.include <prog.mk>
.include <inc.mk>
