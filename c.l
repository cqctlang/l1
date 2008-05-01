D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
O			[0-7]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*
CHARC			(\\([ntbvrfa\\\?'"]|x{H}+|{O}|{O}{O}|{O}{O}{O})|[^\\'])
STRC			(\\([ntbvrfa\\\?'"]|x{H}+|{O}|{O}{O}|{O}{O}{O})|[^\\"])

%{
#include "sys.h"
#include "util.h"
#include "l1.h"
#include "c.tab.h"

struct YYstate {
	YY_BUFFER_STATE yy;
};

static void count();
static void comment();
static int check_type();

%}

%%
@include[ \t]+\"[^\"]+\"[ \t]*\n   { count(); tryinclude(ctx.ns, yytext); }
@include[ \t]+\<[^\<\>]+\>[ \t]*\n { count(); tryinclude(ctx.ns, yytext); }
<<EOF>>                 { if(popyy() == 0) yyterminate(); }

\/\/.*			{ /* comment */ ctx.inp->col = 0; }
"/*"			{ comment(); }
"@local"		{ count(); return(LOCAL); }
"break"			{ count(); return(BREAK); }
"char"			{ count(); return(CHAR); }
"continue"		{ count(); return(CONTINUE); }
"do"			{ count(); return(DO); }
"double"		{ count(); return(DOUBLE); }
"else"			{ count(); return(ELSE); }
"enum"			{ count(); return(ENUM); }
"float"			{ count(); return(FLOAT); }
"for"			{ count(); return(FOR); }
"if"			{ count(); return(IF); }
"int"			{ count(); return(INT); }
"lambda"		{ count(); return(LAMBDA); }
"long"			{ count(); return(LONG); }
"return"		{ count(); return(RETURN); }
"short"			{ count(); return(SHORT); }
"signed"		{ count(); return(SIGNED); }
"sizeof"		{ count(); return(SIZEOF); }
"struct"		{ count(); return(STRUCT); }
"switch"		{ count(); return(SWITCH); }
"typedef"		{ count(); return(TYPEDEF); }
"union"			{ count(); return(UNION); }
"unsigned"		{ count(); return(UNSIGNED); }
"void"			{ count(); return(VOID); }
"while"			{ count(); return(WHILE); }

{L}({L}|{D})*		{ count(); return(check_type()); }

0[xX]{H}+{IS}?		{ count(); yylval.id = yytext; return(CONSTANT); }
0{D}+{IS}?		{ count(); yylval.id = yytext; return(CONSTANT); }
{D}+{IS}?		{ count(); yylval.id = yytext; return(CONSTANT); }
L?'{CHARC}'  		{ count(); yylval.id = yytext; return(CONSTANT); }

{D}+{E}{FS}?		{ count(); yylval.id = yytext; return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ count(); yylval.id = yytext; return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ count(); yylval.id = yytext; return(CONSTANT); }

L?(\"{STRC}*\"[ \t\v\n\f]*)+   { count(); yylval.id = xstrdup(yytext); return(STRING_LITERAL); }

"..."			{ count(); return(ELLIPSIS); }
">>="			{ count(); return(RIGHT_ASSIGN); }
"<<="			{ count(); return(LEFT_ASSIGN); }
"+="			{ count(); return(ADD_ASSIGN); }
"-="			{ count(); return(SUB_ASSIGN); }
"*="			{ count(); return(MUL_ASSIGN); }
"/="			{ count(); return(DIV_ASSIGN); }
"%="			{ count(); return(MOD_ASSIGN); }
"&="			{ count(); return(AND_ASSIGN); }
"^="			{ count(); return(XOR_ASSIGN); }
"|="			{ count(); return(OR_ASSIGN); }
">>"			{ count(); return(RIGHT_OP); }
"<<"			{ count(); return(LEFT_OP); }
"++"			{ count(); return(INC_OP); }
"--"			{ count(); return(DEC_OP); }
"->"			{ count(); return(PTR_OP); }
"&&"			{ count(); return(AND_OP); }
"||"			{ count(); return(OR_OP); }
"<="			{ count(); return(LE_OP); }
">="			{ count(); return(GE_OP); }
"=="			{ count(); return(EQ_OP); }
"!="			{ count(); return(NE_OP); }
";"			{ count(); return(';'); }
("{"|"<%")		{ count(); return('{'); }
("}"|"%>")		{ count(); return('}'); }
","			{ count(); return(','); }
":"			{ count(); return(':'); }
"="			{ count(); return('='); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
("["|"<:")		{ count(); return('['); }
("]"|":>")		{ count(); return(']'); }
"."			{ count(); return('.'); }
"&"			{ count(); return('&'); }
"!"			{ count(); return('!'); }
"~"			{ count(); return('~'); }
"-"			{ count(); return('-'); }
"+"			{ count(); return('+'); }
"*"			{ count(); return('*'); }
"/"			{ count(); return('/'); }
"%"			{ count(); return('%'); }
"<"			{ count(); return('<'); }
">"			{ count(); return('>'); }
"^"			{ count(); return('^'); }
"|"			{ count(); return('|'); }
"?"			{ count(); return('?'); }
"@"			{ count(); return('@'); }
"`"			{ count(); return('`'); }

[ \t\v\n\f]		{ count(); }
.			{ /* ignore bad characters */ }

%%

int
yywrap()
{
	return 1;
}

static void
comment()
{
	char c, c1;

loop:
	while((c = input()) != '*' && c != 0){
		if(c == '\n'){
			ctx.inp->col = 0;
			ctx.inp->line++;
		}else
			ctx.inp->col++;
	}

	if((c1 = input()) != '/' && c != 0){
		ctx.inp->col++;	/* '*' */
		unput(c1);
		goto loop;
	}
}

static void
count()
{
	int i;

	for(i = 0; yytext[i] != '\0'; i++)
		if(yytext[i] == '\n'){
			ctx.inp->col = 0;
			ctx.inp->line++;
		}else
			ctx.inp->col++;
}

static int
check_type()
{
	yylval.id = yytext;
	if(tidlookup(ctx.ns, yylval.id))
		return TYPE_NAME;
	else
		return IDENTIFIER;
}

YYstate*
mkyystate(FILE *fp)
{
	YYstate *yy;
	yy = xmalloc(sizeof(YYstate));
	yy->yy = yy_create_buffer(fp, YY_BUF_SIZE);
	return yy;
}

void
freeyystate(YYstate *yy)
{
	yy_delete_buffer(yy->yy);
	free(yy);
}

void
setyystate(YYstate *yy)
{
	yy_switch_to_buffer(yy->yy);
}
