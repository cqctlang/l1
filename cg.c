#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct CGEnv {
	Ctl *Return;
	HT *labels;
} CGEnv;

typedef
struct Stats
{
	u64 ninsn;
} Stats;

static Location toploc[8];
static Location *Effect;
static Location *AC, *FP, *SP, *PC, *ARG0, *ARG1, *ARG2;
static Code* cglambda(Expr *el, char *id);
static Stats	stats;

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

// register constants in code object constant pool
static Expr*
konsts(Expr *e, Ode *code)
{
	Expr *p;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Elambda:
		return e;
	case Eval:
		setaux(e, konval(code->konst, e->aux));
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, konsts(p->e1, code));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, konsts(e->e1, code));
		sete2(e, konsts(e->e2, code));
		sete3(e, konsts(e->e3, code));
		sete4(e, konsts(e->e4, code));
		return e;
	}
}

static void
newloc(Location *loc, unsigned kind, unsigned idx, unsigned indirect)
{
	loc->loc = LOC(idx,indirect,kind);
}

static Ctl*
mklabel(Ode *code)
{
	Ctl *ctl;
	ctl = emalloc(sizeof(Ctl));
	ctl->link = code->clist;
	ctl->ode = code;
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

static unsigned labelseed = 0;

void
resetlabels()
{
	labelseed = 0;
}

static Ctl*
genlabel(Ode *code, char *s)
{
	Ctl *ctl;
	static char buf[32];

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
genlabelpair(Ode *code, Ctl *l1, Ctl *l2)
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
	Ode *code;
	code = ctl->ode;
	if(ctl->ckind != Clabel)
		fatal("attempt to emit label pair");

	ctl->insn = code->ninsn;
	if(code->labels[code->ninsn])
		return;
//		fatal("multiple labels %s,%s", ctl->label,
//		      code->labels[code->ninsn]->label);
	code->labels[code->ninsn] = ctl;
}

static Ode*
mkode(char *id)
{
	Ode *code;
	code = (Ode*)malode();
	code->id = mkcid0(id);
	code->maxinsn = InsnAlloc;
	code->insn = emalloc(code->maxinsn*sizeof(Insn));
	code->labels = emalloc(code->maxinsn*sizeof(Ctl*));
	code->src = emalloc(code->maxinsn*sizeof(Src));
	code->ninsn = 0;
	code->konst = mktabq();
	code->lm = emalloc(128*sizeof(u64));
	code->nlm = 0;
	code->mlm = 128;
	quard((Val)code);
	return code;
}

int
freeode(Head *hd)
{
	Ode *code;
	Ctl *p, *q;

	code = (Ode*)hd;
	p = code->clist;
	while(p){
		q = p->link;
		freelabel(p);
		p = q;
	}
	efree(code->insn);
	efree(code->labels);
	efree(code->lm);
	efree(code->src);
	return 1;
}

static Code*
mkcode(Imm nbytes)
{
	Code *c;
	c = (Code*)malv(Qcode, codesize(nbytes));
	return c;
}

static void
addreloc(Code *code, uptr coff, uptr ioff)
{
	Reloc *r;
	if(code->nreloc >= code->reloc->len/sizeof(Reloc))
		code->reloc = strrealloc(code->reloc, 2*code->reloc->len);
	r = (Reloc*)strdata(code->reloc)+code->nreloc++;
	r->coff = coff;
	r->ioff = ioff;
}

static Insn*
nextinsn(Ode *code, Src src)
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
		code->src = erealloc(code->src,
				     code->maxinsn*sizeof(Src),
				     2*code->maxinsn*sizeof(Src));
		code->maxinsn *= 2;
	}
	in = &code->insn[code->ninsn];
	if(src == 0)
		src = mksrcfake("system code");
	code->src[code->ninsn] = src;
	code->ninsn++;
	stats.ninsn++;
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
printval(Val v)
{
	Cid *id;
	Cval *cv;
	Str *str;
	Ctype *t;
	char c, *p;
	Imm i, m;

	switch(Vkind(v)){
	case Qcval:
		cv = valcval(v);
		xprintf("%" PRIu64, cvalu(cv));
		break;
	case Qcid:
		id = valcid(v);
		p = ciddata(id);
		m = id->len-1;
		if(m > 15)
			m = 15;
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
		if(id->len-1 > m)
			xprintf("...");
		break;
	case Qstr:
		str = valstr(v);
		p = strdata(str);
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
			case '\0':
				xprintf("\0");
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
	case Qctype:
		t = valctype(v);
		p = fmtctypec(t);
		xprintf("#%p %s#", t, p);
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
			xprintf("<%s>", ciddata(valcid(loc->v)));
			break;
		case Ltopr:
			xprintf("<%s>", ciddata(valcid(car(loc->v))));
			break;
		}
		break;
	case Oval:
		printval(r->u.val);
		break;
	case Oimm:
		xprintf("%lu", r->u.imm);
		break;
	case Onil:
		xprintf("nil");
		break;
	default:
		fatal("unknown operand kind %d", r->okind);
	}
}

