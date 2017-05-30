#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void printcqct0(Expr *e, unsigned ni);

enum {
	Abbrevlen = 10,
};

char* S[] = {
	[Ebool] =	"bool",
	[Echar] =	"char",
	[Ecomplex] =	"complex",
	[Edouble] =	"double",
	[Efloat] =	"float",
	[Eint] =	"int",
	[Elong] =	"long",
	[Eshort] =	"short",
	[Esigned] =	"signed",
	[Eunsigned] =	"unsigned",
	[Evoid] =	"void",
	[Enbase] =	"!!BUG!!",

	[Eadd] =	"add",
	[Eambig] = 	"ambig",
	[Earef] =	"aref",
	[Earr] =	"arr",
	[Earrow] =	"arrow",
	[Ebase] =	"base",
	[Eband] =	"band",
	[Ebitfield] =	"bitfield",
	[Eblock] =	"block",
	[Ebor] =	"bor",
	[Eboxref] =	"boxref",
	[Eboxset] =	"boxset",
	[Ebreak] =	"break",
	[Ebxor] =	"bxor",
	[Ecall] =	"call",
	[Ecase] =	"case",
	[Ecast] =	"cast",
	[Ecomma] =	"comma",
	[Econd] =	"cond",
	[Econtainer] =	"container",
	[Econtinue] =	"continue",
	[Edecl] =	"decl",
	[Edecls] =	"decls",
	[Edefault] =	"default",
	[Edefine] =	"define",
	[Edefloc] =	"defloc",
	[Edefstx] =	"defstx",
	[Ederef] =	"deref",
	[Ediv] =	"div",
	[Edo] =		"do",
	[Edot] =	"dot",
	[Edotdot] =	"dotdot",
	[Eelist] =	"elist",
	[Eellipsis] =	"ellipsis",
	[Eenum] =	"enum",
	[Eenumel] =	"enumel",
	[Eeq] =		"eq",
	[Efieldoff] =	"fieldoff",
	[Efields] =	"fields",
	[Efor] =	"for",
	[Efun] =	"fun",
	[Eg] =		"g",
	[Egadd] = 	"gadd",
	[Egband] =	"gband",
	[Egbor] =	"gbor",
	[Egbxor] =	"gbxor",
	[Egdiv] =	"gdiv",
	[Ege] =		"ge",
	[Eglobal] =	"global",
	[Egmod] =	"gmod",
	[Egmul] =	"gmul",
	[Egoto] =	"goto",
	[Egshl] =	"gshl",
	[Egshr] =	"gshr",
	[Egsub] = 	"gsub",
	[Egt] =		"gt",
	[Eid] =		"id",
	[Eif] =		"if",
	[Elabel] =	"label",
	[Elambda] =	"lambda",
	[Eland] =	"land",
	[Elapply] =	"lapply",
	[Ele] =		"le",
	[Eletrec] =	"letrec",
	[Elist] =	"list",
	[Epair] =	"pair",
	[Elor] =	"lor",
	[Elt] =		"lt",
	[Emcall] =	"mcall",
	[Ematch] =	"match",
	[Emkbox] =	"mkbox",
	[Emkbox0] =	"mkbox0",
	[Emkctype] =	"mkctype",
	[Emod] =	"mod",
	[Emul] =	"mul",
	[Enames] =	"names",
	[Eneq] =	"neq",
	[Enil] =	"nil",
	[Enop] =	"nop",
	[Enull] =	"null",
	[Eorpat] = 	"orpat",
	[Epostdec] =	"postdec",
	[Epostinc] =	"postinc",
	[Epredec] =	"predec",
	[Epreinc] =	"preinc",
	[Eptr] =	"ptr",
	[Eref] =	"ref",
	[Eret] =	"ret",
	[Escope] =	"scope",
	[Eshl] =	"shl",
	[Eshr] =	"shr",
	[Esizeofe] =	"sizeofe",
	[Esizeoft] =	"sizeoft",
	[Estruct] =	"struct",
	[Estx] =	"stx",
	[Estxlist] =	"stxlist",
	[Estxquote] =	"stxquote",
	[Estxquasi] =	"stxquasi",
	[Estxsplice] =	"stxsplice",
	[Estxunquote] =	"stxunquote",
	[Esub] =	"sub",
	[Eswitch] =	"switch",
	[Etab] =	"tab",
	[Eticke] = 	"ticke",
	[Etickt] = 	"tickt",
	[Etypedef] =	"typedef",
	[Etypename] =	"typename",
	[Etypeofe] =	"typeofe",
	[Etypeoft] =	"typeoft",
	[Etypespec] =	"typespec",
	[Euminus] =	"uminus",
	[Eunion] =	"union",
	[Eunot] =	"unot",
	[Euplus] =	"uplus",
	[Eutwiddle] =	"utwiddle",
	[Eval] =	"val",
	[Ewhile] =	"while",
	[Excast] =	"xcast",

	[E_tg] =	"_tg",
	[E_tid] =	"_tid",

	[E_attr] =	"_attr",
};

