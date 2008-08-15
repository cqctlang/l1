%{
#include "sys.h"
#include "util.h"
#include "l1.h"

extern int yylex();
extern char *yytext;

%}

%union{
	Expr *expr;
	struct {
		/* GLR may pick tokens from input stream more than
		   once; remembering length in char-based tokens
		   ensures that we get the same token each time,
		   regardless of state of yytext. */
		char *p;
		unsigned long len;
	} chars;
	int kind;
}

%token <chars> IDENTIFIER CONSTANT STRING_LITERAL 
%token SIZEOF TYPEOF TYPEDEF NIL DEFINE CONTAINEROF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token LOCAL LAMBDA NAMES
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token IF ELSE SWITCH WHILE DO FOR CONTINUE BREAK RETURN CASE DEFAULT

%type <expr> base base_list
%type <expr> declaration typedef specifier_list constant_expression
%type <expr> declarator_list primary_expression postfix_expression
%type <expr> argument_expression_list unary_expression cast_expression
%type <expr> multiplicative_expression additive_expression shift_expression
%type <expr> relational_expression equality_expression and_expression
%type <expr> exclusive_or_expression inclusive_or_expression maybe_expression
%type <expr> logical_and_expression logical_or_expression conditional_expression
%type <expr> assignment_expression lambda_expression expression root_expression
%type <expr> names_expression names_declaration_list names_declaration
%type <expr> identifier_list local_list local type_specifier
%type <expr> id tag tickid struct_or_union_specifier 
%type <expr> struct_declaration_list struct_declaration struct_size
%type <expr> struct_declarator_list struct_declarator enum_specifier
%type <expr> enumerator_list enumerator declarator direct_declarator pointer
%type <expr> parameter_type_list parameter_list parameter_declaration
%type <expr> abstract_declarator direct_abstract_declarator statement
%type <expr> compound_statement statement_list
%type <expr> expression_statement define_statement labeled_statement
%type <expr> selection_statement iteration_statement jump_statement
%type <expr> type_name tn_type_specifier tn_struct_or_union_specifier
%type <expr> tn_enum_specifier tn_parameter_type_list tn_parameter_list
%type <expr> tn_parameter_declaration tn_abstract_declarator
%type <expr> tn_direct_abstract_declarator tn_declarator tn_direct_declarator
%type <expr> tn_param_type_specifier tn_param_struct_or_union_specifier
%type <expr> tn_param_enum_specifier

%type <kind> unary_operator assignment_operator struct_or_union

%start translation_unit_seq
%glr-parser
%pure-parser
%parse-param {U *ctx}
%lex-param   {U *ctx}
%expect 1
%expect-rr 5
%{
	static void yyerror(U *ctx, const char *s);
	static Expr* castmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* mulmerge(YYSTYPE e1, YYSTYPE e2);
	static Expr* ofmerge(YYSTYPE e1, YYSTYPE e2);
%}
%%

id
	: IDENTIFIER
	{ $$ = doidn($1.p, $1.len); }

tickid
	: id '`' id
	{ $$ = dotick($1, $3); }
	;

tag:	id;

primary_expression
	: id
	| tickid
	| NIL
	{ $$ = newexpr(Enil, 0, 0, 0, 0); }
	| CONSTANT
	{ $$ = doconst(ctx, $1.p, $1.len); }
	| STRING_LITERAL
	{ $$ = doconsts($1.p, $1.len); }
	| '(' expression ')'
	{ $$ = $2; }
	| '[' ']'
	{ $$ = newexpr(Elist, nullelist(), 0, 0, 0); }
	| '[' argument_expression_list ']'
	{ $$ = newexpr(Elist, invert($2), 0, 0, 0); }
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	{ $$ = newexpr(Earef, $1, $3, 0, 0); }
	| postfix_expression '(' ')'
	{ $$ = newexpr(Ecall, $1, nullelist(), 0, 0); }
	| postfix_expression '(' argument_expression_list ')'
	  /* don't invert -- compiler evaluates arguments in reverse order */
	{ $$ = newexpr(Ecall, $1, $3, 0, 0); }
	| postfix_expression '.' id
	{ $$ = newexpr(Edot, $1, $3, 0, 0); }
	| postfix_expression PTR_OP id
	{ $$ = newexpr(Earrow, $1, $3, 0, 0); }
	| postfix_expression INC_OP
	{ $$ = newexpr(Epostinc, $1, 0, 0, 0); }
	| postfix_expression DEC_OP
	{ $$ = newexpr(Epostdec, $1, 0, 0, 0); }
	| CONTAINEROF '(' expression ',' type_name ',' id ')'
        { $$ = newexpr(Econtainer, $3, $5, $7, 0); }
	;

