#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct CGEnv {
	Ctl *Return;
	HT *labels;
} CGEnv;

static Location toploc[8];
static Location *Effect;
static Location *AC, *FP, *SP, *PC, *ARG0, *ARG1, *ARG2;
static void cglambda(Ctl *name, Code *code, Expr *el);

static Val
konval(Tab *kon, Val v)
{
	Val hv;
	hv = tabget(kon, v);
	if(hv)
		return hv;
	tabput(kon, v, v);
	return v;
}

static Val
konimm(Tab *kon, Cbase base, Imm imm)
{
	return konval(kon, mkvallitcval(base, imm));
}

static Val
koncstr(Tab *kon, char *s, Imm len)
{
	return konval(kon, mkvalstr(mkstr(s, len)));
}

// allocate and bind constants to constant pool
static Expr*
konsts(Expr *e, Code *code)
{
	Expr *p;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Econst:
		p = Zkon(konimm(code->konst, e->liti.base, e->liti.val));
		putsrc(p, &e->src);
		freeexpr(e);
		return p;
	case Econsts:
		p = Zkon(koncstr(code->konst, e->lits->s, e->lits->len));
		putsrc(p, &e->src);
		freeexpr(e);
		return p;
	case Ekon:
		e->xp = konval(code->konst, e->xp);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = konsts(p->e1, code);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = konsts(e->e1, code);
		e->e2 = konsts(e->e2, code);
		e->e3 = konsts(e->e3, code);
		e->e4 = konsts(e->e4, code);
		return e;
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
		return;
//		fatal("multiple labels %s,%s", ctl->label,
//		      code->labels[code->ninsn]->label);
	code->labels[code->ninsn] = ctl;
	if(e && e->src.line)
		ctl->src = &e->src;
}

static Code*
mkcode(void)
{
	Code *code;

	code = newcode();
	code->maxinsn = InsnAlloc;
	code->insn = emalloc(code->maxinsn*sizeof(Insn));
	code->labels = emalloc(code->maxinsn*sizeof(Ctl*));
	code->ninsn = 0;
	code->konst = mktab();

	return code;
}

int
freecode(Head *hd)
{
	Code *code;
	Ctl *p, *q;

	code = (Code*)hd;
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

	p = code->clist;
	while(p){
		if(p->ckind == Clabel)
			m += esize(p->label);
		m += esize(p);
		p = p->link;
	}
	m += esize(code->labels);
	m += esize(code->insn);
	m += szexpr(code->src);
	return m;
}

static Insn*
nextinsn(Code *code, Src *src)
{
	Insn *in;
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
	in = &code->insn[code->ninsn++];
	in->src = src;
	return in;
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

/* lightweight val printer compared to fmtval, but does not require a VM */
void
printkon(Val v)
{
	Cval *cv;
	Str *str;
	Xtypename *xtn;
	char c, *p;
	Imm i, m;

	switch(Vkind(v)){
	case Qcval:
		cv = valcval(v);
		xprintf("%" PRIu64, cv->val);
		break;
	case Qstr:
		str = valstr(v);
		p = str->s;
		m = str->len;
		if(m > 15)
			m = 15;
		xprintf("\"");
		for(i = 0; i < m; i++){
			c = *p++;
			switch(c){
			case '\n':
				xprintf("\\n");
				break;
			case '\t':
				xprintf("\\t");
				break;
			default:
				xprintf("%c", c);
				break;
			}
		}
		if(str->len > m)
			xprintf("...");
		xprintf("\"");
		break;
	case Qxtn:
		xtn = valxtn(v);
		p = fmtxtnc(xtn);
		xprintf("#%p %s#", xtn, p);
		efree(p);
		break;
	default:
		xprintf("<%s %p>", qname[Vkind(v)], v);
		break;
	}
}

static void
printrand(Operand *r)
{
	Location *loc;

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
	case Okon:
		printkon(r->u.kon);
		break;
	case Onil:
		xprintf("nil");
		break;
	default:
		fatal("unknown operand kind %d", r->okind);
	}
}

