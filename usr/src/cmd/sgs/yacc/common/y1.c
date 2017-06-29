/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/* Copyright (c) 1988 AT&T */
/* All Rights Reserved */

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include "dextern.h"
#include <sys/param.h>
#include <sys/errno.h>
#include <unistd.h>
#include <locale.h>
#include <stdarg.h>	/* For error() */

static void mktbls(void);
static void others(void);
static void summary(void);
static wchar_t *chcopy(wchar_t *, wchar_t *);
static int setunion(int *, int *);
static void prlook(LOOKSETS *);
static void cpres(void);
static void cpfir(void);
static void cempty(void);
static void stagen(void);
static LOOKSETS *flset(LOOKSETS *);
static void exp_lkst(void);
static void exp_wsets(void);
static void exp_states(void);
static void exp_psmem(void);

	/* lookahead computations */

int TBITSET;
static int tbitset;	/* size of lookahead sets */
LOOKSETS *lkst;
static int lsetsize;

static int nlset = 0; 	/* next lookahead set index */
int nolook = 0; 	/* flag to suppress lookahead computations */
static LOOKSETS clset;  /* temporary storage for lookahead computations */

static ITEM *psmem, *zzmemsz;
static int new_pstsize = PSTSIZE;

	/* working set computations */

WSET *wsets;
int cwp;
static int wsetsz = 0;		/* number of WSET items in wsets block */

	/* state information */

int nstate = 0;			/* number of states */
static int nstatesz = NSTATES;	/* number of state space allocated	*/
ITEM **pstate;			/* ptr to descriptions of the states	*/
int *tystate;			/* contains type info about the states	*/
int *indgo;			/* index to the stored goto table	*/
static int *tmp_lset;
static int *tstates;		/* states generated by terminal gotos	*/
static int *ntstates;		/* states generated by non-term gotos	*/
static int *mstates;		/* chain of overflows of term/nonterm	*/
				/* generation lists			*/

	/* storage for the actions in the parser */

int *amem, *memp;		/* next free action table position */
int new_actsize = ACTSIZE;

	/* other storage areas */

int *temp1;		/* temp storate, indexed by terms+ntokens or states */
int lineno = 0;		/* current input line number */
int size;
static int fatfl = 1;  	/* if on, error is fatal */
static int nerrors = 0;	/* number of errors */

	/* storage for information about the nonterminals */

static int ***pres;		/* vector of pointers to productions	*/
				/* yielding each nonterminal		*/
static LOOKSETS **pfirst;	/* vector of pointers to first sets for	*/
				/* each nonterminal			*/
static int *pempty;		/* vector of nonterminals nontrivially	*/
				/* deriving e				*/
extern int nprodsz;

int
main(int argc, char *argv[])
{
	(void) setlocale(LC_ALL, "");
#if !defined(TEXT_DOMAIN)	/* Should be defined by cc -D */
#define	TEXT_DOMAIN "SYS_TEST"	/* Use this only if it weren't */
#endif
	(void) textdomain(TEXT_DOMAIN);

	setup(argc, argv); 		/* initialize and read productions */
	TBITSET = NWORDS(ntoksz*LKFACTOR);
	tbitset = NWORDS(ntokens*LKFACTOR);
	mktbls();
	cpres(); 	/* make table of which productions yield a */
			/* given nonterminal */
	cempty(); 	/* make a table of which nonterminals can match	*/
			/* the empty string */
	cpfir(); 	/* make a table of firsts of nonterminals */
	stagen();	/* generate the states 	*/
	output();  	/* write the states and the tables */
	go2out();
	hideprod();
	summary();
	callopt();
	others();
	return (0);
}


static void
mktbls()
{
	int i;

	size = ntoksz + nnontersz +1;
	if (size < nstatesz)
		size = nstatesz;
	if (size < new_memsize)
		size = new_memsize;

	amem = (int *) malloc(sizeof (int) * new_actsize);
	psmem = (ITEM *) malloc(sizeof (ITEM) * new_pstsize);
	if ((psmem == NULL) || (amem == NULL))
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	This error happens when yacc could not allocate
 *	initial memory to be used for internal tables.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"couldn't allocate initial table"));
	zzmemsz = psmem;
	memp = amem;

	/*
	 * For lkst
	 */