argument_expression_list
	: root_expression
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| argument_expression_list ',' root_expression
	{ $$ = newexpr(Eelist, $3, $1, 0, 0); }
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	{ $$ = newexpr(Epreinc, $2, 0, 0, 0); }
	| DEC_OP unary_expression
	{ $$ = newexpr(Epredec, $2, 0, 0, 0); }
	| unary_operator cast_expression
	{ $$ = newexpr($1, $2, 0, 0, 0); }
	| SIZEOF unary_expression			%merge <ofmerge>
	{ $$ = newexpr(Esizeofe, $2, 0, 0, 0); }
	| SIZEOF '(' type_name ')'			%merge <ofmerge>
	{ $$ = newexpr(Esizeoft, $3, 0, 0, 0); }
	| TYPEOF unary_expression			%merge <ofmerge>
	{ $$ = newexpr(Etypeofe, $2, 0, 0, 0); }
	| TYPEOF '(' type_name ')'			%merge <ofmerge>
	{ $$ = newexpr(Etypeoft, $3, 0, 0, 0); }
	;

unary_operator
	: '&'
	{ $$ = Eref; }
	| '*'
	{ $$ = Ederef; }
	| '+'
	{ $$ = Euplus; }
	| '-'
	{ $$ = Euminus; }
	| '~'
	{ $$ = Eutwiddle; }
	| '!'
	{ $$ = Eunot; }
	;

cast_expression
	: unary_expression				%merge <castmerge>
	| '(' type_name ')' cast_expression		%merge <castmerge>
	{ $$ = newexpr(Ecast, $2, $4, 0, 0); }
	| '{' expression '}' cast_expression
	{ $$ = newbinop(Excast, $2, $4); }
	;

multiplicative_expression
	: cast_expression				%merge <mulmerge>
	| multiplicative_expression '*' cast_expression	%merge <mulmerge>
	{ $$ = newbinop(Emul, $1, $3); }
	| multiplicative_expression '/' cast_expression
	{ $$ = newbinop(Ediv, $1, $3); }
	| multiplicative_expression '%' cast_expression
	{ $$ = newbinop(Emod, $1, $3); }
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	{ $$ = newbinop(Eadd, $1, $3); }
	| additive_expression '-' multiplicative_expression
	{ $$ = newbinop(Esub, $1, $3); }
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	{ $$ = newbinop(Eshl, $1, $3); }
	| shift_expression RIGHT_OP additive_expression
	{ $$ = newbinop(Eshr, $1, $3); }
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	{ $$ = newbinop(Elt, $1, $3); }
	| relational_expression '>' shift_expression
	{ $$ = newbinop(Egt, $1, $3); }
	| relational_expression LE_OP shift_expression
	{ $$ = newbinop(Ele, $1, $3); }
	| relational_expression GE_OP shift_expression
	{ $$ = newbinop(Ege, $1, $3); }
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	{ $$ = newbinop(Eeq, $1, $3); }
	| equality_expression NE_OP relational_expression
	{ $$ = newbinop(Eneq, $1, $3); }
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	{ $$ = newbinop(Eband, $1, $3); }
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	{ $$ = newbinop(Ebxor, $1, $3); }
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	{ $$ = newbinop(Ebor, $1, $3); }
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	{ $$ = newexpr(Eland, $1, $3, 0, 0); }
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	{ $$ = newexpr(Elor, $1, $3, 0, 0); }
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	{ $$ = newexpr(Econd, $1, $3, $5, 0); }
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator root_expression
	{ if($2 == Eg)
	  	$$ = newexpr($2, $1, $3, 0, 0);
	  else
	  	$$ = newgop($2, $1, $3);
	}
	;