void
printinsn(Insn *i)
{
	xprintf("\t");
	switch(i->kind){
	case Iargc:
		xprintf("argc ");
		printrand(&i->op1);
		break;
	case Ivargc:
		xprintf("vargc ");
		printrand(&i->op1);
		break;
	case Icallc:
		xprintf("callc");
		break;
	case Iinv:
	case Ineg:
	case Inot:
		xprintf("%s ", itos(i->kind));
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->dst);
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
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->op2);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Isubsp:
		xprintf("subsp ", itos(i->kind));
		printrand(&i->op1);
		break;
	case Imov:
		xprintf("mov ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Ipush:
		xprintf("push ");
		printrand(&i->op1);
		break;
	case Ipushi:
		xprintf("pushi ");
		printrand(&i->op1);
		break;
	case Icall:
		xprintf("call ");
		printrand(&i->op1);
		break;
	case Icallt:
		xprintf("callt ");
		printrand(&i->op1);
		break;
	case Icval:
		xprintf("cval ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->op2);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Ixcast:
		xprintf("xcast ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->op2);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Ilist:
		xprintf("list ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->op2);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Isizeof:
		xprintf("sizeof ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->dst);
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
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Ikg:
		xprintf("kg ");
		printrand(&i->dst);
		break;
	case Ikp:
		xprintf("kp");
		break;
	case Ijmp:
		xprintf("jmp %s", i->dstlabel->label);
		break;
	case Ijnz:
		xprintf("jnz ");
		printrand(&i->op1);
		xprintf(" %s", i->dstlabel->label);
		break;
	case Ijz:
		xprintf("jz ");
		printrand(&i->op1);
		xprintf(" %s", i->dstlabel->label);
		break;
	case Ibox:
		xprintf("box ");
		printrand(&i->op1);
		break;
	case Ibox0:
		xprintf("box0 ");
		printrand(&i->op1);
		break;
	case Inop:
		xprintf("nop");
		break;
	case Iref:
		xprintf("ref ");
		printrand(&i->op1);
		break;
	default:
		fatal("printinsn: unrecognized insn %d", i->kind);
	}
}

static void
printcode(Code *code)
{
	Src *s;
	char *fn;
	unsigned i;
	for(i = 0; i < code->ninsn; i++){
		s = addr2line(code, i);
		if(s != 0){
			fn = s->filename;
			if(fn == 0)
				fn = "<stdin>";
			xprintf("\t%s:%u", fn, s->line);
		}else
			xprintf("\t\t");
		xprintf("\t%4d\t", i);
		if(code->labels[i] && code->labels[i]->used)
			xprintf("%s", code->labels[i]->label);
		printinsn(&code->insn[i]);
		xprintf("\n");
	}
}

static void
reclabels(Expr *e, Code *code, HT *ls)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
		break;
	case Elabel:
		id = e->id;
		hput(ls, id, strlen(id), genlabel(code, id));
		reclabels(e->e1, code, ls);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			reclabels(p->e1, code, ls);
			p = p->e2;
		}
		break;
	default:
		reclabels(e->e1, code, ls);
		reclabels(e->e2, code, ls);
		reclabels(e->e3, code, ls);
		reclabels(e->e4, code, ls);
		break;
	}
}

static HT*
labels(Expr *e, Code *code)
{
	HT *ls;
	ls = mkht();
	reclabels(e, code, ls);
	return ls;
}

static void
freelabels(HT *ls)
{
	freeht(ls);
}

static void
randloc(Operand *rand, Location* loc)
{
	rand->okind = Oloc;
	rand->u.loc = *loc;
}

