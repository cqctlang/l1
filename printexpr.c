#include "sys.h"
#include "util.h"
#include "l1.h"

static void printcqct0(Expr *e, unsigned ni);
enum {
	Abbrevlen = 10,
};

char* S[] = {
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
	[Ebits] =	"Ebits",
	[Eblock] =	"Eblock",
	[Ebor] =	"Ebor",
	[Ebreak] =	"Ebreak",
	[Ebxor] =	"Ebxor",
	[Ecall] =	"Ecall",
	[Ecast] =	"Ecast",
	[Ecomma] =	"Ecomma",
	[Econd] =	"Econd",
	[Econst] =	"Econst",
	[Econsts] =	"Econsts",
	[Econtinue] =	"Econtinue",
	[Edecl] =	"Edecl",
	[Edecls] =	"Edecls",
	[Edefine] =	"Edefine",
	[Ederef] =	"Ederef",
	[Ediv] =	"Ediv",
	[Edo] =		"Edo",
	[Edot] =	"Edot",
	[Edotdot] =	"Edotdot",
	[Eelist] =	"Eelist",
	[Eenum] =	"Eenum",
	[Eenumel] =	"Eenumel",
	[Eeq] =		"Eeq",
	[Efieldoff] =	"Efieldoff",
	[Efields] =	"Efields",
	[Efor] =	"Efor",
	[Efun] =	"Efun",
	[Eg] =		"Eg",
	[Ege] =		"Ege",
	[Egt] =		"Egt",
	[Eid] =		"Eid",
	[Eif] =		"Eif",
	[Elambda] =	"Elambda",
	[Eland] =	"Eland",
	[Ele] =		"Ele",
	[Elor] =	"Elor",
	[Elt] =		"Elt",
	[Emod] =	"Emod",
	[Emul] =	"Emul",
	[Ens] =		"Ens",
	[Eneq] =	"Eneq",
	[Enil] =	"Enil",
	[Enop] =	"Enop",
	[Enull] =	"Enull",
	[Epostdec] =	"Epostdec",
	[Epostinc] =	"Epostinc",
	[Epredec] =	"Epredec",
	[Epreinc] =	"Epreinc",
	[Eptr] =	"Eptr",
	[Eref] =	"Eref",
	[Eret] =	"Eret",
	[Eshl] =	"Eshl",
	[Eshr] =	"Eshr",
	[Esizeofe] =	"Esizeofe",
	[Esizeoft] =	"Esizeoft",
	[Estruct] =	"Estruct",
	[Esub] =	"Esub",
	[Etick] = 	"Etick",
	[Etid] = 	"Etid",
	[Etypedef] =	"Etypedef",
	[Etypeofe] =	"Etypeofe",
	[Etypeoft] =	"Etypeoft",
	[Euminus] =	"Euminus",
	[Eunion] =	"Eunion",
	[Eunot] =	"Eunot",
	[Euplus] =	"Euplus",
	[Eutwiddle] =	"Eutwiddle",
	[Ewhile] =	"Ewhile",      
	[Excast] =	"Excast",

	[E_car] =	"E_car",
	[E_cdr] =	"E_cdr",
	[E_cval] =	"E_cval",
	[E_encode] =	"E_encode",
	[E_lenl] =	"E_lenl",
	[E_lens] =	"E_lens",
	[E_lenv] =	"E_lenv",
	[E_range] =	"E_range",
	[E_ref] =	"E_ref",
	[E_sizeof] =	"E_sizeof",
	[E_slices] =	"E_slices",
	[E_str] =	"E_str",
	[E_tn] =	"E_tn",
	[E_vec] =	"E_vec",
	[E_vecl] =	"E_vecl",
	[E_vecref] =	"E_vecref",
	[E_vecset] =	"E_vecset",
};

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

void
printexpr(Expr *e)
{
	static char consts[Abbrevlen];

	switch(e->kind){
	case Econst:
		printf("%" PRIu64, e->liti.val);
		break;
	case Econsts:
//		printf("(Econsts %.*s)", e->lits->len, e->lits->s);
		abbrevlits(consts, sizeof(consts), e->lits);
		printf("(Econsts \"%s\")", consts);
		break;
	case Eid:
		printf("(Eid %s)", e->id);
		break;
	case Ebinop:
		printf("(%s ", S[e->op]);
		printexpr(e->e1);
		printf(" ");
		printexpr(e->e2);
		printf(")");
		break;
	case Egop:
		printf("(Egop %s ", S[e->op]);
		printexpr(e->e1);
		printf(" ");
		printexpr(e->e2);
		printf(")");
		break;
	default:
		printf("(%s", S[e->kind]);
		if(e->e1){
			printf(" ");
			printexpr(e->e1);
		}
		if(e->e2){
			printf(" ");
			printexpr(e->e2);
		}
		if(e->e3){
			printf(" ");
			printexpr(e->e3);
		}
		if(e->e4){
			printf(" ");
			printexpr(e->e4);
		}
		printf(")");
	}
}

static void
indent(unsigned nindent)
{
	while(nindent--)
		printf("\t");
}

static int
printlocals(Expr *e, unsigned ni)
{
	Expr *p, *q;
	unsigned cnt = 0;

	p = e;
	while(p->kind != Enull){
		q = p->e1;
		while(q->kind != Enull){
			if(!cnt){
				indent(ni); printf("@local ");
			}else
				printf(", ");
			cnt++;
			printf("%s", q->e1->id);
			q = q->e2;
		}
		p = p->e2;
	}
	if(cnt)
		printf(";");
	return cnt;
}

