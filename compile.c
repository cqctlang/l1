#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef struct Lambda Lambda;

typedef
struct Cases {
	unsigned n, max;
	Expr **cmp;
	Ctl **ctl;
	Ctl *dflt;
	Expr *dflte;
} Cases;

typedef
struct CGEnv {
	Lambda *b;
	Ctl *Return;
	Ctl *Return0;
	Ctl *Break;
	Ctl *Continue;
	Cases *cases;
} CGEnv;

static Location toploc[8];
static Location *Effect;
static Location *AC, *FP, *SP, *PC, *ARG0, *ARG1, *ARG2;
static void compilelambda(Ctl *name, Code *code, Expr *el);
static Topvec *mktopvec();
static void freetopvec(Topvec *tv);
static Konst* mkkonst();
static void freekonst(Konst *kon);
static Konsti* mkkonsti();
static void freekonsti(Konsti *koni);
static void freedecl(Decl *d);

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
	ctl = emalloc(sizeof(Ctl));
	ctl->link = code->clist;
	ctl->code = code;
	code->clist = ctl;
	return ctl;
}

static void
freelabel(Ctl *ctl)
{
	if(ctl->ckind == Clabel)
		efree(ctl->label);
	efree(ctl);
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
		ctl->label = xstrdup(s);
	else{
		snprintf(buf, sizeof(buf), "L%d", labelseed++);
		ctl->label = xstrdup(buf);
	}
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
	if(e && e->src.line)
		ctl->src = &e->src;
}

static Code*
mkcode()
{
	Code *code;
	
	code = newcode();
	code->maxinsn = InsnAlloc;
	code->insn = emalloc(code->maxinsn*sizeof(Insn));
	code->labels = emalloc(code->maxinsn*sizeof(Ctl*));
	code->ninsn = 0;
	code->topvec = mktopvec();
	code->konst = mkkonst();
	code->konsti = mkkonsti();

	return code;
}

int
freecode(Head *hd)
{
	Code *code;
	Ctl *p, *q;

	code = (Code*)hd;
	freekonst(code->konst);
	freekonsti(code->konsti);
	freetopvec(code->topvec);
	p = code->clist;
	while(p){
		q = p->link;
		freelabel(p);
		p = q;
	}
	
	freeexpr(code->src);
	efree(code->insn);
	efree(code->labels);
	return 1;
}

static Insn*
nextinsn(Code *code)
{
	/* plan for one extra instruction for emitlabel */
	if(code->ninsn+1 >= code->maxinsn){
		code->insn = erealloc(code->insn,
				      code->maxinsn*sizeof(Insn),
				      2*code->maxinsn*sizeof(Insn));
		code->labels = erealloc(code->labels,
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
			xprintf("%s", regtos(loc->idx));
			break;
		case Lparam:
			if(loc->indirect)
				xprintf("[");
			xprintf("%d(%s)", loc->idx+1, regtos(Rfp));
			if(loc->indirect)
				xprintf("]");
			break;
		case Llocal:
			if(loc->indirect)
				xprintf("[");
			xprintf("-%d(%s)", loc->idx+1, regtos(Rfp));
			if(loc->indirect)
				xprintf("]");
			break;
		case Ldisp:
			if(loc->indirect)
				xprintf("[");
			xprintf("%d(%s)", loc->idx, regtos(Rcl));
			if(loc->indirect)
				xprintf("]");
			break;
		case Ltopl:
			xprintf("<%s>", topvecid(loc->idx, code->topvec));
			break;
		}
		break;
	case Oliti:
		xprintf("%" PRIu64, valimm(r->u.liti));
		break;
	case Onil:
		xprintf("nil");
		break;
	case Olits:
		lits = r->u.lits;
		if(lits->len > 10)
			xprintf("*lits*");
		else
			xprintf("\"%.*s\"", lits->len, lits->s);
		break;
	default:
		fatal("unknown operand kind %d", r->okind);
	}
}

