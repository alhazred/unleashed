/*
 * Copyright (c) 2015-2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

%pure-parser
%lex-param {void *scanner}
%parse-param {struct sexpr_parser_state *data}

%{
#include <jeffpc/error.h>

#include "sexpr_impl.h"

#define scanner data->scanner

extern int sexpr_reader_lex(void *, void *);

void yyerror(void *scan, char *e)
{
	cmn_err(CE_ERROR, "Error: %s", e);
}

void sexpr_error2(char *e, char *yytext)
{
	cmn_err(CE_ERROR, "Error: %s (%s)", e, yytext);
}

%}

%union {
	struct str *s;
	uint64_t i;
	bool b;
	struct val *lv;
};

%token <s> SYMBOL STRING
%token <i> NUMBER
%token <b> BOOL

%type <lv> document tok list toklist

%%
document : tok			{ data->output = $1; }
	 ;

tok : SYMBOL			{ $$ = VAL_ALLOC_SYM($1); }
    | STRING			{ $$ = VAL_ALLOC_STR($1); }
    | NUMBER			{ $$ = VAL_ALLOC_INT($1); }
    | BOOL			{ $$ = VAL_ALLOC_BOOL($1); }
    | list			{ $$ = $1; }
    | '\'' tok			{ $$ = VAL_ALLOC_CONS(
				         VAL_ALLOC_SYM_CSTR("quote"),
				         VAL_ALLOC_CONS($2, NULL)); }
    ;

/*
 * We are not left-recursive because we want to make cons cells right
 * recursively.
 */
toklist : tok toklist		{ $$ = VAL_ALLOC_CONS($1, $2); }
	| tok			{ $$ = VAL_ALLOC_CONS($1, NULL); }
	;

list : '(' ')'			{ $$ = NULL; }
     | '(' toklist ')'		{ $$ = $2; }
     | '(' tok '.' tok ')'	{ $$ = VAL_ALLOC_CONS($2, $4); }
     ;

%%