static void
setrelocobj(Code *c, void *a)
{
	void *b;
	uptr coff;
	b = (void*)c;
	coff = a-b;
	addreloc(c, coff, 0);
}

static void
setrelocrand(Code *c, Operand *r)
{
	void *a;
	switch(r->okind){
	case Oval:
		a = &r->u.val;
		break;
	case Oloc:
		if(LOCKIND(r->u.loc.loc) != Ltopl
		   && LOCKIND(r->u.loc.loc) != Ltopr)
			return;
		a = &r->u.loc.v;
		break;
	default:
		return;
	}
	setrelocobj(c, a);
}

static void
setrelocinsn(Code *c, Insn **p)
{
	void *a, *b;
	uptr coff, ioff;
	a = p;
	b = (void*)c;
	coff = a-b;
	ioff = *(void**)a-b;
	addreloc(c, coff, ioff);
}

static void
setreloc(Code *c)
{
	Insn *i, *e;
	i = codeinsn(c);
	e = codeend(c);
	for(; i < e; i++){
		switch(i->kind){
		case Icallc:
		case Iapply:
		case Iret:
		case Ihalt:
		case Ipanic:
		case Ikp:
		case Ifmask:
		case Ifsize:
		case Ipushi:
		case Inop:
		case Iargc:
		case Ivargc:
		case Isubsp:
		case Iframe:
		case Icode:
			break;
		case Ikg:
		case Ilist:
			setrelocrand(c, &i->dst);
			break;
		case Iinv:
		case Ineg:
		case Inot:
		case Imov:
		case Iclo:
			setrelocrand(c, &i->op1);
			setrelocrand(c, &i->dst);
			break;
		case Ipush:
		case Icall:
		case Icallt:
		case Ibox:
		case Ibox0:
			setrelocrand(c, &i->op1);
		case Imovra:
		case Ijmp:
		case Ijnz:
		case Ijz:
			setrelocinsn(c, &i->targ);
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
		case Ixcast:
			setrelocrand(c, &i->op1);
			setrelocrand(c, &i->op2);
			setrelocrand(c, &i->dst);
			break;
		}
	}
}

static void
setinsn(Code *c)
{
	Insn *i, *e;
	i = codeinsn(c);
	e = codeend(c);
	for(; i < e; i++)
		switch(i->kind){
		case Imovra:
		case Ijmp:
		case Ijnz:
		case Ijz:
			i->targ = codeinsn(c)+i->dstlabel->insn;
			break;
		case Icode:
			i->cnt = (void*)i-(void*)c;
			break;
		default:
			break;
		}
}

static Code*
prepcode(Ode *o)
{
	Code *c;
	Imm n;

	n = o->ninsn;
	c = mkcode(n*sizeof(Insn));
	c->sz = codesize(n*sizeof(Insn));
	c->id = o->id;
	memcpy(codeinsn(c), o->insn, o->ninsn*sizeof(Insn));
	c->konst = o->konst;
	c->lm = mkstr((char*)o->lm, o->nlm*sizeof(u64));
	c->reloc = mkstrn(128);
	c->nreloc = 0;
	c->src = mkvec(n);
	memcpy(vecdata(c->src), o->src, n*sizeof(Src));

	setinsn(c);
	setreloc(c);
	setgo(c);
	return c;
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
	case Iapply:
		xprintf("apply");
		break;
	case Icode:
		xprintf("-code-  0x%p", (void*)i-i->cnt);
		break;
	case Ifsize:
		xprintf("-fsize- 0x%x", i->cnt);
		break;
	case Ifmask:
		xprintf("-fmask- 0x%x", i->cnt);
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
		xprintf("subsp ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->op2);
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
	case Iframe:
		xprintf("frame");
		break;
	case Imovra:
		xprintf("movra %p", i->targ);
		printrand(&i->dst);
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
		xprintf("clo ");
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
		xprintf("jmp %p", i->targ);
		break;
	case Ijnz:
		xprintf("jnz ");
		printrand(&i->op1);
		xprintf(" %p", i->targ);
		break;
	case Ijz:
		xprintf("jz ");
		printrand(&i->op1);
		xprintf(" %p", i->targ);
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
	default:
		fatal("printinsn: unrecognized insn %d", i->kind);
	}
}

