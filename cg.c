#include "sys.h"
#include "util.h"
#include "syscqct.h"

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
	Ctl *Return;
	Ctl *Return0;
	Ctl *Break;
	Ctl *Continue;
	Cases *cases;
} CGEnv;

static Location toploc[8];
static Location *Effect;
static Location *AC, *FP, *SP, *PC, *ARG0, *ARG1, *ARG2;
static void cglambda(Ctl *name, Code *code, Expr *el);
static Konst* mkkonst();
static void freekonst(Konst *kon);
static Konsti* mkkonsti();
static void freekonsti(Konsti *koni);

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

static void
sz1konst(void *u, char *k, void *v)
{
	u64 *p;
	Lits *l;
	p = u;
	l = v;
	*p += esize(l);
}

static u64
szkonst(Konst *kon)
{
	u64 m;
	m = 0;
	hforeach(kon->ht, sz1konst, &m);
	m += hsz(kon->ht);
	m += sizeof(*kon);
	return m;
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
	char buf[18+Maxliti];	/* "unsigned long long"+Maxliti */
	char *s;

	snprint(buf, sizeof(buf), "%s%" PRIu64, cbasename[base], imm);
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

static void
sz1konsti(void *u, char *k, void *v)
{
	u64 *p;
	p = u;
	*p += esize(k);
}

static u64
szkonsti(Konsti *kon)
{
	u64 m;
	m = 0;
	hforeach(kon->ht, sz1konsti, &m);
	m += hsz(kon->ht);
	m += sizeof(*kon);
	return m;
}

// allocate and bind constants to constant pool
// FIXME: do this for ctypes?
static void
konsts(Expr *e, Code *code)
{
	Expr *p;

	if(e == 0)
		return;

	switch(e->kind){
	case Econst:
		e->xp = konstival(&e->liti, code->konsti);
		break;
	case Econsts:
		e->xp = konstlookup(e->lits, code->konst);
		if(e->xp == 0)
			e->xp = konstadd(e->lits, code->konst);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			konsts(p->e1, code);
			p = p->e2;
		}
		break;
	default:
		konsts(e->e1, code);
		konsts(e->e2, code);
		konsts(e->e3, code);
		konsts(e->e4, code);
		break;
	}
}

