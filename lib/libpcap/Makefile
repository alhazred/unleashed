.include <unleashed.mk>

PCAP_DISTDIR=	${SRCTOP}/contrib/libpcap

LIB=	pcap

GENSRCS = grammar.c scanner.c version.c

SRCS= ${GENSRCS}

BUILDFIRST = ${GENSRCS} 

SRCS+=	pcap-libdlpi.c fad-getad.c pcap.c inet.c fad-helpers.c \
	gencode.c optimize.c nametoaddr.c etherent.c savefile.c \
	sf-pcap.c sf-pcap-ng.c pcap-common.c bpf_image.c bpf_dump.c bpf_filter.c \
	dlpisubs.c ${GENSRCS} 

# Old compatibility headers
INCS=	${PCAP_DISTDIR}/pcap.h ${PCAP_DISTDIR}/pcap-namedb.h ${PCAP_DISTDIR}/pcap-bpf.h

MAN=	pcap.3 \
	pcap_activate.3 \
	pcap_breakloop.3 \
	pcap_can_set_rfmon.3 \
	pcap_close.3 \
	pcap_compile.3 \
	pcap_create.3 \
	pcap_datalink.3 \
	pcap_datalink_name_to_val.3 \
	pcap_datalink_val_to_name.3 \
	pcap_dump.3 \
	pcap_dump_close.3 \
	pcap_dump_file.3 \
	pcap_dump_flush.3 \
	pcap_dump_ftell.3 \
	pcap_dump_open.3 \
	pcap_file.3 \
	pcap_fileno.3 \
	pcap_findalldevs.3 \
	pcap_freecode.3 \
	pcap_get_selectable_fd.3 \
	pcap_get_tstamp_precision.3 \
	pcap_geterr.3 \
	pcap_inject.3 \
	pcap_is_swapped.3 \
	pcap_lib_version.3 \
	pcap_list_datalinks.3 \
	pcap_list_tstamp_types.3 \
	pcap_lookupdev.3 \
	pcap_lookupnet.3 \
	pcap_loop.3 \
	pcap_major_version.3 \
	pcap_next_ex.3 \
	pcap_offline_filter.3 \
	pcap_open_dead.3 \
	pcap_open_live.3 \
	pcap_open_offline.3 \
	pcap_set_buffer_size.3 \
	pcap_set_datalink.3 \
	pcap_set_immediate_mode.3 \
	pcap_set_promisc.3 \
	pcap_set_rfmon.3 \
	pcap_set_snaplen.3 \
	pcap_set_timeout.3 \
	pcap_set_tstamp_precision.3 \
	pcap_set_tstamp_type.3 \
	pcap_setdirection.3 \
	pcap_setfilter.3 \
	pcap_setnonblock.3 \
	pcap_snapshot.3 \
	pcap_stats.3 \
	pcap_statustostr.3 \
	pcap_strerror.3 \
	pcap-savefile.5 \
	pcap_tstamp_type_name_to_val.3 \
	pcap_tstamp_type_val_to_name.3 \
	pcap-filter.7 \
	pcap-linktype.7

# Our man pages are a special copy from the distdir. See below.
CLEANFILES+=${MAN}
CLEANFILES+=${GENSRCS} grammar.h scanner.h

CFLAGS+=-DHAVE_CONFIG_H -I${.CURDIR} -I${.OBJDIR}
CFLAGS+=-D_U_="__attribute__((unused))"
CFLAGS+=-DBUILDING_PCAP
CFLAGS+=-I${PCAP_DISTDIR}

WARNS?=	0

SHLIB_MAJOR=	1
SHLIB_MINOR=	8
SHLIB_TEENY=	1

SHLIB_LDADD+=   -M${PCAP_DISTDIR}/mapfile

.PATH:	${PCAP_DISTDIR}

grammar.c:	
	bison -y -p pcap_ -o grammar.c -d ${PCAP_DISTDIR}/grammar.y

scanner.c: grammar.c
	flex -P pcap_ --header-file=scanner.h --nounput -o scanner.c ${PCAP_DISTDIR}/scanner.l

version.c: ${PCAP_DISTDIR}/VERSION
	@rm -f $@
	sed 's/.*/char pcap_version[] = "&";/' ${PCAP_DISTDIR}/VERSION > $@

#
# Magic to convert the man pages to something non Solarish
#
.for _page in ${MAN}
${_page}:
	if [ -f ${PCAP_DISTDIR}/${_page:S/3$/3pcap/} ]; then		\
		F=${_page:S/3$/3pcap/}; 				\
	fi;								\
	sed -e 's/3PCAP/3/g' ${PCAP_DISTDIR}/$$F > ${_page}
.endfor

afterinstall:
	@test -d ${DESTDIR}/usr/include/pcap || \
	    mkdir ${DESTDIR}/usr/include/pcap
	@for i in ${PCAP_DISTDIR}/pcap/*; do \
	    j="cmp -s $$i ${DESTDIR}/usr/include/pcap/`basename $$i` || \
	    ${INSTALL} ${INSTALL_COPY} ${INC_INSTALL_OWN} -m 444 $$i\
	        ${DESTDIR}/usr/include/pcap"; \
	    echo $$j; \
	    eval "$$j"; \
	done;


.include <bsd.lib.mk>
