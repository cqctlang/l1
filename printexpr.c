#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void printcqct0(Expr *e, unsigned ni);

enum {
	Abbrevlen = 10,
};

char* S[] = {
	[Ebool] =	"Ebool",
	[Echar] =	"Echar",
	[Edouble] =	"Edouble",
	[Efloat] =	"Efloat",
	[Eint] =	"Eint",
	[Elong] =	"Elong",
	[Eshort] =	"Eshort",
	[Esigned] =	"Esigned",
	[Eunsigned] =	"Eunsigned",
	[Evoid] =	"Evoid",
	[Enbase] =	"huh?",

	[Eadd] =	"Eadd",
	[Eambig] = 	"Eambig",
	[Earef] =	"Earef",
	[Earr] =	"Earr",
	[Earrow] =	"Earrow",
	[Ebase] =	"Ebase",
	[Eband] =	"Eband",
	[Ebinop] =	"Ebinop",
	[Ebitfield] =	"Ebitfield",
	[Eblock] =	"Eblock",
	[Ebor] =	"Ebor",
	[Ebreak] =	"Ebreak",
	[Ebxor] =	"Ebxor",
	[Ecall] =	"Ecall",
	[Ecase] =	"Ecase",
	[Ecast] =	"Ecast",
	[Ecomma] =	"Ecomma",
	[Econd] =	"Econd",
	[Econtainer] =	"Econtainer",
	[Econtinue] =	"Econtinue",
	[Edecl] =	"Edecl",
	[Edecls] =	"Edecls",
	[Edefault] =	"Edefault",
	[Edefconst] =	"Edefconst",
	[Edefine] =	"Edefine",
	[Edefloc] =	"Edefloc",
	[Edefrec] =	"Edefrec",
	[Ederef] =	"Ederef",
	[Ediv] =	"Ediv",
	[Edo] =		"Edo",
	[Edot] =	"Edot",
	[Edotdot] =	"Edotdot",
	[Eelist] =	"Eelist",
	[Eellipsis] =	"Eellipsis",
	[Eenum] =	"Eenum",
	[Eenumel] =	"Eenumel",
	[Eeq] =		"Eeq",
	[Efieldoff] =	"Efieldoff",
	[Efields] =	"Efields",
	[Efor] =	"Efor",
	[Efun] =	"Efun",
	[Eg] =		"Eg",
	[Ege] =		"Ege",
	[Eglobal] =	"Eglobal",
	[Egop] =	"Egop",
	[Egoto] =	"Egoto",
	[Egt] =		"Egt",
	[Eid] =		"Eid",
	[Eif] =		"Eif",
	[Ekon] =	"Ekon",
	[Elabel] =	"Elabel",
	[Elambda] =	"Elambda",
	[Eland] =	"Eland",
	[Elapply] =	"Elapply",
	[Ele] =		"Ele",
	[Eletrec] =	"Eletrec",
	[Elist] =	"Elist",
	[Epair] =	"Epair",
	[Elor] =	"Elor",
	[Elt] =		"Elt",
	[Emkctype] =	"Emkctype",
	[Emod] =	"Emod",
	[Emul] =	"Emul",
	[Enames] =	"Enames",
	[Eneq] =	"Eneq",
	[Enil] =	"Enil",
	[Enop] =	"Enop",
	[Enull] =	"Enull",
	[Epostdec] =	"Epostdec",
	[Epostinc] =	"Epostinc",
	[Epredec] =	"Epredec",
	[Epreinc] =	"Epreinc",
	[Eptr] =	"Eptr",
	[Equote] =	"Equote",
	[Eref] =	"Eref",
	[Eret] =	"Eret",
	[Escope] =	"Escope",
	[Eshl] =	"Eshl",
	[Eshr] =	"Eshr",
	[Esizeofe] =	"Esizeofe",
	[Esizeoft] =	"Esizeoft",
	[Estruct] =	"Estruct",
	[Estxquote] =	"Estxquote",
	[Estxquasi] =	"Estxquasi",
	[Estxunquote] =	"Estxunquote",
	[Esub] =	"Esub",
	[Eswitch] =	"Eswitch",
	[Etab] =	"Etab",
	[Eticke] = 	"Eticke",
	[Etickt] = 	"Etickt",
	[Etid] = 	"Etid",
	[Etypedef] =	"Etypedef",
	[Etypename] =	"Etypename",
	[Etypeofe] =	"Etypeofe",
	[Etypeoft] =	"Etypeoft",
	[Etypespec] =	"Etypespec",
	[Euminus] =	"Euminus",
	[Eunion] =	"Eunion",
	[Eunot] =	"Eunot",
	[Euplus] =	"Euplus",
	[Eutwiddle] =	"Eutwiddle",
	[Ewhile] =	"Ewhile",
	[Excast] =	"Excast",

	[E_tg] =	"E_tg",
	[E_tid] =	"E_tid",
};