static void
printinsn(Code *code, Insn *i)
{
	xprintf("\t");
	switch(i->kind){
	case Iargc:
		xprintf("argc ");
		printrand(code, &i->op1);
		break;
	case Icallc:
		xprintf("callc");
		break;
	case Iinv:
	case Ineg:
	case Inot:
		xprintf("%s ", itos(i->kind));
		printrand(code, &i->op1);
		xprintf(" ");
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
		xprintf("%s ", itos(i->kind));
		printrand(code, &i->op1);
		xprintf(" ");
		printrand(code, &i->op2);
		xprintf(" ");
		printrand(code, &i->dst);
		break;
	case Imov:
		xprintf("mov ");
		printrand(code, &i->op1);
		xprintf(" ");
		printrand(code, &i->dst);
		break;
	case Ipush:
		xprintf("push ");
		printrand(code, &i->op1);
		break;
	case Ipushi:
		xprintf("pushi ");
		printrand(code, &i->op1);
		break;
	case Icall:
		xprintf("call ");
		printrand(code, &i->op1);
		break;
	case Icallt:
		xprintf("callt ");
		printrand(code, &i->op1);
		break;
	case Icval:
		xprintf("cval ");
		printrand(code, &i->op1);
		xprintf(" ");
		printrand(code, &i->op2);
		xprintf(" ");
		printrand(code, &i->dst);
		break;
	case Ixcast:
		xprintf("xcast ");
		printrand(code, &i->op1);
		xprintf(" ");
		printrand(code, &i->op2);
		xprintf(" ");
		printrand(code, &i->dst);
		break;
	case Ilist:
		xprintf("list ");
		printrand(code, &i->op1);
		xprintf(" ");
		printrand(code, &i->dst);
		break;
	case Isizeof:
		xprintf("sizeof ");
		printrand(code, &i->op1);
		xprintf(" ");
		printrand(code, &i->dst);
		break;
	case Iframe:
		xprintf("frame %s", i->dstlabel->label);
		break;
	case Iret:
		xprintf("ret");
		break;
	case Ihalt:
		xprintf("halt");
		break;
	case Ipanic:
		xprintf("panic");
		break;
	case Iclo:
		xprintf("clo %s ", i->dstlabel->label);
		printrand(code, &i->op1);
		xprintf(" ");
		printrand(code, &i->dst);
		break;
	case Ikg:
		xprintf("kg ");
		printrand(code, &i->dst);
		break;
	case Ikp:
		xprintf("kp");
		break;
	case Ijmp:
		xprintf("jmp %s", i->dstlabel->label);
		break;
	case Ijnz:
		xprintf("jnz ");
		printrand(code, &i->op1);
		xprintf(" %s", i->dstlabel->label);
		break;
	case Ijz:
		xprintf("jz ");
		printrand(code, &i->op1);
		xprintf(" %s", i->dstlabel->label);
		break;
	case Ibox:
		xprintf("box ");
		printrand(code, &i->op1);
		break;
	case Ibox0:
		xprintf("box0 ");
		printrand(code, &i->op1);
		break;
	case Inop:
		xprintf("nop");
		break;
	case Iref:
		xprintf("ref ");
		printrand(code, &i->op1);
		break;
	default:
		fatal("printinsn: unrecognized insn %d", i->kind);
	}
	xprintf("\n");
}