static void
printargs(Expr *e, unsigned ni, int more)
{
	if(e->kind == Enull)
		return;
	printargs(e->e2, ni, 1);
	printcqct0(e->e1, ni);
	if(more)
		printf(", ");
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
	[Elt] = "<=",
	[Ele] = "<",
	[Eeq] = "==",
	[Eneq] = "!=",
	[Euminus] = "-",
	[Eunot] = "!",
	[Euplus] = "+",
	[Eutwiddle] = "~",
};

static char*
opstr(unsigned op)
{
	return Opstr[op] ? Opstr[op] : "";
}

static void
printcqct0(Expr *e, unsigned ni)
{
	static char consts[Abbrevlen];
	int wasstmt = 0;

	if(e == 0)
		return;

	switch(e->kind){
	case Etick:
		printcqct0(e->e1, ni);
		printf("`");
		printcqct0(e->e2, ni);
		break;
	case Edefine:
		printf("define ");
		printcqct0(e->e1, ni);
		if(e->e2->kind == Eid){
			printf(" ");
			printcqct0(e->e2, ni);
		}else{
			printf("(");
			printargs(e->e2, ni, 0);
			printf(")");
		}
		printcqct0(e->e3, ni);
		break;
	case Econst:
		printf("%" PRIu64, e->liti.val);
		break;
	case Econsts:
		abbrevlits(consts, sizeof(consts), e->lits);
		printf("\"%s\"", consts);
		break;
	case Eid:
		printf("%s", e->id);
		break;
	case Elambda:
		printf("lambda");
		if(e->e1->kind == Eid){
			printf(" ");
			printcqct0(e->e1, ni);
		}else{
			printf("(");
			printargs(e->e1, ni, 0);
			printf(")");
		}
		printcqct0(e->e2, ni);
		break;
	case Ecall:
		printcqct0(e->e1, ni);
		printf("(");
		printargs(e->e2, ni, 0);
		printf(")");
		break;
	case Eblock:
		printf("\n");
		indent(ni); printf("{\n");
		if(printlocals(e->e1, ni+1) && e->e2->e1->kind != Eblock)
			printf("\n");
		printcqct0(e->e2, ni+1);
		indent(ni); printf("}");
		break;
	case Eg:
		printcqct0(e->e1, ni);
		printf(" = ");
		printcqct0(e->e2, ni);
		break;
	case Ebinop:
		switch(e->op){
		case Excast:
			printf("{");
			printcqct0(e->e1, ni);
			printf("}(");
			printcqct0(e->e2, ni);
			printf(")");
			break;
		case E_range:
			printf("E_range(");
			printcqct0(e->e1, ni);
			printf(", ");
			printcqct0(e->e2, ni);
			printf(")");
			break;
		default:
			printf("(");
			printcqct0(e->e1, ni);
			printf(" %s ", opstr(e->op));
			printcqct0(e->e2, ni);
			printf(")");
			break;
		}
		break;
	case Enull:
	case Enop:
		break;
	case Epreinc:
	case Epredec:
		printf("%s(", opstr(e->kind));
		printcqct0(e->e1, ni);
		printf(")");
		break;
	case Epostinc:
	case Epostdec:
		printf("(");
		printcqct0(e->e1, ni);
		printf(")%s", opstr(e->kind));
		break;
	case Eref:
		printf("&(");
		printcqct0(e->e1, ni);
		printf(")");
		break;
	case Ederef:
		printf("*(");
		printcqct0(e->e1, ni);
		printf(")");
		break;
	case Earef:
		printf("(");
		printcqct0(e->e1, ni);
		printf(")[");
		printcqct0(e->e2, ni);
		printf("]");
		break;
	case Edot:
		printf("(");
		printcqct0(e->e1, ni);
		printf(").");
		printcqct0(e->e2, ni);
		break;
	case Earrow:
		printf("(");
		printcqct0(e->e1, ni);
		printf(")->");
		printcqct0(e->e2, ni);
		break;
	case Ens:
		printf("@names ...");
		break;
	case Eelist:
		indent(ni); printcqct0(e->e1, ni);
		printf(";\n");
		printcqct0(e->e2, ni);
		break;
	case Eif:
		printf("if(");
		printcqct0(e->e1, ni);
		printf(")");
		if(e->e2->kind != Eblock){
			printf("\n");
			indent(ni+1);
			printcqct0(e->e2, ni+1);
			wasstmt = 1;
		}else
			printcqct0(e->e2, ni);
		if(e->e3){
			if(wasstmt)
				printf(";");
			printf("\n");
			indent(ni); printf("else\n");
			if(e->e3->kind != Eblock){
				indent(ni+1);
				printcqct0(e->e3, ni+1);
			}else
				printcqct0(e->e3, ni);
		}
		break;
	case Eret:
		printf("return");
		if(e->e1){
			printf(" ");
			printcqct0(e->e1, ni);
		}
		break;
	case E_tn:
		printf("E_tn(...)");
		break;
	case E_car:
	case E_cdr:
	case E_lenl:
	case E_lens:
	case E_lenv:
	case E_sizeof:
	case E_encode:
		printf("%s(", S[e->kind]);
		printcqct0(e->e1, ni);
		printf(")");
		break;
	case E_range:
	case E_vecref:
	case E_vec:
	case E_vecl:
		printf("%s(", S[e->kind]);
		printcqct0(e->e1, ni);
		printf(", ");
		printcqct0(e->e2, ni);
		printf(")");
		break;
	case E_slices:
	case E_vecset:
	case E_ref:
	case E_cval:
		printf("%s(", S[e->kind]);
		printcqct0(e->e1, ni);
		printf(", ");
		printcqct0(e->e2, ni);
		printf(", ");
		printcqct0(e->e3, ni);
		printf(")");
		break;
	default:
		printf("[%d]???", e->kind);
		break;
	}
	
}

void
printcqct(Expr *e)
{
	printcqct0(e, 0);
}
