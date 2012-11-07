#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct CGstats
{
	u32 nvm;
	u32 nx86;
	u64 bytesvm;
	u64 bytesx86;
} CGstats;

typedef
struct CGEnv {
	Ctl *Return;
	HT *labels;
} CGEnv;

typedef
struct Frame
{
	u64 *b, *l, *e;
	u32 narg;	/* args in frame (after varg normalization) */
	u32 nloc;	/* locals in frame */
	u32 ntmp;	/* temporaries in frame */
	u64 sp;		/* offset to farthest defined object on stack */
	u64 maxsp;	/* maximum sp seen at pop events */
	u32 tmp;	/* offset to first temporary */
	u32 ml;
	u64 *live;
} Frame;

enum { Maxtoploc = 7 };
static Location toploc[Maxtoploc];
static Location *Effect;
static Location *AC, *FP, *CL, *ARG0, *ARG1, *ARG2;
static Code* cglambda(Expr *el, char *id);
static CGstats cgstats;

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
	resetcpilabel();
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
		[Rvc] = "%vc",
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
	Closure *cl;
	Code *cd;
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
	case Qcl:
		cl = valcl(v);
		xprintf("<closure %s>", ciddata(cl->code->id));
		break;
	case Qcode:
		cd = valcode(v);
		xprintf("<code %s>", ciddata(cd->id));
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
		case Lframe:
			if(LOCBOX(loc->loc))
				xprintf("[");
			xprintf("%d(%s)", LOCIDX(loc->loc)+1, regtos(Rfp));
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
	b = c+1;
	coff = a-b;
	addreloc(c, coff);
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
setreloc(Code *c)
{
	Insn *i, *e;
	i = codeinsn(c);
	e = codeend(c);
	for(; i < e; i++){
		switch(i->kind){
		case Iapply:
		case Iret:
		case Ihalt:
		case Ipanic:
		case Ikp:
		case Ifmask:
		case Ifsize:
		case Inop:
		case Iargc:
		case Ivargc:
		case Ichksp:
		case Iaddfp:
		case Icode:
		case Imovvc:
		case Ijmp:
		case Ijnz:
		case Ijz:
			break;
		case Imovra:
		case Ikg:
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
		case Icall:
		case Ibox:
		case Ibox0:
			setrelocrand(c, &i->op1);
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
	Insn *i, *t, *e;
	i = codeinsn(c);
	e = codeend(c);
	for(; i < e; i++)
		switch(i->kind){
		case Imovra:
		case Ijmp:
		case Ijnz:
		case Ijz:
			t = (Insn*)codeinsn(c)+i->dstlabel->insn;
			i->scnt = t-(i+1);
			break;
		case Icode:
			i->cnt = (void*)i-(void*)c;
			break;
		default:
			break;
		}
}

Code*
mkcode(Ckind kind, Imm nbytes)
{
	Code *c;
	c = (Code*)malv(Qcode, codesize(nbytes));
	c->sz = codesize(nbytes);
	c->kind = kind;
	switch(kind){
	case Cvm:
		cgstats.nvm++;
		cgstats.bytesvm += nbytes;
		break;
	case Cxfn:
		cgstats.nx86++;
		cgstats.bytesx86 += nbytes;
		break;
	default:
		break;
	}
	return c;
}

static Code*
mkvmcode(Ode *o, u32 eoff, u32 nfree)
{
	Code *c;
	Imm n;

	n = o->ninsn;
	c = mkcode(Cvm, trampsize+n*sizeof(Insn));
	c->id = o->id;
	memcpy(codetramp(c), trampentry->buf, trampentry->n);
	memcpy(codeinsn(c), o->insn, o->ninsn*sizeof(Insn));
	c->lm = mkstr((char*)o->lm, o->nlm*sizeof(u64));
	c->reloc = mkstrn(128);
	c->nreloc = 0;
	c->src = mkvec(n);
	c->nfree = nfree;
	c->eoff = eoff;
	memcpy(vecdata(c->src), o->src, n*sizeof(Src));

	setinsn(c);
	setreloc(c);
	setgo(c);
//	printcode(c);
	return c;
}

void
printinsn(Insn *i)
{
	xprintf("\t%p ", i);
	switch(i->kind){
	case Iargc:
		xprintf("argc ");
		printrand(&i->op1);
		break;
	case Ivargc:
		xprintf("vargc ");
		printrand(&i->op1);
		break;
	case Iapply:
		xprintf("apply");
		break;
	case Icode:
		xprintf("-code-  0x%x", (void*)i-i->cnt);
		break;
	case Ifsize:
		xprintf("-size- 0x%x", i->cnt);
		break;
	case Ifmask:
		xprintf("-mask- 0x%x", i->cnt);
		break;
	case Iinv:
	case Ineg:
	case Inot:
		xprintf("%s ", itos(i->kind));
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Iaddfp:
		xprintf("iaddfp %d", i->scnt);
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
	case Ichksp:
		xprintf("chksp ");
		printrand(&i->op1);
		break;
	case Imov:
		xprintf("mov ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Imovvc:
		xprintf("movvc ");
		printrand(&i->op1);
		break;
	case Icall:
		xprintf("call ");
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
	case Imovra:
		xprintf("movra %d ", i->scnt);
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
		xprintf("jmp %d", i->scnt);
		break;
	case Ijnz:
		xprintf("jnz ");
		printrand(&i->op1);
		xprintf(" %d", i->scnt);
		break;
	case Ijz:
		xprintf("jz ");
		printrand(&i->op1);
		xprintf(" %d", i->scnt);
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

void
printcode(Code *c)
{
	Src s;
	Insn *i, *e;
	xprintf("code %p (%s)\n", c, ciddata(c->id));
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
		xprintf("\t%06ld\t", i-(Insn*)codeinsn(c));
		printinsn(i);
		xprintf("\n");
	}
	xprintf("\n");
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
varloc(Frame *f, Location *loc, Var *v, int deref)
{
	if(deref && !v->box)
		bug();
	deref = deref && v->box;
	switch(v->where){
	case Vparam:
		newloc(loc, Lframe, Oarg0+v->idx, deref);
		break;
	case Vlocal:
		newloc(loc, Lframe, Oarg0+f->narg+v->idx, deref);
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
randvarloc(Frame *f, Operand *rand, Var *v, int deref)
{
	rand->okind = Oloc;
	varloc(f, &rand->u.loc, v, deref);
}

static void
randframeloc(Operand *rand, unsigned idx)
{
	rand->okind = Oloc;
	newloc(&rand->u.loc, Lframe, idx, 0);
}

static void
cgrand(Frame *f, Operand *rand, Expr *e)
{
	switch(e->kind){
	case Eboxref:
		randvarloc(f, rand, e->e1->xp, 1);
		break;
	case E_tid:
	case Eid:
		randvarloc(f, rand, e->xp, 0);
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
	// FIXME harder: is the above still relevant now that box init
	//               is emitted in cpv?
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

static void
finit(Frame *f, u32 narg, u32 nloc, u32 ntmp)
{
	f->ml = 1024;
	f->b = f->l = emalloc(f->ml*sizeof(u64));
	f->e = f->b+f->ml;
	f->narg = narg;
	f->nloc = nloc;
	f->ntmp = ntmp;
	f->sp = Onfrhd-1;
	f->sp += narg+nloc;
	f->tmp = f->sp+1;
	f->sp += ntmp;
	f->maxsp = f->sp;
}

static void
ffini(Frame *f)
{
	efree(f->l);
}

#define roundup(l,n)   (((u32)(l)+((n)-1))&~((n)-1))

static void
fgrow(Frame *f)
{
	u64 *p;
	p = erealloc(f->b, f->ml*sizeof(u64), 2*f->ml*sizeof(u64));
	f->ml *= 2;
	f->e = p+f->ml;
	f->l = p+(f->l-f->b);
	f->b = p;
}

static void
fpush(Frame *f)
{
	u32 nw;
	nw = roundup(f->sp, mwbits)/mwbits;
	while(f->l+nw+1+nw > f->e)
		fgrow(f);
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
	memset(f->l, 0, (1+ow)*sizeof(u64));
	nw = roundup(f->sp, mwbits)/mwbits;
	f->l -= nw;
}

static void
fset(Frame *f, u32 i)
{
	u64 *p;
	while((p = f->l+i/mwbits) > f->e)
		fgrow(f);
	*p |= (1ULL<<(i%mwbits));
}

static void
fsetarg(Frame *f, u32 i)
{
	fset(f, Oarg0+i);
}

static void
fsetloc(Frame *f, u32 i)
{
	fset(f, Oarg0+f->narg+i);
}

static void
femitin(Frame *f, u64 fsz, Ode *c, Src s)
{
	Insn *i;
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
}

static void
femitex(Frame *f, u64 fsz, Ode *c, Src s)
{
	Insn *i;
	u32 nw;
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
femit(Frame *f, Ode *c, Src s, u64 fsz)
{
	if(fsz < mwbits-1)
		femitin(f, fsz, c, s);
	else
		femitex(f, fsz, c, s);
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
	Location dst;
	int m;
	Src src;
	Val fn;
	Imm rap, nfp, nap;

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
	case Eboxset:
		if(e->e1->kind != Eid)
			bug();
		varloc(f, &dst, e->e1->xp, 1);
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
	case E_tg:
	case Eg:
		varloc(f, &dst, e->e1->xp, 0);
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
	case Euminus:
	case Eutwiddle:
	case Eunot:
	case Euplus:
		if(issimple(e->e1))
			cgrand(f, &r1, e->e1);
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
			cgrand(f, &r1, e->e1);
			cgrand(f, &r2, e->e2);
		}else if(issimple(e->e1)){
			cgrand(f, &r1, e->e1);
			L = genlabel(code, 0);
			cg(e->e2, code, p, AC, L, prv, L, f);
			emitlabel(L, e);
			randloc(&r2, AC);
		}else if(issimple(e->e2)){
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, f);
			emitlabel(L, e);
			randloc(&r1, AC);
			cgrand(f, &r2, e->e2);
		}else{
			L0 = genlabel(code, 0);
			randframeloc(&r1, f->tmp);
			cg(e->e1, code, p, &r1.u.loc, L0, prv, L0, f);
			emitlabel(L0, e->e2);
			L = genlabel(code, 0);
			fpush(f);
			fset(f, f->tmp++);
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
		for(m = 0; m < b->nloc; m++)
			fsetloc(f, b->loc[m].idx);
		cg(e->e2, code, p, loc, ctl, prv, nxt, f);
		fpop(f);
		break;
	case Ecall:
		R = 0;
		istail = (returnlabel(p, ctl) && (loc == AC || loc == Effect));
		fpush(f);
		nfp = f->sp+1;
		rap = 0;
		if(!istail){
			R = genlabel(code, 0);
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randloc(&i->op1, CL);
			randframeloc(&i->dst, nfp+Ocl);
			fset(f, nfp+Ocl);
			rap = nfp+Ora;
			nap = nfp+Onfrhd;
			f->sp += Onfrhd;
		}else
			nap = nfp;

		q = e->e2;
		narg = 0;
		while(q->kind != Enull){
			narg++;
			q = q->e2;
		}

		f->sp += narg;
		L0 = prv;
		for(m = 0, q = e->e2; m < narg; m++, q = q->e2){
			if(issimple(q->e1)){
				L = L0;
				cgrand(f, &r1, q->e1);
				i = nextinsn(code, q->e1->src);
				i->kind = Imov;
				i->op1 = r1;
				randframeloc(&i->dst, nap+m);
			}else{
				L = genlabel(code, 0);
				cg(q->e1, code, p, AC, L, L0, L, f);
				emitlabel(L, q->e2);
				i = nextinsn(code, q->e1->src);
				i->kind = Imov;
				randloc(&i->op1, AC);
				randframeloc(&i->dst, nap+m);
			}
			fset(f, nap+m); /* argument */
			L0 = L;
		}

		if(issimple(e->e1)){
			cgrand(f, &r1, e->e1);
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
			randframeloc(&i->dst, rap);
			/* make sure callee is in AC,
			   in case it is frame relative */
			if(issimple(e->e1)){
				i = nextinsn(code, e->src);
				i->kind = Imov;
				i->op1 = r1;
				randloc(&i->dst, AC);
			}
			i = nextinsn(code, e->src);
			i->kind = Imovvc;
			randimm(&i->op1, narg);
			i = nextinsn(code, e->src);
			i->kind = Iaddfp;
			i->scnt = nfp; /* like femit */
			i = nextinsn(code, e->src);
			i->kind = Icall;
			randloc(&i->op1, AC);
			femit(f, code, e->src, nfp);
			emitlabel(R, e);
			i = nextinsn(code, e->src);
			i->kind = Iaddfp;
			i->scnt = -nfp; /* like femit */
			if(loc != Effect && loc != AC){
				i = nextinsn(code, e->src);
				i->kind = Imov;
				randloc(&i->op1, AC);
				randloc(&i->dst, loc);
			}
			cgctl(code, p, ctl, nxt, e->src);
		}else{
			/* make sure callee is in AC,
			   in case it is an argument
			   we're about to clobber */
			if(issimple(e->e1)){
				i = nextinsn(code, e->src);
				i->kind = Imov;
				i->op1 = r1;
				randloc(&i->dst, AC);
			}
			/* move arguments to base of frame.
			   FIXME:
			      it would be nice to emit a block move;
			      don't move if you don't have to
			*/
			for(m = 0; m < narg; m++){
				i = nextinsn(code, e->src);
				i->kind = Imov;
				randframeloc(&i->op1, nap+m);
				randframeloc(&i->dst, Oarg0+m);
			}
			i = nextinsn(code, e->src);
			i->kind = Imovvc;
			randimm(&i->op1, narg);
			i = nextinsn(code, e->src);
			i->kind = Icall;
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
	/* can Eid/E_tid (and Eval?) (and Eboxref?)
	   be rationalized with cgrand? */
	case Eboxref:
		i = nextinsn(code, e->src);
		i->kind = Imov;
		randvarloc(f, &i->op1, e->e1->xp, 1);
		randloc(&i->dst, loc);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case E_tid:
	case Eid:
		i = nextinsn(code, e->src);
		i->kind = Imov;
		randvarloc(f, &i->op1, e->xp, 0);
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
		if(loc == Effect)
			fatal("lambda for effect");
		l = (Lambda*)e->xp;
		src = e->e1->src; /* argument list */
		fn = mkvalcode(cglambda(e, l->id));
		i = nextinsn(code, src);
		i->kind = Imov;
		randkon(&i->op1, fn);
		randloc(&i->dst, loc);
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
	case Emkbox:
		i = nextinsn(code, e->src);
		i->kind = Ibox;
		randvarloc(f, &i->op1, e->e1->xp, 0);
		break;
	case Emkbox0:
		i = nextinsn(code, e->src);
		i->kind = Ibox0;
		randvarloc(f, &i->op1, e->e1->xp, 0);
		break;
	case Egoto:
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
	unsigned m;
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
			       name->label, 1+l->nloc, l->ntmp);
		else
			xprintf("%s: %u params, %u locals, %u temps\n",
			       name->label, l->narg, l->nloc, l->ntmp);
	}

	src = e->e1->src; /* argument list */
	if(l->isvarg){
		i = nextinsn(code, src);
		i->kind = Ivargc;
		randimm(&i->op1, l->narg-1);
	}else{
		i = nextinsn(code, src);
		i->kind = Iargc;
		randimm(&i->op1, l->narg);
	}

	/* begin chksp; finish below, after cg for body,
	   when we know stack size */
	idx = code->ninsn;
	i = nextinsn(code, src);
	i->kind = Ichksp;

	finit(&f, l->narg, l->nloc, l->ntmp);
	fset(&f, Ocl);
	for(m = 0; m < l->narg; m++)
		fsetarg(&f, l->arg[m].idx);

	/* FIXME: do we need a special fset in varg case? */

	top = genlabel(code, 0);
	emitlabel(top, e->e2);

	p.Return = genlabel(code, 0);
	p.labels = labels(e->e2, code);

	cg(e->e2, code, &p, AC, p.Return, top, p.Return, &f);

	/* finish Ichksp */
	i = &code->insn[idx];
	randimm(&i->op1, f.maxsp);

	/* FIXME: is this redundant wrt p.Return?
	   (I bet not always; I bet we've tried this before.) */
	emitlabel(p.Return, e->e2);
	i = nextinsn(code, e->src);
	i->kind = Iret;
	freelabels(p.labels);
	ffini(&f);
}

static Code*
cglambda(Expr *e, char *id)
{
	Lambda *l;
	Ctl *L;
	Ode *ode;
	Code *code;

	if(id == 0)
		id = "anon";
	ode = mkode(id);
	L = genlabel(ode, id);
	L->used = 1;
	emitlabel(L, e);
	xcglambda(L, ode, e);
	l = (Lambda*)e->xp;
	code = mkvmcode(ode, 0, l->ncap);
	if(cqctflags['o']){
		xprintf("code for %s (%s:%u):\n", id,
			srcfile(e->src), srcline(e->src));
		printcode(code);
	}
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
	cl = mkcl(code, l->ncap);
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
	i->cnt = 1ULL<<Ocl;		/* cl */
	i = nextinsn(ode, 0);
	i->kind = Icode;
	i->cnt = 0;
	i = nextinsn(ode, 0);
	i->kind = Ifsize;
	i->cnt = Onfrhd;
	L = genlabel(ode, "$halt");
	e = ode->ninsn;
	L->used = 1;
	emitlabel(L, 0);
	i = nextinsn(ode, 0);
	i->kind = Ihalt;
	code = mkvmcode(ode, e*sizeof(Insn), 0);
	cl = mkcl(code, 0);

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
	i->kind = Icall;
	randloc(&i->op1, AC);
	code = mkvmcode(ode, 0, 0);
	cl = mkcl(code, 0);

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
	code = mkvmcode(ode, 0, 0);
	return mkcl(code, 0);
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
	code = mkvmcode(ode, 0, 0);

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
	code = mkvmcode(ode, 0, 0);
	cl = mkcl(code, 0);

	return cl;
}

void
initcg(void)
{
	unsigned m;
	m = 0;
	Effect = &toploc[m++];
	newloc(Effect, Lreg, Rac, 0);
	AC = &toploc[m++];
	newloc(AC, Lreg, Rac, 0);
	FP = &toploc[m++];
	newloc(FP, Lreg, Rfp, 0);
	CL = &toploc[m++];
	newloc(CL, Lreg, Rcl, 0);
	ARG0 = &toploc[m++];
	newloc(ARG0, Lframe, Oarg0+0, 0);
	ARG1 = &toploc[m++];
	newloc(ARG1, Lframe, Oarg0+1, 0);
	ARG2 = &toploc[m++];
	newloc(ARG2, Lframe, Oarg0+2, 0);
	if(m != Maxtoploc)
		bug();
}

void
finicg(void)
{
}

void
cgstatistics(Tab *t)
{
	tabput(t, mkvalcid(mkcid0("nvm")),
	       mkvallitcval(Vuvlong, cgstats.nvm));
	tabput(t, mkvalcid(mkcid0("nx86")),
	       mkvallitcval(Vuvlong, cgstats.nx86));
	tabput(t, mkvalcid(mkcid0("bytesvm")),
	       mkvallitcval(Vuvlong, cgstats.bytesvm));
	tabput(t, mkvalcid(mkcid0("bytesx86")),
	       mkvallitcval(Vuvlong, cgstats.bytesx86));
}