static void
randkon(Operand *rand, Val v)
{
	rand->okind = Okon;
	rand->u.kon = v;
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
cgrand(Operand *rand, Expr *e)
{
	switch(e->kind){
	case Eid:
		randvarloc(rand, e->xp, 1);
		break;
	case Econst:
	case Econsts:
		fatal("residual Econst");
	case Ekon:
		randkon(rand, e->xp);
		break;
	case Enil:
		randnil(rand);
		break;
	default:
		fatal("bug: cgrand on node %d", e->kind);
	}
}

static void
cgjmp(Code *code, CGEnv *p, Ctl *ctl, Ctl *nxt, Src *src)
{
	Insn *i;

	if(ctl == p->Return){
		i = nextinsn(code, src);
		i->kind = Iret;
	}else if(ctl != nxt){
		i = nextinsn(code, src);
		i->kind = Ijmp;
		i->dstlabel = ctl;
		ctl->used = 1;
	}
}

static int
returnlabel(CGEnv *p, Ctl *ctl)
{
	return ctl == p->Return;
}

static Expr*
escaping(Expr *e)
{
	switch(e->kind){
//	case Egoto:
//		return e;
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

	rv = 0;
	kind = escaping(e);
	if(kind == 0)
		fatal("not an escaping expression");
	// FIXME: need to emit box init prologue; then enable goto in escaping
	else if(kind->kind == Egoto)
		rv = hget(p->labels, kind->id, strlen(kind->id));
	else
		fatal("not an escaping expression");
	if(rv == 0)
		fatal("escaping expression has undefined target");
	return rv;
}

static void
cgbranch(Code *code, CGEnv *p, Ctl *ctl, Ctl *nxt, Src *src)
{
	Insn *i;
	Ctl *l1, *l2;

	l1 = ctl->l1;
	l2 = ctl->l2;

	/* this condition is ambiguous in dybvig's report.  either
	   grouping seems correct if we assume l1 and l2 both cannot
	   be return; otherwise, i'm not sure that it matters. */
	if(returnlabel(p, l2) || (l2 == nxt && !returnlabel(p, l1))){
		i = nextinsn(code, src);
		i->kind = Ijnz;
		randloc(&i->op1, AC);
		i->dstlabel = l1;
		l1->used = 1;
		cgjmp(code, p, l2, nxt, src);
	}else{
		i = nextinsn(code, src);
		i->kind = Ijz;
		randloc(&i->op1, AC);
		i->dstlabel = l2;
		l2->used = 1;
		cgjmp(code, p, l1, nxt, src);
	}
}

static void
cgctl(Code *code, CGEnv *p, Ctl *ctl, Ctl *nxt, Src *src)
{
	if(ctl->ckind == Clabel)
		cgjmp(code, p, ctl, nxt, src);
	else if(ctl->ckind == Clabelpair)
		cgbranch(code, p, ctl, nxt, src);
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
	[Euplus] = Imov,
	[Eutwiddle] = Iinv,
	[Excast] = Ixcast,

	[E_cval] = Icval,
	[E_ref] = Iref,
	[E_sizeof] = Isizeof,
};

static void
cgunop(Code *code, CGEnv *p, unsigned kind, Operand *r1,
       Location *loc, Ctl *ctl, Ctl *nxt, Src *src)
{
	Insn *i;

	if(loc == Effect && ctl->ckind == Clabelpair)
		fatal("branch on effect");

	i = nextinsn(code, src);
	i->kind = EtoVM[kind];
	i->op1 = *r1;
	randloc(&i->dst, loc);
	cgctl(code, p, ctl, nxt, src);
}

static void
cgbinop(Code *code, CGEnv *p, unsigned kind, Operand *r1, Operand *r2,
	Location *loc, Ctl *ctl, Ctl *nxt, Src *src)
{
	Insn *i;

	if(loc == Effect && ctl->ckind == Clabelpair)
		fatal("branch on effect");

	i = nextinsn(code, src);
	i->kind = EtoVM[kind];
	i->op1 = *r1;
	i->op2 = *r2;
	randloc(&i->dst, loc);
	cgctl(code, p, ctl, nxt, src);
}

static void
cg(Expr *e, Code *code, CGEnv *p, Location *loc, Ctl *ctl, Ctl *prv, Ctl *nxt,
   unsigned tmp)
{
	Ctl *L0, *L, *R, *Lthen, *Lelse, *lpair;
	Operand r1, r2;
	Expr *q, *ep;
	Insn *i;
	unsigned narg, istail;
	Lambda *l;
	Block *b;
	Boxset *bxst;
	Location dst;
	int m;
	Src *src;

	switch(e->kind){
	case Enop:
//		i = nextinsn(code, &e->src);
//		i->kind = Inop;
		cgctl(code, p, ctl, nxt, &e->src);
		break;
	case Eelist:
		ep = e;
		while(ep->kind == Eelist){
			if(ep->e2->kind == Enull){
				cg(ep->e1, code, p, loc, ctl, prv, nxt, tmp);
				break;
			}
			L = genlabel(code, 0);
			cg(ep->e1, code, p, Effect, L, prv, L, tmp);
			emitlabel(L, ep->e2);
			prv = L;
			ep = ep->e2;
		}
		break;
	case Enull:
		cgctl(code, p, ctl, nxt, &e->src);
		break;
	case Eblock:
		b = (Block*)e->xp;
		for(m = 0; m < b->nloc; m++)
			if(b->loc[m].box){
				i = nextinsn(code, &e->src);
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
			i = nextinsn(code, &e->src);
			i->kind = Imov;
			randloc(&i->op1, &dst);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, nxt, &e->src);
		}else
			cg(e->e2, code, p, &dst, ctl, prv, nxt, tmp);
		break;
	case Euplus:
	case Euminus:
	case Eutwiddle:
	case Eunot:
	case E_sizeof:
		if(issimple(e->e1))
			cgrand(&r1, e->e1);
		else{
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, tmp);
			emitlabel(L, e);
			randloc(&r1, AC);
		}
		cgunop(code, p, e->kind, &r1, loc, ctl, nxt, &e->src);
		break;
	case E_cval:
	case E_ref:
		/* rather than compute temp requirements for arbitrary
		   3-operand applications, assume that all 3 operands
		   are simple. */
		if(!issimple(e->e1) || !issimple(e->e2) || !issimple(e->e3))
			fatal("%s with non-simple operands", EtoVM[e->kind]);
		i = nextinsn(code, &e->src);
		i->kind = EtoVM[e->kind];
		cgrand(&i->op1, e->e1);
		cgrand(&i->op2, e->e2);
		cgrand(&i->op3, e->e3);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt, &e->src);
		break;
	case Ebinop:
		if(issimple(e->e1) && issimple(e->e2)){
			cgrand(&r1, e->e1);
			cgrand(&r2, e->e2);
		}else if(issimple(e->e1)){
			cgrand(&r1, e->e1);
			L = genlabel(code, 0);
			cg(e->e2, code, p, AC, L, prv, L, tmp);
			emitlabel(L, e);
			randloc(&r2, AC);
		}else if(issimple(e->e2)){
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, tmp);
			emitlabel(L, e);
			randloc(&r1, AC);
			cgrand(&r2, e->e2);
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
		cgbinop(code, p, e->op, &r1, &r2, loc, ctl, nxt, &e->src);
		break;
	case Ecall:
		R = 0;
		istail = (returnlabel(p, ctl) && (loc == AC || loc == Effect));
		if(!istail){
			if(loc != Effect)
				R = genlabel(code, 0);
			else
				R = ctl;
			i = nextinsn(code, &e->src);
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
				cgrand(&r1, q->e1);
				i = nextinsn(code, &q->e1->src);
				i->kind = Ipush;
				i->op1 = r1;
			}else{
				L = genlabel(code, 0);
// we may be able to track frame state here.
				cg(q->e1, code, p, AC, L, L0, L, tmp);
				emitlabel(L, q->e2);
				i = nextinsn(code, &q->e1->src);
				i->kind = Ipush;
				randloc(&i->op1, AC);
			}
			q = q->e2;
			narg++;
			L0 = L;
		}

		i = nextinsn(code, &e->src);
		i->kind = Ipushi;
		randkon(&i->op1, konimm(code->konst, Vint, narg));

		if(issimple(e->e1)){
			cgrand(&r1, e->e1);
		}else {
			L0 = genlabel(code, 0);
			emitlabel(L0, e->e1);
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, L0, L, tmp);
			emitlabel(L, e);
		}

		if(!istail){
			i = nextinsn(code, &e->src);
			i->kind = Icall;
			if(issimple(e->e1))
				i->op1 = r1;
			else
				randloc(&i->op1, AC);
			if(loc != Effect){
				emitlabel(R, e);
				if(loc != AC){
					i = nextinsn(code, &e->src);
					i->kind = Imov;
					randloc(&i->op1, AC);
					randloc(&i->dst, loc);
				}
			}
			cgctl(code, p, ctl, nxt, &e->src);
		}else{
			i = nextinsn(code, &e->src);
			i->kind = Icallt;
			if(issimple(e->e1))
				i->op1 = r1;
			else
				randloc(&i->op1, AC);
		}
		break;
	case Eret:
		if(e->e1)
			cg(e->e1, code, p, AC, p->Return, prv, nxt, tmp);
		else
			cgctl(code, p, p->Return, nxt, &e->src);
		break;
	/* can Eid and Econst be rationalized with cgrand? */
	case Eid:
		i = nextinsn(code, &e->src);
		i->kind = Imov;
		randvarloc(&i->op1, e->xp, 1);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt, &e->src);
		break;
	case Ekon:
		i = nextinsn(code, &e->src);
		i->kind = Imov;
		randkon(&i->op1, e->xp);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt, &e->src);
		break;
	case Enil:
		if(loc != Effect){
			i = nextinsn(code, &e->src);
			i->kind = Imov;
			randnil(&i->op1);
			randloc(&i->dst, loc);
		}
		cgctl(code, p, ctl, nxt, &e->src);
		break;
	case Elambda:
		l = (Lambda*)e->xp;
		L = genlabel(code, l->id);
		src = &e->e1->src; /* argument list */

		for(m = l->ncap-1; m >= 0; m--){
			i = nextinsn(code, src);
			i->kind = Imov;
			randvarloc(&i->op1, l->cap[m], 0);
			randloc(&i->dst, AC);
			i = nextinsn(code, src);
			i->kind = Ipush;
			randloc(&i->op1, AC);
		}

		i = nextinsn(code, src);
		i->kind = Iclo;
		randkon(&i->op1, konimm(code->konst, Vint, l->ncap));
		randloc(&i->dst, loc);
		i->dstlabel = L;
		L->used = 1;

		cgctl(code, p, ctl, L, &e->src);

		emitlabel(L, e);
		cglambda(L, code, e);

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
	case Egoto:
		bxst = e->xp;
		for(m = 0; m < bxst->n; m++){
			i = nextinsn(code, &e->src);
			i->kind = Ibox0;
			randvarloc(&i->op1, bxst->var[m], 0);
		}
		L = hget(p->labels, e->id, strlen(e->id));
		if(L == 0)
			fatal("goto bug");
		cgjmp(code, p, L, nxt, &e->src);
		break;
	case Elabel:
		L = hget(p->labels, e->id, strlen(e->id));
		if(L == 0)
			fatal("goto bug");
		emitlabel(L, e);
		cg(e->e1, code, p, loc, ctl, prv, nxt, tmp);
		break;
	default:
		fatal("cg undefined for expression %d", e->kind);
		break;
	}
}