static void
newloc(Location *loc, unsigned kind, unsigned idx, unsigned indirect)
{
	loc->loc = LOC(idx,indirect,kind);
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
		snprint(buf, sizeof(buf), "L%d", labelseed++);
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

u64
szcode(Code *code)
{
	u64 m;
	Ctl *p;

	m = 0;

	m += szkonst(code->konst);
	m += szkonsti(code->konsti);
	p = code->clist;
	while(p){
		if(p->ckind == Clabel)
			m += esize(p->label);
		m += sizeof(*p);
		p = p->link;
	}
	m += szexpr(code->src);
	m += esize(code->insn);
	m += esize(code->labels);
	return m;
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
		switch(LOCKIND(loc->loc)){
		case Lreg:
			xprintf("%s", regtos(LOCIDX(loc->loc)));
			break;
		case Lparam:
			if(LOCBOX(loc->loc))
				xprintf("[");
			xprintf("%d(%s)", LOCIDX(loc->loc)+1, regtos(Rfp));
			if(LOCBOX(loc->loc))
				xprintf("]");
			break;
		case Llocal:
			if(LOCBOX(loc->loc))
				xprintf("[");
			xprintf("-%d(%s)", LOCIDX(loc->loc)+1, regtos(Rfp));
			if(LOCBOX(loc->loc))
				xprintf("]");
			break;
		case Ldisp:
			if(LOCBOX(loc->loc))
				xprintf("[");
			xprintf("%d(%s)", LOCIDX(loc->loc), regtos(Rcl));
			if(LOCBOX(loc->loc))
				xprintf("]");
			break;
		case Ltopl:
			xprintf("<%s>", loc->var->id);
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

void
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
		xprintf("\t%4d\t", i);
		if(code->labels[i] && code->labels[i]->used)
			xprintf("%s", code->labels[i]->label);
		printinsn(code, &code->insn[i]);
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

/* if DEREF, reference box contents; otherwise the box itself */
static void
varloc(Location *loc, Var *v, int deref)
{
	deref = deref && v->box;
	switch(v->where){
	case Vparam:
		newloc(loc, Lparam, v->idx, deref);
		break;
	case Vlocal:
		newloc(loc, Llocal, v->idx, deref);
		break;
	case Vdisp:
		newloc(loc, Ldisp, v->idx, deref);
		break;
	case Vtop:
		/* toplevel vars are not referencable boxes
		   and use var instead of index */
		newloc(loc, Ltopl, 0, 0);
		loc->var = v;
		break;
	default:
		fatal("bug");
	}
}

/* if DEREF, reference box contents; otherwise the box itself */
static void
randvarloc(Operand *rand, Var *v, int deref)
{
	rand->okind = Oloc;
	varloc(&rand->u.loc, v, deref);
}

static void
randstkloc(Operand *rand, unsigned kind, unsigned idx)
{
	rand->okind = Oloc;
	switch(kind){
	case Lparam:
	case Llocal:
		newloc(&rand->u.loc, kind, idx, 0);
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
		randvarloc(rand, e->xp, 1);
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
	Lambda *l;
	Block *b;
	Location dst;
	int m;
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
		b = (Block*)e->xp;
		for(m = 0; m < b->nloc; m++)
			if(b->loc[m].box){
				i = nextinsn(code);
				i->kind = Ibox0;
				randvarloc(&i->op1, &b->loc[m], 0);
			}
		cg(e->e2, code, p, loc, ctl, prv, nxt, tmp);
		break;
	case Eg:
		if(e->e1->kind != Eid)
			fatal("bug");
		varloc(&dst, e->e1->xp, 1);
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
		varloc(&dst, e->e1->xp, 1);
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
		varloc(&dst, e->e1->xp, 1);
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
		varloc(&dst, e->e1->xp, 1);
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
			randstkloc(&r1, Llocal, tmp);
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
		randvarloc(&i->op1, e->xp, 1);
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
		l = (Lambda*)e->xp;
		L = genlabel(code, l->id);

		for(m = l->ncap-1; m >= 0; m--){
			i = nextinsn(code);
			i->kind = Imov;
			randvarloc(&i->op1, l->cap[m], 0);
			randloc(&i->dst, AC);
			i = nextinsn(code);
			i->kind = Ipush;
			randloc(&i->op1, AC);
		}

		i = nextinsn(code);
		i->kind = Iclo;
		randliti(&i->op1, konsti2val(Vint, l->ncap, code->konsti));
		randloc(&i->dst, loc);
		i->dstlabel = L;
		L->used = 1;

		i = nextinsn(code);
		i->kind = Ijmp;
		i->dstlabel = ctl;
		ctl->used = 1;

		emitlabel(L, e);
		cglambda(L, code, e);

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
		
		/* bit of a hack: guarantee that locals in the switch
		   block are initialized, replicating the Eblock prologue */
		if(e->e2->kind == Eblock){
			b = (Block*)e->e2->xp;
			for(m = 0; m < b->nloc; m++)
				if(b->loc[m].box){
					i = nextinsn(code);
					i->kind = Ibox0;
					randvarloc(&i->op1, &b->loc[m], 0);
				}
		}

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
cglambda(Ctl *name, Code *code, Expr *e)
{
	unsigned entry;
	Lambda *l;
	Insn *i;
	CGEnv p;
	unsigned m, needtop = 0;
	Ctl *top;

	if(e->kind != Elambda)
		fatal("cglambda on non-lambda");

	memset(&p, 0, sizeof(p));
	l = (Lambda*)e->xp;
	if(cqctflags['b']){
		if(l->isvarg)
			xprintf("%s: var params, %u locals, %u temps\n",
			       name->label, l->nloc, l->ntmp);
		else
			xprintf("%s: %u params, %u locals, %u temps\n",
			       name->label, l->nparam, l->nloc, l->ntmp);
	}

	entry = code->ninsn;
	if(!l->isvarg){
		i = nextinsn(code);
		i->kind = Iargc;
		randliti(&i->op1, konsti2val(Vuint, l->nparam, code->konsti));
		needtop = 1;
	}
	if(l->nloc+l->ntmp > 0){
		i = nextinsn(code);
		i->kind = Isub;
		randloc(&i->op1, SP);
		randliti(&i->op2,
			 konsti2val(Vint, l->nloc+l->ntmp, code->konsti));
		randloc(&i->dst, SP);
		needtop = 1;
	}
	for(m = 0; m < l->nparam; m++)
		if(l->param[m].box){
			i = nextinsn(code);
			i->kind = Ibox;
			randvarloc(&i->op1, &l->param[m], 0);
			needtop = 1;
		}
	if(l->isvarg){
		i = nextinsn(code);
		i->kind = Ilist;
		randloc(&i->op1, FP);
		/* by convention param 0 is first local stack variable */
		randvarloc(&i->dst, &l->param[0], 1);
	}

	/* was: hack to return nil in degenerate cases; insert nop instead */
	i = nextinsn(code);
	i->kind = Inop;

	if(needtop){
		top = genlabel(code, 0);
		emitlabel(top, e->e2);
	}else
		top = name;

	p.Return0 = genlabel(code, 0);
	p.Return = genlabel(code, 0);
	p.Break = 0;
	p.Continue = 0;
	cg(e->e2, code, &p, AC, p.Return0, top, p.Return0, l->nloc);

	if(p.Return0->used) /* hack for lambdas with empty bodies */
		emitlabel(p.Return0, e->e2);

	i = nextinsn(code);
	i->kind = Inop;

	emitlabel(p.Return, e->e2);
	i = nextinsn(code);
	i->kind = Iret;
}

Closure*
codegen(Expr *e)
{
	Ctl *L;
	Lambda *l;
	Closure *cl;
	Code *code;

	code = mkcode();
	L = genlabel(code, "entry");
	L->used = 1;
	emitlabel(L, e);
	konsts(e, code);
	code->src = e;
	cglambda(L, code, e);
	l = (Lambda*)e->xp;
	cl = mkcl(code, 0, l->ncap, L->label);
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
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code);
	i->kind = Ikg;
	randloc(&i->dst, ARG0);
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
	randloc(&i->op1, ARG0);
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
initcg()
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
finicg()
{
}
