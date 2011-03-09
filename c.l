B			[0-1]
D			[0-9]
L			([a-zA-Z_$]|::|::~)
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
#include "syscqct.h"
#include "c.tab.h"

#define YY_DECL int yylex(YYSTYPE *lvalp, U *ctx)

struct YYstate {
	YY_BUFFER_STATE yy;
};

static void count(U *ctx);
static void comment(U *ctx, char *end);
static int chars(YYSTYPE *lvalp, int);

%}

%%
@include[ \t]*\"[^\"]+\"[ \t]*   { count(ctx); tryinclude(ctx, yytext); }
@include[ \t]*\<[^\<\>]+\>[ \t]* { count(ctx); tryinclude(ctx, yytext); }
<<EOF>>                 { if(maybepopyy(ctx) == 0) yyterminate(); }

\/\/.*			{ /* comment */ ctx->inp->src.col = 0; }
^#!			{ comment(ctx, "!#"); }
"/*"			{ comment(ctx, "*/"); }
"@const"		{ count(ctx); }
"@containerof"		{ count(ctx); return(CONTAINEROF); }
"@define"		{ count(ctx); return(DEFINE); }
"@global"		{ count(ctx); return(GLOBAL); }
"@lambda"		{ count(ctx); return(LAMBDA); }
"@lazyapply"		{ count(ctx); return(LAPPLY); }
"@let"			{ count(ctx); return(LET); }
"@local"		{ count(ctx); return(LOCAL); }
"@names"		{ count(ctx); return(NAMES); }
"@quote"		{ count(ctx); return(QUOTE); }
"@record"		{ count(ctx); return(DEFREC); }
"@typeof"		{ count(ctx); return(TYPEOF); }
"_Bool"			{ count(ctx); return(BOOL); }
"break"			{ count(ctx); return(BREAK); }
"case"			{ count(ctx); return(CASE); }
"char"			{ count(ctx); return(CHAR); }
"const"			{ count(ctx); return(CONST); }
"continue"		{ count(ctx); return(CONTINUE); }
"default"		{ count(ctx); return(DEFAULT); }
"do"			{ count(ctx); return(DO); }
"double"		{ count(ctx); return(DOUBLE); }
"else"			{ count(ctx); return(ELSE); }
"enum"			{ count(ctx); return(ENUM); }
"float"			{ count(ctx); return(FLOAT); }
"for"			{ count(ctx); return(FOR); }
"goto"			{ count(ctx); return(GOTO); }
"if"			{ count(ctx); return(IF); }
"int"			{ count(ctx); return(INT); }
"long"			{ count(ctx); return(LONG); }
"return"		{ count(ctx); return(RETURN); }
"short"			{ count(ctx); return(SHORT); }
"signed"		{ count(ctx); return(SIGNED); }
"sizeof"		{ count(ctx); return(SIZEOF); }
"struct"		{ count(ctx); return(STRUCT); }
"switch"		{ count(ctx); return(SWITCH); }
"typedef"		{ count(ctx); return(TYPEDEF); }
"union"			{ count(ctx); return(UNION); }
"unsigned"		{ count(ctx); return(UNSIGNED); }
"void"			{ count(ctx); return(VOID); }
"volatile"		{ count(ctx); return(VOLATILE); }
"while"			{ count(ctx); return(WHILE); }
\'\{			{ count(ctx); return(SYNTAXQUOTE); }
`\{			{ count(ctx); return(SYNTAXQUASI); }
,@\{			{ count(ctx); return(SYNTAXSPLICE); }

{L}({L}|{D})*		{ count(ctx); return(chars(lvalp, IDENTIFIER)); }

0[bB]{B}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
0[xX]{H}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
0{D}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
{D}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
L?\'{CHARC}\'		{ count(ctx); return(chars(lvalp, CONSTANT)); }
\'{L}({L}|{D})*		{ count(ctx); return(chars(lvalp, SYMBOL)); }
{D}+{E}{FS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
{D}*"."{D}+({E})?{FS}?	{ count(ctx); return(chars(lvalp, CONSTANT)); }
{D}+"."{D}*({E})?{FS}?	{ count(ctx); return(chars(lvalp, CONSTANT)); }
L?(\"{STRC}*\"[ \t\v\n\f]*)+ { count(ctx); return(chars(lvalp, STRING_LITERAL)); }
"..."			{ count(ctx); return(ELLIPSIS); }
">>="			{ count(ctx); return(RIGHT_ASSIGN); }
"<<="			{ count(ctx); return(LEFT_ASSIGN); }
"+="			{ count(ctx); return(ADD_ASSIGN); }
"-="			{ count(ctx); return(SUB_ASSIGN); }
"*="			{ count(ctx); return(MUL_ASSIGN); }
"/="			{ count(ctx); return(DIV_ASSIGN); }
"%="			{ count(ctx); return(MOD_ASSIGN); }
"&="			{ count(ctx); return(AND_ASSIGN); }
"^="			{ count(ctx); return(XOR_ASSIGN); }
"|="			{ count(ctx); return(OR_ASSIGN); }
"()="			{ count(ctx); return(CAST_ASSIGN); }
"{}="			{ count(ctx); return(XCAST_ASSIGN); }
">>"			{ count(ctx); return(RIGHT_OP); }
"<<"			{ count(ctx); return(LEFT_OP); }
"++"			{ count(ctx); return(INC_OP); }
"--"			{ count(ctx); return(DEC_OP); }
"->"			{ count(ctx); return(PTR_OP); }
"&&"			{ count(ctx); return(AND_OP); }
"||"			{ count(ctx); return(OR_OP); }
"<="			{ count(ctx); return(LE_OP); }
">="			{ count(ctx); return(GE_OP); }
"=="			{ count(ctx); return(EQ_OP); }
"!="			{ count(ctx); return(NE_OP); }
";"			{ count(ctx); return(';'); }
("{"|"<%")		{ count(ctx); return('{'); }
("}"|"%>")		{ count(ctx); return('}'); }
","			{ count(ctx); return(','); }
":"			{ count(ctx); return(':'); }
"="			{ count(ctx); return('='); }
"("			{ count(ctx); return('('); }
")"			{ count(ctx); return(')'); }
("["|"<:")		{ count(ctx); return('['); }
("]"|":>")		{ count(ctx); return(']'); }
"."			{ count(ctx); return('.'); }
"&"			{ count(ctx); return('&'); }
"!"			{ count(ctx); return('!'); }
"~"			{ count(ctx); return('~'); }
"-"			{ count(ctx); return('-'); }
"+"			{ count(ctx); return('+'); }
"*"			{ count(ctx); return('*'); }
"/"			{ count(ctx); return('/'); }
"%"			{ count(ctx); return('%'); }
"<"			{ count(ctx); return('<'); }
">"			{ count(ctx); return('>'); }
"^"			{ count(ctx); return('^'); }
"|"			{ count(ctx); return('|'); }
"?"			{ count(ctx); return('?'); }
"@"			{ count(ctx); return('@'); }
"`"			{ count(ctx); return('`'); }
"#"			{ count(ctx); return('#'); }

