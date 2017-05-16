HWDATADIR=	${SHAREDIR}/hwdata

FILES=  	pci.ids \
		usb.ids

# URLs for PCI IDs and USB IDs
PCIURL=http://pci-ids.ucw.cz/v2.2/pci.ids
USBURL=http://www.linux-usb.org/usb.ids

WGET=wget

all:

proginstall:
.for f in ${FILES}
	[ -d ${DESTDIR}${HWDATADIR} ] || \
	    ${INSTALL} -d ${PROG_INSTALL_OWN} -m 755 ${DESTDIR}${HWDATADIR}
	${INSTALL} ${COPY} ${PROG_INSTALL_OWN} -m 444 \
	    ${f} ${DESTDIR}${HWDATADIR}
.endfor

update:
	$(WGET) -O pci.ids $(PCIURL)
	$(WGET) -O usb.ids $(USBURL)
	sed -e '/^$$/,$$ d' < pci.ids > THIRDPARTYLICENSE.pciids
	cat THIRDPARTYLICENSE.pciids.tmpl >> THIRDPARTYLICENSE.pciids

.PHONY: update

.include <prog.mk>
