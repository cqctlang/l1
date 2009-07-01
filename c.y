%{
#include "sys.h"
#include "util.h"
#include "syscqct.h"

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

%token <chars> IDENTIFIER CONSTANT STRING_LITERAL CONST VOLATILE
%token SIZEOF TYPEOF TYPEDEF NIL DEFINE DEFLOCAL DEFREC CONTAINEROF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token GLOBAL LOCAL LAMBDA NAMES LET
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE VOID
%token STRUCT UNION ENUM ELLIPSIS DEFCONST
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
%type <expr> defconst_statement compound_statement statement_list
%type <expr> expression_statement define_statement labeled_statement
%type <expr> selection_statement iteration_statement jump_statement
%type <expr> global_statement defrec_expression let_expression
%type <expr> type_name tn_type_specifier tn_type_qual_specifier
%type <expr> tn_struct_or_union_specifier
%type <expr> tn_enum_specifier tn_parameter_type_list tn_parameter_list
%type <expr> tn_parameter_declaration tn_abstract_declarator
%type <expr> tn_direct_abstract_declarator tn_declarator tn_direct_declarator
%type <expr> tn_param_type_specifier tn_param_struct_or_union_specifier
%type <expr> tn_param_type_qual_specifier
%type <expr> tn_param_enum_specifier
%type <expr> table_init table_init_list
%type <expr> maybe_attr

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
	{ $$ = doidnsrc(&ctx->inp->src, $1.p, $1.len); }

tickid
	: id '`' id
	{ $$ = doticksrc(&ctx->inp->src, $1, $3); }
	;

tag:	id;

lambda_expression
	: LAMBDA '(' identifier_list ')' compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Elambda, invert($3), $5, 0, 0); }
	| LAMBDA '(' ')' compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Elambda, nullelist(), $4, 0, 0); }
	| LAMBDA id compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Elambda, $2, $3, 0, 0); }
	;

defrec_expression
	: DEFREC id '{' identifier_list '}'
	{ $$ = newexprsrc(&ctx->inp->src, Edefrec, $2, invert($4), 0, 0); }	
	| DEFREC id '{' identifier_list ',' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Edefrec, $2, invert($4), 0, 0); }	
	| DEFREC id '{' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Edefrec, $2, nullelist(), 0, 0); }	
	;

let_expression
	: LET compound_statement
	{ $$ = $2; }
	;

table_init
	: root_expression ':' root_expression
	{ $$ = newexprsrc(&ctx->inp->src, Eelist,
			  $1,
			  newexprsrc(&ctx->inp->src, Eelist,
				     $3, nullelist(), 0, 0),
			  0, 0);
	}
	;

table_init_list
	: table_init
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| table_init_list ',' table_init
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;


primary_expression
	: id
	| tickid
	| NIL
	{ $$ = newexprsrc(&ctx->inp->src, Enil, 0, 0, 0, 0); }
	| CONSTANT
	{ $$ = doconst(ctx, $1.p, $1.len); }
	| STRING_LITERAL
	{ $$ = doconstssrc(&ctx->inp->src, $1.p, $1.len); }
	| '(' expression ')'
	{ $$ = $2; }
	| '[' ']'
	{ $$ = newexprsrc(&ctx->inp->src, Elist, nullelist(), 0, 0, 0); }
	| '[' argument_expression_list ']'
	{ $$ = newexprsrc(&ctx->inp->src, Elist, invert($2), 0, 0, 0); }
	| '{' ':' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Etab, nullelist(), 0, 0, 0); }
	| '{' table_init_list '}'
	{ $$ = newexprsrc(&ctx->inp->src, Etab, invert($2), 0, 0, 0); }
	| '{' table_init_list ',' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Etab, invert($2), 0, 0, 0); }
	| lambda_expression
	| defrec_expression
	| let_expression
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earef, $1, $3, 0, 0); }
	| postfix_expression '(' ')'
	{ $$ = newexprsrc(&ctx->inp->src, Ecall, $1, nullelist(), 0, 0); }
	| postfix_expression '(' argument_expression_list ')'
	  /* don't invert -- compiler evaluates arguments in reverse order */
	{ $$ = newexprsrc(&ctx->inp->src, Ecall, $1, $3, 0, 0); }
	| postfix_expression '.' id
	{ $$ = newexprsrc(&ctx->inp->src, Edot, $1, $3, 0, 0); }
	| postfix_expression PTR_OP id
	{ $$ = newexprsrc(&ctx->inp->src, Earrow, $1, $3, 0, 0); }
	| postfix_expression INC_OP
	{ $$ = newexprsrc(&ctx->inp->src, Epostinc, $1, 0, 0, 0); }
	| postfix_expression DEC_OP
	{ $$ = newexprsrc(&ctx->inp->src, Epostdec, $1, 0, 0, 0); }
	| CONTAINEROF '(' expression ',' type_name ',' id ')'
        { $$ = newexprsrc(&ctx->inp->src, Econtainer, $3, $5, $7, 0); }
	;

