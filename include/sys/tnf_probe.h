/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 *	Copyright (c) 1994, by Sun Microsytems, Inc.
 */

#ifndef _SYS_TNF_PROBE_H
#define	_SYS_TNF_PROBE_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <sys/tnf_writer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define	TNF_STRINGIFY(x) #x
#define	TNF_STRINGVALUE(x) TNF_STRINGIFY(x)

/*
 * Alignment of tnf_ref32_t
 */

struct _tnf_ref32_align {
	char		c;
	tnf_ref32_t	t;
};
#define	TNF_REF32_ALIGN		TNF_OFFSETOF(struct _tnf_ref32_align, t)

/*
 * Probe versioning
 */

struct tnf_probe_version {
	size_t	version_size;		/* sizeof(struct tnf_probe_version) */
	size_t	probe_control_size;	/* sizeof(tnf_probe_control_t) */
};

extern struct tnf_probe_version __tnf_probe_version_1;
#pragma weak __tnf_probe_version_1

/*
 * Typedefs
 */

typedef struct tnf_probe_control tnf_probe_control_t;
typedef struct tnf_probe_setup tnf_probe_setup_t;

/* returns pointer to buffer */
typedef void * (*tnf_probe_test_func_t)(void *,
					tnf_probe_control_t *,
					tnf_probe_setup_t *);

/* returns buffer pointer */
typedef void * (*tnf_probe_alloc_func_t)(tnf_ops_t *,	/* tpd	*/
					tnf_probe_control_t *,
					tnf_probe_setup_t *);

typedef void (*tnf_probe_func_t)(tnf_probe_setup_t *);

/*
 * Probe argument block
 */

struct tnf_probe_setup {
	tnf_ops_t		*tpd_p;
	void			*buffer_p;
	tnf_probe_control_t	*probe_p;
};

/*
 * Probe control block
 */

struct tnf_probe_control {
	const struct tnf_probe_version	*version;
	tnf_probe_control_t	*next;
	tnf_probe_test_func_t	test_func;
	tnf_probe_alloc_func_t	alloc_func;
	tnf_probe_func_t	probe_func;
	tnf_probe_func_t	commit_func;
	uintptr_t		index;
	const char		*attrs;
	tnf_tag_data_t		***slot_types;
	unsigned long		tnf_event_size;
};

#ifdef _KERNEL

#define	TNF_NEXT_INIT	0

#else

#define	TNF_NEXT_INIT	-1

#endif	/* _KERNEL */

/*
 * TNF Type extension
 */

#define	TNF_DECLARE_RECORD(ctype, record)				\
	typedef tnf_reference_t record##_t

#define	TNF_DEFINE_RECORD_1(ctype, ctype_record, t1, n1)
#define	TNF_DEFINE_RECORD_2(ctype, ctype_record, t1, n1, t2, n2)
#define	TNF_DEFINE_RECORD_3(ctype, ctype_record, t1, n1, t2, n2, t3, n3)
#define	TNF_DEFINE_RECORD_4(ctype, ctype_record, t1, n1, t2, n2, t3, n3, t4, n4)
/* CSTYLED */
#define	TNF_DEFINE_RECORD_5(ctype, ctype_record, t1, n1, t2, n2, t3, n3, t4, n4, t5, n5)

/*
 * Probe Macros
 */

#define	TNF_PROBE_0(namearg, keysarg, detail) \
		((void)0)
#define	TNF_PROBE_1(namearg, keysarg, detail, type_1, namearg_1, valarg_1) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_2(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_3(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2, type_3, namearg_3, valarg_3) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_4(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2, type_3, namearg_3, valarg_3, type_4, namearg_4, valarg_4) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_5(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2, type_3, namearg_3, valarg_3, type_4, namearg_4, valarg_4, type_5, namearg_5, valarg_5) \
		((void)0)

/*
 * Debug Probe Macros (contain an additional "debug" attribute)
 */

#define	TNF_PROBE_0_DEBUG(namearg, keysarg, detail) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_1_DEBUG(namearg, keysarg, detail, type_1, namearg_1, valarg_1) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_2_DEBUG(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_3_DEBUG(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2, type_3, namearg_3, valarg_3) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_4_DEBUG(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2, type_3, namearg_3, valarg_3, type_4, namearg_4, valarg_4) \
		((void)0)
/* CSTYLED */
#define	TNF_PROBE_5_DEBUG(namearg, keysarg, detail, type_1, namearg_1, valarg_1, type_2, namearg_2, valarg_2, type_3, namearg_3, valarg_3, type_4, namearg_4, valarg_4, type_5, namearg_5, valarg_5) \
		((void)0)

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TNF_PROBE_H */