static void
cglambda(Ctl *name, Code *code, Expr *e)
{
	Lambda *l;
	Insn *i;
	CGEnv p;
	unsigned m, needtop;
	Ctl *top;
	Src *src;

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

	src = &e->e1->src; /* argument list */
	if(l->isvarg){
		i = nextinsn(code, src);
		i->kind = Ivargc;
		randkon(&i->op1, konimm(code->konst, Vuint, l->nparam-1));
		needtop = 1;
	}else{
		i = nextinsn(code, src);
		i->kind = Iargc;
		randkon(&i->op1, konimm(code->konst, Vuint, l->nparam));
		needtop = 1;
	}
	if(l->nloc+l->ntmp > 0){
		i = nextinsn(code, src);
		i->kind = Isubsp;
		randkon(&i->op1, konimm(code->konst, Vint, l->nloc+l->ntmp));
		needtop = 1;
	}
	if(l->isvarg){
		m = 0;
		while(m < l->nparam-1){
			if(l->param[m].box){
				i = nextinsn(code, src);
				i->kind = Ibox;
				randvarloc(&i->op1, &l->param[m], 0);
				needtop = 1;
			}
			m++;
		}
		/* by convention varg is first local stack variable */
		if(l->param[m].box){
			i = nextinsn(code, src);
			i->kind = Ibox0;
			randvarloc(&i->op1, &l->param[m], 0);
			needtop = 1;
		}
		i = nextinsn(code, src);
		i->kind = Ilist;
		randkon(&i->op1, konimm(code->konst, Vuint, 0));
		randkon(&i->op2, konimm(code->konst, Vuint, m));
		randvarloc(&i->dst, &l->param[m], 1);
	}else
		for(m = 0; m < l->nparam; m++)
			if(l->param[m].box){
				i = nextinsn(code, src);
				i->kind = Ibox;
				randvarloc(&i->op1, &l->param[m], 0);
				needtop = 1;
			}
	if(needtop){
		top = genlabel(code, 0);
		emitlabel(top, e->e2);
	}else
		top = name;

	p.Return = genlabel(code, 0);
	p.labels = labels(e->e2, code);

	cg(e->e2, code, &p, AC, p.Return, top, p.Return, l->nloc);
	emitlabel(p.Return, e->e2);
	i = nextinsn(code, &e->src);
	i->kind = Iret;
	freelabels(p.labels);
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
	e = konsts(e, code);
	code->src = e;
	cglambda(L, code, e);
	if(cqctflags['o'])
		printcode(code);
	if(cqctflags['6'])
		cg6(code);
	l = (Lambda*)e->xp;
	cl = mkcl(code, 0, l->ncap, L->label);
	return cl;
}

Closure*
haltthunk(void)
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
	i = nextinsn(code, 0);
	i->kind = Ihalt;
	return cl;
}

Closure*
callcc(void)
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
	i = nextinsn(code, 0);
	i->kind = Imov;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code, 0);
	i->kind = Ikg;
	randloc(&i->dst, ARG0);
	i = nextinsn(code, 0);
	i->kind = Icallt;
	randloc(&i->op1, AC);

	return cl;
}

Code*
callccode(void)
{
	Insn *i;
	Code *code;

	code = mkcode();
	i = nextinsn(code, 0);
	i->kind = Icallc;
	i = nextinsn(code, 0);
	i->kind = Iret;

	return code;
}

Code*
contcode(void)
{
	Insn *i;
	Code *code;

	code = mkcode();
	i = nextinsn(code, 0);
	i->kind = Imov;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(code, 0);
	i->kind = Ikp;
	i = nextinsn(code, 0);
	i->kind = Iret;

	return code;
}

Closure*
panicthunk(void)
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
	i = nextinsn(code, 0);
	i->kind = Ipanic;

	return cl;
}

void
initcg(void)
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
finicg(void)
{
}