[ \t\v\n\f]		{ count(ctx); }
.			{ cerror(ctx, 0, "unexpected token: '%c'", yytext[0]); }
%%

/* Adapted from patch on bison-patches mailing list:
   Non-reentrant scanners generated by Flex 2.5.9 and later (and some earlier
   versions according to the Flex manual) leak memory if yylex_destroy is not
   invoked.  However, yylex_destroy is not defined before Flex 2.5.9.
   an implementation here that at least appears to work with Flex 2.5.4.  */
#if !defined(YY_FLEX_MAJOR_VERSION) || YY_FLEX_MAJOR_VERSION < 2 \
    || (YY_FLEX_MAJOR_VERSION == 2 \
        && (!defined(YY_FLEX_MINOR_VERSION) || YY_FLEX_MINOR_VERSION < 5 \
            || (YY_FLEX_MINOR_VERSION == 5 \
                && (!defined(YY_FLEX_SUBMINOR_VERSION) \
                    || YY_FLEX_SUBMINOR_VERSION < 9))))
int
yylex_destroy()
{
	return 0;
}
#endif

int
yywrap()
{
	return 1;
}

static void
comment(U *ctx, char *end)
{
	char c;

loop:
	while((c = input()) != end[0] && c != EOF && c != 0){
		if(c == '\n'){
			ctx->inp->src.col = 0;
			ctx->inp->src.line++;
		}else
			ctx->inp->src.col++;
	}
	if(c == EOF || c == 0)
		parseerror(ctx, "end of input inside comment");
	if((c = input()) != end[1] && c != EOF && c != 0){
		ctx->inp->src.col++;	/* end[0] */
		unput(c);
		goto loop;
	}
	if(c == EOF || c == 0)
		parseerror(ctx, "end of input inside comment");
}

static void
count(U *ctx)
{
	int i;

	for(i = 0; yytext[i] != '\0'; i++)
		if(yytext[i] == '\n'){
			ctx->inp->src.col = 0;
			ctx->inp->src.line++;
		}else
			ctx->inp->src.col++;
}

static int
chars(YYSTYPE *lvalp, int token)
{
	lvalp->chars.p = yytext;
	lvalp->chars.len = strlen(yytext);
	return token;
}

YYstate*
mkyystate(FILE *fp)
{
	YYstate *yy;
	yy = emalloc(sizeof(YYstate));
//	yy->yy = yy_create_buffer(fp, YY_BUF_SIZE);
	return yy;
}

YYstate*
mkyystatestr(char *buf)
{
	YYstate *yy;
	yy = emalloc(sizeof(YYstate));
	yy->yy = yy_scan_string(buf);
	return yy;
}

void
freeyystate(YYstate *yy)
{
	yy_delete_buffer(yy->yy);
	efree(yy);
}

void
setyystate(YYstate *yy)
{
	yy_switch_to_buffer(yy->yy);
}