static void
printcode(Code *c)
{
	Src s;
	Insn *i, *e;
	i = codeinsn(c);
	e = codeend(c);
	for(; i < e; i++){
		s = addr2line(c, i);
		if(s){
			if(srclineval(s) == Xnil)
				xprintf("\t(%s)", srcfile(s));
			else
				xprintf("\t%s:%u", srcfile(s), srcline(s));
		}else
			xprintf("\t\t");
		printinsn(i);
		xprintf("\n");
	}
}

static void
reclabels(Expr *e, Ode *code, HT *ls)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
		break;
	case Elabel:
		id = idsym(e->e1);
		hputs(ls, id, strlen(id), genlabel(code, id));
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
labels(Expr *e, Ode *code)
{
	HT *ls;
	ls = mkhts();
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
	rand->okind = Oval;
	rand->u.val = v;
}

static void
randnil(Operand *rand)
{
	rand->okind = Onil;
}

static void
randimm(Operand *rand, Imm i)
{
	rand->okind = Oimm;
	rand->u.imm = i;
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
	case Vtopl:
		/* only the location type is used for toplevel var */
		newloc(loc, Ltopl, 0, 0);
		loc->v = mkvalcid(v->sym);
		break;
	case Vtopr:
		/* only the location type is used for toplevel var */
		newloc(loc, Ltopr, 0, 0);
		loc->v = mkvalpair(v->kv);
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
	case E_tid:
	case Eid:
		randvarloc(rand, e->xp, 1);
		break;
	case Eval:
		randkon(rand, e->aux);
		break;
	case Enil:
		randnil(rand);
		break;
	default:
		fatal("bug: cgrand on node %d", e->kind);
	}
}

static void
cgjmp(Ode *code, CGEnv *p, Ctl *ctl, Ctl *nxt, Src src)
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

#if 0
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
#endif

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
		rv = hgets(p->labels, idsym(kind), strlen(idsym(kind)));
	else
		fatal("not an escaping expression");
	if(rv == 0)
		fatal("escaping expression has undefined target");
	return rv;
}

static void
cgbranch(Ode *code, CGEnv *p, Ctl *ctl, Ctl *nxt, Src src)
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
cgctl(Ode *code, CGEnv *p, Ctl *ctl, Ctl *nxt, Src src)
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
	[Esub] = Isub,
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
};

static void
cgunop(Ode *code, CGEnv *p, unsigned kind, Operand *r1,
       Location *loc, Ctl *ctl, Ctl *nxt, Src src)
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
cgbinop(Ode *code, CGEnv *p, unsigned kind, Operand *r1, Operand *r2,
	Location *loc, Ctl *ctl, Ctl *nxt, Src src)
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

typedef
struct Frame
{
	u64 *b, *l, *e;
	u32 ml;
	u32 nloc;
	u32 ntmp;
	u64 sp, maxsp;
	u64 *live;
	u32 tmp;
} Frame;

static void
finit(Frame *f, u32 nloc, u32 ntmp)
{
	f->ml = 1024;
	f->b = f->l = emalloc(f->ml*sizeof(u64));
	f->e = f->b+f->ml;
	f->nloc = nloc;
	f->ntmp = ntmp;
	f->tmp = nloc;
	f->sp = nloc+ntmp;
	f->maxsp = f->sp;
}

static void
ffini(Frame *f)
{
	efree(f->l);
}

#define roundup(l,n)   (((u32)(l)+((n)-1))&~((n)-1))

static void
fpush(Frame *f)
{
	u32 nw;
	u64 *p;
	nw = roundup(f->sp, mwbits)/mwbits;
	if(f->l+nw+1+nw > f->e){
		p = erealloc(f->b, f->ml*sizeof(u64), 2*f->ml*sizeof(u64));
		f->ml *= 2;
		f->e = p+f->ml;
		f->l = p+(f->l-f->b);
		f->b = p;
		if(f->l+nw > f->e)
			bug();
	}
	*(f->l+nw) = f->sp;
	memcpy(f->l+nw+1, f->l, nw*sizeof(u64));
	f->l += nw+1;
}