static void
printcode(Code *code)
{
	unsigned i;
	for(i = 0; i < code->ninsn; i++){
		if(code->labels[i] && code->labels[i]->used)
			xprintf("%s", code->labels[i]->label);
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
	return (e->kind == Eid
		|| e->kind == Econst
		|| e->kind == Enil);
}

static unsigned
listlen(Expr *l)
{
	unsigned n;
	n = 0;
	while(l->kind != Enull){
		l = l->e2;
		n++;
	}
	return n;
}

/* flatten local declarations within and determine # of locals for lambda E */
static unsigned
locpass(Expr *e)
{
	unsigned m;
	Expr *p;

	if(e == NULL)
		return 0;

	switch(e->kind){
	case Elambda:
		return 0;
	case Eblock:
		m = listlen(e->e1);
		return m+locpass(e->e2);
	case Eelist:
		p = e;
		m = 0;
		while(p->kind == Eelist){
			m = max(m, locpass(p->e1));
			p = p->e2;
		}
		return m;
	default:
		m = locpass(e->e1);
		m = max(m, locpass(e->e2));
		m = max(m, locpass(e->e3));
		m = max(m, locpass(e->e4));
		return m;
	}
}

/* determine # of tmps for lambda E */
static unsigned
tmppass(Expr *e)
{
	Expr *p;
	unsigned m;

	if(e == NULL)
		return 0;

	switch(e->kind){
	case Elambda:
		return 0;
	case Eblock:
		return tmppass(e->e2);
	case Ebinop:
		if(issimple(e->e1) && issimple(e->e2))
			return 0;
		else if(issimple(e->e1))
			return tmppass(e->e2);
		else if(issimple(e->e2))
			return tmppass(e->e1);
		else
			return 1+max(tmppass(e->e1),
				     tmppass(e->e2));
	case Eelist:
		p = e;
		m = 0;
		while(p->kind == Eelist){
			m = max(m, tmppass(p->e1));
			p = p->e2;
		}
		return m;
	default:
		m = tmppass(e->e1);
		m = max(m, tmppass(e->e2));
		m = max(m, tmppass(e->e3));
		m = max(m, tmppass(e->e4));
		return m;
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
	Lambda *lmbda;
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
	unsigned ntmp, npar, maxloc, vararg;
	VEnv *ve;
	VRset *capture;
	char *id;
};

Xenv*
mkxenv(Xenv *link)
{
	Xenv *xe;
	xe = emalloc(sizeof(Xenv));
	xe->ht = mkht();
	xe->link = link;
	return xe;
}

void
freexenv(Xenv *xe)
{
	freeht(xe->ht);
	efree(xe);
}

void*
xenvlook(Xenv *xe, char *id)
{
	void *v;

	if(xe == 0)
		return 0;
	v = hget(xe->ht, id, strlen(id));
	if(v)
		return v;
	return xenvlook(xe->link, id);
}

void
xenvbind(Xenv *xe, char *id, void *v)
{
	hput(xe->ht, id, strlen(id), v);
}

void
xenvforeach(Xenv *xe, void (*f)(void *u, char *k, void *v), void *u)
{
	hforeach(xe->ht, f, u);
}

unsigned long
xenvsize(Xenv *xe)
{
	return hnent(xe->ht);
}

static Topvec*
mktopvec()
{
	Topvec *tv;
	tv = emalloc(sizeof(Topvec));
	tv->maxid = 128;
	tv->id = emalloc(tv->maxid*sizeof(char*));
	tv->val = emalloc(tv->maxid*sizeof(Val*));
	return tv;
}

static void
freetopvec(Topvec *tv)
{
	unsigned m;
	for(m = 0; m < tv->nid; m++)
		efree(tv->id[m]);
	efree(tv->id);
	efree(tv->val);
	efree(tv);
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
		tv->id = erealloc(tv->id,
				  tv->maxid*sizeof(char*),
				  2*tv->maxid*sizeof(char*));
		tv->val = erealloc(tv->val,
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
	kon = emalloc(sizeof(Konst));
	kon->ht = mkht();
	return kon;
}

static Lits*
konstlookup(Lits *lits, Konst *kon)
{
	return hget(kon->ht, lits->s, lits->len);
}

static Lits*
konstadd(Lits *lits, Konst *kon)
{
	lits = copylits(lits);
	hput(kon->ht, lits->s, lits->len, lits);
	return lits;
}

static void
free1konst(void *u, char *k, void *v)
{
	freelits((Lits*)v);
}

static void
freekonst(Konst *kon)
{
	hforeach(kon->ht, free1konst, 0);
	freeht(kon->ht);
	efree(kon);
}

static Konsti*
mkkonsti()
{
	Konsti *koni;
	koni = emalloc(sizeof(Konsti));
	koni->ht = mkht();
	return koni;
}

static Val
konsti2val(Cbase base, Imm imm, Konsti *koni)
{
	Val v;
	char buf[11+Maxliti];	/* Vlongdouble+Maxliti */
	char *s;

	snprintf(buf, sizeof(buf), "%s%" PRIu64, cbasename[base], imm);
	v = hget(koni->ht, buf, strlen(buf));
	if(v)
		return v;
	s = xstrdup(buf);
	v = mkvallitcval(base, imm);
	hput(koni->ht, s, strlen(s), v);
	return v;
}

static Val
konstival(Liti *liti, Konsti *koni)
{
	return konsti2val(liti->base, liti->val, koni);
}

static void
free1konsti(void *u, char *k, void *v)
{
	efree(k);
}

static void
freekonsti(Konsti *koni)
{
	hforeach(koni->ht, free1konsti, 0);
	freeht(koni->ht);
	efree(koni);
}

static VEnv*
mkvenv()
{
	VEnv *ve;
	ve = emalloc(sizeof(VEnv));
	return ve;
}

static void
freevenv(VEnv *ve)
{
	efree(ve);
}

static VDset*
mkvdset()
{
	VDset *vs;
	vs = emalloc(sizeof(VDset));
	vs->maxvd = 128;
	vs->vd = emalloc(vs->maxvd*sizeof(*vs->vd));
	return vs;
}

static void
freevdset(VDset *vs)
{
	efree(vs->vd);
	efree(vs);
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
		vs->vd = erealloc(vs->vd,
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
	vs = emalloc(sizeof(VRset));
	vs->nvr = n;
	vs->vr = emalloc(vs->nvr*sizeof(Varref));
	return vs;
}

static void
freevrset(VRset *vs)
{
	efree(vs->vr);
	efree(vs);
}

static Lambda*
mklambda(Expr *p)
{
	Lambda *b;
	Expr *q;
	Vardef *vd;
	unsigned m;
	
	b = emalloc(sizeof(Lambda));

	b->npar = 0;
	q = p->e1;
	if(q->kind == Eid){
		b->vararg = 1;
		/* wrap lambda in block declaration to allocate conventional
		   local 0 vararg list bound to vararg variable */
		p->e2 = newexpr(Eblock, Zlocals(1, q->id), p->e2, 0, 0);
		freeexpr(q);
		p->e1 = 0;
	}else
		/* ordinary argument list */
		b->npar = listlen(q);

	b->param = emalloc(b->npar*sizeof(Vardef));
	b->ntmp = tmppass(p->e2);
	b->maxloc = locpass(p->e2);
	b->local = emalloc(b->maxloc*sizeof(Vardef));

	if(!b->vararg){
		vd = b->param;
		m = 0;
		q = p->e1;
		while(q->kind != Enull){
			vd->lmbda = b;
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
	efree(b->local);
	efree(b->param);
	efree(b->id);
	efree(b);
}

static unsigned
bindlocal(Lambda *b, Expr *p, unsigned nloc)
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
	if(m+nloc > b->maxloc)
		fatal("bug");

	vd = b->local+nloc;
	q = p;
	while(q->kind != Enull){
		vd->lmbda = b;
		vd->id = q->e1->id;
		vd->kind = VDlocal;
		/* all locals are indirect, since they must be written
		   to be useful */
		vd->indirect = 1;
		vd->idx = nloc;
		q = q->e2;
		vd++;
		nloc++;
	}

	return nloc;
}

static Varref*
mkvarref(Vardef *vd)
{
	Varref *vr;
	vr = emalloc(sizeof(Varref));
	vr->vd = vd;
	return vr;
}

static Varref*
mktoplevelref(char *id, Topvec *tv, Env *env)
{
	Vardef *vd;
	int idx;

	vd = emalloc(sizeof(Vardef));
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
		efree(vr->vd);
	efree(vr);
}

static void
freetype(Type *t)
{
	if(t == 0)
		return;

	freetype(t->link);
	efree(t->dom);
	efree(t->tid);
	efree(t->tag);
	freedecl(t->field);
	freeenum(t->en);
	freedecl(t->param);
	freeexpr(t->bitw);
	freeexpr(t->bit0);
	freeexpr(t->sz);
	freeexpr(t->cnt);
	efree(t);
}

static void
freedecl(Decl *d)
{
	Decl *nxt;

	nxt = d;
	while(nxt){
		d = nxt;
		nxt = d->link;
		freetype(d->type);
		freeexpr(d->offs);
		efree(d->id);
		efree(d);
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
	case Etypedef:
	case Edecl:
	case Edecls:
		freedecl((Decl*)e->xp);
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
mapframe(Expr *e, Lambda *curb, VEnv *ve, Topvec *tv, Env *env,
	 Konst *kon, Konsti *koni, unsigned ploc)
{
	char *id;
	Lambda *b;
	VEnv *nve;
	Vardef *vd;
	Varref *vr;
	unsigned nloc;
	Expr *p;

	if(e == NULL)
		return;

	switch(e->kind){
	case Econst:
		e->xp = konstival(&e->liti, koni);
		break;
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
	case Epreinc:
	case Epostinc:
	case Epredec:
	case Epostdec:
	case Eg:
	case Egop:
		if(e->e1->kind != Eid)
			fatal("bug");
		id = e->e1->id;
		vd = varlookup(id, ve);
		if(vd == NULL)
			vr = mktoplevelref(id, tv, env);
		else{
			vd->indirect = 1;
			vr = mkvarref(vd);
		}
		e->e1->xp = vr;
		if(e->kind == Eg || e->kind == Egop)
			mapframe(e->e2, curb, ve, tv, env, kon, koni, ploc);
		break;
	case Eblock:
		nloc = bindlocal(curb, e->e1, ploc);
		if(nloc > ploc){
			nve = mkvenv();
			nve->link = ve;
			nve->nv = nloc-ploc;
			nve->hd = &curb->local[ploc];
			e->xp = nve;
			ve = nve;
		}
		mapframe(e->e2, curb, ve, tv, env, kon, koni, nloc);
		break;
	case Elambda:
		b = mklambda(e);
		e->xp = b;
		b->ve->link = ve;
		ve = b->ve;
		if(e->e3) /* Edefine, Edeflocal */
			b->id = xstrdup(e->e3->id);
		mapframe(e->e2, b, b->ve, tv, env, kon, koni, 0);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			mapframe(p->e1, curb, ve, tv, env, kon, koni, ploc);
			p = p->e2;
		}
		break;
	default:
		mapframe(e->e1, curb, ve, tv, env, kon, koni, ploc);
		mapframe(e->e2, curb, ve, tv, env, kon, koni, ploc);
		mapframe(e->e3, curb, ve, tv, env, kon, koni, ploc);
		mapframe(e->e4, curb, ve, tv, env, kon, koni, ploc);
		break;
	}	
}

static int
lexbinds(Expr *e, char *id)
{
	Expr *p;

	if(e == 0)
		return 0;
	
	/* special case: vararg Lambda */
	if(e->kind == Elambda && e->e1->kind == Eid){
		if(!strcmp(e->e1->id, id))
			return 1;
		return lexbinds((Expr*)e->xp, id);
	}

	/* Eblock or Elambda with variable list */
	p = e->e1;
	while(p->kind != Enull){
		if(!strcmp(p->e1->id, id))
			return 1;
		p = p->e2;
	}
	return lexbinds((Expr*)e->xp, id);
}

static void
newlocal(Expr *e, char *id)
{
	if(e == 0)
		fatal("bug");	/* there should be an outer Eblock */
	if(e->kind == Elambda){
		newlocal((Expr*)e->xp, id);
		return;
	}
	e->e1 = newexpr(Eelist, doid(id), e->e1, 0, 0);
}

static Expr*
globals(Expr *e, Env *env)
{
	Expr *p;
	char *id, *is;
	unsigned len;

	if(e == 0)
		return e;

	switch(e->kind){
	case Edeflocal:
	case Edefine:
		p = Zset(e->e1,
			 Zlambdn(e->e2,
				 globals(e->e3, env),
				 copyexpr(e->e1)));
		if(e->kind == Edefine)
			envgetbind(env, e->e1->id);
		e->e1 = 0;
		e->e2 = 0;
		e->e3 = 0;
		p->src = e->src;
		freeexpr(e);
		return p;
	case Edefrec:
		id = e->e1->id;
		len = 2+strlen(id)+1;
		is = emalloc(len);
		snprintf(is, len, "is%s", id);
		envgetbind(env, id);
		envgetbind(env, is);
		p = Zblock(Zlocals(1, "$rd"),
			   Zset(doid("$rd"),
				Zcall(doid("mkrd"), 2,
				      Zconsts(id),
				      Zids2strs(e->e2))),
			   Zset(doid(id), Zcall(doid("rdmk"), 1, doid("$rd"))),
			   Zset(doid(is), Zcall(doid("rdis"), 1, doid("$rd"))),
			   doid("$rd"),
			   NULL);
		efree(is);
		p->src = e->src;
		freeexpr(e);
		return p;
	case Eglobal:
		p = e->e1;
		while(p->kind == Eelist){
			envgetbind(env, p->e1->id);
			p = p->e2;
		}
		freeexpr(e);
		return newexpr(Enop, 0, 0, 0, 0);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = globals(p->e1, env);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = globals(e->e1, env);
		e->e2 = globals(e->e2, env);
		e->e3 = globals(e->e3, env);
		e->e4 = globals(e->e4, env);
		return e;
	}
}

static void
topresolve(Expr *e, Env *env, Expr *lex)
{
	Expr *p;
	char *id;

	if(e == 0)
		return;

	switch(e->kind){
	case Epreinc:
	case Epostinc:
	case Epredec:
	case Epostdec:
	case Eg:
	case Egop:
		if(e->e1->kind != Eid)
			fatal("bug");
		id = e->e1->id;
		if(lexbinds(lex, id) || envbinds(env, id))
			; /* no binding required */
		else if(lex){
			/* create binding in inner-most lexical scope */
			if(cqctflags['w'])
				cwarn(e, "assignment to unbound variable: %s",
				      id);
			newlocal(lex, id);
		}else
			/* create top-level binding */
			envgetbind(env, id);
		if(e->kind == Eg || e->kind == Egop)
			topresolve(e->e2, env, lex);
		break;
	case Elambda:
	case Eblock:
		e->xp = lex;
		topresolve(e->e2, env, e);
		e->xp = 0;
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			topresolve(p->e1, env, lex);
			p = p->e2;
		}
		break;
	default:
		topresolve(e->e1, env, lex);
		topresolve(e->e2, env, lex);
		topresolve(e->e3, env, lex);
		topresolve(e->e4, env, lex);
		break;
	}
}

void
freeconst(void *u, char *id, void *v)
{
	efree(id);
	freeexpr((Expr*)v);
}

static void
bindids(Xenv *xe, Expr *e, void *v)
{
	Expr *p;
	switch(e->kind){
	case Eid:
		xenvbind(xe, e->id, v);
		break;
	case Enull:
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			xenvbind(xe, p->e1->id, v);
			p = p->e2;
		}
		break;
	default:
		fatal("bug");
	}
}

static Expr*
expandconst(Expr *e, Env *top, Xenv *lex, Xenv *con)
{
	Expr *p;
	Xenv *lexrib, *conrib;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Edefconst:
		e->e2 = expandconst(e->e2, top, lex, con);
		xenvbind(con, xstrdup(e->e1->id), e->e2);
		e->e2 = 0;
		freeexpr(e);
		return newexpr(Enop, 0, 0, 0, 0);
	case Eid:
		if(xenvlook(lex, e->id) || envbinds(top, e->id))
			return e;
		p = xenvlook(con, e->id);
		if(p){
			freeexpr(e);
			return copyexpr(p);
		}
		return e;
	case Elambda:
		lexrib = mkxenv(lex);
		bindids(lexrib, e->e1, e);
		e->e2 = expandconst(e->e2, top, lexrib, con);
		freexenv(lexrib);
		return e;
	case Eblock:
		conrib = mkxenv(con);
		lexrib = mkxenv(lex);
		bindids(lexrib, e->e1, e);
		e->e2 = expandconst(e->e2, top, lexrib, conrib);
		freexenv(lexrib);
		xenvforeach(conrib, freeconst, 0);
		freexenv(conrib);
		return e;
	case Epreinc:
	case Epostinc:
	case Epredec:
	case Epostdec:
		/* don't expand assigned identifiers */
		return e;
	case Eg:
	case Egop:
		/* don't expand assigned identifiers */
		e->e2 = expandconst(e->e2, top, lex, con);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = expandconst(p->e1, top, lex, con);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = expandconst(e->e1, top, lex, con);
		e->e2 = expandconst(e->e2, top, lex, con);
		e->e3 = expandconst(e->e3, top, lex, con);
		e->e4 = expandconst(e->e4, top, lex, con);
		return e;
	}
}

static void
freevars(Expr *e, VEnv *ve, VDset *fr)
{
	Lambda *b;
	Varref *vr;
	VEnv *nve;
	Expr *p;

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
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			freevars(p->e1, ve, fr);
			p = p->e2;
		}
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
	Expr *p;

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
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			mapcapture(p->e1, cap);
			p = p->e2;
		}
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
		xprintf("%" PRIu64, e->liti.val);
		break;
	case Econsts:
		xprintf("(Econsts %.*s)", e->lits->len, e->lits->s);
		break;
	case Eblock:
		printframe(e->e2);
		break;
	case Eid:
		xprintf("(Eid ");
		vr = (Varref*)e->xp;
		if(vr == NULL)
			fatal("no varref for Eid");
		if(vr->vd->kind == VDtoplevel)
			xprintf("<t,%d,%s>", vr->vd->idx, vr->vd->id);
		else
			xprintf("<%c,%p,%d,%s,%d>",
			       vr->vd->kind == VDparam ? 'p' : 'l',
			       vr->vd->lmbda,
			       vr->vd->idx,
			       vr->vd->id,
			       vr->closed ? vr->cidx : -1);
		xprintf(")");
		break;
	case Elambda:
		b = (Lambda*)e->xp;
		if(b == NULL)
			fatal("no lambda for Elambda");
		xprintf("(Elambda %p", b);
		vd = b->param;
		for(m = 0; m < b->npar; m++)
			xprintf(" <p,%d,%s,%d>",
			       vd[m].idx, vd[m].id, vd[m].indirect);
		vd = b->local;
		for(m = 0; m < b->maxloc; m++)
			xprintf(" <l,%d,%s,%d>",
			       vd[m].idx, vd[m].id, vd[m].indirect);
		xprintf(" ");
		for(m = 0; m < b->capture->nvr; m++){
			vr = &b->capture->vr[m];
			xprintf(" <%c,%p,%d,%s,%d>",
			       vr->vd->kind == VDparam ? 'p' : 'l',
			       vr->vd->lmbda,
			       vr->vd->idx,
			       vr->vd->id,
			       vr->closed ? vr->cidx : -1);
		}
		xprintf(" ");
		printframe(e->e2);
		xprintf(")");
		break;
	default:
		xprintf("(%s", S[e->kind]);
		if(e->e1){
			xprintf(" ");
			printframe(e->e1);
		}
		if(e->e2){
			xprintf(" ");
			printframe(e->e2);
		}
		if(e->e3){
			xprintf(" ");
			printframe(e->e3);
		}
		if(e->e4){
			xprintf(" ");
			printframe(e->e4);
		}
		xprintf(")");
	}
}