assignment_operator
	: '='
	{ $$ = Eg; }
	| MUL_ASSIGN
	{ $$ = Emul; }
	| DIV_ASSIGN
	{ $$ = Ediv; }
	| MOD_ASSIGN
	{ $$ = Emod; }
	| ADD_ASSIGN
	{ $$ = Eadd; }
	| SUB_ASSIGN
	{ $$ = Esub; }
	| LEFT_ASSIGN
	{ $$ = Eshl; }
	| RIGHT_ASSIGN
	{ $$ = Eshr; }
	| AND_ASSIGN
	{ $$ = Eband; }
	| XOR_ASSIGN
	{ $$ = Ebxor; }
	| OR_ASSIGN
	{ $$ = Ebor; }
	;

identifier_list
	: id
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| identifier_list ',' id
	{ $$ = newexpr(Eelist, $3, $1, 0, 0); }
	;

lambda_expression
	: assignment_expression
	| LAMBDA '(' identifier_list ')' compound_statement
	{ $$ = newexpr(Elambda, invert($3), $5, 0, 0); }
	| LAMBDA '(' ')' compound_statement
	{ $$ = newexpr(Elambda, nullelist(), $4, 0, 0); }
	| LAMBDA id compound_statement
	{ $$ = newexpr(Elambda, $2, $3, 0, 0); }
	;

names_declaration_list
	: names_declaration
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| names_declaration_list names_declaration
	{ $$ = newexpr(Eelist, $2, $1, 0, 0); }
	;

names_declaration
	: declaration
	| typedef
	;

names_expression
	: lambda_expression
	| NAMES expression '{' names_declaration_list '}'
	{ $$ = newexpr(Ens, $2, invert($4), 0, 0); }
	| NAMES expression '{' '}'
	{ $$ = newexpr(Ens, $2, nullelist(), 0, 0); }
	;

root_expression
	: names_expression

expression
	: root_expression
	| expression ',' root_expression
	{ $$ = newexpr(Ecomma, $1, $3, 0, 0); }
	;

constant_expression
	: conditional_expression
	;

typedef
	: TYPEDEF specifier_list declarator ';'
	{ $$ = newexpr(Etypedef, $2, $3, 0, 0); }
	;

declaration
	: specifier_list ';'
	{ $$ = newexpr(Edecls, $1, nullelist(), 0, 0); }
	| '@' constant_expression specifier_list declarator_list ';'
	{ $$ = newexpr(Edecls, $3, invert($4), $2, 0); }
	| specifier_list declarator_list ';'
	{ $$ = newexpr(Edecls, $1, invert($2), 0, 0); }
	;

declarator_list
	: declarator
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| declarator_list ',' declarator
	{ $$ = newexpr(Eelist, $3, $1, 0, 0); }
	;

base
	: VOID
	{ $$ = newexpr(Evoid, 0, 0, 0, 0); }
	| CHAR
	{ $$ = newexpr(Echar, 0, 0, 0, 0); }
	| SHORT
	{ $$ = newexpr(Eshort, 0, 0, 0, 0); }
	| INT
	{ $$ = newexpr(Eint, 0, 0, 0, 0); }
	| LONG
	{ $$ = newexpr(Elong, 0, 0, 0, 0); }
	| FLOAT
	{ $$ = newexpr(Efloat, 0, 0, 0, 0); }
	| DOUBLE
	{ $$ = newexpr(Edouble, 0, 0, 0, 0); }
	| SIGNED
	{ $$ = newexpr(Esigned, 0, 0, 0, 0); }
	| UNSIGNED
	{ $$ = newexpr(Eunsigned, 0, 0, 0, 0); }

base_list
	: base
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| base_list base
	{ $$ = newexpr(Eelist, $2, $1, 0, 0); }
	;

type_specifier
	: base_list
	{ $$ = newexpr(Ebase, $1, 0, 0, 0); }
	| id
	{ $$ = newexpr(Etid, $1, 0, 0, 0); }
	| struct_or_union_specifier
	| enum_specifier
	;