static void
fpop(Frame *f)
{
	u32 ow, nw;
	if(f->l <= f->b)
		bug();
	if(f->sp > f->maxsp)
		f->maxsp = f->sp;
	ow = roundup(f->sp, mwbits)/mwbits;
	f->l--;
	f->sp = *f->l;
	memset(f->l, 0, (ow+1)*sizeof(u64));
	nw = roundup(f->sp, mwbits)/mwbits;
	f->l -= nw;
}

static void
fset(Frame *f, u32 i)
{
	u64 *p;
	p = f->l+i/mwbits;
	*p |= (1UL<<(i%mwbits));
}

/* FIXME: we should mask out frame mask bits above the
   frame size in the extended form.  they are harmless,
   but may be confusing if inspected.  (they are *not*
   harmless in the short form.) */
static void
femit(Frame *f, Ode *c, Src s, u32 narg)
{
	u32 nw;
	u64 fsz;
	Insn *i;

	fsz = f->sp-(narg+4); /* args+narg+Iframe */
	if(fsz < mwbits-1){
		i = nextinsn(c, s);
		i->kind = Ifmask;
		i->cnt = (*f->l)&((1UL<<fsz)-1);
		if((i->cnt>>(mwbits-1))&1)
			bug();
		i = nextinsn(c, s);
		i->kind = Icode;
		i->cnt = 0;
		i = nextinsn(c, s);
		i->kind = Ifsize;
		i->cnt = fsz;
		return;
	}
	nw = roundup(fsz, mwbits)/mwbits;
	if(c->nlm+nw > c->mlm){
		c->lm = erealloc(c->lm, c->mlm*sizeof(u64),
				 2*c->mlm*sizeof(u64));
		c->mlm *= 2;
	}
	i = nextinsn(c, s);
	i->kind = Ifmask;
	i->cnt = (1ULL<<(mwbits-1))|c->nlm;
	i = nextinsn(c, s);
	i->kind = Icode;
	i->cnt = 0;
	i = nextinsn(c, s);
	i->kind = Ifsize;
	i->cnt = fsz;
	memcpy(c->lm+c->nlm, f->l, nw*sizeof(u64));
	c->nlm += nw;
}