static void
recswitchctl(Expr *e, Code *code, Cases *cs)
{
	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
	case Eswitch:
		return;
	case Ecase:
		if(cs->n >= cs->max){
			cs->cmp = erealloc(cs->cmp,
					   cs->max*sizeof(Expr),
					   2*cs->max*sizeof(Expr));
			cs->ctl = erealloc(cs->ctl,
					   cs->max*sizeof(Expr),
					   2*cs->max*sizeof(Expr));
			cs->max *= 2;
		}
		cs->cmp[cs->n] = e;
		cs->ctl[cs->n] = genlabel(code, 0);
// xprintf("case label %s\n", cs->ctl[cs->n]->label);
		cs->n++;
		recswitchctl(e->e2, code, cs);
		break;
	case Edefault:
		if(cs->dflt)
			fatal("too many default cases in switch");
		cs->dflt = genlabel(code, 0);
// xprintf("default label %s\n", cs->dflt->label);
		cs->dflte = e;
		recswitchctl(e->e1, code, cs);
		break;
	default:
		recswitchctl(e->e1, code, cs);
		recswitchctl(e->e2, code, cs);
		recswitchctl(e->e3, code, cs);
		recswitchctl(e->e4, code, cs);
		break;
	}
}

static Cases*
switchctl(Expr *e, Code *code)
{
	Cases *cs;
	cs = emalloc(sizeof(Cases));
	cs->max = 128;
	cs->cmp = emalloc(cs->max*sizeof(Expr*));
	cs->ctl = emalloc(cs->max*sizeof(Expr*));
	recswitchctl(e, code, cs);
	return cs;
}