void
printexpr(Expr *e)
{
	Expr *p;

	switch(e->kind){
	case Eval:
		xprintf("(Eval ");
		printval(e->aux);
		xprintf(")");
		break;
	case Eid:
		xprintf("(%s ", S[e->kind]);
		printval(e->aux);
		xprintf(")");
		break;
	case Eelist:
		p = e;
		xprintf("(");
		while(!isnull(p)){
			printexpr(p->e1);
			p = p->e2;
			if(!isnull(p))
				xprintf(" ");
		}
		xprintf(")");
		break;
	default:
		xprintf("(%s", S[e->kind]);
		if(e->e1){
			xprintf(" ");
			printexpr(e->e1);
		}
		if(e->e2){
			xprintf(" ");
			printexpr(e->e2);
		}
		if(e->e3){
			xprintf(" ");
			printexpr(e->e3);
		}
		if(e->e4){
			xprintf(" ");
			printexpr(e->e4);
		}
		xprintf(")");
	}
}

static void
indent(unsigned nindent)
{
	while(nindent--)
		xprintf("\t");
}

static int
printlocals(Expr *e, unsigned ni)
{
	Expr *p;
	unsigned cnt = 0;

	p = e;
	while(p->kind != Enull){
		if(!cnt){
			indent(ni); xprintf("@local ");
		}else
			xprintf(", ");
		cnt++;
		printval(p->e1->aux);
		p = p->e2;
	}
	if(cnt)
		xprintf(";");
	return cnt;
}

/* E must be an Eelist of Eids or Eellipsis */
void
printids(Expr *e)
{
	Expr *p;
	if(e->kind == Enull)
		return;
	printcqct0(e->e1, 0);
	p = e->e2;
	while(p->kind != Enull){
		xprintf(", ");
		printcqct0(p->e1, 0);
		p = p->e2;
	}
}

static void
printargs(Expr *e, unsigned ni)
{
	Expr *p;
	if(e->kind == Enull)
		return;
	printcqct0(e->e1, ni);
	p = e->e2;
	while(p->kind != Enull){
		xprintf(", ");
		printcqct0(p->e1, ni);
		p = p->e2;
	}
}

static void
printbind(Expr *e, unsigned ni)
{
	xprintf("[ ");
	printcqct0(e->e1->e1, ni);
	xprintf(" = ");
	printcqct0(e->e1->e2->e1, ni);
	xprintf(" ]");
}

static void
printbinds(Expr *e, unsigned ni)
{
	Expr *p;
	p = e;
	if(p->kind == Enull)
		return;
	printbind(p, ni);
	p = p->e2;
	while(p->kind != Enull){
		xprintf(",\n");
		printbind(p, ni);
		p = p->e2;
	}
}

