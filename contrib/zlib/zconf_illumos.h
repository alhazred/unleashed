#ifndef	ZCONF_ILLUMOS_H
#define	ZCONF_ILLUMOS_H

/*
 * Disable debugging in zlib code.
 */
#undef DEBUG

/*
 * We define our own memory allocation and deallocation routines that use kmem.
 */
#define	MY_ZCALLOC

/*
 * Don't define HAVE_MEMCPY because we implement our own versions of
 * zmemcpy(), zmemzero(), and zmemcmp().
 */

/*
 * We have a sufficiently capable compiler as to not need zlib's compiler hack.
 */
#define	NO_DUMMY_DECL

/*
 * Enable "solo" mode since we don't want to pull in userspace headers.
 */
#define Z_SOLO

#define	compressBound(len)	(len + (len >> 12) + (len >> 14) + 11)

#if defined(_LP64) || _FILE_OFFSET_BITS == 32
#define	z_off_t	long
#elif _FILE_OFFSET_BITS == 64
#define	z_off_t	long long
#endif

#define	deflateInit_		z_deflateInit_
#define	deflate			z_deflate
#define	deflateEnd		z_deflateEnd
#define	inflateInit_		z_inflateInit_
#define	inflate			z_inflate
#define	inflateEnd		z_inflateEnd
#define	deflateInit2_		z_deflateInit2_
#define	deflateSetDictionary	z_deflateSetDictionary
#define	deflateCopy		z_deflateCopy
#define	deflateReset		z_deflateReset
#define	deflateParams		z_deflateParams
#define	deflateBound		z_deflateBound
#define	deflatePrime		z_deflatePrime
#define	inflateInit2_		z_inflateInit2_
#define	inflateSetDictionary	z_inflateSetDictionary
#define	inflateSync		z_inflateSync
#define	inflateSyncPoint	z_inflateSyncPoint
#define	inflateCopy		z_inflateCopy
#define	inflateReset		z_inflateReset
#define	inflateBack		z_inflateBack
#define	inflateBackEnd		z_inflateBackEnd
#define	compress		zz_compress
#define	compress2		zz_compress2
#define	uncompress		zz_uncompress
#define	adler32			z_adler32
#define	crc32			z_crc32
#define	get_crc_table		z_get_crc_table
#define	zError			z_zError

#ifdef	__cplusplus
}
#endif

#endif	/* _ZCONF_H */