struct_or_union_specifier
	: struct_or_union tag '{' struct_declaration_list struct_size '}'
	{ $$ = newexpr($1, $2, invert(newexpr(Eelist, $5, $4, 0, 0)), 0, 0); } 
	| struct_or_union '{' struct_declaration_list struct_size '}'
	{ $$ = newexpr($1, 0, invert(newexpr(Eelist, $4, $3, 0, 0)), 0, 0); } 
	| struct_or_union tag '{' struct_size '}'
	{ $$ = newexpr($1, $2, newexpr(Eelist, $4, nullelist(), 0, 0), 0, 0); }
	| struct_or_union '{' struct_size '}'
	{ $$ = newexpr($1, 0, newexpr(Eelist, $3, nullelist(), 0, 0), 0, 0); }
	| struct_or_union tag '{'  '}'
	{ $$ = newexpr($1, $2, nullelist(), 0, 0); }
	| struct_or_union '{' '}'
	{ $$ = newexpr($1, 0, nullelist(), 0, 0); }
	| struct_or_union tag
	{ $$ = newexpr($1, $2, 0, 0, 0); }
	;

struct_or_union
	: STRUCT
	{ $$ = Estruct; }
	| UNION
	{ $$ = Eunion; }
	;

struct_declaration_list
	: struct_declaration
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| struct_declaration_list struct_declaration
	{ $$ = newexpr(Eelist, $2, $1, 0, 0); }
	;

struct_declaration
	: '@' constant_expression specifier_list struct_declarator_list ';'
	{ $$ = newexpr(Efields, $3, invert($4), $2, 0); }
	| '@' '@' constant_expression specifier_list struct_declarator ':' constant_expression ';'
	{ $$ = newexpr(Ebitfield, $4, $5, $3, $7); }
	| specifier_list struct_declarator_list ';'
	{ $$ = newexpr(Efields, $1, invert($2), 0, 0); }
	;

struct_size
	: '@' constant_expression ';'
	{ $$ = newexpr(Efieldoff, $2, 0, 0, 0); }
	;

/* specifier_list order does not affect meaning of specifier */
specifier_list
	: type_specifier
	;

struct_declarator_list
	: struct_declarator
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| struct_declarator_list ',' struct_declarator
	{ $$ = newexpr(Eelist, $3, $1, 0, 0); }
	;

struct_declarator
	: declarator
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	{ $$ = newexpr(Eenum, 0, invert($3), 0, 0); }
	| ENUM tag '{' enumerator_list '}'
	{ $$ = newexpr(Eenum, $2, invert($4), 0, 0); }
	| ENUM '{' enumerator_list ',' '}'
	{ $$ = newexpr(Eenum, 0, invert($3), 0, 0); }
	| ENUM tag '{' enumerator_list ',' '}'
	{ $$ = newexpr(Eenum, $2, invert($4), 0, 0); }
	| ENUM tag
	{ $$ = newexpr(Eenum, $2, 0, 0, 0); }
	;

enumerator_list
	: enumerator
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| enumerator_list ',' enumerator
	{ $$ = newexpr(Eelist, $3, $1, 0, 0); }
	;

/* typedef names and enumator components are in the same
   overload class, so use id not tag. */
enumerator
	: id
	{ $$ = newexpr(Eenumel, $1, 0, 0, 0); }
	| id '=' constant_expression
	{ $$ = newexpr(Eenumel, $1, $3, 0, 0); }
	;

declarator
	: pointer direct_declarator
	{ $$ = ptrto($1, $2); }
	| direct_declarator
	;

direct_declarator
	: id
	{ $$ = $1; }
	| '(' declarator ')'
	{ $$ = $2; }
	| direct_declarator '[' constant_expression ']'
	{ $$ = newexpr(Earr, $1, $3, 0, 0); }
	| direct_declarator '[' ']'
	{ $$ = newexpr(Earr, $1, 0, 0, 0); }
	| direct_declarator '(' parameter_type_list ')'
	{ $$ = newexpr(Efun, $1, $3, 0, 0); }
	| direct_declarator '(' ')'
	{ $$ = newexpr(Efun, $1, nullelist(), 0, 0); }
	;

pointer
	: '*'
	{ $$ = newexpr(Eptr, 0, 0, 0, 0); }
	| '*' pointer
	{ $$ = newexpr(Eptr, $2, 0, 0, 0); }
	;

parameter_type_list
	: parameter_list
	{ $$ = invert($1); }
	| parameter_list ',' ELLIPSIS
/*
	{ $$ = invert(newexpr(Eelist,
	                      newexpr(Edotdot, 0, 0, 0, 0), $1, 0, 0)); }
			      
*/
	{ $$ = invert($1); }
	;

parameter_list
	: parameter_declaration
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| parameter_list ',' parameter_declaration
	{ $$ = newexpr(Eelist, $3, $1, 0, 0); }
	;