static char* Opstr[Emax] = {
	[Eadd] = "+",
	[Epreinc] = "++",
	[Epostinc] = "++",
	[Esub] = "-",
	[Epredec] = "--",
	[Epostdec] = "--",
	[Emul] = "*",
	[Ediv] = "/",
	[Emod] = "%",
	[Eshl] = "<<",
	[Eshr] = ">>",
	[Eband] = "&",
	[Ebor] = "|",
	[Ebxor] = "^",
	[Egt] = ">",
	[Ege] = ">=",
	[Elt] = "<",
	[Ele] = "<=",
	[Eeq] = "==",
	[Eneq] = "!=",
	[Euminus] = "-",
	[Eunot] = "!",
	[Euplus] = "+",
	[Eutwiddle] = "~",
	[Elor] = "||",
	[Eland] = "&&",
	[Egadd] = "+=",
	[Egband] = "&=",
	[Egbor] = "|=",
	[Egbxor] = "^=",
	[Egdiv]	= "/=",
	[Egmod]	= "%=",
	[Egmul] = "*=",
	[Egshl]	= "<<=",
	[Egshr]	= ">>=",
	[Egsub] = "-=",
};

static char*
opstr(unsigned op)
{
	return Opstr[op] ? Opstr[op] : "";
}

static int
iscompound(Expr *e)
{
	return e->kind == Eblock || e->kind == Escope;
}

