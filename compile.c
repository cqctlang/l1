#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

typedef struct Lambda Lambda;

typedef
struct CGEnv {
	Lambda *b;
	Ctl *Return;
	Ctl *Return0;
	Ctl *Break;
	Ctl *Continue;
} CGEnv;

typedef
struct Frstat {
	unsigned nloc;
	HT *locals;
} Frstat;

struct Konst {
	Lits **lits;
	unsigned nlits, maxlits;
};

static Location toploc[8];
static Location *Effect;
static Location *AC, *FP, *SP, *PC, *ARG0, *ARG1, *ARG2;
static void compilelambda(Ctl *name, Code *code, Expr *el);
static Topvec *mktopvec();
static void freetopvec(Topvec *tv);
static Konst* mkkonst();
static void freekonst(Konst *kon);

static void
newloc(Location *loc, unsigned kind, unsigned idx, unsigned indirect)
{
	loc->kind = kind;
	loc->idx = idx;
	loc->indirect = indirect;
}

static Ctl*
mklabel(Code *code)
{
	Ctl *ctl;
	ctl = xmalloc(sizeof(Ctl));
	ctl->link = code->clist;
	ctl->code = code;
	code->clist = ctl;
	return ctl;
}

static void
freelabel(Ctl *ctl)
{
	if(ctl->ckind == Clabel)
		free(ctl->label);
	free(ctl);
}

static Ctl*
genlabel(Code *code, char *s)
{
	Ctl *ctl;
	static char buf[32];
	static unsigned labelseed = 0;

	ctl = mklabel(code);
	ctl->ckind = Clabel;
	if(s)
		snprintf(buf, sizeof(buf), "%s", s);
	else
		snprintf(buf, sizeof(buf), "L%d", labelseed++);
	ctl->label = xstrdup(buf);
	ctl->used = 0;
	return ctl;
}

static Ctl*
genlabelpair(Code *code, Ctl *l1, Ctl *l2)
{
	Ctl *ctl;
	ctl = mklabel(code);
	ctl->ckind = Clabelpair;
	ctl->l1 = l1;
	ctl->l2 = l2;
	return ctl;
}

/* mark CTL as label for current insn in associated
   code.  if E is non-null, it is the expression whose
   code follows label CTL. */
static void
emitlabel(Ctl *ctl, Expr *e)
{
	Code *code;
	code = ctl->code;
	if(ctl->ckind != Clabel)
		fatal("attempt to emit label pair");

	ctl->insn = code->ninsn;
	if(code->labels[code->ninsn])
		fatal("multiple labels %s,%s", ctl->label,
		      code->labels[code->ninsn]->label);
	code->labels[code->ninsn] = ctl;
	if(e && e->src.filename)
		ctl->src = &e->src;
}

static Code*
mkcode()
{
	Code *code;
	
	code = (Code*)halloc(&heapcode);
	code->maxinsn = InsnAlloc;
	code->insn = xmalloc(code->maxinsn*sizeof(Insn));
	code->labels = xmalloc(code->maxinsn*sizeof(Ctl*));
	code->ninsn = 0;
	code->topvec = mktopvec();
	code->konst = mkkonst();

	return code;
}

void
freecode(Head *hd)
{
	Code *code;
	Ctl *p, *q;

	code = (Code*)hd;
	freekonst(code->konst);
	freetopvec(code->topvec);
	p = code->clist;
	while(p){
		q = p->link;
		freelabel(p);
		p = q;
	}
	
	freeexpr(code->src, freeexprx);
	free(code->insn);
	free(code->labels);
}

static Insn*
nextinsn(Code *code)
{
	if(code->ninsn >= code->maxinsn){
		code->insn = xrealloc(code->insn,
				      code->maxinsn*sizeof(Insn),
				      2*code->maxinsn*sizeof(Insn));
		code->labels = xrealloc(code->labels,
					code->maxinsn*sizeof(Ctl*),
					2*code->maxinsn*sizeof(Ctl*));
		code->maxinsn *= 2;
	}
	return &code->insn[code->ninsn++];
}

static char*
itos(ikind kind)
{
	static char *s[] = {
		[Iadd] = "add",
		[Iand] = "and",
		[Idiv] = "div",
		[Iinv] = "inv",
		[Imod] = "mod",
		[Imul] = "mul",
		[Ineg] = "neg",
		[Inot] = "not",
		[Ior] = "or",
		[Ishl] = "shl",
		[Ishr] = "shr",
		[Isub] = "sub",
		[Ixor] = "xor",
		[Icmpgt] = "cmpgt",
		[Icmpge] = "cmpge",
		[Icmplt] = "cmplt",
		[Icmple] = "cmple",
		[Icmpeq] = "cmpeq",
		[Icmpneq] = "cmpneq",
	};

	return s[kind];
}

static char*
regtos(Reg reg)
{
	static char *s[] = {
		[Rsp] = "%sp",
		[Rfp] = "%fp",
		[Rpc] = "%pc",
		[Rac] = "%ac",
		[Rcl] = "%cl",
	};

	return s[reg];
}

static void
printrand(Code *code, Operand *r)
{
	Location *loc;
	Lits *lits;

	switch(r->okind){
	case Oloc:
		loc = &r->u.loc;
		switch(loc->kind){
		case Lreg:
			printf("%s", regtos(loc->idx));
			break;
		case Lparam:
			if(loc->indirect)
				printf("[");
			printf("%d(%s)", loc->idx+1, regtos(Rfp));
			if(loc->indirect)
				printf("]");
			break;
		case Llocal:
			if(loc->indirect)
				printf("[");
			printf("-%d(%s)", loc->idx+1, regtos(Rfp));
			if(loc->indirect)
				printf("]");
			break;
		case Ldisp:
			if(loc->indirect)
				printf("[");
			printf("%d(%s)", loc->idx, regtos(Rcl));
			if(loc->indirect)
				printf("]");
			break;
		case Ltopl:
			printf("<%s>", topvecid(loc->idx, code->topvec));
			break;
		}
		break;
	case Ocval:
		printf("%" PRIu64, r->u.cval.val);
		break;
	case Onil:
		printf("nil");
		break;
	case Olits:
		lits = r->u.lits;
		if(lits->len > 10)
			printf("*lits*");
		else
			printf("\"%.*s\"", lits->len, lits->s);
		break;
	default:
		fatal("unknown operand kind %d", r->okind);
	}
}