parameter_declaration
	: specifier_list declarator
	{ $$ = newexpr(Edecl, $1, $2, 0, 0); }
	| specifier_list abstract_declarator
	{ $$ = newexpr(Edecl, $1, $2, 0, 0); }
	| specifier_list
	{ $$ = newexpr(Edecl, $1, 0, 0, 0); }
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	{ $$ = ptrto($1, $2); }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	{ $$ = $2; }
	| '[' ']'
	{ $$ = newexpr(Earr, 0, 0, 0, 0); }
	| '[' constant_expression ']'
	{ $$ = newexpr(Earr, 0, $2, 0, 0); }
	| direct_abstract_declarator '[' ']'
	{ $$ = newexpr(Earr, $1, 0, 0, 0); }
	| direct_abstract_declarator '[' constant_expression ']'
	{ $$ = newexpr(Earr, $1, $3, 0, 0); }
	| '(' ')'
	{ $$ = newexpr(Efun, 0, nullelist(), 0, 0); }
/*
 * without this rule, you cannot write abstract function types like
 * `int(int)' (perhaps the equivalent type can be formed with enough
 * parens).  with this rule, bison reports 3 reduce/reduce conflicts
 * on '(', '[', and ')'; these seem to be related to distinguishing
 * a symbol identifier in a non-abstract declarator from a type
 * identifier in this abstract declarator.
 * in practice, there does not seem to be an actual ambiguity;
 * perhaps it is an artifact of lalr(1) simplification.
 * unfortunately, i do not know how to analyze the conflict to
 * be sure.
 * it seems it would be safe to arrange for a default reduction to
 * type identifier in all cases (i.e., the only way to reach the
 * conflicted state is via a context in which type_specifier is
 * always the right reduction).  since this rule seems obscure,
 * we leave it out.
	| '(' parameter_type_list ')'
	{ $$ = newexpr(Efun, 0, $2, 0, 0); }
 * (if you restore this, be sure to revisit tn_direct_abstract_declarator)
*/
	| direct_abstract_declarator '(' ')'
	{ $$ = newexpr(Efun, $1, nullelist(), 0, 0); }
	| direct_abstract_declarator '(' parameter_type_list ')'
	{ $$ = newexpr(Efun, $1, $3, 0, 0); }
	;

type_name
	: tn_type_specifier
	{ $$ = newexpr(Edecl, $1, 0, 0, 0); }
        | tn_type_specifier tn_abstract_declarator
	{ $$ = newexpr(Edecl, $1, $2, 0, 0); }
	;

tn_type_specifier
	: base_list
	{ $$ = newexpr(Ebase, $1, 0, 0, 0); }
	| id '`' base_list
	{ $$ = newexpr(Ebase, $3, $1, 0, 0); }
	| '`' id
	{ $$ = newexpr(Etid, $2, 0, 0, 0); }
	| id '`' id
	{ $$ = newexpr(Etid, $3, $1, 0, 0); }
	| tn_struct_or_union_specifier
	| tn_enum_specifier
	;

tn_struct_or_union_specifier
	: struct_or_union tag
	{ $$ = newexpr($1, $2, 0, 0, 0); }
	| struct_or_union id '`' tag
	{ $$ = newexpr($1, $4, 0, $2, 0); }
	;

tn_enum_specifier
	: ENUM tag
	{ $$ = newexpr(Eenum, $2, 0, 0, 0); }
	| ENUM id '`' tag
	{ $$ = newexpr(Eenum, $4, 0, $2, 0); }
	;

tn_parameter_type_list
	: tn_parameter_list
	{ $$ = invert($1); }
	| tn_parameter_list ',' ELLIPSIS
/*
	{ $$ = invert(newexpr(Eelist,
	                      newexpr(Edotdot, 0, 0, 0, 0), $1, 0, 0)); }
			      
*/
	{ $$ = invert($1); }
	;

tn_parameter_list
	: tn_parameter_declaration
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| tn_parameter_list ',' tn_parameter_declaration
	{ $$ = newexpr(Eelist, $3, $1, 0, 0); }
	;

tn_parameter_declaration
	: tn_param_type_specifier tn_declarator
	{ $$ = newexpr(Edecl, $1, $2, 0, 0); }
	| tn_param_type_specifier tn_abstract_declarator
	{ $$ = newexpr(Edecl, $1, $2, 0, 0); }
	| tn_param_type_specifier
	{ $$ = newexpr(Edecl, $1, 0, 0, 0); }
	;