argument_expression_list
	: root_expression
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| argument_expression_list ',' root_expression
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	{ $$ = newexprsrc(&ctx->inp->src, Epreinc, $2, 0, 0, 0); }
	| DEC_OP unary_expression
	{ $$ = newexprsrc(&ctx->inp->src, Epredec, $2, 0, 0, 0); }
	| unary_operator cast_expression
	{ $$ = newexprsrc(&ctx->inp->src, $1, $2, 0, 0, 0); }
	| SIZEOF unary_expression			%merge <ofmerge>
	{ $$ = newexprsrc(&ctx->inp->src, Esizeofe, $2, 0, 0, 0); }
	| SIZEOF '(' type_name ')'			%merge <ofmerge>
	{ $$ = newexprsrc(&ctx->inp->src, Esizeoft, $3, 0, 0, 0); }
	| TYPEOF unary_expression			%merge <ofmerge>
	{ $$ = newexprsrc(&ctx->inp->src, Etypeofe, $2, 0, 0, 0); }
	| TYPEOF '(' type_name ')'			%merge <ofmerge>
	{ $$ = newexprsrc(&ctx->inp->src, Etypeoft, $3, 0, 0, 0); }
	| '#' type_name '#'
	{ $$ = newexprsrc(&ctx->inp->src, Etypeoft, $2, 0, 0, 0); }
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
	{ $$ = newexprsrc(&ctx->inp->src, Ecast, $2, $4, 0, 0); }
	| '{' expression '}' cast_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Excast, $2, $4); }
	;

multiplicative_expression
	: cast_expression				%merge <mulmerge>
	| multiplicative_expression '*' cast_expression	%merge <mulmerge>
	{ $$ = newbinopsrc(&ctx->inp->src, Emul, $1, $3); }
	| multiplicative_expression '/' cast_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Ediv, $1, $3); }
	| multiplicative_expression '%' cast_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Emod, $1, $3); }
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Eadd, $1, $3); }
	| additive_expression '-' multiplicative_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Esub, $1, $3); }
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Eshl, $1, $3); }
	| shift_expression RIGHT_OP additive_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Eshr, $1, $3); }
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Elt, $1, $3); }
	| relational_expression '>' shift_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Egt, $1, $3); }
	| relational_expression LE_OP shift_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Ele, $1, $3); }
	| relational_expression GE_OP shift_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Ege, $1, $3); }
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Eeq, $1, $3); }
	| equality_expression NE_OP relational_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Eneq, $1, $3); }
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Eband, $1, $3); }
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Ebxor, $1, $3); }
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	{ $$ = newbinopsrc(&ctx->inp->src, Ebor, $1, $3); }
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	{ $$ = newexprsrc(&ctx->inp->src, Eland, $1, $3, 0, 0); }
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	{ $$ = newexprsrc(&ctx->inp->src, Elor, $1, $3, 0, 0); }
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	{ $$ = newexprsrc(&ctx->inp->src, Econd, $1, $3, $5, 0); }
	;

