B			[0-1]
D			[0-9]
L			([a-zA-Z_$]|::|::~)
H			[a-fA-F0-9]
O			[0-7]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L|k|K|m|M|g|G|t|T)*
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
static int datalen(U *ctx, YYSTYPE *lvalp);
static int datadelim(U *ctx, YYSTYPE *lvalp);
static int databracket(U *ctx, YYSTYPE *lvalp);

%}

%%
@include[ \t]*\"[^\<\>\"]+\"[ \t]*   { count(ctx); tryinclude(ctx, yytext); }
@include[ \t]*\<[^\<\>\"]+\>[ \t]*   { count(ctx); tryinclude(ctx, yytext); }
<<EOF>>                 { if(maybepopyy(ctx) == 0) yyterminate(); }

\/\/.*			{ /* comment */ ctx->inp->src.col = 0; }
^#!			{ comment(ctx, "!#"); }
"/*"			{ comment(ctx, "*/"); }
#\'\{			{ count(ctx); return(SYNTAXQUOTE); }
#`\{			{ count(ctx); return(SYNTAXQUASI); }
#,			{ count(ctx); return(SYNTAXUNQUOTE); }
#\[			{ count(ctx); return(SYNTAXLIST); }
#,@			{ count(ctx); return(SYNTAXSPLICE); }
#{L}({L}|{D})*		{ count(ctx); return(chars(lvalp, SYNTAXID)); }
"@const"		{ count(ctx); }
"@containerof"		{ count(ctx); return(CONTAINEROF); }
"@define"		{ count(ctx); return(DEFINE); }
"@defloc"		{ count(ctx); return(DEFLOC); }
"@defstx"		{ count(ctx); return(DEFSTX); }
"@db"			{ return(databracket(ctx, lvalp)); }
"@dd"			{ return(datadelim(ctx, lvalp)); }
"@dl"			{ return(datalen(ctx, lvalp)); }
"@file"			{ count(ctx); return(ATFILE); }
"@global"		{ count(ctx); return(GLOBAL); }
"@lambda"		{ count(ctx); return(LAMBDA); }
"@lazyapply"		{ count(ctx); return(LAPPLY); }
"@let"			{ count(ctx); return(LET); }
"@line"			{ count(ctx); return(ATLINE); }
"@local"		{ count(ctx); return(LOCAL); }
"@match"		{ count(ctx); return(MATCH); }
"@names"		{ count(ctx); return(NAMES); }
"@_record"		{ count(ctx); return(DEFREC); }
"@typename"		{ count(ctx); return(TYPENAME); }
"@typeof"		{ count(ctx); return(TYPEOF); }
@{L}({L}|{D})*		{ count(ctx); return(chars(lvalp, ATIDENTIFIER)); }
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
{L}({L}|{D})*		{ count(ctx); return(chars(lvalp, IDENTIFIER)); }
0[bB]{B}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
0[xX]{H}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
0{D}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
{D}+{IS}?		{ count(ctx); return(chars(lvalp, CONSTANT)); }
L?\'{CHARC}\'		{ count(ctx); return(chars(lvalp, CONSTANT)); }
\'@?{L}({L}|{D})*	{ count(ctx); return(chars(lvalp, SYMBOL)); }
\'%?{L}({L}|{D})*	{ count(ctx); return(chars(lvalp, SYMBOL)); }
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
">>"			{ count(ctx); return(RIGHT_OP); }
"<<"			{ count(ctx); return(LEFT_OP); }
"++"			{ count(ctx); return(INC_OP); }
"--"			{ count(ctx); return(DEC_OP); }
"->"			{ count(ctx); return(PTR_OP); }
"&&"			{ count(ctx); return(AND_OP); }
"||"			{ count(ctx); return(OR_OP); }
"<="			{ count(ctx); return(LE_OP); }
">="			{ count(ctx); return(GE_OP); }
"<|"			{ count(ctx); return(LPAIR); }
"|>"			{ count(ctx); return(RPAIR); }
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
"^^"			{ count(ctx); return (NOBIND_PRE); }
"|"			{ count(ctx); return('|'); }
"?"			{ count(ctx); return('?'); }
"@"			{ count(ctx); return('@'); }
"`"			{ count(ctx); return('`'); }

[ \t\v\n\f\r]		{ count(ctx); }
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
	int c;

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
updateln(U *ctx, char c) {
	if(c == '\n'){
		ctx->inp->src.col = 0;
		ctx->inp->src.line++;
	}else
		ctx->inp->src.col++;
}

static void
count(U *ctx)
{
	int i;

	for(i = 0; yytext[i] != '\0'; i++)
		updateln(ctx, yytext[i]);
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
mkyystatestr(char *buf, u64 len)
{
	YYstate *yy;
	yy = emalloc(sizeof(YYstate));
	yy->yy = yy_scan_bytes(buf, len);
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

static int datalen(U *ctx, YYSTYPE *lvalp) {
	int c, i, len;
	char *s, *err;

	err = "";

	while ( (c = input()) != EOF && isspace(c) )
		updateln(ctx, (char)c);
	if (EOF == c)
		parseerror(ctx, "unexpected EOF");

	if (!isdigit(c))
		parseerror(ctx, "unexpected a digit");

	len = c-'0';
	while ( (c = input()) != EOF && isdigit(c)) {
		updateln(ctx, (char)c);
		len = len*10 + (c-'0');
	}
	if (EOF == c)
		parseerror(ctx, "unexpected EOF");

	if (len <= 0)
		parseerror(ctx, "bad length");

	s = (char*)emalloc(len);
	if (0 == s)
		parseerror(ctx, "out of memory");

	s[0] = (char)c;

	for(i=1; i<len; ++i) {
		c = input();
		if (c == EOF) {
			err = "unexpected EOF";
			goto error;
		}
		updateln(ctx, (char)c);
		s[i] = (char)c;
	}

	lvalp->chars.p = s;
	lvalp->chars.len = len;

	return DATA;

error:
	efree(s);
	parseerror(ctx, err);

	//shouldn't get here
	return -1;
}

static void
addtostr(char **p_s, int *p_len, int off, char c) {
	char *s;
	if (off >= *p_len) {
		s = emalloc(2*(*p_len));
		memcpy(s, *p_s, *p_len);
		*p_len = 2 * (*p_len);
		efree(*p_s);
		*p_s = s;
	}
	(*p_s)[off] = c;
}

static int
datadelim(U *ctx, YYSTYPE *lvalp) {
	char *s, *d;
	int c, k, dsz, dlen, ssz, slen;

	while ( (c = input()) != EOF && isspace(c) )
		updateln(ctx, (char)c);
	if (EOF == c)
		parseerror(ctx, "unexpected EOF");

	dsz = 0x20;
	d = emalloc(dsz);
	dlen = 0;

	addtostr(&d, &dsz, dlen++, '\n');
	addtostr(&d, &dsz, dlen++, (char)c);

	while ( ( c = input() ) != EOF && !isspace(c) ) {
		updateln(ctx, (char)c);
		addtostr(&d, &dsz, dlen++, (char)c);
	}
	if (EOF == c) {
		efree(d);
		parseerror(ctx, "unexpected EOF");
	}

	ssz = 0x20;
	s = emalloc(ssz);
	slen = 0;

	k = 0;

	while ( ( c = input() ) != EOF ) {
		updateln(ctx, (char)c);
		if (d[k] == c) {
			if (++k == dlen) break;
		} else if (d[0] == c) k = 1;
		else k = 0;
		addtostr(&s, &ssz, slen++, (char)c);
	}
	efree(d);

	if (EOF == c) {
		efree(s);
		parseerror(ctx, "unexpected EOF");
	}

	lvalp->chars.p = s;
	lvalp->chars.len = slen - dlen + 1;

	return DATA;
}

static int
databracket(U *ctx, YYSTYPE *lvalp) {
	char *s, *d;
	char *matched, *lmatched, *tmp;
	//sz is the size of the buffer, len is the nubmer of elements in it
	int dsz, dlen, ssz, slen;
	int k, c;

	//maps '(' -> ')', '[' -> ']', '{' -> '}', and '<' -> '>'
	static char matching_bracket[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,')',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		'>',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,']',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,'}',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	while( (c = input()) != EOF && isspace(c) )
		updateln(ctx, (char)c);
	if(EOF == c)
		parseerror(ctx, "unexpected EOF");

	dsz = 0x20;
	d = emalloc(dsz);
	dlen = 0;


	if(!matching_bracket[c]) {

		addtostr(&d, &dsz, dlen++, (char)c);

		while ( ( c = input() ) != EOF && !matching_bracket[c] ) {
			updateln(ctx, (char)c);
			addtostr(&d, &dsz, dlen++, (char)c);
		}

		if (EOF == c) {
			efree(d);
			parseerror(ctx, "unexpected EOF");
		}
	}

	addtostr(&d, &dsz, dlen++, matching_bracket[c]);

	matched = (char*)emalloc(dlen);
	memset(matched, 0, dlen);
	lmatched = (char*)emalloc(dlen);
	memset(lmatched, 0, dlen);

	ssz = 0x20;
	s = emalloc(ssz);
	slen = 0;

	while ( ( c = input() ) != EOF ) {
		updateln(ctx, (char)c);

		tmp = matched;
		matched = lmatched;
		lmatched = tmp;

		matched[0] = (d[0] == c) ? 1 : 0;
		for(k=0; k<dlen-1; ++k) {
			matched[k+1] = (lmatched[k] && d[k+1] == c) ? 1 : 0;
		}

		//k == dlen-1
		if(matched[k]) break;

		addtostr(&s, &ssz, slen++, (char)c);
	}
	efree(d);
	efree(lmatched);
	efree(matched);

	if (EOF == c) {
		efree(s);
		parseerror(ctx, "could not find close of @db");
	}

	lvalp->chars.p = s;
	lvalp->chars.len = slen - dlen + 1;

	return DATA;

}

// vim:noet:ts=8:sw=8