static void
cg(Expr *e, Ode *code, CGEnv *p, Location *loc, Ctl *ctl, Ctl *prv, Ctl *nxt,
   Frame *f)
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
	Src src;
	Val fn;
	u64 rap;

	switch(e->kind){
	case Enop:
//		i = nextinsn(code, &e->src);
//		i->kind = Inop;
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Eelist:
		ep = e;
		while(ep->kind == Eelist){
			if(ep->e2->kind == Enull){
				cg(ep->e1, code, p, loc, ctl, prv, nxt, f);
				break;
			}
			L = genlabel(code, 0);
			cg(ep->e1, code, p, Effect, L, prv, L, f);
			emitlabel(L, ep->e2);
			prv = L;
			ep = ep->e2;
		}
		break;
	case Enull:
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case E_tg:
	case Eg:
		if(e->e1->kind != Eid && e->e1->kind != E_tid)
			fatal("bug");
		varloc(&dst, e->e1->xp, 1);
		if(loc != Effect){
			L = genlabel(code, 0);
			cg(e->e2, code, p, &dst, L, prv, L, f);
			emitlabel(L, e);
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randloc(&i->op1, &dst);
			randloc(&i->dst, loc);
			cgctl(code, p, ctl, nxt, e->src);
		}else
			cg(e->e2, code, p, &dst, ctl, prv, nxt, f);
		break;
	case Euplus:
	case Euminus:
	case Eutwiddle:
	case Eunot:
		if(issimple(e->e1))
			cgrand(&r1, e->e1);
		else{
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, f);
			emitlabel(L, e);
			randloc(&r1, AC);
		}
		cgunop(code, p, e->kind, &r1, loc, ctl, nxt, e->src);
		break;
	case EBINOP:
		if(issimple(e->e1) && issimple(e->e2)){
			cgrand(&r1, e->e1);
			cgrand(&r2, e->e2);
		}else if(issimple(e->e1)){
			cgrand(&r1, e->e1);
			L = genlabel(code, 0);
			cg(e->e2, code, p, AC, L, prv, L, f);
			emitlabel(L, e);
			randloc(&r2, AC);
		}else if(issimple(e->e2)){
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, f);
			emitlabel(L, e);
			randloc(&r1, AC);
			cgrand(&r2, e->e2);
		}else{
			L0 = genlabel(code, 0);
			randstkloc(&r1, Llocal, f->tmp);
			cg(e->e1, code, p, &r1.u.loc, L0, prv, L0, f);
			emitlabel(L0, e->e2);
			L = genlabel(code, 0);
			fpush(f);
			fset(f, f->tmp);
			f->tmp++;
			cg(e->e2, code, p, AC, L, L0, L, f);
			f->tmp--;
			fpop(f);
			emitlabel(L, e);
			randloc(&r2, AC);
		}
		cgbinop(code, p, e->kind, &r1, &r2, loc, ctl, nxt, e->src);
		break;
	case Eblock:
		b = (Block*)e->xp;
		fpush(f);
		for(m = 0; m < b->nloc; m++){
			fset(f, b->loc[m].idx);
			if(b->loc[m].box){
				i = nextinsn(code, e->src);
				i->kind = Ibox0;
				randvarloc(&i->op1, &b->loc[m], 0);
			}
		}
		cg(e->e2, code, p, loc, ctl, prv, nxt, f);
		fpop(f);
		break;
	case Ecall:
		R = 0;
		rap = 0;
		istail = (returnlabel(p, ctl) && (loc == AC || loc == Effect));
		fpush(f);
		if(!istail){
			if(1 || loc != Effect)
				R = genlabel(code, 0);
			else
				R = ctl;
			i = nextinsn(code, e->src);
			i->kind = Iframe;
			fset(f, f->sp+1);  /* Iframe cl */
			f->sp += 3;
			rap = f->sp-1;     /* ra slot as local index */
		}

		sete2(e, invert(e->e2)); /* push arguments in reverse order */
		q = e->e2;
		narg = 0;
		L0 = prv;
		while(q->kind != Enull){
			// FIXME: can we check for simple? here and push
			// the operand directly? (and likewise for the call?)
			if(issimple(q->e1)){
				L = L0;
				cgrand(&r1, q->e1);
				i = nextinsn(code, q->e1->src);
				i->kind = Ipush;
				i->op1 = r1;
			}else{
				L = genlabel(code, 0);
				cg(q->e1, code, p, AC, L, L0, L, f);
				emitlabel(L, q->e2);
				i = nextinsn(code, q->e1->src);
				i->kind = Ipush;
				randloc(&i->op1, AC);
			}
			fset(f, f->sp); /* argument */
			f->sp++;
			q = q->e2;
			narg++;
			L0 = L;
		}

		i = nextinsn(code, e->src);
		i->kind = Ipushi;
		randimm(&i->op1, narg);
		f->sp++;

		if(issimple(e->e1)){
			cgrand(&r1, e->e1);
		}else {
			L0 = genlabel(code, 0);
			emitlabel(L0, e->e1);
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, L0, L, f);
			emitlabel(L, e);
		}

		if(!istail){
			i = nextinsn(code, e->src);
			i->kind = Imovra;
			i->dstlabel = R;
			R->used = 1;
			randstkloc(&i->dst, Llocal, rap);
			i = nextinsn(code, e->src);
			i->kind = Icall;
			if(issimple(e->e1))
				i->op1 = r1;
			else
				randloc(&i->op1, AC);
			femit(f, code, e->src, narg);
			if(1 || loc != Effect){
				emitlabel(R, e);
				if(loc != Effect && loc != AC){
					i = nextinsn(code, e->src);
					i->kind = Imov;
					randloc(&i->op1, AC);
					randloc(&i->dst, loc);
				}
			}
			cgctl(code, p, ctl, nxt, e->src);
		}else{
			i = nextinsn(code, e->src);
			i->kind = Icallt;
			if(issimple(e->e1))
				i->op1 = r1;
			else
				randloc(&i->op1, AC);
		}
		fpop(f);
		break;
	case Eret:
		if(e->e1)
			cg(e->e1, code, p, AC, p->Return, prv, nxt, f);
		else{
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randnil(&i->op1);
			randloc(&i->dst, AC);
			cgctl(code, p, p->Return, nxt, e->src);
		}
		break;
	/* can Eid/E_tid (and Eval?) be rationalized with cgrand? */
	case E_tid:
	case Eid:
		i = nextinsn(code, e->src);
		i->kind = Imov;
		randvarloc(&i->op1, e->xp, 1);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Eval:
		i = nextinsn(code, e->src);
		i->kind = Imov;
		randkon(&i->op1, e->aux);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Enil:
		if(loc != Effect){
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randnil(&i->op1);
			randloc(&i->dst, loc);
		}
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Elambda:
		l = (Lambda*)e->xp;
		src = e->e1->src; /* argument list */
		fpush(f);
		for(m = l->ncap-1; m >= 0; m--){
			i = nextinsn(code, src);
			i->kind = Imov;
			randvarloc(&i->op1, l->cap[m], 0);
			randloc(&i->dst, AC);
			i = nextinsn(code, src);
			i->kind = Ipush;
			randloc(&i->op1, AC);
			fset(f, f->sp++);
		}
		fn = konval(code->konst, mkvalcode(cglambda(e, l->id)));
		i = nextinsn(code, src);
		i->kind = Iclo;
		randkon(&i->op1, fn);
		randimm(&i->op2, l->ncap);
		randloc(&i->dst, loc);
		fpop(f);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Eif:
		if(e->e3){
			if(escaping(e->e2) && !escaping(e->e3)){
				Lelse = genlabel(code, 0);
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     Lelse);
				cg(e->e1, code, p, AC, lpair, prv, Lelse, f);
				emitlabel(Lelse, e->e3);
				cg(e->e3, code, p, loc, ctl,
				   Lelse, nxt, f);
			}else if(!escaping(e->e2) && escaping(e->e3)){
				Lthen = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen,
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, prv, Lthen, f);
				emitlabel(Lthen, e->e2);
				cg(e->e2, code, p, loc, ctl,
				   Lthen, nxt, f);
			}else if(escaping(e->e2) && escaping(e->e3)){
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, prv, nxt, f);
			}else{
				Lthen = genlabel(code, 0);
				Lelse = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen, Lelse);
				cg(e->e1, code, p, AC, lpair, prv, Lthen, f);
				emitlabel(Lthen, e->e2);
				cg(e->e2, code, p, loc, ctl,
				   Lthen, Lelse, f);
				emitlabel(Lelse, e->e3);
				cg(e->e3, code, p, loc, ctl,
				   Lelse, nxt, f);
			}
		}else{
			if(escaping(e->e2)){
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     ctl);
				cg(e->e1, code, p, AC, lpair, prv, nxt, f);
			}else{
				Lthen = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen, ctl);
				cg(e->e1, code, p, AC, lpair, prv, Lthen, f);
				emitlabel(Lthen, e->e2);
				cg(e->e2, code, p, loc, ctl,
				   Lthen, nxt, f);
			}
		}
		break;
	case Egoto:
		bxst = e->xp;
		for(m = 0; m < bxst->n; m++){
			i = nextinsn(code, e->src);
			i->kind = Ibox0;
			randvarloc(&i->op1, bxst->var[m], 0);
		}
		L = hgets(p->labels, idsym(e->e1), strlen(idsym(e->e1)));
		if(L == 0)
			fatal("goto bug");
		cgjmp(code, p, L, nxt, e->src);
		break;
	case Elabel:
		L = hgets(p->labels, idsym(e->e1), strlen(idsym(e->e1)));
		if(L == 0)
			fatal("goto bug");
		emitlabel(L, e);
		break;
	default:
		fatal("cg undefined for expression %s", S[e->kind]);
		break;
	}
}

