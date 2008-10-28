#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void printcqct0(Expr *e, unsigned ni);
static char* fmtdecl(Decl *d);

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
	[Econst] =	"Econst",
	[Econsts] =	"Econsts",
	[Econtainer] =	"Econtainer",
	[Econtinue] =	"Econtinue",
	[Edecl] =	"Edecl",
	[Edecls] =	"Edecls",
	[Edefault] =	"Edefault",
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
	[Elist] =	"Elist",
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
	[Eswitch] =	"Eswitch",
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

	[E_cval] =	"E_cval",
	[E_ref] =	"E_ref",
	[E_sizeof] =	"E_sizeof",
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
		xprintf("%" PRIu64, e->liti.val);
		break;
	case Econsts:
//		xprintf("(Econsts %.*s)", e->lits->len, e->lits->s);
		abbrevlits(consts, sizeof(consts), e->lits);
		xprintf("(Econsts \"%s\")", consts);
		break;
	case Eid:
		xprintf("(Eid %s)", e->id);
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
		xprintf("%s", p->e1->id);
		p = p->e2;
	}
	if(cnt)
		xprintf(";");
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
		xprintf(", ");
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
	[Elor] = "||",
	[Eland] = "&&",
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
		xprintf("`");
		printcqct0(e->e2, ni);
		break;
	case Edefine:
		xprintf("define ");
		printcqct0(e->e1, ni);
		if(e->e2->kind == Eid){
			xprintf(" ");
			printcqct0(e->e2, ni);
		}else{
			xprintf("(");
			printargs(e->e2, ni, 0);
			xprintf(")");
		}
		printcqct0(e->e3, ni);
		break;
	case Enil:
		xprintf("nil");
		break;
	case Econst:
		xprintf("%" PRIu64, e->liti.val);
		break;
	case Econsts:
		abbrevlits(consts, sizeof(consts), e->lits);
		xprintf("\"%s\"", consts);
		break;
	case Eid:
		xprintf("%s", e->id);
		break;
	case Elambda:
		xprintf("lambda");
		if(e->e1->kind == Eid){
			xprintf(" ");
			printcqct0(e->e1, ni);
		}else{
			xprintf("(");
			printargs(e->e1, ni, 0);
			xprintf(")");
		}
		printcqct0(e->e2, ni);
		break;
	case Ecall:
		printcqct0(e->e1, ni);
		xprintf("(");
		printargs(e->e2, ni, 0);
		xprintf(")");
		break;
	case Eblock:
		xprintf("\n");
		indent(ni); xprintf("{\n");
		if(printlocals(e->e1, ni+1) && e->e2->e1->kind != Eblock)
			xprintf("\n");
		printcqct0(e->e2, ni+1);
		indent(ni); xprintf("}");
		break;
	case Eg:
		printcqct0(e->e1, ni);
		xprintf(" = ");
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
		xprintf("%s(", opstr(e->kind));
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Epostinc:
	case Epostdec:
		xprintf("(");
		printcqct0(e->e1, ni);
		xprintf(")%s", opstr(e->kind));
		break;
	case Eref:
		xprintf("&(");
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case Ederef:
		xprintf("*(");
		printcqct0(e->e1, ni);
		xprintf(")");
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
	case Ens:
		xprintf("@names ...");
		break;
	case Eelist:
		if(e->e1->kind == Edefault || e->e1->kind == Ecase)
			indent(ni-1);
		else
			indent(ni);
		printcqct0(e->e1, ni);
		if(e->e1->kind == Edefault || e->e1->kind == Ecase)
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
	case Ebreak:
		xprintf("break");
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
		if(e->e4->kind != Eblock){
			xprintf("\n");
			indent(ni+1);
			printcqct0(e->e4, ni+1);
		}else
			printcqct0(e->e4, ni);
		break;
	case Eif:
		xprintf("if(");
		printcqct0(e->e1, ni);
		xprintf(")");
		if(e->e2->kind != Eblock){
			xprintf("\n");
			indent(ni+1);
			printcqct0(e->e2, ni+1);
			wasstmt = 1;
		}else
			printcqct0(e->e2, ni);
		if(e->e3){
			if(wasstmt)
				xprintf(";");
			xprintf("\n");
			indent(ni); xprintf("else\n");
			if(e->e3->kind != Eblock){
				indent(ni+1);
				printcqct0(e->e3, ni+1);
			}else
				printcqct0(e->e3, ni);
		}
		break;
	case Eret:
		xprintf("return");
		if(e->e1){
			xprintf(" ");
			printcqct0(e->e1, ni);
		}
		break;
	case E_sizeof:
		xprintf("%s(", S[e->kind]);
		printcqct0(e->e1, ni);
		xprintf(")");
		break;
	case E_ref:
	case E_cval:
		xprintf("%s(", S[e->kind]);
		printcqct0(e->e1, ni);
		xprintf(", ");
		printcqct0(e->e2, ni);
		xprintf(", ");
		printcqct0(e->e3, ni);
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