#define	INIT_LSIZE	nnontersz*LKFACTOR
	tmp_lset = (int *)
	    calloc((size_t)(TBITSET * (INIT_LSIZE+1)), sizeof (int));
	if (tmp_lset == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Yacc could not allocate memory for table named lookset.
 *	Do not translate 'lookset'.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"could not allocate lookset array"));
	lkst = (LOOKSETS *) malloc(sizeof (LOOKSETS) * (INIT_LSIZE + 1));
	for (i = 0; i <= INIT_LSIZE; ++i)
		lkst[i].lset = tmp_lset + TBITSET * i;
	tmp_lset = NULL;

	/*
	 * For wsets
	 */
	tmp_lset = (int *)
	    calloc((size_t)(TBITSET * (nnontersz+1)), sizeof (int));
	if (tmp_lset == NULL)
		error(gettext(
		"could not allocate lookset array"));
	wsets = (WSET *) malloc(sizeof (WSET) * (nnontersz + 1));
	for (i = 0; i <= nnontersz; ++i)
		wsets[i].ws.lset = tmp_lset + TBITSET * i;
	tmp_lset = NULL;

	clset.lset = (int *)malloc(sizeof (int)*TBITSET);
	tstates = (int *)malloc(sizeof (int)*(ntoksz + 1));
	ntstates = (int *)malloc(sizeof (int)*(nnontersz + 1));
	temp1 = (int *)malloc(sizeof (int)*size);
	pres = (int ***)malloc(sizeof (int **)*(nnontersz + 2));
	pfirst = (LOOKSETS **)malloc(sizeof (LOOKSETS *)*(nnontersz + 2));
	pempty = (int *)malloc(sizeof (int)*(nnontersz + 1));

	pstate = (ITEM **)malloc(sizeof (ITEM *)*(nstatesz+2));
	tystate = (int *)malloc(sizeof (int)*nstatesz);
	indgo = (int *)malloc(sizeof (int)*nstatesz);
	mstates = (int *)malloc(sizeof (int)*nstatesz);
	defact = (int *)malloc(sizeof (int)*nstatesz);

	if ((lkst == NULL) || (wsets == NULL) || (tstates == NULL) ||
	    (ntstates == NULL) || (temp1 == NULL) || (pres == NULL) ||
	    (pfirst == NULL) || (pempty == NULL) || (pstate == NULL) ||
	    (tystate == NULL) || (indgo == NULL) || (mstates == NULL) ||
	    (defact == NULL) || (clset.lset == NULL))
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Do not translate mktbls(). It is a function name.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
			error(gettext(
			"cannot allocate tables in mktbls()"));

	aryfil(ntstates, nnontersz+1, 0);
	aryfil(tstates, ntoksz+1, 0);
	wsetsz = nnontersz + 1;
	lsetsize = INIT_LSIZE + 1;
}