static void
xcglambda(Ctl *name, Ode *code, Expr *e)
{
	Lambda *l;
	Insn *i;
	CGEnv p;
	unsigned m, needtop;
	Ctl *top;
	Src src;
	Frame f;
	unsigned long idx;

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

	src = e->e1->src; /* argument list */
	if(l->isvarg){
		i = nextinsn(code, src);
		i->kind = Ivargc;
		randimm(&i->op1, l->nparam-1);
		needtop = 1;
	}else{
		i = nextinsn(code, src);
		i->kind = Iargc;
		randimm(&i->op1, l->nparam);
		needtop = 1;
	}
	finit(&f, l->nloc, l->ntmp);
	f.tmp = l->nloc;
	f.sp = l->nloc+l->ntmp;
	idx = code->ninsn;
	i = nextinsn(code, src);
	i->kind = Isubsp;
	randimm(&i->op1, l->nloc+l->ntmp);
	needtop = 1;
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
		randimm(&i->op1, 0);
		randimm(&i->op2, m);
		randvarloc(&i->dst, &l->param[m], 1);
		fset(&f, l->param[m].idx);
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

	cg(e->e2, code, &p, AC, p.Return, top, p.Return, &f);
	i = &code->insn[idx]; /* finish Isubsp */
	randimm(&i->op2, f.maxsp);
	emitlabel(p.Return, e->e2);
	i = nextinsn(code, e->src);
	i->kind = Iret;
	freelabels(p.labels);
	ffini(&f);
}