static void
printcqct0(Expr *e, unsigned ni)
{
	int wasstmt = 0;

	if(e == 0)
		return;

	switch(e->kind){
	case Ecast:
		xprintf("(");
		printcqct0(e->e1, ni);
		xprintf(")");
		printcqct0(e->e2, ni);
		break;
	case Eticke:
	case Etickt:
		printcqct0(e->e1, ni);
		xprintf("`");
		printcqct0(e->e2, ni);
		break;
	case Edefstx:
		xprintf("@defstx ...");
		break;
	case Edefine:
	case Edefloc:
		if(e->kind == Edefine)
			xprintf("@define ");
		else
			xprintf("@defloc ");
		printcqct0(e->e1, ni);
		if(e->e2->kind == Eid){
			xprintf(" ");
			printcqct0(e->e2, ni);
		}else{
			xprintf("(");
			printargs(e->e2, ni);
			xprintf(")");
		}
		printcqct0(e->e3, ni);
		break;
	case Enil:
		xprintf("nil");
		break;
	case Eval:
		printval(e->aux);
		break;
	case Eid:
		printval(e->aux);
		break;
	case E_tid:
		xprintf("#");
		printcqct0(e->e1, ni);
		break;
	case E_tg:
		printcqct0(e->e1, ni);
		xprintf(" = ");
		printcqct0(e->e2, ni);
		break;
	case Eellipsis:
		xprintf("...");
		break;
	case Elambda:
		xprintf("@lambda");
		if(e->e1->kind == Eid){
			xprintf(" ");
			printcqct0(e->e1, ni);
			xprintf(" ");
		}else{
			xprintf("(");
			printargs(e->e1, ni);
			xprintf(")");
		}
		xprintf("\n");
		printcqct0(e->e2, ni);
		break;
	case Eletrec:
		xprintf("letrec(");
		xprintf("[");
		printbinds(e->e1, ni);
		xprintf("], ");
		printcqct0(e->e2, ni);
		xprintf(")");
		break;
	case Ecall:
		printcqct0(e->e1, ni);
		xprintf("(");
		printargs(e->e2, ni);
		xprintf(")");
		break;
	case Escope:
		printcqct0(e->e1, ni);
		break;
	case Eblock:
		indent(ni); xprintf("{\n");
		if(printlocals(e->e1, ni+1) && !iscompound(e->e2->e1))
			xprintf("\n");
		printcqct0(e->e2, ni+1);
		indent(ni); xprintf("}");
		break;
	case Eglobal:
		xprintf("@global ");
		printargs(e->e1, ni);
		xprintf(";");
		break;
	case Eg:
		printcqct0(e->e1, ni);
		xprintf(" = ");
		printcqct0(e->e2, ni);
		break;
	case EGOP:
		printcqct0(e->e1, ni);
		xprintf(" %s ", opstr(e->kind));
		printcqct0(e->e2, ni);
		break;
	case Euminus:
	case Eunot:
	case Euplus:
	case Eutwiddle:
		xprintf("%s", opstr(e->kind));
		printcqct0(e->e1, ni);
		break;
	case Excast:
		xprintf("{");
		printcqct0(e->e1, ni);
		xprintf("}(");
		printcqct0(e->e2, ni);
		xprintf(")");
		break;
	 case Eadd:
	 case Esub:
	 case Emul:
	 case Ediv:
	 case Emod:
	 case Eshl:
	 case Eshr:
	 case Eband:
	 case Ebor:
	 case Ebxor:
	 case Egt:
	 case Ege:
	 case Elt:
	 case Ele:
	 case Eeq:
	 case Eneq:
		 xprintf("(");
		 printcqct0(e->e1, ni);
		 xprintf(" %s ", opstr(e->kind));
		 printcqct0(e->e2, ni);
		 xprintf(")");
		 break;
	case Eland:
	case Elor:
		xprintf("(");
		printcqct0(e->e1, ni);
		xprintf(" %s ", opstr(e->kind));
		printcqct0(e->e2, ni);
		xprintf(")");
		break;
	case Enull:
	case Enop:
		break;
	case Epreinc:
	case Epredec:
		if(e->e1->kind != Eid){
			xprintf("%s(", opstr(e->kind));
			printcqct0(e->e1, ni);
			xprintf(")");
		}else{
			xprintf("%s", opstr(e->kind));
			printcqct0(e->e1, ni);
		}
		break;
	case Epostinc:
	case Epostdec:
		if(e->e1->kind != Eid){
			xprintf("(");
			printcqct0(e->e1, ni);
			xprintf(")%s", opstr(e->kind));
		}else{
			printcqct0(e->e1, ni);
			xprintf("%s", opstr(e->kind));
		}
		break;
	case Eref:
		xprintf("&(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Ederef:
		xprintf("*");
		printcqct0(e->e1, ni);
		break;
	case Earef:
		xprintf("(");
		printcqct0(e->e1, ni);
		xprintf(")[");
		printcqct0(e->e2, ni);
		xprintf("]");
		break;
	case Edot:
		xprintf("(");
		printcqct0(e->e1, ni);
		xprintf(").");
		printcqct0(e->e2, ni);
		break;
	case Earrow:
		xprintf("(");
		printcqct0(e->e1, ni);
		xprintf(")->");
		printcqct0(e->e2, ni);
		break;
	case Eboxref:
		xprintf("boxref(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Eboxset:
		xprintf("boxset(");
		printcqct0(e->e1, ni);
		xprintf(", ");
		printcqct0(e->e2, ni);
		xprintf(")");
		break;
	case Emkbox:
		xprintf("mkbox(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Emkbox0:
		xprintf("mkbox0(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Enames:
		xprintf("@names ...");
		break;
	case Eelist:
		if(ni > 0 &&
		   (e->e1->kind == Edefault
		    || e->e1->kind == Ecase
		    || e->e1->kind == Ematch
		    || e->e1->kind == Elabel))
			indent(ni-1);
		else if(e->e1->kind != Eblock)
			indent(ni);
		printcqct0(e->e1, ni);
		if(e->e1->kind == Edefault
		   || e->e1->kind == Ecase
		   || e->e1->kind == Ematch
		   || e->e1->kind == Elabel)
			xprintf("\n");
		else
			xprintf(";\n");
		printcqct0(e->e2, ni);
		break;
	case Eswitch:
		xprintf("switch(");
		printcqct0(e->e1, ni);
		xprintf(")");
		printcqct0(e->e2, ni);
		break;
	case Elabel:
		printcqct0(e->e1, ni);
		xprintf(":");
		break;
	case Edefault:
		xprintf("default:\n");
		indent(ni); printcqct0(e->e1, ni);
		xprintf(";");
		break;
	case Ecase:
		xprintf("case ");
		printcqct0(e->e1, ni);
		xprintf(":\n");
		indent(ni); printcqct0(e->e2, ni);
		xprintf(";");
		break;
	case Ematch:
		xprintf("@match ");
		printcqct0(e->e1, ni);
		xprintf(":\n");
		indent(ni); printcqct0(e->e2, ni);
		xprintf(";");
		break;
	case Econtinue:
		xprintf("continue");
		break;
	case Ebreak:
		xprintf("break");
		break;
	case Ewhile:
		xprintf("while(");
		printcqct0(e->e1, ni);
		xprintf(")");
		if(!iscompound(e->e2)){
			xprintf("\n");
			indent(ni+1);
			printcqct0(e->e2, ni+1);
		}else{
			xprintf("\n");
			printcqct0(e->e2, ni);
		}
		break;
	case Edo:
		xprintf("do");
		if(!iscompound(e->e1)){
			xprintf("\n");
			indent(ni+1);
			printcqct0(e->e1, ni+1);
			xprintf("\n");
			indent(ni);
		}else{
			xprintf("\n");
			printcqct0(e->e1, ni);
		}

		xprintf("while(");
		printcqct0(e->e2, ni);
		xprintf(")");
		break;
	case Efor:
		xprintf("for(");
		if(e->e1)
			printcqct0(e->e1, ni);
		xprintf("; ");
		if(e->e2)
			printcqct0(e->e2, ni);
		xprintf("; ");
		if(e->e3)
			printcqct0(e->e3, ni);
		xprintf(")");
		if(!iscompound(e->e4)){
			xprintf("\n");
			indent(ni+1);
			printcqct0(e->e4, ni+1);
		}else{
			xprintf("\n");
			printcqct0(e->e4, ni);
		}
		break;
	case Eif:
		xprintf("if(");
		printcqct0(e->e1, ni);
		xprintf(")");
		if(!iscompound(e->e2)){
			xprintf("\n");
			indent(ni+1);
			printcqct0(e->e2, ni+1);
			wasstmt = 1;
		}else{
			xprintf("\n");
			printcqct0(e->e2, ni);
		}
		if(e->e3){
			if(wasstmt)
				xprintf(";");
			xprintf("\n");
			indent(ni); xprintf("else");
			if(!iscompound(e->e3)){
				xprintf("\n");
				indent(ni+1);
				printcqct0(e->e3, ni+1);
			}else{
				xprintf("\n");
				printcqct0(e->e3, ni);
			}
		}
		break;
	case Egoto:
		xprintf("goto ");
		printcqct0(e->e1, ni);
		break;
	case Eret:
		xprintf("return");
		if(e->e1){
			xprintf(" ");
			printcqct0(e->e1, ni);
		}
		break;
	case Emcall:
		printcqct0(e->e1, ni);
		if(e->e2){
			xprintf("(");
			printargs(e->e2, ni);
			xprintf(")");
		}
		xprintf("\n");
		printcqct0(e->e3, ni);
		break;
	case Estxquote:
		xprintf("quote(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Estxunquote:
		xprintf("unquote(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Estxsplice:
		xprintf("splice(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Estxquasi:
		xprintf("quasi(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case E_attr:
		xprintf("attr(");
		printcqct0(e->e1, ni);
		xprintf(", ");
		printcqct0(e->e2, ni);
		xprintf(")");
		break;
	default:
		xprintf("[%d]???", e->kind);
		break;
	}

}

void
printcqct(Expr *e)
{
	printcqct0(e, 0);
}

Kind
s2kind(char *s)
{
	Kind k;

	for(k = 0; k < Emax; k++){
		if(S[k] == 0)
			continue;
		if(!strcmp(S[k], s))
			return k;
	}
	return Euser;
}
