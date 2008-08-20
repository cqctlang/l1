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

typedef
struct Frstat {
	unsigned nloc;
	HT *locals;
} Frstat;

struct Konst {
	HT *ht;
};

static Location toploc[8];
static Location *Effect;
static Location *AC, *FP, *SP, *PC, *ARG0, *ARG1, *ARG2;
static void compilelambda(Ctl *name, Code *code, Expr *el);
static Topvec *mktopvec();
static void freetopvec(Topvec *tv);
static Konst* mkkonst();
static void freekonst(Konst *kon);
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
	if(e && e->src.filename)
		ctl->src = &e->src;
}

static Code*
mkcode()
{
	Code *code;
	
	code = newcode();
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
	
	freeexpr(code->src);
	free(code->insn);
	free(code->labels);
}

static Insn*
nextinsn(Code *code)
{
	/* plan for one extra instruction for emitlabel */
	if(code->ninsn+1 >= code->maxinsn){
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
	case Oliti:
		printf("%" PRIu64, r->u.liti.val);
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
	case Iargc:
		printf("argc ");
		printrand(code, &i->op1);
		break;
	case Icallc:
		printf("callc");
		break;
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
	case Icval:
		printf("cval ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ixcast:
		printf("xcast ");
		printrand(code, &i->op1);
		printf(" ");
		printrand(code, &i->op2);
		printf(" ");
		printrand(code, &i->dst);
		break;
	case Ilist:
		printf("list ");
		printrand(code, &i->op1);
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
	case Iframe:
		printf("frame %s", i->dstlabel->label);
		break;
	case Iret:
		printf("ret");
		break;
	case Ihalt:
		printf("halt");
		break;
	case Ipanic:
		printf("panic");
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
	case Inop:
		printf("nop");
		break;
	case Iref:
		printf("ref ");
		printrand(code, &i->op1);
		break;
	default:
		fatal("printinsn: unrecognized insn %d", i->kind);
	}
	printf("\n");
}

static void
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
	return (e->kind == Eid
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
	freeexpr(b->e1);
	b->e1 = invert(nl);

	return nloc;
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
		m = flattenlocal(e);
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
	unsigned ntmp, npar, maxloc, vararg;
	VEnv *ve;
	VRset *capture;
	char *id;
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
	b->ntmp = tmppass(p->e2);
	b->maxloc = locpass(p->e2);
	b->local = xmalloc(b->maxloc*sizeof(Vardef));

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
	free(b->id);
	free(b);
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
		vd->lambda = b;
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

static void
freetype(Type *t)
{
	if(t == 0)
		return;

	freetype(t->link);
	free(t->dom);
	free(t->tid);
	free(t->tag);
	freedecl(t->field);
	freeenum(t->en);
	freedecl(t->param);
	freeexpr(t->bitw);
	freeexpr(t->bit0);
	freeexpr(t->sz);
	freeexpr(t->cnt);
	free(t);
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
	 Konst *kon, unsigned ploc)
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
		mapframe(e->e2, curb, ve, tv, env, kon, ploc);
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
		mapframe(e->e2, curb, ve, tv, env, kon, nloc);
		break;
	case Elambda:
		b = mklambda(e);
		e->xp = b;
		b->ve->link = ve;
		ve = b->ve;
		if(e->e3) /* Edefine */
			b->id = xstrdup(e->e3->id);
		mapframe(e->e2, b, b->ve, tv, env, kon, 0);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			mapframe(p->e1, curb, ve, tv, env, kon, ploc);
			p = p->e2;
		}
		break;
	default:
		mapframe(e->e1, curb, ve, tv, env, kon, ploc);
		mapframe(e->e2, curb, ve, tv, env, kon, ploc);
		mapframe(e->e3, curb, ve, tv, env, kon, ploc);
		mapframe(e->e4, curb, ve, tv, env, kon, ploc);
		break;
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
		printf("%" PRIu64, e->liti.val);
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
		for(m = 0; m < b->maxloc; m++)
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
			cs->cmp = xrealloc(cs->cmp,
					   cs->max*sizeof(Expr),
					   2*cs->max*sizeof(Expr));
			cs->ctl = xrealloc(cs->ctl,
					   cs->max*sizeof(Expr),
					   2*cs->max*sizeof(Expr));
			cs->max *= 2;
		}
		cs->cmp[cs->n] = e;
		cs->ctl[cs->n] = genlabel(code, 0);