#if 0
static void
abbrevlits(char *buf, unsigned len, Lits *lits)
{
	char *p, *e, ch;
	unsigned m;
	p = buf;
	e = buf+len-1;
	m = 0;
	if(lits->len >= len)
		e -= 3;
	while(p < e){
		if(m >= lits->len)
			break;
		ch = lits->s[m];
		if(ch == '\n')
			break;
		if(ch == '"')
			break;
		*p++ = ch;
		m++;
	}
	if(lits->len >= len)
		for(m = 0; m < 3; m++)
			*p++ = '.';
	*p = 0;
}
#endif

void
printexpr(Expr *e)
{
	Expr *p;

	switch(e->kind){
	case Ekon:
		xprintf("(Ekon ");
		printkon(e->aux);
		xprintf(")");
		break;
	case E_tid:
	case Elabel:
	case Egoto:
	case Eid:
		xprintf("(%s ", S[e->kind]);
		printkon(e->aux);
		xprintf(")");
		break;
	case Ebinop:
		xprintf("(%s ", S[e->op]);
		printexpr(e->e1);
		xprintf(" ");
		printexpr(e->e2);
		xprintf(")");
		break;
	case Egop:
		xprintf("(Egop %s ", S[e->op]);
		printexpr(e->e1);
		xprintf(" ");
		printexpr(e->e2);
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
		printkon(p->e1->aux);
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
	case Edefconst:
		printcqct0(e->e1, ni);
		xprintf(" = ");
		printcqct0(e->e2, ni);
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
	case Edefrec:
		xprintf("@record ");
		printcqct0(e->e1, ni);
		xprintf("{ ");
		printargs(e->e2, ni);
		xprintf(" }");
		break;
	case Enil:
		xprintf("nil");
		break;
	case Ekon:
		printkon(e->aux);
		break;
	case Eid:
		printkon(e->aux);
		break;
	case E_tid:
		xprintf("#");
		printkon(e->aux);
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
	case Egop:
		printcqct0(e->e1, ni);
		xprintf(" %s= ", opstr(e->op));
		printcqct0(e->e2, ni);
		break;
	case Euminus:
	case Eunot:
	case Euplus:
	case Eutwiddle:
		xprintf("%s", opstr(e->kind));
		printcqct0(e->e1, ni);
		break;
	case Ebinop:
		switch(e->op){
		case Excast:
			xprintf("{");
			printcqct0(e->e1, ni);
			xprintf("}(");
			printcqct0(e->e2, ni);
			xprintf(")");
			break;
		default:
			xprintf("(");
			printcqct0(e->e1, ni);
			xprintf(" %s ", opstr(e->op));
			printcqct0(e->e2, ni);
			xprintf(")");
			break;
		}
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
	case Enames:
		xprintf("@names ...");
		break;
	case Eelist:
		if(ni > 0 &&
		   (e->e1->kind == Edefault
		    || e->e1->kind == Ecase
		    || e->e1->kind == Elabel))
			indent(ni-1);
		else if(e->e1->kind != Eblock)
			indent(ni);
		printcqct0(e->e1, ni);
		if(e->e1->kind == Edefault
		   || e->e1->kind == Ecase
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
		printkon(e->aux);
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
		printkon(e->aux);
		break;
	case Eret:
		xprintf("return");
		if(e->e1){
			xprintf(" ");
			printcqct0(e->e1, ni);
		}
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
s2kind(Str *s)
{
	Kind k;

	for(k = 0; k < Emax; k++){
		if(S[k] == 0)
			continue;
		if(!strncmp(S[k], strdata(s), s->len))
			return k;
	}
	return Ebad;
}