assignment_expression
	: conditional_expression
	| unary_expression assignment_operator root_expression
	{ if($2 == Eg)
	  	$$ = newexprsrc(&ctx->inp->src, $2, $1, $3, 0, 0);
	  else
	  	$$ = newgopsrc(&ctx->inp->src, $2, $1, $3);
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
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| identifier_list ',' id
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;

names_declaration_list
	: names_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| names_declaration_list names_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $2, $1, 0, 0); }
	;

names_declaration
	: declaration
	| typedef
	;

names_expression
	: assignment_expression
	| NAMES expression '{' names_declaration_list '}'
	{ $$ = newexprsrc(&ctx->inp->src, Ens, $2, invert($4), 0, 0); }
	| NAMES expression '{' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Ens, $2, nullelist(), 0, 0); }
	| NAMES expression '$' expression '{' names_declaration_list '}'
	{ $$ = newexprsrc(&ctx->inp->src, Ens, $2, invert($6), $4, 0); }
	| NAMES expression '$' expression  '{' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Ens, $2, nullelist(), $4, 0); }
	;

root_expression
	: names_expression

expression
	: root_expression
	| expression ',' root_expression
	{ $$ = newexprsrc(&ctx->inp->src, Ecomma, $1, $3, 0, 0); }
	;

constant_expression
	: conditional_expression
	;

typedef
	: TYPEDEF specifier_list declarator ';'
	{ $$ = newexprsrc(&ctx->inp->src, Etypedef, $2, $3, 0, 0); }
	;

maybe_attr
	: '@' constant_expression
	{ $$ = $2; }
	|
	{ $$ = 0; }
	;

declaration
	: specifier_list ';'
	{ $$ = newexprsrc(&ctx->inp->src, Edecls, $1, nullelist(), 0, 0); }
	| '@' constant_expression specifier_list declarator_list ';'
	{ $$ = newexprsrc(&ctx->inp->src, Edecls, $3, invert($4), $2, 0); }
	| specifier_list declarator_list ';'
	{ $$ = newexprsrc(&ctx->inp->src, Edecls, $1, invert($2), 0, 0); }
	;

declarator_list
	: declarator
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| declarator_list ',' declarator
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;

base
	: VOID
	{ $$ = newexprsrc(&ctx->inp->src, Evoid, 0, 0, 0, 0); }
	| CHAR
	{ $$ = newexprsrc(&ctx->inp->src, Echar, 0, 0, 0, 0); }
	| SHORT
	{ $$ = newexprsrc(&ctx->inp->src, Eshort, 0, 0, 0, 0); }
	| INT
	{ $$ = newexprsrc(&ctx->inp->src, Eint, 0, 0, 0, 0); }
	| LONG
	{ $$ = newexprsrc(&ctx->inp->src, Elong, 0, 0, 0, 0); }
	| FLOAT
	{ $$ = newexprsrc(&ctx->inp->src, Efloat, 0, 0, 0, 0); }
	| DOUBLE
	{ $$ = newexprsrc(&ctx->inp->src, Edouble, 0, 0, 0, 0); }
	| SIGNED
	{ $$ = newexprsrc(&ctx->inp->src, Esigned, 0, 0, 0, 0); }
	| UNSIGNED
	{ $$ = newexprsrc(&ctx->inp->src, Eunsigned, 0, 0, 0, 0); }

base_list
	: base
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| base_list base
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $2, $1, 0, 0); }
	;

type_specifier
	: base_list
	{ $$ = newexprsrc(&ctx->inp->src, Ebase, $1, 0, 0, 0); }
	| id
	{ $$ = newexprsrc(&ctx->inp->src, Etid, $1, 0, 0, 0); }
	| struct_or_union_specifier
	| enum_specifier
	;