static int
needsparen(unsigned kind)
{
	switch(kind){
	case Tfun:
	case Tarr:
		return 1;
	default:
		return 0;
	}
}

static char*
fmtdecllist(Decl *p)
{
	char **ds, *buf, *bp;
	unsigned i, n;
	unsigned long m;
	Decl *q;

	n = 0;
	q = p;
	while(q){
		n++;
		q = q->link;
	}

	if(n == 0)
		return xstrdup("");

	ds = emalloc(n*sizeof(char**));
	m = 1;			/* null */
	for(i = 0, q = p; i < n; i++, q = q->link){
		ds[i] = fmtdecl(q);
		m += strlen(ds[i]);
		if(i < n-1)
			m += 2;	/* comma, space */
	}

	buf = emalloc(m);
	bp = buf;
	for(i = 0; i < n; i++){
		strcpy(bp, ds[i]);
		bp += strlen(ds[i]);
		if(i < n-1){
			strcpy(bp, ", ");
			bp += 2;
		}
	}
	*bp = 0;

	return buf;
}

/* o must be dynamically allocated; caller turns over its management
   to fmttype.  returned string is also dynamically allocated; caller
   must free it */
char*
fmttype(Type *t, char *o)
{
	char *buf, *w, *pl;
	unsigned m;

	switch(t->kind){
	case Tvoid:
		m = 4+1+strlen(o)+1;
		buf = emalloc(m);
		snprintf(buf, m, "void %s", o);
		efree(o);
		return buf;
	case Tbase:
		m = strlen(basename[t->base])+1+strlen(o)+1;
		buf = emalloc(m);
		snprintf(buf, m, "%s %s", basename[t->base], o);
		efree(o);
		return buf;
	case Ttypedef:
		m = strlen(t->tid)+1+strlen(o)+1;
		buf = emalloc(m);
		snprintf(buf, m, "%s %s", t->tid, o);
		efree(o);
		return buf;
	case Tstruct:
	case Tunion:
		w = tkindstr[t->kind];
		if(t->tag){
			m = strlen(w)+1+strlen(t->tag)+1+strlen(o)+1;
			buf = emalloc(m);
			snprintf(buf, m, "%s %s %s", w, t->tag, o);
		}else{
			m = strlen(w)+1+strlen(o)+1;
			buf = emalloc(m);
			snprintf(buf, m, "%s %s", w, o);
		}
		efree(o);
		return buf;
	case Tenum:
		if(t->tag){
			m = 4+1+strlen(t->tag)+1+strlen(o)+1;
			buf = emalloc(m);
			snprintf(buf, m, "enum %s %s", t->tag, o);
		}else{
			m = 4+1+strlen(o)+1;
			buf = emalloc(m);
			snprintf(buf, m, "enum %s", o);
		}
		efree(o);
		return buf;
	case Tptr:
		m = 2+strlen(o)+1+1;
		buf = emalloc(m);
		if(needsparen(t->link->kind))
			snprintf(buf, m, "(*%s)", o);
		else
			snprintf(buf, m, "*%s", o);
		efree(o);
		return fmttype(t->link, buf);
	case Tarr:
		m = strlen(o)+2+1;
		buf = emalloc(m);
		snprintf(buf, m, "%s[]", o);
		efree(o);
		return fmttype(t->link, buf);
	case Tfun:
		pl = fmtdecllist(t->param);
		m = strlen(o)+1+strlen(pl)+1+1;
		buf = emalloc(m);
		snprintf(buf, m, "%s(%s)", o, pl);
		efree(o);
		efree(pl);
		return fmttype(t->link, buf);
	default:
		fatal("bug");
	}
	return NULL;
}

static char*
fmtdecl(Decl *d)
{
	char *o;

	if(d->type == NULL)
		return NULL;

	if(d->id)
		o = xstrdup(d->id);
	else
		o = xstrdup("");

	return fmttype(d->type, o);
}

void
printdecl(Decl *d)
{
	char *o;

	o = fmtdecl(d);
	xprintf("%s", o);
	efree(o);
}

void
printvars(Expr *e)
{
	int first = 1;

	Expr *q;
	while(e->kind != Enull){
		q = e->e1;
		while(q->kind != Enull){
			if(!first)
				xprintf(", ");
			xprintf("%s", q->e1->id);
			first = 0;
			q = q->e2;
		}
		e = e->e2;
	}
}