static void
freeswitchctl(Cases *cs)
{
	efree(cs->cmp);
	efree(cs->ctl);
	efree(cs);
}

static void
randloc(Operand *rand, Location* loc)
{
	rand->okind = Oloc;
	rand->u.loc = *loc;
}

static void
randlits(Operand *rand, Lits *lits)
{
	rand->okind = Olits;
	rand->u.lits = lits;
}

static void
randliti(Operand *rand, Val v)
{
	rand->okind = Oliti;
	rand->u.liti = v;
}

static void
randnil(Operand *rand)
{
	rand->okind = Onil;
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
		randliti(rand, e->xp);
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

static Expr*
nextstmt(Expr *e)
{
	if(e == 0)
		return 0;
	switch(e->kind){
	case Eblock:
		return nextstmt(e->e2);
	case Eelist:
		return nextstmt(e->e1);
	default:
		return e;
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
		i->kind = Ijz;
		randloc(&i->op1, AC);
		i->dstlabel = l2;
		l2->used = 1;
		cgjmp(code, p, l1, nxt);
	}else{
		i = nextinsn(code);
		i->kind = Ijnz;
		randloc(&i->op1, AC);
		i->dstlabel = l1;
		l1->used = 1;
		cgjmp(code, p, l2, nxt);
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
	[Excast] = Ixcast,

	[E_cval] = Icval,
	[E_ref] = Iref,
	[E_sizeof] = Isizeof,
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
	Expr *q, *ep;
	Insn *i;
	unsigned narg, istail;
	Lambda *b;
	Location dst;
	int m;
	Varref *vr;
	CGEnv np;
	unsigned genl;

	switch(e->kind){
	case Enop:
		i = nextinsn(code);
		i->kind = Inop;
		cgctl(code, p, ctl, nxt);
		break;
	case Eelist:
		ep = e;
		while(ep->kind == Eelist){
			if(ep->e2->kind == Enull){
				cg(ep->e1, code, p, loc, ctl, prv, nxt, tmp);
				break;
			}
			L = 0;
			genl = 0;
			if(p->cases){
				q = nextstmt(ep->e2);
				if(p->cases->dflte == q)
					L = p->cases->dflt;
				else
					for(m = 0; m < p->cases->n; m++)
						if(p->cases->cmp[m] == q){
							L = p->cases->ctl[m];
							break;
						}
			}
			if(L == 0){
				L = genlabel(code, 0);
				genl = 1;
//				xprintf("elist label %s %s %s\n",
//				       L->label,
//				       S[ep->e1->kind],
//				       S[ep->e2->e1->kind]);
			}
			cg(ep->e1, code, p, Effect, L, prv, L, tmp);
			if(genl)
				emitlabel(L, ep->e2);
			prv = L;
			ep = ep->e2;
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
			fatal("bug");
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
			fatal("bug");
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
			fatal("bug");
		varloc(&dst, e->e1);
		randloc(&r1, &dst);
		randliti(&r2, konsti2val(Vint, 1, code->konsti));
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
			fatal("bug");
		varloc(&dst, e->e1);
		randloc(&r1, &dst);
		randliti(&r2, konsti2val(Vint, 1, code->konsti));
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
	case E_sizeof:
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
	case E_cval:
	case E_ref:
		/* rather than compute temp requirements for arbitrary
		   3-operand applications, assume that all 3 operands
		   are simple. */
		if(!issimple(e->e1) || !issimple(e->e2) || !issimple(e->e3))
			fatal("%s with non-simple operands", EtoVM[e->kind]);
		i = nextinsn(code);
		i->kind = EtoVM[e->kind];
		cgrand(&i->op1, e->e1, p);
		cgrand(&i->op2, e->e2, p);
		cgrand(&i->op3, e->e3, p);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt);
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
			// FIXME: can we check for simple? here and push
			// the operand directly? (and likewise for the call?)
			if(issimple(q->e1)){
				L = L0;
				cgrand(&r1, q->e1, p);
				i = nextinsn(code);
				i->kind = Ipush;
				i->op1 = r1;
			}else{
				L = genlabel(code, 0);
// we may be able to track frame state here.
				cg(q->e1, code, p, AC, L, L0, L, tmp);
				emitlabel(L, q->e2);
				i = nextinsn(code);
				i->kind = Ipush;
				randloc(&i->op1, AC);
			}
			q = q->e2;
			narg++;
			L0 = L;
		}
		
		i = nextinsn(code);
		i->kind = Ipushi;
		randliti(&i->op1, konsti2val(Vint, narg, code->konsti));

		if(issimple(e->e1)){
			cgrand(&r1, e->e1, p);
		}else {
			L0 = genlabel(code, 0);
			emitlabel(L0, e->e1);
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, L0, L, tmp);
			emitlabel(L, e);
		}

		if(!istail){
			i = nextinsn(code);
			i->kind = Icall;
			if(issimple(e->e1))
				i->op1 = r1;
			else
				randloc(&i->op1, AC);
			if(loc != Effect){
				emitlabel(R, e);
				if(loc == AC){
					/* FIXME: nop because there
					   seems to be no simple way to manage
					   the labels otherwise. */
					i = nextinsn(code);
					i->kind = Inop;
				}else{
					i = nextinsn(code);
					i->kind = Imov;
					randloc(&i->op1, AC);
					randloc(&i->dst, loc);
				}
			}
			cgctl(code, p, ctl, nxt);
		}else{
			i = nextinsn(code);
			i->kind = Icallt;
			if(issimple(e->e1))
				i->op1 = r1;
			else
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
		randliti(&i->op1, e->xp);
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
		L = genlabel(code, b->id);

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
		randliti(&i->op1,
			 konsti2val(Vint, b->capture->nvr, code->konsti));
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
			randliti(&i->op1, konsti2val(Vint, 1, code->konsti));
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, Lelse);
			emitlabel(Lelse, e);
			i = nextinsn(code);
			i->kind = Imov;
			randliti(&i->op1, konsti2val(Vint, 0, code->konsti));
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
			randliti(&i->op1, konsti2val(Vint, 1, code->konsti));
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, Lelse);
			emitlabel(Lelse, e);
			i = nextinsn(code);
			i->kind = Imov;
			randliti(&i->op1, konsti2val(Vint, 0, code->konsti));
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
				cg(e->e3, code, p, loc, ctl,
				   Lelse, nxt, tmp);
			}else if(!escaping(e->e2) && escaping(e->e3)){
				Lthen = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen,
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, prv, Lthen, tmp);
				emitlabel(Lthen, e->e2);
				cg(e->e2, code, p, loc, ctl,
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
				cg(e->e2, code, p, loc, ctl,
				   Lthen, Lelse, tmp);
				emitlabel(Lelse, e->e3);
				cg(e->e3, code, p, loc, ctl,
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
				cg(e->e2, code, p, loc, ctl,
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
	case Eswitch:
		np = *p;
		np.Break = ctl;
		np.cases = switchctl(e->e2, code);

		/* operand expression (expect form $tmp = e) */
		L0 = genlabel(code, 0);
		cg(e->e1, code, &np, Effect, L0, prv, L0, tmp);
		emitlabel(L0, e->e2);
		
		/* case comparisons (expect form $tmp == v) */
		for(m = 0; m < np.cases->n; m++){
			Lthen = np.cases->ctl[m];
			Lelse = genlabel(code, 0);
			lpair = genlabelpair(code, Lthen, Lelse);
			cg(np.cases->cmp[m]->e1, code, &np, AC,
			   lpair, L0, Lelse, tmp);
			emitlabel(Lelse, e->e2);
			L0 = Lelse;
		}
		if(np.cases->dflt)
			cgjmp(code, &np, np.cases->dflt, np.cases->ctl[0]);
		else
			cgjmp(code, &np, ctl, np.cases->ctl[0]);
		cg(e->e2, code, &np, Effect, ctl, np.cases->ctl[0], nxt, tmp);

		/* hack for unique labels in code like:
			   switch(e){ ... default: printf; break; } e = 5;
		*/
		i = nextinsn(code);
		i->kind = Inop;

		freeswitchctl(np.cases);
		np.cases = 0;
		break;
	case Ecase:
		if(p->cases == 0)
			fatal("case label with no switch");
		for(m = 0; m < p->cases->n; m++)
			if(p->cases->cmp[m] == e){
				emitlabel(p->cases->ctl[m], e);
				i = nextinsn(code);
				i->kind = Inop;
				cg(e->e2, code, p, Effect, ctl,
				   p->cases->ctl[m],
				   nxt, tmp);
				i = nextinsn(code);
				i->kind = Inop;
				break;
			}
		cgctl(code, p, ctl, nxt);
		break;
	case Edefault:
		if(p->cases == 0)
			fatal("default label with no switch");
		if(p->cases->dflt){
			emitlabel(p->cases->dflt, e);
			i = nextinsn(code);
			i->kind = Inop;
			cg(e->e1, code, p, Effect, ctl,
			   p->cases->dflt,
			   nxt, tmp);
		}
		cgctl(code, p, ctl, nxt);
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

	memset(&p, 0, sizeof(p));
	b = (Lambda*)e->xp;
	if(cqctflags['b']){
		if(b->vararg)
			xprintf("%s: var params, %u locals, %u temps\n",
			       name->label, b->maxloc, b->ntmp);
		else
			xprintf("%s: %u params, %u locals, %u temps\n",
			       name->label, b->npar, b->maxloc, b->ntmp);
	}
	p.b = b;

	entry = code->ninsn;
	if(!b->vararg){
		i = nextinsn(code);
		i->kind = Iargc;
		randliti(&i->op1, konsti2val(Vuint, b->npar, code->konsti));
		needtop = 1;
	}
	if(b->maxloc+b->ntmp > 0){
		i = nextinsn(code);
		i->kind = Isub;
		randloc(&i->op1, SP);
		randliti(&i->op2,
			 konsti2val(Vint, b->maxloc+b->ntmp, code->konsti));
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
	for(m = 0; m < b->maxloc; m++)
		if(b->local[m].indirect){
			i = nextinsn(code);
			i->kind = Ibox0;
			randvdloc(&i->op1, &b->local[m]);
			needtop = 1;
		}
	if(b->vararg){
		i = nextinsn(code);
		i->kind = Ilist;
		randloc(&i->op1, FP);
		/* by convention local 0 is var arg list */
		randstkloc(&i->dst, Llocal, 0, b->local[0].indirect); 
	}

	/* hack to return nil in degenerate cases */
	i = nextinsn(code);
	i->kind = Imov;
	randnil(&i->op1);
	randloc(&i->dst, AC);

	if(needtop){
		top = genlabel(code, 0);
		emitlabel(top, e->e2);
	}else
		top = name;

	p.Return0 = genlabel(code, 0);
	p.Return = genlabel(code, 0);
	p.Break = 0;
	p.Continue = 0;
	cg(e->e2, code, &p, AC, p.Return0, top, p.Return0, b->maxloc);

	if(p.Return0->used) /* hack for lambdas with empty bodies */
		emitlabel(p.Return0, e->e2);
	i = nextinsn(code);
	i->kind = Imov;


// what does a function without an explicit return <expr> return?
// either nil or the value of last expression evaluated.
	randnil(&i->op1);
//	randloc(&i->op1, AC);

	randloc(&i->dst, AC);
	emitlabel(p.Return, e->e2);
	i = nextinsn(code);
	i->kind = Iret;
}

Closure*
compileentry(Expr *el, Toplevel *top)
{
	Ctl *L;
	VDset *cap;
	Code *code;
	Closure *cl;
	Lambda *b;

	/* enclose expression in block to reduce top-level pollution */
//	el = Zblock(nullelist(), el, 0);

	/* add @global and implicit @global bindings to env */
	el = globals(el, top->env);

	/* resolve top-level bindings */
	topresolve(el, top->env, 0);

	/* expand @const references */
	el = expandconst(el, top->env, 0, top->env->con);

	/* enclose expression in lambda to make it callable */
	el = newexpr(Elambda, doid("args"),
		     newexpr(Eret,
			     newexpr(Eblock, nullelist(), el, 0, 0),
			     0, 0, 0),
		     0, 0);

	/* map variable references and bindings to storage */
	code = mkcode();
	L = genlabel(code, "entry");
	L->used = 1;
	emitlabel(L, el);
	mapframe(el, 0, 0, code->topvec, top->env,
		 code->konst, code->konsti, 0);
	cap = mkvdset();
	mapcapture(el, cap);
	freevdset(cap);
	code->src = el;

//	printframe(el);

	/* compile and prepare closure */
	compilelambda(L, code, el);
	b = (Lambda*)el->xp;
	cl = mkcl(code, 0, b->capture->nvr, L->label);

	if(cqctflags['o'])
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
	L = genlabel(code, "$halt");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ihalt;
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
callccode()
{
	Insn *i;
	Code *code;

	code = mkcode();
	i = nextinsn(code);
	i->kind = Icallc;
	i = nextinsn(code);
	i->kind = Iret;

	return code;
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
panicthunk()
{
	Ctl *L;
	Insn *i;
	Code *code;
	Closure *cl;

	code = mkcode();
	L = genlabel(code, "panic");
	cl = mkcl(code, code->ninsn, 0, L->label);
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(code);
	i->kind = Ipanic;

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