struct_or_union_specifier
	: struct_or_union tag '{' struct_declaration_list struct_size '}'
	{ $$ = newexprsrc(&ctx->inp->src, $1, $2, invert(newexprsrc(&ctx->inp->src, Eelist, $5, $4, 0, 0)), 0, 0); } 
	| struct_or_union '{' struct_declaration_list struct_size '}'
	{ $$ = newexprsrc(&ctx->inp->src, $1, 0, invert(newexprsrc(&ctx->inp->src, Eelist, $4, $3, 0, 0)), 0, 0); } 
	| struct_or_union tag '{' struct_size '}'
	{ $$ = newexprsrc(&ctx->inp->src, $1, $2, newexprsrc(&ctx->inp->src, Eelist, $4, nullelist(), 0, 0), 0, 0); }
	| struct_or_union '{' struct_size '}'
	{ $$ = newexprsrc(&ctx->inp->src, $1, 0, newexprsrc(&ctx->inp->src, Eelist, $3, nullelist(), 0, 0), 0, 0); }
	| struct_or_union tag '{'  '}'
	{ $$ = newexprsrc(&ctx->inp->src, $1, $2, nullelist(), 0, 0); }
	| struct_or_union '{' '}'
	{ $$ = newexprsrc(&ctx->inp->src, $1, 0, nullelist(), 0, 0); }
	| struct_or_union tag
	{ $$ = newexprsrc(&ctx->inp->src, $1, $2, 0, 0, 0); }
	;

struct_or_union
	: STRUCT
	{ $$ = Estruct; }
	| UNION
	{ $$ = Eunion; }
	;

struct_declaration_list
	: struct_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| struct_declaration_list struct_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $2, $1, 0, 0); }
	;

struct_declaration
	: '@' constant_expression specifier_list struct_declarator_list ';'
	{ $$ = newexprsrc(&ctx->inp->src, Efields, $3, invert($4), $2, 0); }
	| '@' '@' constant_expression specifier_list struct_declarator ':' constant_expression ';'
	{ $$ = newexprsrc(&ctx->inp->src, Ebitfield, $4, $5, $3, $7); }
	| '@' constant_expression specifier_list ';'
	{ $$ = newexprsrc(&ctx->inp->src, Efields, $3, nullelist(), $2, 0); }
	| specifier_list struct_declarator_list ';'
	{ $$ = newexprsrc(&ctx->inp->src, Efields, $1, invert($2), 0, 0); }
	/* accept (but discard) c++ labels such as "public:" */
	| id ':'
	{ $$ = newexprsrc(&ctx->inp->src, Enop, 0, 0, 0, 0); }
	;

struct_size
	: '@' constant_expression ';'
	{ $$ = newexprsrc(&ctx->inp->src, Efieldoff, $2, 0, 0, 0); }
	;

// discard type qualifiers

type_qualifier
	: CONST
	| VOLATILE
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier type_qualifier_list
	;

specifier_list
	: type_specifier
	| type_qualifier_list type_specifier
	{ $$ = $2; }
	| type_specifier type_qualifier_list
	{ $$ = $1; }
	;

struct_declarator_list
	: struct_declarator
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| struct_declarator_list ',' struct_declarator
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;

struct_declarator
	: declarator
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, 0, invert($3), 0, 0); }
	| ENUM tag '{' enumerator_list '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $2, invert($4), 0, 0); }
	| ENUM '{' enumerator_list ',' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, 0, invert($3), 0, 0); }
	| ENUM tag '{' enumerator_list ',' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $2, invert($4), 0, 0); }
	| ENUM '{' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, 0, nullelist(), 0, 0); }
	| ENUM tag '{' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $2, nullelist(), 0, 0); }
	| ENUM '{' ',' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, 0, nullelist(), 0, 0); }
	| ENUM tag '{' ',' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $2, nullelist(), 0, 0); }
	| ENUM tag
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $2, 0, 0, 0); }
	;

