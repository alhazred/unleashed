MAN = bsdmalloc.3malloc \
      malloc.3malloc \
      mapmalloc.3malloc \
      mtmalloc.3malloc \
      umem_alloc.3malloc \
      umem_cache_create.3malloc \
      umem_debug.3malloc \
      watchmalloc.3malloc

MLINKS+=	malloc.3malloc calloc.3malloc
MLINKS+=	malloc.3malloc free.3malloc
MLINKS+=	malloc.3malloc mallinfo.3malloc
MLINKS+=	malloc.3malloc mallopt.3malloc
MLINKS+=	malloc.3malloc memalign.3malloc
MLINKS+=	malloc.3malloc realloc.3malloc
MLINKS+=	malloc.3malloc valloc.3malloc
MLINKS+=	mtmalloc.3malloc mallocctl.3malloc
MLINKS+=	umem_alloc.3malloc umem_free.3malloc
MLINKS+=	umem_alloc.3malloc umem_nofail_callback.3malloc
MLINKS+=	umem_alloc.3malloc umem_zalloc.3malloc
MLINKS+=	umem_cache_create.3malloc umem_cache_alloc.3malloc
MLINKS+=	umem_cache_create.3malloc umem_cache_destroy.3malloc
MLINKS+=	umem_cache_create.3malloc umem_cache_free.3malloc

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
