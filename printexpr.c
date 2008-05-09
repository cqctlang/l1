#include "sys.h"
#include "util.h"
#include "l1.h"

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
	[Enbase] =	"Enbase",

	[Eadd] =	"Eadd",
	[Earef] =	"Earef",
	[Earr] =	"Earr",
	[Earrow] =	"Earrow",
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
	[Eneq] =	"Eneq",
	[Enop] =	"Enop",
	[Enull] =	"Enull",
	[Epostdec] =	"Epostdec",
	[Epostinc] =	"Epostinc",
	[Epredec] =	"Epredec",
	[Epreinc] =	"Epreinc",
	[Eptr] =	"Eptr",
	[Eret] =	"Eret",
	[Eshl] =	"Eshl",
	[Eshr] =	"Eshr",
	[Esizeofe] =	"Esizeofe",
	[Esizeoft] =	"Esizeoft",
	[Estruct] =	"Estruct",
	[Esub] =	"Esub",
	[Etick] = 	"Etick",
	[Etypedef] =	"Etypedef",
	[Euand] =	"Euand",
	[Euminus] =	"Euminus",
	[Eumul] =	"Eumul",
	[Eunion] =	"Eunion",
	[Eunot] =	"Eunot",
	[Euplus] =	"Euplus",
	[Eutwiddle] =	"Eutwiddle",
	[Ewhile] =	"Ewhile",      

	[E_car] =	"E_car",
	[E_cdr] =	"E_cdr",
	[E_cval] =	"E_cval",
	[E_range] =	"E_range",
	[E_sizeof] =	"E_sizeof",
};

void
printexpr(Expr *e)
{
	switch(e->kind){
	case Econst:
		printf("%llu", e->cval.val);
		break;
	case Econsts:
		printf("(Econsts %.*s)", e->lits->len, e->lits->s);
		break;
	case Eid:
		printf("(Eid %s)", e->id);
		break;
	case Etick:
		printf("(Etick %.*s)", e->lits->len, e->lits->s);
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