enumerator_list
	: enumerator
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| enumerator_list ',' enumerator
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;

/* typedef names and enumator components are in the same
   overload class, so use id not tag. */
enumerator
	: id
	{ $$ = newexprsrc(&ctx->inp->src, Eenumel, $1, 0, 0, 0); }
	| id '=' constant_expression
	{ $$ = newexprsrc(&ctx->inp->src, Eenumel, $1, $3, 0, 0); }
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
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, $3, 0, 0); }
	| direct_declarator '[' ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, 0, 0, 0); }
	| direct_declarator '(' parameter_type_list ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, $3, 0, 0); }
	| direct_declarator '(' ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, nullelist(), 0, 0); }
	;

pointer
	: '*'
	{ $$ = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); }
	| '*' type_qualifier_list
	{ $$ = newexprsrc(&ctx->inp->src, Eptr, 0, 0, 0, 0); }
	| '*' pointer
	{ $$ = newexprsrc(&ctx->inp->src, Eptr, $2, 0, 0, 0); }
	| '*' type_qualifier_list pointer
	{ $$ = newexprsrc(&ctx->inp->src, Eptr, $3, 0, 0, 0); }
	;

parameter_type_list
	: parameter_list
	{ $$ = invert($1); }
	| parameter_list ',' ELLIPSIS
/*
	{ $$ = invert(newexprsrc(&ctx->inp->src, Eelist,
	                      newexprsrc(&ctx->inp->src, Edotdot, 0, 0, 0, 0), $1, 0, 0)); }
			      
*/
	{ $$ = invert($1); }
	;

parameter_list
	: parameter_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| parameter_list ',' parameter_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;

parameter_declaration
	: maybe_attr specifier_list declarator
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $2, $3, $1, 0); }
	| maybe_attr specifier_list abstract_declarator
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $2, $3, $1, 0); }
	| maybe_attr specifier_list
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $2, 0, $1, 0); }
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
	{ $$ = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); }
	| '[' constant_expression ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, 0, $2, 0, 0); }
	| direct_abstract_declarator '[' ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, 0, 0, 0); }
	| direct_abstract_declarator '[' constant_expression ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, $3, 0, 0); }
	| '(' ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); }
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
	{ $$ = newexprsrc(&ctx->inp->src, Efun, 0, $2, 0, 0); }
 * (if you restore this, be sure to revisit tn_direct_abstract_declarator)
*/
	| direct_abstract_declarator '(' ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, nullelist(), 0, 0); }
	| direct_abstract_declarator '(' parameter_type_list ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, $3, 0, 0); }
	;

type_name
	: tn_type_qual_specifier
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $1, 0, 0, 0); }
        | tn_type_qual_specifier tn_abstract_declarator
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $1, $2, 0, 0); }
	;

tn_type_qual_specifier
	: tn_type_specifier
	| type_qualifier_list tn_type_specifier
	{ $$ = $2; }
	| tn_type_specifier type_qualifier_list
	{ $$ = $1; }
	;

tn_type_specifier
	: base_list
	{ $$ = newexprsrc(&ctx->inp->src, Ebase, $1, 0, 0, 0); }
	| id '`' base_list
	{ $$ = newexprsrc(&ctx->inp->src, Ebase, $3, $1, 0, 0); }
	| '`' id
	{ $$ = newexprsrc(&ctx->inp->src, Etid, $2, 0, 0, 0); }
	| id '`' id
	{ $$ = newexprsrc(&ctx->inp->src, Etid, $3, $1, 0, 0); }
	| tn_struct_or_union_specifier
	| tn_enum_specifier
	;

tn_struct_or_union_specifier
	: struct_or_union tag
	{ $$ = newexprsrc(&ctx->inp->src, $1, $2, 0, 0, 0); }
	| struct_or_union id '`' tag
	{ $$ = newexprsrc(&ctx->inp->src, $1, $4, 0, $2, 0); }
	;