/* put out other arrays, copy the parsers */
static void
others()
{
	extern int gen_lines;
	int c, i, j;
	int tmpline;

	finput = fopen(parser, "r");
	if (finput == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	This error message is issued when yacc can not find
 *	the parser to be copied.
 */
		error(gettext(
		"cannot find parser %s"),
		    parser);

	warray(L"yyr1", levprd, nprod);

	aryfil(temp1, nprod, 0);
					/* had_act[i] is either 1 or 0 */
	PLOOP(1, i)
		temp1[i] = ((prdptr[i+1] - prdptr[i]-2) << 1) | had_act[i];
	warray(L"yyr2", temp1, nprod);

	aryfil(temp1, nstate, -10000000);
	TLOOP(i)
		for (j = tstates[i]; j != 0; j = mstates[j])
			temp1[j] = tokset[i].value;
	NTLOOP(i)
		for (j = ntstates[i]; j != 0; j = mstates[j])
			temp1[j] = -i;
	warray(L"yychk", temp1, nstate);

	warray(L"yydef", defact, nstate);

	if ((fdebug = fopen(DEBUGNAME, "r")) == NULL)
		error("cannot open yacc.debug");
	while ((c = getwc(fdebug)) != EOF)
		(void) putwc(c, ftable);
	(void) fclose(fdebug);
	ZAPFILE(DEBUGNAME);

	if (gen_lines)
		(void) fprintf(ftable, "# line\t1 \"%s\"\n", parser);
	tmpline = 1;
	/* copy parser text */
	while ((c = getwc(finput)) != EOF) {
		if (c == '\n')
			tmpline++;
		if (c == L'$') {
			if ((c = getwc(finput)) != L'A')
				(void) putwc(L'$', ftable);
			else { /* copy actions */
				tmpline++;
				faction = fopen(ACTNAME, "r");
				if (faction == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	This error is issued when yacc can not open a
 *	temporary file to be used. You do not need to
 *	use the word 'tempfile'. You can translate it to
 *	mean 'temporary file'.
 */
					error(gettext(
					"cannot open action tempfile"));
				while ((c = getwc(faction)) != EOF)
					(void) putwc(c, ftable);
				(void) fclose(faction);
				if (gen_lines)
					(void) fprintf(ftable,
					    "\n# line\t%d \"%s\"",
					    tmpline,
					    parser);
				ZAPFILE(ACTNAME);
				c = getwc(finput);
			}
		}
		(void) putwc(c, ftable);
	}
	(void) fclose(ftable);
}


/* copies string q into p, returning next free char ptr */
static wchar_t *
chcopy(p, q)
wchar_t *p, *q;
{
	while (*p = *q++)
		++p;
	return (p);
}

#define	ISIZE 400
/* creates output string for item pointed to by pp */
wchar_t *
writem(pp)
int *pp;
{
	int i, *p;
	static int isize = ISIZE;
	static wchar_t *sarr = NULL;
	wchar_t *q;

	if (sarr == NULL) {
		sarr = (wchar_t *)malloc(sizeof (wchar_t) * isize);
		if (sarr == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	This error is issued when yacc could not allocate
 *	memory for internally used array.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
			error(gettext(
			"could not allocate output string array"));
		for (i = 0; i < isize; ++i)
			sarr[i] = L' ';
	}
	for (p = pp; *p > 0; ++p) /* NULL */;
	p = prdptr[-*p];
	q = chcopy(sarr, nontrst[*p-NTBASE].name);
	q = chcopy(q, L" : ");

	for (;;) {
		*q++ = ++p == pp ? L'_' : L' ';
		*q = 0;
		if ((i = *p) <= 0)
			break;
		q = chcopy(q, symnam(i));
		while (q > &sarr[isize-30]) {
			static wchar_t *sarrbase;

			sarrbase = sarr;
			isize += ISIZE;
			sarr = (wchar_t *)
				realloc((char *)sarr, sizeof (*sarr) * isize);
			if (sarr == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	This error is issued when yacc could not allocate
 *	memory for internally used array.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
				error(gettext(
				"cannot expand sarr arrays"));
			q = q - sarrbase + sarr;
		}
	}

	/* an item calling for a reduction */
	if ((i = *pp) < 0) {
		q = chcopy(q, L"    (");
		(void) wsprintf(q, "%d)", -i);
	}
	return (sarr);
}

/* return a pointer to the name of symbol i */
wchar_t *
symnam(int i)
{
	wchar_t *cp;

	cp = (i >= NTBASE) ? nontrst[i-NTBASE].name : tokset[i].name;
	if (*cp == L' ')
		++cp;
	return (cp);
}

static int zzcwp = 0;
static int zzclose = 0;
int zzgoent = 0;
int zzgobest = 0;
int zzacent = 0;
int zzexcp = 0;
int zzsrconf = 0;
int zzrrconf = 0;

/* output the summary on the tty */
static void
summary()
{
	if (foutput != NULL) {
		(void) fprintf(foutput,
		    "\n%d/%d terminals, %d/%d nonterminals\n",
		    ntokens, ntoksz, nnonter, nnontersz);
		(void) fprintf(foutput,
		    "%d/%d grammar rules, %d/%d states\n",
		    nprod, nprodsz, nstate, nstatesz);
		(void) fprintf(foutput,
		"%d shift/reduce, %d reduce/reduce conflicts reported\n",
		    zzsrconf, zzrrconf);
		(void) fprintf(foutput,
		    "%d/%d working sets used\n", zzcwp, wsetsz);
		(void) fprintf(foutput,
		    "memory: states,etc. %" PRIdPTR
		    "/%d, parser %" PRIdPTR "/%d\n",
		    mem-tracemem, new_memsize,
		    memp-amem, new_actsize);
		(void) fprintf(foutput,
		    "%d/%d distinct lookahead sets\n", nlset, lsetsize);
		(void) fprintf(foutput,
		    "%d extra closures\n", zzclose - 2*nstate);
		(void) fprintf(foutput,
		    "%d shift entries, %d exceptions\n", zzacent, zzexcp);
		(void) fprintf(foutput,
		    "%d goto entries\n", zzgoent);
		(void) fprintf(foutput,
		    "%d entries saved by goto default\n", zzgobest);
	}
	if (zzsrconf != 0 || zzrrconf != 0) {
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	You may just leave this message un-translated.
 *	This message only makes sense to those who knows
 *	how yacc works, and the person should know what
 *	this message means in English.
 */
		(void) fprintf(stderr, gettext(
		    "\nconflicts: "));
		if (zzsrconf)
			(void) fprintf(stderr, "%d shift/reduce", zzsrconf);
		if (zzsrconf && zzrrconf)
			(void) fprintf(stderr, ", ");
		if (zzrrconf)
			(void) fprintf(stderr, "%d reduce/reduce", zzrrconf);
		(void) fprintf(stderr, "\n");
	}

	if (ftemp != NULL)
		(void) fclose(ftemp);
	if (fdefine != NULL)
		(void) fclose(fdefine);
}

/* write out error comment */
/*PRINTFLIKE1*/
void
error(char *s, ...)
{
	extern char *infile;
	va_list	ap;

	va_start(ap, s);

	++nerrors;
	if (!lineno)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is a prefix to the error messages
 *	passed to error() function.
 */
		(void) fprintf(stderr, gettext(
		    "command line: fatal: "));
	else {
		(void) fprintf(stderr, "\"%s\", ", infile);
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is a prefix to the error messages
 *	passed to error() function.
 */
		(void) fprintf(stderr, gettext(
		    "line %d: fatal: "),
		    lineno);
	}
	(void) vfprintf(stderr, s, ap);
	(void) fprintf(stderr, "\n");
	va_end(ap);
	if (!fatfl)
		return;
	summary();
	exit(1);
}

/*
 * Print out a warning message.
 */
/*PRINTFLIKE2*/
void
warning(int flag, char *s, ...)
{
	extern char *infile;
	va_list	ap;
	va_start(ap, s);

	(void) fprintf(stderr, "\"%s\", ", infile);
	/*
	 * If flag, print lineno as well.
	 */
	if (flag == 0)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is a prefix to the warning messages
 *	passed to warning() function.
 */
		(void) fprintf(stderr, gettext(
		    "warning: "));
	else
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is a prefix to the warning messages
 *	passed to warning() function.
 */
		(void) fprintf(stderr, gettext(
		    "line %d: warning: "),
		    lineno);
	(void) vfprintf(stderr, s, ap);
	(void) fprintf(stderr, "\n");
	va_end(ap);
}

/* set elements 0 through n-1 to c */
void
aryfil(v, n, c)
int *v, n, c;
{
	int i;
	for (i = 0; i < n; ++i)
		v[i] = c;
}

/* set a to the union of a and b */
/* return 1 if b is not a subset of a, 0 otherwise */
static int
setunion(a, b)
int *a, *b;
{
	int i, x, sub;

	sub = 0;
	SETLOOP(i) {
		*a = (x = *a) | *b++;
		if (*a++ != x)
			sub = 1;
	}
	return (sub);
}

static void
prlook(p)
LOOKSETS *p;
{
	int j, *pp;
	pp = p->lset;
	if (pp == 0)
		(void) fprintf(foutput, "\tNULL");
	else {
		(void) fprintf(foutput, " { ");
		TLOOP(j) {
			if (BIT(pp, j))
				(void) fprintf(foutput,  WSFMT("%ws "),
				    symnam(j));
		}
		(void) fprintf(foutput,  "}");
	}
}

/*
 * compute an array with the beginnings of  productions yielding
 * given nonterminals
 * The array pres points to these lists
 * the array pyield has the lists: the total size is only NPROD+1
 */
static void
cpres()
{
	int **ptrpy;
	int **pyield;
	int c, j, i;

	/*
	 * 2/29/88 -
	 * nprodsz is the size of the tables describing the productions.
	 * Normally this will be NPROD unless the production tables have
	 * been expanded, in which case the tables will be NPROD * N(where
	 * N is the number of times the tables had to be expanded.)
	 */
	if ((pyield = (int **) malloc(sizeof (int *) * nprodsz)) == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	This error is issued when yacc could not allocate
 *	memory for internally used array.
 *
 *	pyield is name of an array. You should not try to translate
 *	this word.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"cannot allocate space for pyield array"));

	ptrpy = pyield;

	NTLOOP(i) {
		c = i+NTBASE;
		pres[i] = ptrpy;
		fatfl = 0;  /* make undefined  symbols  nonfatal */
		PLOOP(0, j) {
			if (*prdptr[j] == c)  	/* linear search for all c's */
				*ptrpy++ =  prdptr[j] + 1;
		}
		if (pres[i] == ptrpy) { 		/* c not found */
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Ask somebody who knows yacc how to translate nonterminal or
 *	look at translated yacc document.
 */
			error(gettext(
			"undefined nonterminal: %ws"),
			    nontrst[i].name);
		}
	}
	pres[i] = ptrpy;
	fatfl = 1;
	if (nerrors) {
		summary();
		exit(1);
	}
	if (ptrpy != &pyield[nprod])
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	This is an internal error message.
 *	Very little use to user. You may leave it
 *	un-translated.
 *
 *	pyied is name of an array. Do not translate it.
 */
		error(gettext(
		"internal Yacc error: pyield %d"),
		    ptrpy-&pyield[nprod]);
}

static int indebug = 0;
/* compute an array with the first of nonterminals */
static void
cpfir()
{
	int *p, **s, i, **t, ch, changes;

	zzcwp = nnonter;
	NTLOOP(i) {
		aryfil(wsets[i].ws.lset, tbitset, 0);
		t = pres[i+1];
		/* initially fill the sets */
		for (s = pres[i]; s < t; ++s) {
			/* check if ch is non-terminal */
			for (p = *s; (ch = *p) > 0; ++p) {
				if (ch < NTBASE) { 	/* should be token */
					SETBIT(wsets[i].ws.lset, ch);
					break;
				} else if (!pempty[ch-NTBASE])
					break;
			}
		}
	}

	/* now, reflect transitivity */

	changes = 1;
	while (changes) {
		changes = 0;
		NTLOOP(i) {
			t = pres[i+1];
			for (s = pres[i]; s < t; ++s) {
				for (p = *s; (ch = (*p-NTBASE)) >= 0; ++p) {
					changes |= setunion(wsets[i].ws.lset,
					    wsets[ch].ws.lset);
					if (!pempty[ch])
						break;
				}
			}
		}
	}

	NTLOOP(i)
		pfirst[i] = flset(&wsets[i].ws);
	if (!indebug)
		return;
	if ((foutput != NULL)) {
		NTLOOP(i) {
			(void) fprintf(foutput, WSFMT("\n%ws: "),
			    nontrst[i].name);
			prlook(pfirst[i]);
			(void) fprintf(foutput, " %d\n", pempty[i]);
		}
	}
}

/* sorts last state,and sees if it equals earlier ones. returns state number */
int
state(int c)
{
	int size1, size2;
	int i;
	ITEM *p1, *p2, *k, *l, *q1, *q2;
	p1 = pstate[nstate];
	p2 = pstate[nstate+1];
	if (p1 == p2)
		return (0); /* null state */
	/* sort the items */
	for (k = p2 - 1; k > p1; k--) {	/* make k the biggest */
		for (l = k-1; l >= p1; --l)
			if (l->pitem > k->pitem) {
				int *s;
				LOOKSETS *ss;
				s = k->pitem;
				k->pitem = l->pitem;
				l->pitem = s;
				ss = k->look;
				k->look = l->look;
				l->look = ss;
			}
	}
	size1 = p2 - p1; /* size of state */

	for (i = (c >= NTBASE) ? ntstates[c-NTBASE] : tstates[c];
	    i != 0; i = mstates[i]) {
		/* get ith state */
		q1 = pstate[i];
		q2 = pstate[i+1];
		size2 = q2 - q1;
		if (size1 != size2)
			continue;
		k = p1;
		for (l = q1; l < q2; l++) {
			if (l->pitem != k->pitem)
				break;
			++k;
		}
		if (l != q2)
			continue;
		/* found it */
		pstate[nstate+1] = pstate[nstate]; /* delete last state */
		/* fix up lookaheads */
		if (nolook)
			return (i);
		for (l = q1, k = p1; l < q2; ++l, ++k) {
			int s;
			SETLOOP(s)
				clset.lset[s] = l->look->lset[s];
			if (setunion(clset.lset, k->look->lset)) {
				tystate[i] = MUSTDO;
				/* register the new set */
				l->look = flset(&clset);
			}
		}
		return (i);
	}
	/* state is new */
	if (nolook)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	You may leave this untranslated. Leave
 *	state/nolook un-translated.
 */
		error(gettext(
		"yacc state/nolook error"));
	pstate[nstate+2] = p2;
	if (nstate+1 >= nstatesz)
		exp_states();
	if (c >= NTBASE) {
		mstates[nstate] = ntstates[c - NTBASE];
		ntstates[c - NTBASE] = nstate;
	} else {
		mstates[nstate] = tstates[c];
		tstates[c] = nstate;
	}
	tystate[nstate] = MUSTDO;
	return (nstate++);
}

static int pidebug = 0;

void
putitem(ptr, lptr)
int *ptr;
LOOKSETS *lptr;
{
	register ITEM *j;

	if (pidebug && (foutput != NULL))
		(void) fprintf(foutput,
			WSFMT("putitem(%ws), state %d\n"), writem(ptr), nstate);
	j = pstate[nstate+1];
	j->pitem = ptr;
	if (!nolook)
		j->look = flset(lptr);
	pstate[nstate+1] = ++j;
	if (j > zzmemsz) {
		zzmemsz = j;
		if (zzmemsz >=  &psmem[new_pstsize])
			exp_psmem();
			/* error("out of state space"); */
	}
}

/*
 * mark nonterminals which derive the empty string
 * also, look for nonterminals which don't derive any token strings
 */
static void
cempty()
{
#define	EMPTY 1
#define	WHOKNOWS 0
#define	OK 1
	int i, *p;

	/*
	 * first, use the array pempty to detect productions
	 * that can never be reduced
	 */

	/* set pempty to WHONOWS */
	aryfil(pempty, nnonter+1, WHOKNOWS);

	/*
	 * now, look at productions, marking nonterminals which
	 * derive something
	 */
	more:
	PLOOP(0, i) {
		if (pempty[*prdptr[i] - NTBASE])
			continue;
		for (p = prdptr[i] + 1; *p >= 0; ++p)
			if (*p >= NTBASE && pempty[*p-NTBASE] == WHOKNOWS)
				break;
		if (*p < 0) { /* production can be derived */
			pempty[*prdptr[i]-NTBASE] = OK;
			goto more;
		}
	}

	/* now, look at the nonterminals, to see if they are all OK */

	NTLOOP(i) {
		/*
		 * the added production rises or falls as the
		 * start symbol ...
		 */
		if (i == 0)
			continue;
		if (pempty[i] != OK) {
			fatfl = 0;
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Ask somebody who knows yacc how to translate nonterminal or
 *	look at translated yacc document. Check how 'derive' is
 *	translated in these documents also.
 */
			error(gettext(
			"nonterminal %ws never derives any token string"),
			    nontrst[i].name);
		}
	}

	if (nerrors) {
		summary();
		exit(1);
	}

	/*
	 * now, compute the pempty array, to see which nonterminals
	 * derive the empty string
	 */

	/* set pempty to WHOKNOWS */

	aryfil(pempty, nnonter+1, WHOKNOWS);

	/* loop as long as we keep finding empty nonterminals */

again:
	PLOOP(1, i) {
		/* not known to be empty */
		if (pempty[*prdptr[i]-NTBASE] == WHOKNOWS) {
			for (p = prdptr[i]+1;
			    *p >= NTBASE && pempty[*p-NTBASE] == EMPTY; ++p)
				;
			/* we have a nontrivially empty nonterminal */
			if (*p < 0) {
				pempty[*prdptr[i]-NTBASE] = EMPTY;
				goto again; /* got one ... try for another */
			}
		}
	}
}

/* generate the states */
static int gsdebug = 0;
static void
stagen()
{
	int i, j;
	int c;
	register WSET *p, *q;

	/* initialize */

	nstate = 0;

	pstate[0] = pstate[1] = psmem;
	aryfil(clset.lset, tbitset, 0);
	putitem(prdptr[0] + 1, &clset);
	tystate[0] = MUSTDO;
	nstate = 1;
	pstate[2] = pstate[1];

	aryfil(amem, new_actsize, 0);

	/* now, the main state generation loop */

	more:
	SLOOP(i) {
		if (tystate[i] != MUSTDO)
			continue;
		tystate[i] = DONE;
		aryfil(temp1, nnonter + 1, 0);
		/* take state i, close it, and do gotos */
		closure(i);
		WSLOOP(wsets, p) { /* generate goto's */
			if (p->flag)
				continue;
			p->flag = 1;
			c = *(p->pitem);
			if (c <= 1) {
				if (pstate[i+1]-pstate[i] <= p-wsets)
					tystate[i] = MUSTLOOKAHEAD;
				continue;
			}
			/* do a goto on c */
			WSLOOP(p, q) {
				/* this item contributes to the goto */
				if (c == *(q->pitem)) {
					putitem(q->pitem + 1, &q->ws);
					q->flag = 1;
				}
			}
			if (c < NTBASE)
				(void) state(c);  /* register new state */
			else temp1[c-NTBASE] = state(c);
		}
		if (gsdebug && (foutput != NULL)) {
			(void) fprintf(foutput,  "%d: ", i);
			NTLOOP(j) {
				if (temp1[j])
					(void) fprintf(foutput,
					    WSFMT("%ws %d, "), nontrst[j].name,
					    temp1[j]);
			}
			(void) fprintf(foutput, "\n");
		}
		indgo[i] = apack(&temp1[1], nnonter - 1) - 1;
		goto more; /* we have done one goto; do some more */
	}
	/* no more to do... stop */
}

/* generate the closure of state i */
static int cldebug = 0; /* debugging flag for closure */

void
closure(int i)
{
	int c, ch, work, k;
	register WSET *u, *v;
	int *pi;
	int **s, **t;
	ITEM *q;
	register ITEM *p;
	int idx1 = 0;

	++zzclose;

	/* first, copy kernel of state i to wsets */
	cwp = 0;
	ITMLOOP(i, p, q) {
		wsets[cwp].pitem = p->pitem;
		wsets[cwp].flag = 1;    /* this item must get closed */
		SETLOOP(k)
			wsets[cwp].ws.lset[k] = p->look->lset[k];
		WSBUMP(cwp);
	}

	/* now, go through the loop, closing each item */

	work = 1;
	while (work) {
		work = 0;
		/*
		 * WSLOOP(wsets, u) {
		 */
		for (idx1 = 0; idx1 < cwp; idx1++) {
			u = &wsets[idx1];
			if (u->flag == 0)
				continue;
			c = *(u->pitem);  /* dot is before c */
			if (c < NTBASE) {
				u->flag = 0;
				/*
				 * only interesting case is where . is
				 * before nonterminal
				 */
				continue;
			}

			/* compute the lookahead */
			aryfil(clset.lset, tbitset, 0);

			/* find items involving c */

			WSLOOP(u, v) {
				if (v->flag == 1 && *(pi = v->pitem) == c) {
					v->flag = 0;
					if (nolook)
						continue;
					while ((ch = *++pi) > 0) {
						/* terminal symbol */
						if (ch < NTBASE) {
							SETBIT(clset.lset, ch);
							break;
						}
						/* nonterminal symbol */
						(void) setunion(clset.lset,
						    pfirst[ch-NTBASE]->lset);
						if (!pempty[ch-NTBASE])
							break;
					}
					if (ch <= 0)
						(void) setunion(clset.lset,
						    v->ws.lset);
				}
			}

			/*  now loop over productions derived from c */

			c -= NTBASE; /* c is now nonterminal number */

			t = pres[c+1];
			for (s = pres[c]; s < t; ++s) {
				/* put these items into the closure */
				WSLOOP(wsets, v) { /* is the item there */
					/* yes, it is there */
					if (v->pitem == *s) {
						if (nolook)
							goto nexts;
						if (setunion(v->ws.lset,
						    clset.lset))
							v->flag = work = 1;
						goto nexts;
					}
				}

				/*  not there; make a new entry */
				if (cwp + 1 >= wsetsz)
					exp_wsets();

				wsets[cwp].pitem = *s;
				wsets[cwp].flag = 1;
				if (!nolook) {
					work = 1;
					SETLOOP(k)
						wsets[cwp].ws.lset[k] =
						    clset.lset[k];
				}
				WSBUMP(cwp);
				nexts:;
			}
		}
	}

	/* have computed closure; flags are reset; return */

	if (&wsets[cwp] > &wsets[zzcwp])
		zzcwp = cwp;
	if (cldebug && (foutput != NULL)) {
		(void) fprintf(foutput, "\nState %d, nolook = %d\n", i, nolook);
		WSLOOP(wsets, u) {
			if (u->flag)
				(void) fprintf(foutput, "flag set!\n");
			u->flag = 0;
			(void) fprintf(foutput, WSFMT("\t%ws"),
			    writem(u->pitem));
			prlook(&u->ws);
			(void) fprintf(foutput,  "\n");
		}
	}
}

static LOOKSETS *
flset(p)
LOOKSETS *p;
{
	/* decide if the lookahead set pointed to by p is known */
	/* return pointer to a perminent location for the set */

	int j, *w;
	int *u, *v;
	register LOOKSETS *q;

	for (q = &lkst[nlset]; q-- > lkst; ) {
		u = p->lset;
		v = q->lset;
		w = & v[tbitset];
		while (v < w)
			if (*u++ != *v++)
				goto more;
		/* we have matched */
		return (q);
		more:;
	}
	/* add a new one */
	q = &lkst[nlset++];
	if (nlset >= lsetsize) {
		exp_lkst();
		q = &lkst[nlset++];
	}
	SETLOOP(j) q->lset[j] = p->lset[j];
	return (q);
}

static void
exp_lkst()
{
	int i, j;
	static LOOKSETS *lookbase;

	lookbase = lkst;
	lsetsize += LSETSIZE;
	tmp_lset = (int *)
	    calloc((size_t)(TBITSET * (lsetsize-LSETSIZE)), sizeof (int));
	if (tmp_lset == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Memory allocation error. Do not translate lookset.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"could not expand lookset array"));
	lkst = reallocarray(lkst, lsetsize, sizeof (LOOKSETS));
	for (i = lsetsize-LSETSIZE, j = 0; i < lsetsize; ++i, ++j)
		lkst[i].lset = tmp_lset + TBITSET * j;
	tmp_lset = NULL;
	if (lkst == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Memory allocation error. Do not translate lookset.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"could not expand lookahead sets"));
	for (i = 0; i <= nnonter; ++i)
		pfirst[i] = pfirst[i] - lookbase + lkst;
	for (i = 0; i <= nstate+1; ++i) {
		if (psmem[i].look)
			psmem[i].look = psmem[i].look - lookbase + lkst;
		if (pstate[i]->look)
			pstate[i]->look = pstate[i]->look - lookbase + lkst;
	}
}

static void
exp_wsets()
{
	int i, j;

	wsetsz += WSETSIZE;
	tmp_lset = (int *)
	    calloc((size_t)(TBITSET * (wsetsz-WSETSIZE)), sizeof (int));
	if (tmp_lset == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Memory allocation error. Do not translate lookset.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"could not expand lookset array"));
	wsets = reallocarray(wsets, wsetsz, sizeof (WSET));
	for (i = wsetsz-WSETSIZE, j = 0; i < wsetsz; ++i, ++j)
		wsets[i].ws.lset = tmp_lset + TBITSET * j;
	tmp_lset = NULL;
	if (wsets == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Memory allocation error. You may just transltate
 *	this as 'Could not allocate internally used memory.'
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"could not expand working sets"));
}

