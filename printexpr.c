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
	[E_xcast] =	"E_xcast",
};

static void
abbrevlits(char *buf, unsigned len, Lits *lits)
{
	char *p, *e, ch;
	unsigned m;
	p = buf;
	e = buf+len-1;
	m = 0;
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
	*p = 0;
}

void
printexpr(Expr *e)
{
	static char consts[10];

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