tn_enum_specifier
	: ENUM tag
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $2, 0, 0, 0); }
	| ENUM id '`' tag
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $4, 0, $2, 0); }
	;

tn_parameter_type_list
	: tn_parameter_list
	{ $$ = invert($1); }
	| tn_parameter_list ',' ELLIPSIS
/*
	{ $$ = invert(newexprsrc(&ctx->inp->src, Eelist,
	                      newexprsrc(&ctx->inp->src, Edotdot, 0, 0, 0, 0), $1, 0, 0)); }
			      
*/
	{ $$ = invert($1); }
	;

tn_parameter_list
	: tn_parameter_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| tn_parameter_list ',' tn_parameter_declaration
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $3, $1, 0, 0); }
	;

tn_parameter_declaration
	: tn_param_type_qual_specifier tn_declarator
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $1, $2, 0, 0); }
	| tn_param_type_qual_specifier tn_abstract_declarator
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $1, $2, 0, 0); }
	| tn_param_type_qual_specifier
	{ $$ = newexprsrc(&ctx->inp->src, Edecl, $1, 0, 0, 0); }
	;

tn_param_type_qual_specifier
	: tn_param_type_specifier
	| tn_param_type_specifier type_qualifier_list
	{ $$ = $1; }
	| type_qualifier_list tn_param_type_specifier
	{ $$ = $2; }
	;

tn_param_type_specifier
	: base_list
	{ $$ = newexprsrc(&ctx->inp->src, Ebase, $1, 0, 0, 0); }
	| id
	{ $$ = newexprsrc(&ctx->inp->src, Etid, $1, 0, 0, 0); }
	| tn_param_struct_or_union_specifier
	| tn_param_enum_specifier
	;

tn_param_struct_or_union_specifier
	: struct_or_union tag
	{ $$ = newexprsrc(&ctx->inp->src, $1, $2, 0, 0, 0); }
	;

tn_param_enum_specifier
	: ENUM tag
	{ $$ = newexprsrc(&ctx->inp->src, Eenum, $2, 0, 0, 0); }
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
	{ $$ = newexprsrc(&ctx->inp->src, Earr, 0, 0, 0, 0); }
	| '[' constant_expression ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, 0, $2, 0, 0); }
	| tn_direct_abstract_declarator '[' ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, 0, 0, 0); }
	| tn_direct_abstract_declarator '[' constant_expression ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, $3, 0, 0); }
	| '(' ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, 0, nullelist(), 0, 0); }
	| tn_direct_abstract_declarator '(' ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, nullelist(), 0, 0); }
	| tn_direct_abstract_declarator '(' tn_parameter_type_list ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, $3, 0, 0); }
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
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, $3, 0, 0); }
	| tn_direct_declarator '[' ']'
	{ $$ = newexprsrc(&ctx->inp->src, Earr, $1, 0, 0, 0); }
	| tn_direct_declarator '(' tn_parameter_type_list ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, $3, 0, 0); }
	| tn_direct_declarator '(' ')'
	{ $$ = newexprsrc(&ctx->inp->src, Efun, $1, nullelist(), 0, 0); }
	;

statement
	: compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	| define_statement
	| labeled_statement
	| defconst_statement
	| global_statement
	;

global_statement
	: GLOBAL identifier_list ';'
	{ $$ = newexprsrc(&ctx->inp->src, Eglobal, invert($2), 0, 0, 0); }
	;

local
	: LOCAL identifier_list ';'
	{ $$ = invert($2); }
	;

local_list
	: local
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| local_list local
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $2, $1, 0, 0); }
	;

compound_statement
	: '{' '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eblock, nullelist(), nullelist(), 0, 0); }
	| '{' local_list '}'
	{ $$ = newexprsrc(&ctx->inp->src, Eblock, flatten($2), nullelist(), 0, 0); }
	| '{' statement_list '}'
	{
		/* use src of first statement */
		Expr *sl;
		sl = invert($2);
		$$ = newexprsrc(&sl->src, Eblock, nullelist(), sl, 0, 0);
	}
	| '{' local_list statement_list '}'
	{ 
		/* use src of first statement */
		Expr *sl;
		sl = invert($3);
		$$ = newexprsrc(&sl->src, Eblock, flatten($2), sl, 0, 0); 
	}
	;