static void
exp_states()
{
	nstatesz += NSTATES;

	pstate = (ITEM **)
	    reallocarray(pstate, nstatesz + 2, sizeof (ITEM *));
	mstates = reallocarray(mstates, nstatesz, sizeof (int));
	defact = reallocarray(defact, nstatesz, sizeof (int));
	tystate = reallocarray(tystate, nstatesz, sizeof (int));
	indgo = reallocarray(indgo, nstatesz, sizeof (int));

	if ((*pstate == NULL) || (tystate == NULL) || (defact == NULL) ||
	    (indgo == NULL) || (mstates == NULL))
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Memory allocation error.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"cannot expand table of states"));
}

static void
exp_psmem()
{
	int i;

	new_pstsize += PSTSIZE;
	psmem = reallocarray(psmem, new_pstsize, sizeof (ITEM));
	if (psmem == NULL)
/*
 * TRANSLATION_NOTE  -- This is a message from yacc.
 *	This message is passed to error() function.
 *	Memory allocation error.
 *
 *	You may just translate this as:
 *	'Could not allocate internally used memory.'
 */
		error(gettext(
		"cannot expand pstate memory"));

	zzmemsz = zzmemsz - pstate[0] + psmem;
	for (i = 1; i <= nstate+1; ++i)
		pstate[i] = pstate[i] - pstate[0] + psmem;
	pstate[0] = psmem;
}