tn_param_type_specifier
	: base_list
	{ $$ = newexpr(Ebase, $1, 0, 0, 0); }
	| id
	{ $$ = newexpr(Etid, $1, 0, 0, 0); }
	| tn_param_struct_or_union_specifier
	| tn_param_enum_specifier
	;

tn_param_struct_or_union_specifier
	: struct_or_union tag
	{ $$ = newexpr($1, $2, 0, 0, 0); }
	;

tn_param_enum_specifier
	: ENUM tag
	{ $$ = newexpr(Eenum, $2, 0, 0, 0); }
	;

tn_abstract_declarator
	: pointer
	| tn_direct_abstract_declarator
	| pointer tn_direct_abstract_declarator
	{ $$ = ptrto($1, $2); }
	;

tn_direct_abstract_declarator
	: '(' tn_abstract_declarator ')'
	{ $$ = $2; }
	| '[' ']'
	{ $$ = newexpr(Earr, 0, 0, 0, 0); }
	| '[' constant_expression ']'
	{ $$ = newexpr(Earr, 0, $2, 0, 0); }
	| tn_direct_abstract_declarator '[' ']'
	{ $$ = newexpr(Earr, $1, 0, 0, 0); }
	| tn_direct_abstract_declarator '[' constant_expression ']'
	{ $$ = newexpr(Earr, $1, $3, 0, 0); }
	| '(' ')'
	{ $$ = newexpr(Efun, 0, nullelist(), 0, 0); }
	| tn_direct_abstract_declarator '(' ')'
	{ $$ = newexpr(Efun, $1, nullelist(), 0, 0); }
	| tn_direct_abstract_declarator '(' tn_parameter_type_list ')'
	{ $$ = newexpr(Efun, $1, $3, 0, 0); }
	;

tn_declarator
	: pointer tn_direct_declarator
	{ $$ = ptrto($1, $2); }
	| tn_direct_declarator
	;

tn_direct_declarator
	: id
	{ $$ = $1; }
	| '(' tn_declarator ')'
	{ $$ = $2; }
	| tn_direct_declarator '[' constant_expression ']'
	{ $$ = newexpr(Earr, $1, $3, 0, 0); }
	| tn_direct_declarator '[' ']'
	{ $$ = newexpr(Earr, $1, 0, 0, 0); }
	| tn_direct_declarator '(' tn_parameter_type_list ')'
	{ $$ = newexpr(Efun, $1, $3, 0, 0); }
	| tn_direct_declarator '(' ')'
	{ $$ = newexpr(Efun, $1, nullelist(), 0, 0); }
	;

statement
	: compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	| define_statement
	| labeled_statement
	;

local
	: LOCAL identifier_list ';'
	{ $$ = invert($2); }
	;

local_list
	: local
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| local_list local
	{ $$ = newexpr(Eelist, $2, $1, 0, 0); }
	;

compound_statement
	: '{' '}'
	{ $$ = newexpr(Eblock, nullelist(), nullelist(), 0, 0); }
	| '{' statement_list '}'
	{ $$ = newexpr(Eblock, nullelist(), invert($2), 0, 0); }
	| '{' local_list '}'
	{ $$ = newexpr(Eblock, invert($2), nullelist(), 0, 0); }
	| '{' local_list statement_list '}'
	{ $$ = newexpr(Eblock, invert($2), invert($3), 0, 0); }
	;

statement_list
	: statement
	{ $$ = newexpr(Eelist, $1, nullelist(), 0, 0); }
	| statement_list statement
	{ $$ = newexpr(Eelist, $2, $1, 0, 0); }
	;

expression_statement
	: ';'
	{ $$ = newexpr(Enop, 0, 0, 0, 0); }
	| expression ';'
	{ $$ = $1; }
	;

selection_statement
	: IF '(' expression ')' statement
	{ $$ = newexpr(Eif, $3, $5, 0, 0); }
	| IF '(' expression ')' statement ELSE statement
	{ $$ = newexpr(Eif, $3, $5, $7, 0); }
	| SWITCH '(' expression ')' compound_statement
	/* note: C permits body of switch to be a statement */
	{ $$ = newexpr(Eswitch, $3, $5, 0, 0); }
	;