// printf("case label %s\n", cs->ctl[cs->n]->label);
		cs->n++;
		recswitchctl(e->e2, code, cs);
		break;
	case Edefault:
		if(cs->dflt)
			fatal("too many default cases in switch");
		cs->dflt = genlabel(code, 0);
// printf("default label %s\n", cs->dflt->label);
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
	cs = xmalloc(sizeof(Cases));
	cs->max = 128;
	cs->cmp = xmalloc(cs->max*sizeof(Expr*));
	cs->ctl = xmalloc(cs->max*sizeof(Expr*));
	recswitchctl(e, code, cs);
	return cs;
}

static void
freeswitchctl(Cases *cs)
{
	free(cs->cmp);
	free(cs->ctl);
	free(cs);
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
randliti(Operand *rand, Imm val, unsigned base)
{
	rand->okind = Oliti;
	rand->u.liti.val = val;
	rand->u.liti.base = base;
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
		randliti(rand, e->liti.val, e->liti.base);
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
	if(e == 0){
		warn("nextstmt is null"); 
		return 0;
	}
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
	[Excast] = Ixcast,

	[E_cval] = Icval,
	[E_encode] = Iencode,
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
//				printf("elist label %s %s %s\n",
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
		randliti(&r2, 1, Vint);
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
		randliti(&r2, 1, Vint);
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
	case E_encode:
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
		randliti(&i->op1, narg, Vint);

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
		randliti(&i->op1, e->liti.val, e->liti.base);
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
		randliti(&i->op1, b->capture->nvr, Vint);
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
			randliti(&i->op1, 1, Vint);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, Lelse);
			emitlabel(Lelse, e);
			i = nextinsn(code);
			i->kind = Imov;
			randliti(&i->op1, 0, Vint);
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
			randliti(&i->op1, 1, Vint);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, Lelse);
			emitlabel(Lelse, e);
			i = nextinsn(code);
			i->kind = Imov;
			randliti(&i->op1, 0, Vint);
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
			printf("%s: var params, %u locals, %u temps\n",
			       name->label, b->maxloc, b->ntmp);
		else
			printf("%s: %u params, %u locals, %u temps\n",
			       name->label, b->npar, b->maxloc, b->ntmp);
		fflush(stdout);
	}
	p.b = b;

	entry = code->ninsn;
	if(!b->vararg){
		i = nextinsn(code);
		i->kind = Iargc;
		randliti(&i->op1, b->npar, Vuint);
		needtop = 1;
	}
	if(b->maxloc+b->ntmp > 0){
		i = nextinsn(code);
		i->kind = Isub;
		randloc(&i->op1, SP);
		randliti(&i->op2, b->maxloc+b->ntmp, Vint);
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
	cg(e->e2, code, &p, Effect, p.Return0, top, p.Return0, b->maxloc);

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
compileentry(Expr *el, Env *env)
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

//	le = newexpr(Elambda, nullelist(), el, 0, 0);
	le = newexpr(Elambda, nullelist(), 
                    newexpr(Eret,
                            newexpr(Eblock, nullelist(), el, 0, 0),
                            0, 0, 0),
                    0, 0);

	mapframe(le, 0, 0, code->topvec, env, code->konst, 0);
	cap = mkvdset();
	mapcapture(le, cap);
	freevdset(cap);
	code->src = le;

	compilelambda(L, code, le);
	b = (Lambda*)le->xp;
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