static Code*
cglambda(Expr *e, char *id)
{
	Ctl *L;
	Ode *ode;
	Code *code;

	if(id == 0)
		id = "anon";
	ode = mkode(id);
	L = genlabel(ode, id);
	L->used = 1;
	emitlabel(L, e);

	sete1(e, konsts(e->e1, ode));
	sete2(e, konsts(e->e2, ode));
	sete3(e, konsts(e->e3, ode));
	sete4(e, konsts(e->e4, ode));

	xcglambda(L, ode, e);
	code = prepcode(ode);

	return code;
}

Closure*
codegen(Expr *e)
{
	Lambda *l;
	Closure *cl;
	Code *code;

	code = cglambda(e, "entry");
	l = (Lambda*)e->xp;
	cl = mkcl(code, 0, l->ncap);
	if(cqctflags['o'])
		printcode(code);
	return cl;
}

Closure*
haltthunk(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;
	Imm e;

	ode = mkode("$halt");
	i = nextinsn(ode, 0);
	i->kind = Ifmask;
	i->cnt = 0;
	i = nextinsn(ode, 0);
	i->kind = Icode;
	i->cnt = 0;
	i = nextinsn(ode, 0);
	i->kind = Ifsize;
	i->cnt = 0;
	L = genlabel(ode, "$halt");
	e = ode->ninsn;
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(ode, 0);
	i->kind = Ihalt;
	code = prepcode(ode);
	cl = mkcl(code, e, 0);

	return cl;
}

Closure*
callcc(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;

	ode = mkode("callcc");
	L = genlabel(ode, "callcc");
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(ode, 0);
	i->kind = Imov;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(ode, 0);
	i->kind = Ikg;
	randloc(&i->dst, ARG0);
	i = nextinsn(ode, 0);
	i->kind = Icallt;
	randloc(&i->op1, AC);
	code = prepcode(ode);
	cl = mkcl(code, 0, 0);

	return cl;
}

Closure*
mkapply(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;

	ode = mkode("apply");
	L = genlabel(ode, "apply");
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(ode, 0);
	i->kind = Iapply;
	code = prepcode(ode);

	return mkcl(code, 0, 0);
}

Code*
callccode(char *id)
{
	Insn *i;
	Ode *ode;
	Code *code;

	ode = mkode(id);
	i = nextinsn(ode, 0);
	i->kind = Icallc;
	i = nextinsn(ode, 0);
	i->kind = Ifmask;
	i->cnt = 0;
	i = nextinsn(ode, 0);
	i->kind = Icode;
	i->cnt = 0;
	i = nextinsn(ode, 0);
	i->kind = Ifsize;
	i->cnt = 0;
	i = nextinsn(ode, 0);
	i->kind = Iret;
	code = prepcode(ode);

	return code;
}

Code*
contcode(void)
{
	Insn *i;
	Ode *ode;
	Code *code;

	ode = mkode("kp");
	i = nextinsn(ode, 0);
	i->kind = Imov;
	randloc(&i->op1, ARG0);
	randloc(&i->dst, AC);
	i = nextinsn(ode, 0);
	i->kind = Ikp;
	i = nextinsn(ode, 0);
	i->kind = Iret;
	code = prepcode(ode);

	return code;
}

Closure*
panicthunk(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;

	ode = mkode("panic");
	L = genlabel(ode, "panic");
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(ode, 0);
	i->kind = Ipanic;
	code = prepcode(ode);
	cl = mkcl(code, 0, 0);

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

void
cgstatistics(Tab *t)
{
	tabput(t, mkvalcid(mkcid0("ninsn")),
	       mkvallitcval(Vuvlong, stats.ninsn));
}