static void
printinsn(Code *code, Insn *i)
{
	printf("\t");
	switch(i->kind){
	case Iinv:
	case Ineg:
	case Inot:
		printf("%s ", itos(i->kind));
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iadd:
	case Iand:
	case Idiv:
	case Imod:
	case Imul:
	case Ior:
	case Ishl:
	case Ishr:
	case Isub:
	case Ixor:
	case Icmpgt:
	case Icmpge:
	case Icmplt:
	case Icmple:
	case Icmpeq:
	case Icmpneq:
		printf("%s ", itos(i->kind));
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Imov:
		printf("mov ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ipush:
		printf("push ");
		printrand(code, &i->op1);
		break;
	case Icall:
		printf("call ");
		printrand(code, &i->op1);
		break;
	case Icallt:
		printf("callt ");
		printrand(code, &i->op1);
		break;
	case Icar:
		printf("car ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Icdr:
		printf("cdr ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Icons:
		printf("cons ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Irbeg:
		printf("rbeg ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Irlen:
		printf("rlen ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Irange:
		printf("range ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Icval:
		printf("cval ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ilenl:
		printf("lenl ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ilens:
		printf("lens ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ilenv:
		printf("lenv ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Islices:
		printf("slices ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->op3);
		printf(" ");
		printrand(code, &i->dst);
	case Istr:
		printf("str ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Itab:
		printf("tab ");
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Itabdel:
		printf("tabdel ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Itabenum:
		printf("tabenum ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Itabget:
		printf("tabget ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Itabput:
		printf("tabput ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->op3);
		break;
	case Ivec:
		printf("vec ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ivecl:
		printf("vecl ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ivecref:
		printf("vecref ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ivecset:
		printf("vecset ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->op3);
		break;
	case Ixcast:
		printf("xcast ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iencode:
		printf("encode ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Isizeof:
		printf("sizeof ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iiscl:
		printf("iscl ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iiscval:
		printf("iscval ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iisas:
		printf("isas ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iisdom:
		printf("isdom ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iisns:
		printf("isns ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iisnull:
		printf("isnull ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iispair:
		printf("ispair ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iisrange:
		printf("isrange ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iisstr:
		printf("isstr ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iistab:
		printf("istab ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iistype:
		printf("istype ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iisvec:
		printf("isvec ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Inull:
		printf("null ");
		printrand(code, &i->dst);
		break;
	case Ivlist:
		printf("vlist ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ivvec:
		printf("vvec ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Iframe:
		printf("frame %s", i->dstlabel->label);
		break;
	case Igc:
		printf("gc");
		break;
	case Iret:
		printf("ret");
		break;
	case Ihalt:
		printf("halt");
		break;
	case Iding:
		printf("ding");
		break;
	case Iclo:
		printf("clo %s ", i->dstlabel->label);
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ikg:
		printf("kg ");
		printrand(code, &i->dst);
		break;
	case Ikp:
		printf("kp");
		break;
	case Ijmp:
		printf("jmp %s", i->dstlabel->label);
		break;
	case Ijnz:
		printf("jnz ");
		printrand(code, &i->op1);
		printf(" %s", i->dstlabel->label);
		break;
	case Ijz:
		printf("jz ");
		printrand(code, &i->op1);
		printf(" %s", i->dstlabel->label);
		break;
	case Ibox:
		printf("box ");
		printrand(code, &i->op1);
		break;
	case Ibox0:
		printf("box0 ");
		printrand(code, &i->op1);
		break;
	case Iprint:
		printf("print ");
		printrand(code, &i->op1);
		break;
	case Inop:
		printf("nop");
		break;
	case Itn:
		printf("tn.%d.%d ", TBITSTYPE(i->bits), TBITSBASE(i->bits));
		printrand(code, &i->dst);
		break;
	case Itnx:
		printf("tnx ");
		printrand(code, &i->dst);
		break;
	case Iistn:
		printf("istn ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ias:
		printf("as ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Idom:
		printf("dom ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Idomas:
		printf("domas ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Idomns:
		printf("domns ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ins:
		printf("ns ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->dst);
		break;
	default:
		fatal("printinsn: unrecognized insn %d", i->kind);
	}
	printf("\n");
}

void
printcode(Code *code)
{
	unsigned i;
	for(i = 0; i < code->ninsn; i++){
		if(code->labels[i] && code->labels[i]->used)
			printf("%s", code->labels[i]->label);
		printinsn(code, &code->insn[i]);
	}
}

static unsigned
max(unsigned x, unsigned y)
{
	return x>y?x:y;
}

static int
issimple(Expr *e)
{
	return (e == 0 /* maybe for E_tn nodes */
		|| e->kind == Eid
		|| e->kind == Econst
		|| e->kind == Enil);
}

static unsigned
flattenlocal(Expr *b)
{
	Expr *p, *q;
	Expr *nl;
	unsigned nloc;

	nl = nullelist();
	nloc = 0;

	p = b->e1;
	while(p->kind != Enull){
		q = p->e1;
		while(q->kind != Enull){
			nl = newexpr(Eelist, q->e1, nl, 0, 0);
			q->e1 = 0;
			q = q->e2;
			nloc++;
		}
		p = p->e2;
	}
	freeexpr(b->e1, 0);
	b->e1 = invert(nl);

	return nloc;
}

static unsigned
tmplocpass(Expr *e, unsigned *nloc)
{
	unsigned m;

	if(e == NULL)
		return 0;

	switch(e->kind){
	case Elambda:
		/* skip e1 (identifier list) */
		return 0;
	case Eblock:
		*nloc += flattenlocal(e);
		return tmplocpass(e->e2, nloc);
	case E_tn:
	case Ebinop:
		if(issimple(e->e1) && issimple(e->e2))
			return 0;
		else if(issimple(e->e1))
			return tmplocpass(e->e2, nloc);
		else if(issimple(e->e2))
			return tmplocpass(e->e1, nloc);
		else
			return 1+max(tmplocpass(e->e1, nloc),
				     tmplocpass(e->e2, nloc));
	default:
		m = tmplocpass(e->e1, nloc);
		m = max(m, tmplocpass(e->e2, nloc));
		m = max(m, tmplocpass(e->e3, nloc));
		m = max(m, tmplocpass(e->e4, nloc));
		return m;
	}
}

static void
printvars(Expr *e)
{
	int first = 1;

	Expr *q;
	while(e->kind != Enull){
		q = e->e1;
		while(q->kind != Enull){
			if(!first)
				printf(", ");
			printf("%s", q->e1->id);
			first = 0;
			q = q->e2;
		}
		e = e->e2;
	}
}

typedef
struct Vardef {
	char *id;
	enum {
		VDparam = 1,
		VDlocal,
		VDtoplevel,
	} kind;
	Lambda *lambda;
	unsigned idx;
	Val *val;		/* VDtoplevel */
	unsigned indirect;
} Vardef;

typedef
struct Varref {
	Vardef *vd;
	unsigned closed;
	unsigned cidx;
} Varref;

typedef struct VEnv VEnv;
struct VEnv {
	unsigned nv;
	Vardef *hd;
	VEnv *link;
};

typedef
struct VDset {
	unsigned nvd, maxvd;
	Vardef **vd;
} VDset;

typedef
struct VRset {
	unsigned nvr;
	Varref *vr;
} VRset;

struct Topvec {
	unsigned nid, maxid;
	char **id;
	Val **val;
};

struct Lambda {
	Vardef *param;
	Vardef *local;
	unsigned ntmp, npar, nloc, maxloc, vararg;
	VEnv *ve;
	VRset *capture;
};

static Topvec*
mktopvec()
{
	Topvec *tv;
	tv = xmalloc(sizeof(Topvec));
	tv->maxid = 128;
	tv->id = xmalloc(tv->maxid*sizeof(char*));
	tv->val = xmalloc(tv->maxid*sizeof(Val*));
	return tv;
}

static void
freetopvec(Topvec *tv)
{
	unsigned m;
	for(m = 0; m < tv->nid; m++)
		free(tv->id[m]);
	free(tv->id);
	free(tv->val);
	free(tv);
}

char*
topvecid(unsigned idx, Topvec *tv)
{
	if(idx >= tv->nid)
		fatal("bad toplevel symbol index");
	return tv->id[idx];
}

Val*
topvecval(unsigned idx, Topvec *tv)
{
	if(idx >= tv->nid)
		fatal("bad toplevel symbol index");
	return tv->val[idx];
}

static int
topveclookup(char *id, Topvec *tv)
{
	int m;
	for(m = 0; m < tv->nid; m++)
		if(!strcmp(id, tv->id[m]))
			return m;
	return -1;
}

static int
topvecadd(char *id, Topvec *tv, Env *env)
{
	if(tv->nid >= tv->maxid){
		tv->id = xrealloc(tv->id,
				  tv->maxid*sizeof(char*),
				  2*tv->maxid*sizeof(char*));
		tv->val = xrealloc(tv->val,
				   tv->maxid*sizeof(Val*),
				   2*tv->maxid*sizeof(Val*));
		tv->maxid *= 2;
	}
	tv->id[tv->nid] = xstrdup(id);
	tv->val[tv->nid] = envgetbind(env, id);
	return tv->nid++;
}

static Konst*
mkkonst()
{
	Konst *kon;
	kon = xmalloc(sizeof(Konst));
	kon->lits = xmalloc(128*sizeof(Lits*));
	kon->maxlits = 128;
	kon->nlits = 0;
	return kon;
}

static Lits*
konstlookup(Lits *lits, Konst *kon)
{
	unsigned i;
	Lits *x;
	for(i = 0; i < kon->nlits; i++){
		x = kon->lits[i];
		if(x->len != lits->len)
			continue;
		if(memcmp(x->s, lits->s, x->len) == 0)
			return x;
	}
	return 0;
}

static Lits*
konstadd(Lits *lits, Konst *kon)
{
	if(kon->nlits >= kon->maxlits){
		kon->lits = xrealloc(kon->lits,
				     kon->maxlits*sizeof(Lits*),
				     2*kon->maxlits*sizeof(Lits*));
		kon->maxlits *= 2;
	}
	lits = copylits(lits);
	kon->lits[kon->nlits++] = lits;
	return lits;
}

static void
freekonst(Konst *kon)
{
	unsigned m;
	for(m = 0; m < kon->nlits; m++)
		freelits(kon->lits[m]);
	free(kon->lits);
	free(kon);
}

static VEnv*
mkvenv()
{
	VEnv *ve;
	ve = xmalloc(sizeof(VEnv));
	return ve;
}

static void
freevenv(VEnv *ve)
{
	free(ve);
}

static VDset*
mkvdset()
{
	VDset *vs;
	vs = xmalloc(sizeof(VDset));
	vs->maxvd = 128;
	vs->vd = xmalloc(vs->maxvd*sizeof(*vs->vd));
	return vs;
}

static void
freevdset(VDset *vs)
{
	free(vs->vd);
	free(vs);
}

static void
addvdset(Vardef *vd, VDset *vs)
{
	unsigned m;
	Vardef **p;

	for(m = 0, p = vs->vd; m < vs->nvd; m++, p++)
		if(*p == vd)
			return;
	if(vs->nvd >= vs->maxvd){
		vs->vd = xrealloc(vs->vd,
				  vs->maxvd*sizeof(*vs->vd),
				  2*vs->maxvd*sizeof(*vs->vd));
		vs->maxvd *= 2;
	}
	vs->vd[vs->nvd++] = vd;
}

static int
lookupvdset(Vardef *vd, VDset *vs)
{
	unsigned m;
	Vardef **p;

	for(m = 0, p = vs->vd; m < vs->nvd; m++, p++)
		if(*p == vd)
			return m;

	return -1;
}

static VRset*
mkvrset(unsigned n)
{
	VRset *vs;
	vs = xmalloc(sizeof(VRset));
	vs->nvr = n;
	vs->vr = xmalloc(vs->nvr*sizeof(Varref));
	return vs;
}

static void
freevrset(VRset *vs)
{
	free(vs->vr);
	free(vs);
}

static Lambda*
mklambda(Expr *p)
{
	Lambda *b;
	Expr *q;
	Vardef *vd;
	unsigned m;
	
	b = xmalloc(sizeof(Lambda));

	q = p->e1;
	b->npar = 0;
	if(q->kind == Eid){
		b->vararg = 1;
		/* wrap lambda in block declaration to allocate conventional
		   local 0 vararg list bound to vararg variable */
		p->e2 = newexpr(Eblock,
				newexpr(Eelist,
					newexpr(Eelist, q, nullelist(), 0, 0),
					nullelist(), 0, 0),
				p->e2, 0, 0);
		p->e1 = 0;
	}else
		/* ordinary argument list */
		while(q->kind != Enull){
			b->npar++;
			q = q->e2;
		}

	b->param = xmalloc(b->npar*sizeof(Vardef));
	b->ntmp = tmplocpass(p->e2, &b->maxloc);
	b->local = xmalloc(b->maxloc*sizeof(Vardef));
	b->nloc = 0;

	if(!b->vararg){
		vd = b->param;
		m = 0;
		q = p->e1;
		while(q->kind != Enull){
			vd->lambda = b;
			vd->id = q->e1->id;
			vd->kind = VDparam;
			vd->idx = m;
			vd++;
			m++;
			q = q->e2;
		}
	}

	b->ve = mkvenv();
	b->ve->nv = b->npar;
	b->ve->hd = b->param;

	return b;
}

static void
freelambda(Lambda *b)
{
	freevrset(b->capture);
	freevenv(b->ve);
	free(b->local);
	free(b->param);
	free(b);
}

static void
bindlocal(Lambda *b, Expr *p)
{
	Expr *q;
	Vardef *vd;
	unsigned m;

	m = 0;
	q = p;
	while(q->kind != Enull){
		m++;
		q = q->e2;
	}
	if(m+b->nloc > b->maxloc)
		fatal("bug");

	vd = b->local+b->nloc;
	q = p;
	while(q->kind != Enull){
		vd->lambda = b;
		vd->id = q->e1->id;
		vd->kind = VDlocal;
		vd->idx = b->nloc;
		q = q->e2;
		vd++;
		b->nloc++;
	}
}

static Varref*
mkvarref(Vardef *vd)
{
	Varref *vr;
	vr = xmalloc(sizeof(Varref));
	vr->vd = vd;
	return vr;
}

static Varref*
mktoplevelref(char *id, Topvec *tv, Env *env)
{
	Vardef *vd;
	int idx;

	vd = xmalloc(sizeof(Vardef));
	idx = topveclookup(id, tv);
	if(idx == -1)
		idx = topvecadd(id, tv, env);
	vd->id = id;
	vd->kind = VDtoplevel;
	vd->idx = idx;
	vd->val = topvecval(idx, tv);
	return mkvarref(vd);
}

static void
freevarref(Varref *vr)
{
	if(vr == 0)
		return;
	if(vr->vd->kind == VDtoplevel)
		free(vr->vd);
	free(vr);
}

void
freetype(Type *t, void(*xfn)(Expr*))
{
	if(t == 0)
		return;

	freetype(t->link, xfn);
	free(t->tid);
	free(t->tag);
	freedecl(t->field, xfn);
	/* freeenum(t->en); */
	freedecl(t->param, xfn);
	freeexpr(t->sz, xfn);
	freeexpr(t->cnt, xfn);
	free(t);
}

void
freedecl(Decl *d, void(*xfn)(Expr*))
{
	Decl *nxt;

	nxt = d;
	while(nxt){
		d = nxt;
		nxt = d->link;
		freetype(d->type, xfn);
		freeexpr(d->offs, xfn);
		freeexpr(d->bits, xfn);
		free(d->id);
		free(d);
	}
}

void
freeexprx(Expr *e)
{
	switch(e->kind){
	case Eid:
		freevarref((Varref*)e->xp);
		break;
	case Elambda:
		freelambda((Lambda*)e->xp);
		break;
	case Eblock:
		freevenv((VEnv*)e->xp);
		break;
	default:
		break;
	}
}

static Vardef*
varlookup(char *id, VEnv *ve)
{
	Vardef *p;
	unsigned i;
	while(ve){
		for(i = 0, p = ve->hd; i < ve->nv; i++, p++)
			if(!strcmp(p->id, id))
				return p;
		ve = ve->link;
	}
	return NULL;
}

static int
vardeflookup(Vardef *vd, VEnv *ve)
{
	Vardef *p;
	unsigned i;

	while(ve){
		for(i = 0, p = ve->hd; i < ve->nv; i++, p++)
			if(p == vd)
				return 1;
		ve = ve->link;
	}
	return 0;
}

static void
newmapframe(Expr *e, Lambda *curb, VEnv *ve, Topvec *tv, Env *env, Konst *kon)
{
	char *id;
	Lambda *b;
	VEnv *nve;
	unsigned sloc;
	Vardef *vd;
	Varref *vr;

	if(e == NULL)
		return;

	switch(e->kind){
	case Econsts:
		e->xp = konstlookup(e->lits, kon);
		if(e->xp == NULL)
			e->xp = konstadd(e->lits, kon);
		break;
	case Eid:
		id = e->id;
		vd = varlookup(id, ve);
		if(vd == NULL)
			vr = mktoplevelref(id, tv, env);
		else
			vr = mkvarref(vd);
		e->xp = vr;
		break;
	case Eg:
		if(e->e1->kind != Eid)
			fatal("non-id lhs for Eg");
		id = e->e1->id;
		vd = varlookup(id, ve);
		if(vd == NULL)
			vr = mktoplevelref(id, tv, env);
		else{
			vd->indirect = 1;
			vr = mkvarref(vd);
		}
		e->e1->xp = vr;
		newmapframe(e->e2, curb, ve, tv, env, kon);
		break;
	case Eblock:
		sloc = curb->nloc;
		bindlocal(curb, e->e1);
		if(curb->nloc > sloc){
			nve = mkvenv();
			nve->link = ve;
			nve->nv = curb->nloc-sloc;
			nve->hd = &curb->local[sloc];
			e->xp = nve;
			ve = nve;
		}
		newmapframe(e->e2, curb, ve, tv, env, kon);
		break;
	case Elambda:
		b = mklambda(e);
		e->xp = b;
		b->ve->link = ve;
		ve = b->ve;
		newmapframe(e->e2, b, b->ve, tv, env, kon);
		break;
	default:
		newmapframe(e->e1, curb, ve, tv, env, kon);
		newmapframe(e->e2, curb, ve, tv, env, kon);
		newmapframe(e->e3, curb, ve, tv, env, kon);
		newmapframe(e->e4, curb, ve, tv, env, kon);
		break;
	}	
}

static void
freevars(Expr *e, VEnv *ve, VDset *fr)
{
	Lambda *b;
	Varref *vr;
	VEnv *nve;

	if(e == NULL)
		return;

	switch(e->kind){
	case Eid:
		vr = (Varref*)e->xp;
		if(vr->vd->kind != VDtoplevel && !vardeflookup(vr->vd, ve))
			addvdset(vr->vd, fr);
		break;
	case Elambda:
		b = (Lambda*)e->xp;
		nve = b->ve;
		nve->link = ve;
		freevars(e->e2, nve, fr);
		break;
	case Eblock:
		nve = (VEnv*)e->xp;
		if(nve){
			nve->link = ve;
			ve = nve;
		}
		freevars(e->e2, ve, fr);
		break;
	default:
		freevars(e->e1, ve, fr);
		freevars(e->e2, ve, fr);
		freevars(e->e3, ve, fr);
		freevars(e->e4, ve, fr);
		break;
	}
}

static void
mapcapture(Expr *e, VDset *cap)
{
	VDset *fr;
	Varref *vr;
	Vardef **vd;
	Lambda *b;
	unsigned m;
	int idx;

	if(e == NULL)
		return;

	switch(e->kind){
	case Eid:
		vr = (Varref*)e->xp;
		idx = lookupvdset(vr->vd, cap);
		if(idx >= 0){
			vr->closed = 1;
			vr->cidx = idx;
		}
		break;
	case Elambda:
		fr = mkvdset();
		b = (Lambda*)e->xp;
		b->ve->link = 0;
		freevars(e->e2, b->ve, fr);

		b->capture = mkvrset(fr->nvd);
		vr = b->capture->vr;
		vd = fr->vd;
		for(m = 0; m < fr->nvd; m++){
			vr->vd = *vd;
			idx = lookupvdset(*vd, cap);
			if(idx >= 0){
				vr->closed = 1;
				vr->cidx = idx;
			}
			vr++;
			vd++;
		}
		mapcapture(e->e2, fr);
		freevdset(fr);
		break;
	case Eblock:
		mapcapture(e->e2, cap);
		break;
	default:
		mapcapture(e->e1, cap);
		mapcapture(e->e2, cap);
		mapcapture(e->e3, cap);
		mapcapture(e->e4, cap);
		break;
	}
}

static void
printframe(Expr *e)
{
	Lambda *b;
	Varref *vr;
	Vardef *vd;
	unsigned m;

	switch(e->kind){
	case Econst:
		printf("%" PRIu64, e->cval.val);
		break;
	case Econsts:
		printf("(Econsts %.*s)", e->lits->len, e->lits->s);
		break;
	case Eblock:
		printframe(e->e2);
		break;
	case Eid:
		printf("(Eid ");
		vr = (Varref*)e->xp;
		if(vr == NULL)
			fatal("no varref for Eid");
		if(vr->vd->kind == VDtoplevel)
			printf("<t,%d,%s>", vr->vd->idx, vr->vd->id);
		else
			printf("<%c,%p,%d,%s,%d>",
			       vr->vd->kind == VDparam ? 'p' : 'l',
			       vr->vd->lambda,
			       vr->vd->idx,
			       vr->vd->id,
			       vr->closed ? vr->cidx : -1);
		printf(")");
		break;
	case Elambda:
		b = (Lambda*)e->xp;
		if(b == NULL)
			fatal("no lambda for Elambda");
		printf("(Elambda %p", b);
		vd = b->param;
		for(m = 0; m < b->npar; m++)
			printf(" <p,%d,%s,%d>",
			       vd[m].idx, vd[m].id, vd[m].indirect);
		vd = b->local;
		for(m = 0; m < b->nloc; m++)
			printf(" <l,%d,%s,%d>",
			       vd[m].idx, vd[m].id, vd[m].indirect);
		printf(" ");
		for(m = 0; m < b->capture->nvr; m++){
			vr = &b->capture->vr[m];
			printf(" <%c,%p,%d,%s,%d>",
			       vr->vd->kind == VDparam ? 'p' : 'l',
			       vr->vd->lambda,
			       vr->vd->idx,
			       vr->vd->id,
			       vr->closed ? vr->cidx : -1);
		}
		printf(" ");
		printframe(e->e2);
		printf(")");
		break;
	default:
		printf("(%s", S[e->kind]);
		if(e->e1){
			printf(" ");
			printframe(e->e1);
		}
		if(e->e2){
			printf(" ");
			printframe(e->e2);
		}
		if(e->e3){
			printf(" ");
			printframe(e->e3);
		}
		if(e->e4){
			printf(" ");
			printframe(e->e4);
		}
		printf(")");
	}
}

static void
randloc(Operand *rand, Location* loc)
{
	rand->okind = Oloc;
	rand->u.loc = *loc;
}

static void
randcval(Operand *rand, Cval *val)
{
	rand->okind = Ocval;
	rand->u.cval = *val;
}

static void
randlits(Operand *rand, Lits *lits)
{
	rand->okind = Olits;
	rand->u.lits = lits;
}

static void
randimm(Operand *rand, Imm imm)
{
	rand->okind = Ocval;
	initcval(&rand->u.cval, 0, imm);
}

static void
randnil(Operand *rand)
{
	rand->okind = Onil;
}

static void
freerand(Operand *rand)
{
}

static void
varloc(Location *loc, Expr *eid)
{
	Varref *vr;

	vr = (Varref*)eid->xp;
	if(vr->closed)
		newloc(loc, Ldisp, vr->cidx, vr->vd->indirect);
	else
		switch(vr->vd->kind){
		case VDparam:
			newloc(loc, Lparam, vr->vd->idx, vr->vd->indirect);
			break;
		case VDlocal:
			newloc(loc, Llocal, vr->vd->idx, vr->vd->indirect);
			break;
		case VDtoplevel:
			newloc(loc, Ltopl, vr->vd->idx, 0);
			loc->val = vr->vd->val;
			break;
		}
}

static void
randvarloc(Operand *rand, Expr *eid)
{
	rand->okind = Oloc;
	varloc(&rand->u.loc, eid);
}

/* for non-boxes, return the location for VR.
   for boxes, return the location for the box, not its contents. */
static void
randrefloc(Operand *rand, Varref *vr)
{
	rand->okind = Oloc;
	if(vr->closed)
		newloc(&rand->u.loc, Ldisp, vr->cidx, 0);
	else
		switch(vr->vd->kind){
		case VDparam:
			newloc(&rand->u.loc, Lparam, vr->vd->idx, 0);
			break;
		case VDlocal:
			newloc(&rand->u.loc, Llocal, vr->vd->idx, 0);
			break;
		case VDtoplevel:
			fatal("attempt to capture toplevel variable");
			break;
		}
}

/* for non-boxes, return the location for VD.
   for boxes, return the location for the box, not its contents. */
static void
randvdloc(Operand *rand, Vardef *vd)
{
	rand->okind = Oloc;
	switch(vd->kind){
	case VDparam:
		newloc(&rand->u.loc, Lparam, vd->idx, 0);
		break;
	case VDlocal:
		newloc(&rand->u.loc, Llocal, vd->idx, 0);
		break;
	case VDtoplevel:
		fatal("attempt to capture toplevel variable");
		break;
	}
}

static void
randstkloc(Operand *rand, unsigned kind, unsigned off, unsigned indirect)
{
	rand->okind = Oloc;
	switch(kind){
	case Lparam:
	case Llocal:
		newloc(&rand->u.loc, kind, off, indirect);
		break;
	default:
		fatal("bad kind to randstkloc");
	}
}

static void
cgrand(Operand *rand, Expr *e, CGEnv *p)
{
	switch(e->kind){
	case Eid:
		randvarloc(rand, e);
		break;
	case Econst:
		randcval(rand, &e->cval);
		break;
	case Econsts:
		randlits(rand, e->xp);
		break;
	case Enil:
		randnil(rand);
		break;
	default:
		fatal("bug: cgrand on node %d", e->kind);
	}
}

static void
cgjmp(Code *code, CGEnv *p, Ctl *ctl, Ctl *nxt)
{
	Insn *i;

	if(ctl == nxt){
		/* do nothing */
	}else{
		i = nextinsn(code);
		i->kind = Ijmp;
		i->dstlabel = ctl;
		ctl->used = 1;
	}
}

static int
returnlabel(CGEnv *p, Ctl *ctl)
{
	return (ctl == p->Return || ctl == p->Return0);
}

static Expr*
escaping(Expr *e)
{
	switch(e->kind){
	case Ebreak:
	case Econtinue:
		return e;
	case Eblock:
		return escaping(e->e2);
	case Eelist:
		return escaping(e->e1);
	default:
		return 0;
	}
}

static Ctl*
escapectl(Expr *e, CGEnv *p)
{
	Ctl *rv;
	Expr *kind;

	kind = escaping(e);
	if(kind == 0)
		fatal("not an escaping expression");
	else if(kind->kind == Ebreak)
		rv = p->Break;
	else if(kind->kind == Econtinue)
		rv = p->Continue;
	else
		fatal("not an escaping expression");
	if(rv == 0)
		fatal("escaping expression outside loop");
	return rv;
}

static void
cgbranch(Code *code, CGEnv *p, Ctl *ctl, Ctl *nxt)
{
	Insn *i;
	Ctl *l1, *l2;

	l1 = ctl->l1;
	l2 = ctl->l2;

	/* this condition is ambiguous in dybvig's report.  either
	   grouping seems correct if we assume l1 and l2 both cannot
	   be return; otherwise, i'm not sure that it matters. */
	if(returnlabel(p, l2) || (l2 == nxt && !returnlabel(p, l1))){
		i = nextinsn(code);
		i->kind = Ijnz;
		randloc(&i->op1, AC);
		i->dstlabel = l1;
		l1->used = 1;
		cgjmp(code, p, l2, nxt);
	}else{
		i = nextinsn(code);
		i->kind = Ijz;
		randloc(&i->op1, AC);
		i->dstlabel = l2;
		l2->used = 1;
		cgjmp(code, p, l1, nxt);
	}
}

static void
cgctl(Code *code, CGEnv *p, Ctl *ctl, Ctl *nxt)
{
	if(ctl->ckind == Clabel)
		cgjmp(code, p, ctl, nxt);
	else if(ctl->ckind == Clabelpair)
		cgbranch(code, p, ctl, nxt);
	else
		fatal("bad control destination");
}

static ikind EtoVM[] = {
	[Eadd] = Iadd,
	[Epreinc] = Iadd,
	[Epostinc] = Iadd,
	[Esub] = Isub,
	[Epredec] = Isub,
	[Epostdec] = Isub,
	[Emul] = Imul,
	[Ediv] = Idiv,
	[Emod] = Imod,
	[Eshl] = Ishl,
	[Eshr] = Ishr,
	[Eband] = Iand,
	[Ebor] = Ior,
	[Ebxor] = Ixor,
	[Egt] = Icmpgt,
	[Ege] = Icmpge,
	[Elt] = Icmplt,
	[Ele] = Icmple,
	[Eeq] = Icmpeq,
	[Eneq] = Icmpneq,
	[Euminus] = Ineg,
	[Eunot] = Inot,
	[Euplus] = Inop,
	[Eutwiddle] = Iinv,

	[E_car] = Icar,
	[E_cdr] = Icdr,
	[E_cval] = Icval,
	[E_encode] = Iencode,
	[E_lenl] = Ilenl,
	[E_lens] = Ilens,
	[E_lenv] = Ilenv,
	[E_range] = Irange,
	[E_sizeof] = Isizeof,
	[E_slices] = Islices,
	[E_str] = Istr,
	[E_tab] = Itab,
	[E_tabdel] = Itabdel,
	[E_tabenum] = Itabenum,
	[E_tabget] = Itabget,
	[E_tabput] = Itabput,
	[E_tn] = Itn,
	[E_vec] = Ivec,
	[E_vecl] = Ivecl,
	[E_vecref] = Ivecref,
	[E_vecset] = Ivecset,
	[E_xcast] = Ixcast,
};

static void
cgunop(Code *code, CGEnv *p, unsigned kind, Operand *r1,
       Location *loc, Ctl *ctl, Ctl *nxt)
{
	Insn *i;

	if(loc == Effect && ctl->ckind == Clabelpair)
		fatal("branch on effect");

	i = nextinsn(code);
	i->kind = EtoVM[kind];
	if(i->kind != Inop){
		i->op1 = *r1;
		randloc(&i->dst, loc);
	}
	cgctl(code, p, ctl, nxt);
}

static void
cgbinop(Code *code, CGEnv *p, unsigned kind, Operand *r1, Operand *r2,
	Location *loc, Ctl *ctl, Ctl *nxt)
{
	Insn *i;

	if(loc == Effect && ctl->ckind == Clabelpair)
		fatal("branch on effect");

	i = nextinsn(code);
	i->kind = EtoVM[kind];
	i->op1 = *r1;
	i->op2 = *r2;
	randloc(&i->dst, loc);
	cgctl(code, p, ctl, nxt);
}

static void
cg(Expr *e, Code *code, CGEnv *p, Location *loc, Ctl *ctl, Ctl *prv, Ctl *nxt,
   unsigned tmp)
{
	Ctl *L0, *L, *R, *Lthen, *Lelse, *lpair;
	Ctl *Ltest, *Lbody, *Linc;
	Operand r1, r2;
	Expr *q;
	Insn *i;
	unsigned narg, istail;
	Lambda *b;
	Location dst;
	int m;
	Varref *vr;
	CGEnv np;

	switch(e->kind){
	case Enop:
		i = nextinsn(code);
		i->kind = Inop;
		cgctl(code, p, ctl, nxt);
		break;
	case Eelist:
		if(e->e2->kind == Enull)
			cg(e->e1, code, p, loc, ctl, prv, nxt, tmp);
		else if(escaping(e->e2))
			cg(e->e1, code, p, Effect, escapectl(e->e2, p),
			   prv, nxt, tmp);
		else{
			L = genlabel(code, 0);
			cg(e->e1, code, p, Effect, L, prv, L, tmp);
			emitlabel(L, e->e2);
			cg(e->e2, code, p, loc, ctl, L, nxt, tmp);
		}
		break;
	case Enull:
		cgctl(code, p, ctl, nxt);
		break;
	case Eblock:
		cg(e->e2, code, p, loc, ctl, prv, nxt, tmp);
		break;
	case Eg:
		if(e->e1->kind != Eid)
			fatal("assignment to non-variable");
		varloc(&dst, e->e1);
		if(loc != Effect){
			L = genlabel(code, 0);
			cg(e->e2, code, p, &dst, L, prv, L, tmp);
			emitlabel(L, e);
			i = nextinsn(code);
			i->kind = Imov;
			randloc(&i->op1, &dst);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, nxt);
		}else
			cg(e->e2, code, p, &dst, ctl, prv, nxt, tmp);
		break;
	case Egop:
		if(e->e1->kind != Eid)
			fatal("assignment to non-variable");
		varloc(&dst, e->e1);
		randloc(&r1, &dst);
		L0 = genlabel(code, 0);
		cg(e->e2, code, p, AC, L0, prv, L0, tmp);
		emitlabel(L0, e);
		randloc(&r2, AC);
		if(loc != Effect){
			L = genlabel(code, 0);
			cgbinop(code, p, e->op, &r1, &r2, &dst, L, L);
			emitlabel(L, e);
			i = nextinsn(code);
			i->kind = Imov;
			i->op1 = r1;
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, nxt);
		}else
			cgbinop(code, p, e->op, &r1, &r2, &dst, ctl, nxt);
		break;
	case Epreinc:
	case Epredec:
		if(e->e1->kind != Eid)
			fatal("assignment to non-variable");
		varloc(&dst, e->e1);
		randloc(&r1, &dst);
		randimm(&r2, 1);
		if(loc != Effect){
			L = genlabel(code, 0);
			cgbinop(code, p, e->kind, &r1, &r2, &dst, L, L);
			emitlabel(L, e);
			i = nextinsn(code);
			i->kind = Imov;
			i->op1 = r1;
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, nxt);
		}else
			cgbinop(code, p, e->kind, &r1, &r2, &dst, ctl, nxt);
		break;
	case Epostinc:
	case Epostdec:
		if(e->e1->kind != Eid)
			fatal("assignment to non-variable");
		varloc(&dst, e->e1);
		randloc(&r1, &dst);
		randimm(&r2, 1);
		if(loc != Effect){
			i = nextinsn(code);
			i->kind = Imov;
			i->op1 = r1;
			randloc(&i->dst, loc);
		}
		cgbinop(code, p, e->kind, &r1, &r2, &dst, ctl, nxt);
		break;
	case Euplus:
	case Euminus:
	case Eutwiddle:
	case Eunot:
	case E_car:
	case E_cdr:
	case E_encode:
	case E_sizeof:
	case E_str:
		if(issimple(e->e1))
			cgrand(&r1, e->e1, p);
		else{
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, tmp);
			emitlabel(L, e);
			randloc(&r1, AC);
		}
		cgunop(code, p, e->kind, &r1, loc, ctl, nxt);
		break;
	case E_tn:
		if(e->e1 && e->e2){
			/* copied from Ebinop */
			if(issimple(e->e1) && issimple(e->e2)){
				cgrand(&r1, e->e1, p);
				cgrand(&r2, e->e2, p);
			}else if(issimple(e->e1)){
				cgrand(&r1, e->e1, p);
				L = genlabel(code, 0);
				cg(e->e2, code, p, AC, L, prv, L, tmp);
				emitlabel(L, e);
				randloc(&r2, AC);
			}else if(issimple(e->e2)){
				L = genlabel(code, 0);
				cg(e->e1, code, p, AC, L, prv, L, tmp);
				emitlabel(L, e);
				randloc(&r1, AC);
				cgrand(&r2, e->e2, p);
			}else{
				L0 = genlabel(code, 0);
				randstkloc(&r1, Llocal, tmp, 0);
				cg(e->e1, code, p, &r1.u.loc, L0,
				   prv, L0, tmp);
				emitlabel(L0, e->e2);
				L = genlabel(code, 0);
				cg(e->e2, code, p, AC, L, L0, L, tmp+1);
				emitlabel(L, e);
				randloc(&r2, AC);
			}
		}else if(e->e1){
			/* copied from unary ops */
			if(issimple(e->e1))
				cgrand(&r1, e->e1, p);
			else{
				L = genlabel(code, 0);
				cg(e->e1, code, p, AC, L, prv, L, tmp);
				emitlabel(L, e);
				randloc(&r1, AC);
			}
		}
		i = nextinsn(code);
		i->kind = Itn;
		i->bits = e->xn; /* FIXME: casting around warning  */
		if(e->e1)
			i->op1 = r1;
		if(e->e2)
			i->op2 = r2;
		randloc(&i->dst, loc);
		break;
	case Ebinop:
		if(issimple(e->e1) && issimple(e->e2)){
			cgrand(&r1, e->e1, p);
			cgrand(&r2, e->e2, p);
		}else if(issimple(e->e1)){
			cgrand(&r1, e->e1, p);
			L = genlabel(code, 0);
			cg(e->e2, code, p, AC, L, prv, L, tmp);
			emitlabel(L, e);
			randloc(&r2, AC);
		}else if(issimple(e->e2)){
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, tmp);
			emitlabel(L, e);
			randloc(&r1, AC);
			cgrand(&r2, e->e2, p);
		}else{
			L0 = genlabel(code, 0);
			randstkloc(&r1, Llocal, tmp, 0);
			cg(e->e1, code, p, &r1.u.loc, L0, prv, L0, tmp);
			emitlabel(L0, e->e2);
			L = genlabel(code, 0);
			cg(e->e2, code, p, AC, L, L0, L, tmp+1);
			emitlabel(L, e);
			randloc(&r2, AC);
		}
		cgbinop(code, p, e->op, &r1, &r2, loc, ctl, nxt);
		break;
	case Ecall:
		istail = (ctl == p->Return && (loc == AC || loc == Effect));

		if(!istail){
			if(loc != Effect)
				R = genlabel(code, 0);
			else
				R = ctl;
			i = nextinsn(code);
			i->kind = Iframe;
			i->dstlabel = R;
			R->used = 1;
		}
		
		q = e->e2;
		narg = 0;
		L0 = prv;
		while(q->kind != Enull){
			L = genlabel(code, 0);
			// FIXME: can we check for simple? here and push
			// the operand directly? (and likewise for the call?)
			cg(q->e1, code, p, AC, L, L0, L, tmp);
			emitlabel(L, q->e2);
			i = nextinsn(code);
			i->kind = Ipush;
			randloc(&i->op1, AC);
			q = q->e2;
			narg++;
			L0 = L;
		}
		
		i = nextinsn(code);
		i->kind = Ipush;
		randimm(&i->op1, narg);

		L0 = genlabel(code, 0);
		emitlabel(L0, e->e1);
		L = genlabel(code, 0);
		cg(e->e1, code, p, AC, L, L0, L, tmp);
		emitlabel(L, e);

		if(!istail){
			i = nextinsn(code);
			i->kind = Icall;
			randloc(&i->op1, AC);
			if(loc != Effect){
				emitlabel(R, e);
				/* FIXME: we emit this mov even if loc is AC,
				   because there seems to be no simple way
				   to manage the labels otherwise. */
				i = nextinsn(code);
				i->kind = Imov;
				randloc(&i->op1, AC);
				randloc(&i->dst, loc);
			}
			cgctl(code, p, ctl, nxt);
		}else{
			i = nextinsn(code);
			i->kind = Icallt;
			randloc(&i->op1, AC);
		}
		break;
	case Eret:
		if(e->e1)
			cg(e->e1, code, p, loc, p->Return, prv, nxt, tmp);
		else
			cgctl(code, p, p->Return0, nxt);
		break;
	/* can Eid and Econst be rationalized with cgrand? */
	case Eid:
		i = nextinsn(code);
		i->kind = Imov;
		randvarloc(&i->op1, e);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt);
		break;
	case Econst:
		i = nextinsn(code);
		i->kind = Imov;
		randcval(&i->op1, &e->cval);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt);
		break;
	case Econsts:
		i = nextinsn(code);
		i->kind = Imov;
		randlits(&i->op1, e->xp);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt);
		break;
	case Enil:
		i = nextinsn(code);
		i->kind = Imov;
		randnil(&i->op1);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt);
		break;
	case Elambda:
		b = (Lambda*)e->xp;
		L = genlabel(code, 0);

		for(m = b->capture->nvr-1; m >= 0; m--){
			vr = &b->capture->vr[m];
			i = nextinsn(code);
			i->kind = Imov;
			randrefloc(&i->op1, vr);
			randloc(&i->dst, AC);
			i = nextinsn(code);
			i->kind = Ipush;
			randloc(&i->op1, AC);
		}

		i = nextinsn(code);
		i->kind = Iclo;
		randimm(&i->op1, b->capture->nvr);
		randloc(&i->dst, loc);
		i->dstlabel = L;
		L->used = 1;

		i = nextinsn(code);
		i->kind = Ijmp;
		i->dstlabel = ctl;
		ctl->used = 1;

		emitlabel(L, e);
		compilelambda(L, code, e);

		break;
	case Eland:
		if(loc == Effect){
			L = genlabel(code, 0);
			if(ctl->ckind == Clabelpair)
				lpair = genlabelpair(code, L, ctl->l2);
			else
				lpair = genlabelpair(code, L, ctl);
			cg(e->e1, code, p, AC, lpair, prv, L, tmp);
			emitlabel(L, e->e2);
			if(ctl->ckind == Clabelpair)
				cg(e->e2, code, p, AC, ctl, L, nxt, tmp);
			else
				cg(e->e2, code, p, Effect, ctl, L, nxt, tmp);
		}else{
			Lthen = genlabel(code, 0);
			Lelse = genlabel(code, 0);
			lpair = genlabelpair(code, Lthen, Lelse);
			cg(e->e1, code, p, AC, lpair, prv, Lthen, tmp);
			emitlabel(Lthen, e->e2);
			L0 = Lthen;
			Lthen = genlabel(code, 0);
			lpair = genlabelpair(code, Lthen, Lelse);
			cg(e->e2, code, p, AC, lpair, L0, Lthen, tmp);
			emitlabel(Lthen, e);
			i = nextinsn(code);
			i->kind = Imov;
			randimm(&i->op1, 1);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, Lelse);
			emitlabel(Lelse, e);
			i = nextinsn(code);
			i->kind = Imov;
			randimm(&i->op1, 0);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, nxt);
		}
		break;
	case Elor:
		if(loc == Effect){
			L = genlabel(code, 0);
			if(ctl->ckind == Clabelpair)
				lpair = genlabelpair(code, ctl->l1, L);
			else
				lpair = genlabelpair(code, ctl, L);
			cg(e->e1, code, p, AC, lpair, prv, L, tmp);
			emitlabel(L, e->e2);
			L0 = L;
			if(ctl->ckind == Clabelpair)
				cg(e->e2, code, p, AC, ctl, L0, nxt, tmp);
			else
				cg(e->e2, code, p, Effect, ctl, L0, nxt, tmp);
		}else{
			Lthen = genlabel(code, 0);
			Lelse = genlabel(code, 0);
			lpair = genlabelpair(code, Lthen, Lelse);
			cg(e->e1, code, p, AC, lpair, prv, Lelse, tmp);
			emitlabel(Lelse, e->e2);
			L0 = Lelse;
			Lelse = genlabel(code, 0);
			lpair = genlabelpair(code, Lthen, Lelse);
			cg(e->e2, code, p, AC, lpair, L0, Lthen, tmp);
			emitlabel(Lthen, e);
			i = nextinsn(code);
			i->kind = Imov;
			randimm(&i->op1, 1);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, Lelse);
			emitlabel(Lelse, e);
			i = nextinsn(code);
			i->kind = Imov;
			randimm(&i->op1, 0);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, nxt);
		}
		break;
	case Eif:
		if(e->e3){
			if(escaping(e->e2) && !escaping(e->e3)){
				Lelse = genlabel(code, 0);
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     Lelse);
				cg(e->e1, code, p, AC, lpair, prv, Lelse, tmp);
				emitlabel(Lelse, e->e3);
				cg(e->e3, code, p, Effect, ctl,
				   Lelse, nxt, tmp);
			}else if(!escaping(e->e2) && escaping(e->e3)){
				Lthen = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen,
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, prv, Lthen, tmp);
				emitlabel(Lthen, e->e2);
				cg(e->e2, code, p, Effect, ctl,
				   Lthen, nxt, tmp);
			}else if(escaping(e->e2) && escaping(e->e3)){
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, prv, nxt, tmp);
			}else{
				Lthen = genlabel(code, 0);
				Lelse = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen, Lelse);
				cg(e->e1, code, p, AC, lpair, prv, Lthen, tmp);
				emitlabel(Lthen, e->e2);
				cg(e->e2, code, p, Effect, ctl,
				   Lthen, Lelse, tmp);
				emitlabel(Lelse, e->e3);
				cg(e->e3, code, p, Effect, ctl,
				   Lelse, nxt, tmp);
			}
		}else{
			if(escaping(e->e2)){
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     ctl);
				cg(e->e1, code, p, AC, lpair, prv, nxt, tmp);
			}else{
				Lthen = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen, ctl);
				cg(e->e1, code, p, AC, lpair, prv, Lthen, tmp);
				emitlabel(Lthen, e->e2);
				cg(e->e2, code, p, Effect, ctl,
				   Lthen, nxt, tmp);
			}
		}
		break;
	case Ebreak:
		if(p->Break == 0)
			fatal("break outside loop");
		cgctl(code, p, p->Break, nxt);
		break;
	case Econtinue:
		if(p->Continue == 0)
			fatal("continue outside loop");
		cgctl(code, p, p->Continue, nxt);
		break;
	case Efor:
		if(ctl->ckind != Clabel)
			fatal("branch on statement");
		Ltest = genlabel(code, 0);
		Lbody = genlabel(code, 0);
		Linc = genlabel(code, 0);
		lpair = genlabelpair(code, Lbody, ctl);
		np = *p;
		np.Continue = Linc;
		np.Break = ctl;
		
		if(e->e1){
			cg(e->e1, code, p, Effect, Ltest, prv, Ltest, tmp);
			emitlabel(Ltest, e->e2);			
		}else
			Ltest = prv;

		if(e->e2){
			cg(e->e2, code, p, AC, lpair, Ltest, Lbody, tmp);
			emitlabel(Lbody, e->e4);
		}else
			Lbody = Ltest;

		cg(e->e4, code, &np, Effect, Linc, Lbody, Linc, tmp);
		if(code->ninsn == Lbody->insn)
			Linc = Lbody;
		else
			emitlabel(Linc, e->e3);

		if(e->e3)
			cg(e->e3, code, p, Effect, Ltest, Linc, nxt, tmp);
		else
			cgctl(code, p, Ltest, nxt);
		break;
	case Ewhile:
		if(ctl->ckind != Clabel)
			fatal("branch on statement");
		Lbody = genlabel(code, 0);
		lpair = genlabelpair(code, Lbody, ctl);
		Ltest = prv;
		np = *p;
		np.Continue = Ltest;
		np.Break = ctl;
		cg(e->e1, code, p, AC, lpair, Ltest, Lbody, tmp);
		emitlabel(Lbody, e->e2);
		cg(e->e2, code, &np, Effect, Ltest, Lbody, nxt, tmp);
		break;
	case Edo:
		if(ctl->ckind != Clabel)
			fatal("branch on statement");
		Lbody = prv;
		Ltest = genlabel(code, 0);
		lpair = genlabelpair(code, Lbody, ctl);
		np = *p;
		np.Continue = Ltest;
		np.Break = ctl;
		cg(e->e1, code, &np, Effect, Ltest, Lbody, Ltest, tmp);
		if(code->ninsn > Lbody->insn){
			emitlabel(Ltest, e->e2);
			cg(e->e2, code, p, AC, lpair, Ltest, nxt, tmp);
		}else
			cg(e->e2, code, p, AC, lpair, Lbody, nxt, tmp);
		break;
	default:
		fatal("cg undefined for expression %d", e->kind);
		break;
	}
}

static void
compilelambda(Ctl *name, Code *code, Expr *e)
{
	unsigned entry;
	Lambda *b;
	Insn *i;
	CGEnv p;
	unsigned m, needtop = 0;
	Ctl *top;

	if(e->kind != Elambda)
		fatal("compilelambda on non-lambda");

	b = (Lambda*)e->xp;
	if(b->vararg)
		printf("%s: var params, %u locals, %u temps\n",
		       name->label, b->nloc, b->ntmp);
	else
		printf("%s: %u params, %u locals, %u temps\n",
		       name->label, b->npar, b->nloc, b->ntmp);
	p.b = b;

	entry = code->ninsn;
	if(b->nloc+b->ntmp > 0){
		i = nextinsn(code);
		i->kind = Isub;
		randloc(&i->op1, SP);
		randimm(&i->op2, b->nloc+b->ntmp);
		randloc(&i->dst, SP);
		needtop = 1;
	}
	for(m = 0; m < b->npar; m++)
		if(b->param[m].indirect){
			i = nextinsn(code);
			i->kind = Ibox;
			randvdloc(&i->op1, &b->param[m]);
			needtop = 1;
		}
	for(m = 0; m < b->nloc; m++)
		if(b->local[m].indirect){
			i = nextinsn(code);
			i->kind = Ibox0;
			randvdloc(&i->op1, &b->local[m]);
			needtop = 1;
		}
	if(b->vararg){
		i = nextinsn(code);
		i->kind = Ivlist;
		randloc(&i->op1, FP);
		/* by convention local 0 is var arg list */
		randstkloc(&i->dst, Llocal, 0, b->local[0].indirect); 
	}
	if(needtop){
		top = genlabel(code, 0);
		emitlabel(top, e->e2);
	}else
		top = name;

	p.Return0 = genlabel(code, 0);
	p.Return = genlabel(code, 0);
	p.Break = 0;
	p.Continue = 0;
	cg(e->e2, code, &p, Effect, p.Return0, top, p.Return0, b->nloc);

	if(p.Return0->used) /* hack for lambdas with empty bodies */
		emitlabel(p.Return0, e->e2);
	i = nextinsn(code);
	i->kind = Imov;
	randnil(&i->op1);
	randloc(&i->dst, AC);
	emitlabel(p.Return, e->e2);
	i = nextinsn(code);
	i->kind = Iret;
}

Closure*
compileentry(Expr *el, Env *env, int flags)
{
	Ctl *L;
	Expr *le;
	VDset *cap;
	Code *code;
	Closure *cl;
	Lambda *b;

	code = mkcode();
	
	L = genlabel(code, "entry");
	L->used = 1;
	emitlabel(L, el);

	le = newexpr(Elambda, nullelist(), el, 0, 0);

	newmapframe(le, 0, 0, code->topvec, env, code->konst);
	cap = mkvdset();
	mapcapture(le, cap);
	freevdset(cap);
	code->src = le;

	compilelambda(L, code, le);
	b = (Lambda*)le->xp;
	cl = mkcl(code, 0, b->capture->nvr, L->label);

	if(flags&Fprintobj)
		printcode(code);
	return cl;
}

Closure*
haltthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "thunk");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ihalt;

	return cl;
}

Closure*
gcthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "gc");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Igc;
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
callcc()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "callcc");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Imov;
	randstkloc(&i->op1, Lparam, 0, 0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Ikg;
	randstkloc(&i->dst, Lparam, 0, 0);
	i = nextinsn(code);
	i->kind = Icallt;
	randloc(&i->op1, AC);

	return cl;

}

Code*
contcode()
{
	Insn *i;
	Code *code;

	code = mkcode();
	i = nextinsn(code);
	i->kind = Imov;
	randstkloc(&i->op1, Lparam, 0, 0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Ikp;
	i = nextinsn(code);
	i->kind = Iret;

	return code;
}

Closure*
dingthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "ding");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iding;
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
nullthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "null");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Inull;
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
iscvaluethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "iscvalue");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iiscval;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
isasthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isas");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iisas;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
isdomthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isdom");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iisdom;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
isnsthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isns");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iisns;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
isprocedurethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isprocedure");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iiscl;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
isnullthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isnull");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iisnull;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
ispairthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "ispair");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iispair;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
israngethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isrange");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iisrange;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
isstringthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isstring");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iisstr;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
istablethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "istable");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iistab;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
istnthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "istypename");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iistn;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
typenamexthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "typenamex");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Itnx;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
istypethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "istype");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iistype;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
isvectorthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "isvector");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iisvec;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
carthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "car");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Icar;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
cdrthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "cdr");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Icdr;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
consthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "cons");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Icons;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
rangebegthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "rangebeg");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Irbeg;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
rangelenthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "rangelen");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Irlen;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
rangethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "range");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Irange;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
stringthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "string");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Istr;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
strlenthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "strlen");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ilens;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
substrthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "substr");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Islices;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->op3, ARG2);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
printthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "print");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Iprint;
	randloc(&i->op1, ARG0);
	i = nextinsn(code);
	i->kind = Imov;
	randnil(&i->op1);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
mkvecthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "mkvec");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ivec;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
vectorthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "vector");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ivvec;
	randloc(&i->op1, FP);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
veclenthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "veclen");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ilenv;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
vecrefthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "vecref");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ivecref;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
vecsetthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "vecset");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ivecset;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->op3, ARG2);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
tablethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "table");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Itab;
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
tabinsertthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "tabinsert");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Itabput;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->op3, ARG2);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
tabdeletethunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "tabdelete");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Itabdel;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
tablookthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "tablook");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Itabget;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
tabenumthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "tabenum");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Itabenum;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
mkasthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "mkas");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ias;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
mkdomthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "mkdom");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Idom;
	randloc(&i->op1, ARG0);
	randloc(&i->op2, ARG1);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
domasthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "domas");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Idomas;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
domnsthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "domns");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Idomns;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

Closure*
mknsthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "mkns");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ins;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Iret;

	return cl;
}

void
initcompile()
{
	Effect = &toploc[0];
	newloc(Effect, Lreg, Rac, 0);
	AC = &toploc[1];
	newloc(AC, Lreg, Rac, 0);
	FP = &toploc[2];
	newloc(FP, Lreg, Rfp, 0);
	PC = &toploc[3];
	newloc(PC, Lreg, Rpc, 0);
	SP = &toploc[4];
	newloc(SP, Lreg, Rsp, 0);
	ARG0 = &toploc[5];
	newloc(ARG0, Lparam, 0, 0);
	ARG1 = &toploc[6];
	newloc(ARG1, Lparam, 1, 0);
	ARG2 = &toploc[7];
	newloc(ARG2, Lparam, 2, 0);
}

void
finicompile()
{
}