statement_list
	: statement
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $1, nullelist(), 0, 0); }
	| statement_list statement
	{ $$ = newexprsrc(&ctx->inp->src, Eelist, $2, $1, 0, 0); }
	;

expression_statement
	: ';'
	{ $$ = newexprsrc(&ctx->inp->src, Enop, 0, 0, 0, 0); }
	| expression ';'
	{ $$ = $1; }
	;

selection_statement
	: IF '(' expression ')' statement			%dprec 2
	{ $$ = newexprsrc(&ctx->inp->src, Eif, $3, $5, 0, 0); }
	| IF '(' expression ')' statement ELSE statement	%dprec 1
	{ $$ = newexprsrc(&ctx->inp->src, Eif, $3, $5, $7, 0); }
	| SWITCH '(' expression ')' compound_statement
	/* note: C permits body of switch to be a statement */
	{ $$ = newexprsrc(&ctx->inp->src, Eswitch, $3, $5, 0, 0); }
	;

labeled_statement
	: CASE expression ':' statement
	  { $$ = newexprsrc(&ctx->inp->src, Ecase, $2, $4, NULL, NULL); }
	| DEFAULT ':' statement
	  { $$ = newexprsrc(&ctx->inp->src, Edefault, $3, NULL, NULL, NULL); }
	;

maybe_expression
	: expression
	|
	{ $$ = 0; }
	;

iteration_statement
	: WHILE '(' expression ')' statement
	{ $$ = newexprsrc(&ctx->inp->src, Ewhile, $3, $5, 0, 0); }
	| DO statement WHILE '(' expression ')' ';'
	{ $$ = newexprsrc(&ctx->inp->src, Edo, $2, $5, 0, 0); }
	| FOR '(' maybe_expression ';' maybe_expression ';' maybe_expression ')' statement
	{ $$ = newexprsrc(&ctx->inp->src, Efor, $3, $5, $7, $9); }
	;

jump_statement
	: CONTINUE ';'
	{ $$ = newexprsrc(&ctx->inp->src, Econtinue, 0, 0, 0, 0); }
	| BREAK ';'
	{ $$ = newexprsrc(&ctx->inp->src, Ebreak, 0, 0, 0, 0); }
	| RETURN ';'
	{ $$ = newexprsrc(&ctx->inp->src, Eret, 0, 0, 0, 0); }
	| RETURN expression ';'
	{ $$ = newexprsrc(&ctx->inp->src, Eret, $2, 0, 0, 0); }
	;

define_statement
	: DEFINE id '(' identifier_list ')' compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Edefine, $2, invert($4), $6, 0); }
	| DEFINE id '('  ')' compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Edefine, $2, nullelist(), $5, 0); }
	| DEFINE id id compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Edefine, $2, $3, $4, 0); }
	| DEFINE id '(' identifier_list ')' '[' expression ']' compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Edefine, $2, invert($4), $9, $7); }
	| DEFINE id '('  ')' '[' expression ']' compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Edefine, $2, nullelist(), $8, $6); }
	| DEFINE id id '[' expression ']' compound_statement
	{ $$ = newexprsrc(&ctx->inp->src, Edefine, $2, $3, $7, $5); }
	;

defconst_statement
	: DEFCONST id '=' expression ';'
	{ $$ = newexprsrc(&ctx->inp->src, Edefconst, $2, $4, 0, 0); }
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

	return newexprsrc(&cast->src, Eambig, cast, other, NULL, NULL);
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
	return newexprsrc(&e1->src, Eambig, e1, e2, NULL, NULL);
}