labeled_statement
	: CASE expression ':' statement
	  { $$ = newexpr(Ecase, $2, $4, NULL, NULL); }
	| DEFAULT ':' statement
	  { $$ = newexpr(Edefault, $3, NULL, NULL, NULL); }
	;

maybe_expression
	: expression
	|
	{ $$ = 0; }
	;

iteration_statement
	: WHILE '(' expression ')' statement
	{ $$ = newexpr(Ewhile, $3, $5, 0, 0); }
	| DO statement WHILE '(' expression ')' ';'
	{ $$ = newexpr(Edo, $2, $5, 0, 0); }
	| FOR '(' maybe_expression ';' maybe_expression ';' maybe_expression ')' statement
	{ $$ = newexpr(Efor, $3, $5, $7, $9); }
	;

jump_statement
	: CONTINUE ';'
	{ $$ = newexpr(Econtinue, 0, 0, 0, 0); }
	| BREAK ';'
	{ $$ = newexpr(Ebreak, 0, 0, 0, 0); }
	| RETURN ';'
	{ $$ = newexpr(Eret, 0, 0, 0, 0); }
	| RETURN expression ';'
	{ $$ = newexpr(Eret, $2, 0, 0, 0); }
	;

define_statement
	: DEFINE id '(' identifier_list ')' compound_statement
	{ $$ = newexpr(Edefine, $2, invert($4), $6, 0); }
	| DEFINE id '('  ')' compound_statement
	{ $$ = newexpr(Edefine, $2, nullelist(), $5, 0); }
	| DEFINE id id compound_statement
	{ $$ = newexpr(Edefine, $2, $3, $4, 0); }
	;

translation_unit_seq
	: translation_unit
	|
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: statement		{ dotop(ctx, $1); }
	;

%%
static void
yyerror(U *ctx, const char *s)
{
	parseerror(ctx, (char*)s);
}

/* expression trees for ambiguous forms share identifier nodes;
   separate them so that we can handle them independently. */
static void
duptickid(Expr *e)
{
	if(e == 0)
		return;
	if(e->kind == Etick){
		e->e1 = copyexpr(e->e1);
		e->e2 = copyexpr(e->e2);
		return;
	}
	duptickid(e->e1);
	duptickid(e->e2);
	duptickid(e->e3);
	duptickid(e->e4);
}

static Expr*
castmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *cast, *other;

	// (T)(<expr>)  cast expression or function call

	if(ye1.expr->kind == Ecast){
		cast = ye1.expr;
		other = ye2.expr;
	}else if(ye2.expr->kind == Ecast){
		cast = ye2.expr;
		other = ye1.expr;
	}else
		yyerror(0, "unresolved ambiguity");

	/* what else could it be? */ 
	if(other->kind != Ecall)
		yyerror(0, "unresolved ambiguity");
	if(other->e1->kind != Etick)
		yyerror(0, "unresolved ambiguity");

	/* it's not possible to call through a domain reference,
	   so call it a cast. */
	duptickid(other->e1);
	freeexpr(other);
	return cast;
}

static Expr*
mulmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *cast, *other;

	// (dom`T)*p  cast expression or multiply

	if(ye1.expr->kind == Ecast){
		cast = ye1.expr;
		other = ye2.expr;
	}else if(ye2.expr->kind == Ecast){
		cast = ye2.expr;
		other = ye1.expr;
	}else
		yyerror(0, "unresolved ambiguity");

	duptickid(other->e1);

	return newexpr(Eambig, cast, other, NULL, NULL);
}

static int
ofkind(int kind)
{
	return (kind == Esizeoft) || (kind == Esizeofe)
		|| (kind == Etypeoft) || (kind == Etypeofe);
}

static Expr*
ofmerge(YYSTYPE ye1, YYSTYPE ye2)
{
	Expr *e1, *e2;

	// Xof(dom`T)   X is "size" or "type"

	e1 = ye1.expr;
	e2 = ye2.expr;
	if(!ofkind(e1->kind) || !ofkind(e2->kind))
		yyerror(0, "unresolved ambiguity");

	if(e1->kind == Esizeofe || e1->kind == Etypeofe)
		duptickid(e1->e1);
	else
		duptickid(e2->e1);

	return newexpr(Eambig, e1, e2, NULL, NULL);
}
