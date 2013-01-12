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
	s32 depth;
	u64 *b, *l, *e;	/* live mask buffer */
	u32 ml;		/* size of live mask buffer */
	u32 narg;	/* args in frame (after varg normalization) */
	u32 nloc;	/* locals in frame */
	u32 ntmp;	/* temporaries in frame */
	u32 tmp;	/* offset to first temporary */
	u32 fsz;	/* current frame size */
	u32 maxfsz;	/* maximum frame size */

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
}

static Ode*
mkode(char *id)
{
	Ode *code;
	code = (Ode*)malode();
	code->id = mkcid0(id);
	code->maxinsn = InsnAlloc;
	code->insn = mkstrn(code->maxinsn*sizeof(Insn));
	code->src = emalloc(code->maxinsn*sizeof(Src));
	code->ninsn = 0;
	code->lm = emalloc(128*sizeof(u64));
	code->nlm = 0;
	code->mlm = 128;
	code->dbg = emalloc(128*sizeof(Dbg));
	code->ndbg = 0;
	code->mdbg = 128;
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
	efree(code->lm);
	efree(code->dbg);
	efree(code->src);
	return 1;
}

static Insn*
nextinsn(Ode *code, Src src)
{
	Insn *in;
	/* plan for one extra instruction for emitlabel */
	if(code->ninsn+1 >= code->maxinsn){
		code->insn = strrealloc(code->insn, 2*code->maxinsn*sizeof(Insn));
		code->src = erealloc(code->src,
				     code->maxinsn*sizeof(Src),
				     2*code->maxinsn*sizeof(Src));
		code->maxinsn *= 2;
	}
	in = &((Insn*)strdata(code->insn))[code->ninsn];
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
			xprintf("%d(%s)", LOCIDX(loc->loc), regtos(Rfp));
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
		case Iabort:
		case Iapply:
		case Iret:
		case Iiret:
		case Ihalt:
		case Ichkint:
		case Igcpoll:
		case Iunderflow:
		case Ikg:
		case Ikp:
		case Inop:
		case Iargc:
		case Ivargc:
		case Ichksp:
		case Iaddfp:
		case Imovvc:
		case Ijmp:
		case Ijnz:
		case Ijz:
			break;
		case Iinv:
		case Ineg:
		case Inot:
		case Imov:
		case Iclo:
			setrelocrand(c, &i->op1);
			setrelocrand(c, &i->dst);
			break;
		case Ibox:
		case Ibox0:
		case Icall:
		case Icallt:
		case Ichkcl:
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
		case Ijmp:
		case Ijnz:
		case Ijz:
			t = (Insn*)codeinsn(c)+i->dstlabel->insn;
			i->scnt = t-(i+1);
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
mkvmcode(Ode *o, u32 nfree)
{
	Code *c;
	Imm n;

	n = o->ninsn;
	c = mkcode(Cvm, trampsize+n*sizeof(Insn));
	c->id = o->id;
	memcpy(codetramp(c), trampentry->buf, trampentry->n);
	memcpy(codeinsn(c), strdata(o->insn), o->ninsn*sizeof(Insn));
	c->lm = mkstr((char*)o->lm, o->nlm*sizeof(u64));
	c->dbg = mkstr((char*)o->dbg, o->ndbg*sizeof(Dbg));
	c->ndbg = o->ndbg;
	c->reloc = mkstrn(128);
	c->nreloc = 0;
	c->src = mkvec(n);
	c->nfree = nfree;
	memcpy(vecdata(c->src), o->src, n*sizeof(Src));

	setinsn(c);
	setreloc(c);
	setgo(c);
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
	case Ichkint:
		xprintf("chkint");
		break;
	case Ichkcl:
		xprintf("chkcl ");
		printrand(&i->op1);
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
	case Iret:
		xprintf("ret");
		break;
	case Iiret:
		xprintf("iret");
		break;
	case Ihalt:
		xprintf("halt");
		break;
	case Igcpoll:
		xprintf("gcpoll");
		break;
	case Iunderflow:
		xprintf("underflow");
		break;
	case Iclo:
		xprintf("clo ");
		printrand(&i->op1);
		xprintf(" ");
		printrand(&i->dst);
		break;
	case Ikg:
		xprintf("kg");
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
	case Iabort:
		xprintf("abort");
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
				xprintf("\t%20s:%u", srcfile(s), srcline(s));
		}else
			xprintf("\t\t");
		xprintf("\t%06ld", i-(Insn*)codeinsn(c));
		xprintf("\t%3d", ra2size(i, c));
		xprintf("\t%016x\t",   ra2mask(i, c));
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
randdisploc(Operand *rand, unsigned idx, int deref)
{
	rand->okind = Oloc;
	newloc(&rand->u.loc, Ldisp, idx, deref);
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
		i->kind = Ichkint;
		i = nextinsn(code, src);
		i->kind = Iret;
	}else if(ctl != nxt){
		i = nextinsn(code, src);
		i->kind = Ichkint;
		i = nextinsn(code, src);
		i->kind = Ijmp;
		i->dstlabel = ctl;
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
		i->kind = Ichkint;
		i = nextinsn(code, src);
		i->kind = Ijnz;
		randloc(&i->op1, AC);
		i->dstlabel = l1;
		cgjmp(code, p, l2, nxt, src);
	}else{
		i = nextinsn(code, src);
		i->kind = Ichkint;
		i = nextinsn(code, src);
		i->kind = Ijz;
		randloc(&i->op1, AC);
		i->dstlabel = l2;
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
finit(Frame *f, u32 narg, u32 nloc, u32 ntmp)
{
	f->ml = 1024;
	f->b = f->l = emalloc(f->ml*sizeof(u64));
	f->e = f->b+f->ml;
	f->narg = narg;
	f->nloc = nloc;
	f->ntmp = ntmp;
	f->tmp = Onfrhd+narg+nloc;
	f->fsz = Onfrhd+narg+nloc+ntmp;
	f->maxfsz = f->fsz;
	f->depth = 0;
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
fpushlm(Frame *f)
{
	u32 nw;
	nw = roundup(f->fsz, mwbits)/mwbits;
	while(f->l+nw+1+nw >= f->e)
		fgrow(f);
	*(f->l+nw) = (u64)f->fsz;
	memcpy(f->l+nw+1, f->l, nw*sizeof(u64));
	f->l += nw+1;
	f->depth++;
}

static void
fpoplm(Frame *f)
{
	u32 ow, nw;
	if(f->depth-- <= 0)
		bug();
	if(f->l <= f->b)
		bug();
	ow = roundup(f->fsz, mwbits)/mwbits;
	f->l--;
	f->fsz = (u32)*f->l;
	memset(f->l, 0, (1+ow)*sizeof(u64));
	nw = roundup(f->fsz, mwbits)/mwbits;
	f->l -= nw;
}

static void
fset(Frame *f, u32 i)
{
	u64 *p;
	while((p = f->l+i/mwbits) >= f->e)
		fgrow(f);
	*p |= (1ULL<<(i%mwbits));
}

static void
fsetarg(Frame *f, u32 i)
{
	fset(f, Oarg0+i);
}

static void
fsetrand(Frame *f, Operand *r)
{
	Location *loc;
	switch(r->okind){
	case Oloc:
		loc = &r->u.loc;
		switch(LOCKIND(loc->loc)){
		case Lframe:
			if(!LOCBOX(loc->loc))
				fset(f, LOCIDX(loc->loc));
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

static void
fbumpfsz(Frame *f, s32 i)
{
	f->fsz += i;
	if(f->fsz < Onfrhd+f->narg+f->nloc+f->ntmp)
		bug();
	if(f->fsz > f->maxfsz)
		f->maxfsz = f->fsz;
}

static void
femit(Frame *f, Ode *c)
{
	Dbg *dbg;
	u32 nw;

	if(c->ndbg >= c->mdbg){
		c->dbg = erealloc(c->dbg, c->mdbg*sizeof(Dbg),
				  2*c->mdbg*sizeof(Dbg));
		c->mdbg *= 2;
	}
	dbg = &c->dbg[c->ndbg++];
	dbg->off = c->ninsn*sizeof(Insn);
	dbg->fsz = f->fsz;
	if(f->fsz < mwbits-1)
		/* live mask fits in debug record */
		dbg->lm = (*f->l)&((1UL<<f->fsz)-1);
	else{
		/* live mask goes in live mask buffer */
		nw = roundup(f->fsz, mwbits)/mwbits;
		if(c->nlm+nw >= c->mlm){
			c->lm = erealloc(c->lm, c->mlm*sizeof(u64),
					 2*c->mlm*sizeof(u64));
			c->mlm *= 2;
		}
		dbg->lm = (1ULL<<(mwbits-1))|c->nlm; /* offset into lm */
		memcpy(c->lm+c->nlm, f->l, nw*sizeof(u64));
		c->nlm += nw;
	}
}

static void
cg(Expr *e, Ode *code, CGEnv *p, Location *loc, Ctl *ctl, Ctl *prv, Ctl *nxt,
   Frame *f)
{
	Ctl *L0, *L, *Lthen, *Lelse, *lpair;
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
	Imm nfp, nap;

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
			femit(f, code);
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
			femit(f, code);
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randloc(&i->op1, &dst);
			randloc(&i->dst, loc);
			fsetrand(f, &i->dst);
			femit(f, code);
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
			femit(f, code);
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randloc(&i->op1, &dst);
			randloc(&i->dst, loc);
			fsetrand(f, &i->dst);
			femit(f, code);
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
			femit(f, code);
			randloc(&r1, AC);
		}
		i = nextinsn(code, e->src);
		i->kind = EtoVM[e->kind];
		i->op1 = r1;
		randloc(&i->dst, loc);
		fsetrand(f, &i->dst);
		femit(f, code);
		cgctl(code, p, ctl, nxt, e->src);
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
			femit(f, code);
			randloc(&r2, AC);
		}else if(issimple(e->e2)){
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, prv, L, f);
			emitlabel(L, e);
			femit(f, code);
			randloc(&r1, AC);
			cgrand(f, &r2, e->e2);
		}else{
			L0 = genlabel(code, 0);
			randframeloc(&r1, f->tmp);
			cg(e->e1, code, p, &r1.u.loc, L0, prv, L0, f);
			emitlabel(L0, e->e2);
			femit(f, code);
			L = genlabel(code, 0);
			fpushlm(f);
			fset(f, f->tmp++);
			cg(e->e2, code, p, AC, L, L0, L, f);
			f->tmp--;
			fpoplm(f);
			emitlabel(L, e);
			femit(f, code);
			randloc(&r2, AC);
		}
		i = nextinsn(code, e->src);
		i->kind = EtoVM[e->kind];
		i->op1 = r1;
		i->op2 = r2;
		randloc(&i->dst, loc);
		fsetrand(f, &i->dst);
		femit(f, code);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Eblock:
		b = (Block*)e->xp;
		fpushlm(f);
		cg(e->e2, code, p, loc, ctl, prv, nxt, f);
		fpoplm(f);
		break;
	case Ecall:
		istail = (returnlabel(p, ctl) && (loc == AC || loc == Effect));
		fpushlm(f);
		nfp = f->fsz;
		if(!istail){
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randloc(&i->op1, CL);
			randframeloc(&i->dst, nfp+Ocl);
			fbumpfsz(f, Onfrhd);
			fset(f, nfp+Ocl);
			femit(f, code);
			nap = nfp+Onfrhd;
		}else
			nap = nfp;

		narg = elistlen(e->e2);
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
				femit(f, code);
				i = nextinsn(code, q->e1->src);
				i->kind = Imov;
				randloc(&i->op1, AC);
				randframeloc(&i->dst, nap+m);
			}
			/* argument occupies new live stack slot */
			fbumpfsz(f, 1);
			fset(f, nap+m);
			femit(f, code);

			L0 = L;
		}

		if(issimple(e->e1)){
			cgrand(f, &r1, e->e1);
		}else {
			L0 = genlabel(code, 0);
			emitlabel(L0, e->e1);
			femit(f, code);
			L = genlabel(code, 0);
			cg(e->e1, code, p, AC, L, L0, L, f);
			emitlabel(L, e);
			femit(f, code);
		}

		if(!istail){
			/* make sure callee is in AC,
			   in case it is frame relative.
			   FIXME: can't we sort this out here? */
			if(issimple(e->e1)){
				i = nextinsn(code, e->src);
				i->kind = Imov;
				i->op1 = r1;
				randloc(&i->dst, AC);
			}
			i = nextinsn(code, e->src);
			i->kind = Ichkcl;
			randloc(&i->op1, AC);
			i = nextinsn(code, e->src);
			i->kind = Imovvc;
			randimm(&i->op1, narg);
			i = nextinsn(code, e->src);
			i->kind = Iaddfp;
			i->scnt = nfp;
			i = nextinsn(code, e->src);
			i->kind = Icall;
			randloc(&i->op1, AC);
			fpoplm(f);
			femit(f, code);
			i = nextinsn(code, e->src);
			i->kind = Iaddfp;
			i->scnt = -nfp;
			if(loc != Effect && loc != AC){
				i = nextinsn(code, e->src);
				i->kind = Imov;
				randloc(&i->op1, AC);
				randloc(&i->dst, loc);
				fsetrand(f, &i->dst);
				femit(f, code);
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
			i = nextinsn(code, e->src);
			i->kind = Ichkcl;
			randloc(&i->op1, AC);
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
				fsetarg(f, m);
				femit(f, code);
			}
			i = nextinsn(code, e->src);
			i->kind = Imovvc;
			randimm(&i->op1, narg);
			i = nextinsn(code, e->src);
			i->kind = Icallt;
			randloc(&i->op1, AC);
			fpoplm(f);
			femit(f, code);
		}
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
		fsetrand(f, &i->dst);
		femit(f, code);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case E_tid:
	case Eid:
		i = nextinsn(code, e->src);
		i->kind = Imov;
		randvarloc(f, &i->op1, e->xp, 0);
		randloc(&i->dst, loc);
		fsetrand(f, &i->dst);
		femit(f, code);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Eval:
		i = nextinsn(code, e->src);
		i->kind = Imov;
		randkon(&i->op1, e->aux);
		randloc(&i->dst, loc);
		fsetrand(f, &i->dst);
		femit(f, code);
		cgctl(code, p, ctl, nxt, e->src);
		break;
	case Enil:
		if(loc != Effect){
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randnil(&i->op1);
			randloc(&i->dst, loc);
			fsetrand(f, &i->dst);
			femit(f, code);
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
		fsetrand(f, &i->dst);
		femit(f, code);
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
				femit(f, code);
				cg(e->e3, code, p, loc, ctl,
				   Lelse, nxt, f);
			}else if(!escaping(e->e2) && escaping(e->e3)){
				Lthen = genlabel(code, 0);
				lpair = genlabelpair(code, Lthen,
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, prv, Lthen, f);
				emitlabel(Lthen, e->e2);
				femit(f, code);
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
				femit(f, code);
				cg(e->e2, code, p, loc, ctl,
				   Lthen, Lelse, f);
				emitlabel(Lelse, e->e3);
				femit(f, code);
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
				femit(f, code);
				cg(e->e2, code, p, loc, ctl,
				   Lthen, nxt, f);
			}
		}
		break;
	case Emkbox:
		i = nextinsn(code, e->src);
		i->kind = Ibox;
		randvarloc(f, &i->op1, e->e1->xp, 0);
		fsetrand(f, &i->op1);
		femit(f, code);
		break;
	case Emkbox0:
		i = nextinsn(code, e->src);
		i->kind = Ibox0;
		randvarloc(f, &i->op1, e->e1->xp, 0);
		fsetrand(f, &i->op1);
		femit(f, code);
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
		femit(f, code);
		break;
	default:
		fatal("cg undefined for expression %s", S[e->kind]);
		break;
	}
}

static Code*
cglambda(Expr *e, char *id)
{
	Lambda *l;
	Ctl *L, *top;
	Ode *ode;
	Code *code;
	Insn *i;
	CGEnv p;
	unsigned m;
	Src src;
	Frame f;
	unsigned long idx;

	if(e->kind != Elambda)
		fatal("cglambda on non-lambda");
	l = (Lambda*)e->xp;

	if(id == 0)
		id = "anon";
	ode = mkode(id);

	finit(&f, l->narg, l->nloc, l->ntmp);
	fset(&f, Ocl);
	for(m = 0; m < l->narg; m++)
		fsetarg(&f, l->arg[m].idx);
	L = genlabel(ode, id);
	emitlabel(L, e);
	femit(&f, ode);
	memset(&p, 0, sizeof(p));

	if(cqctflags['b']){
		if(l->isvarg)
			xprintf("%s: var params, %u locals, %u temps\n",
				id, 1+l->nloc, l->ntmp);
		else
			xprintf("%s: %u params, %u locals, %u temps\n",
				id, l->narg, l->nloc, l->ntmp);
	}

	src = e->e1->src; /* argument list */
	if(l->isvarg){
		i = nextinsn(ode, src);
		i->kind = Ivargc;
		randimm(&i->op1, l->narg-1);
	}else{
		i = nextinsn(ode, src);
		i->kind = Iargc;
		randimm(&i->op1, l->narg);
	}

	i = nextinsn(ode, src);
	i->kind = Igcpoll;

	/* begin chksp; finish below, after cg for body,
	   when we know stack size */
	idx = ode->ninsn;
	i = nextinsn(ode, src);
	i->kind = Ichksp;

	top = genlabel(ode, 0);
	emitlabel(top, e->e2);
	femit(&f, ode);

	p.Return = genlabel(ode, 0);
	p.labels = labels(e->e2, ode);

	cg(e->e2, ode, &p, AC, p.Return, top, p.Return, &f);

	/* finish Ichksp */
	i = &((Insn*)strdata(ode->insn))[idx];
	randimm(&i->op1, f.maxfsz);

	/* FIXME: is this redundant wrt p.Return?
	   (I bet not always; I bet we've tried this before.) */

	emitlabel(p.Return, e->e2);
	femit(&f, ode);
	i = nextinsn(ode, e->src);
	i->kind = Iret;
	freelabels(p.labels);
	ffini(&f);

	code = mkvmcode(ode, l->ncap);
	if(cqctflags['o']){
		xprintf("code for %s (%s:%u):\n", id,
			srcfile(e->src), srcline(e->src));
		printcode(code);
	}
	return code;
}

Closure*
codegen(U *ctx, Expr *e)
{
	Lambda *l;
	Closure *cl;
	Code *code;

	e = docompilev(ctx, e);
	if(e == 0)
		return 0;
	code = cglambda(e, "entry");
	l = (Lambda*)e->xp;
	cl = mkcl(code, l->ncap);
	return cl;
}

void
l1_codegen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Closure *cl;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to codegen (%d)", argc);
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	cl = codegen(&ctx, valexpr(argv[0]));
	if(cl)
		*rv = mkvalcl(cl);
}

Closure*
haltthunk(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 0, 0, 0);
	fset(&f, Ocl);
	ode = mkode("$halt");
	L = genlabel(ode, "$halt");
	emitlabel(L, 0);
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Ihalt;
	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);

	return cl;
}

Closure*
abortthunk(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 0, 0, 0);
	fset(&f, Ocl);
	ode = mkode("$abort");
	L = genlabel(ode, "$abort");
	emitlabel(L, 0);
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Iabort;
	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);

	return cl;
}

Closure*
mkkcapture(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 1, 0, 0);
	fset(&f, Ocl);
	fsetarg(&f, 0);
	ode = mkode("kcapture");
	L = genlabel(ode, "kcapture");
	emitlabel(L, 0);
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Ikg;

	/* one extra insn for ra2size in case Ikg throws an error */
	i = nextinsn(ode, 0);
	i->kind = Iabort;

	code = mkvmcode(ode, 0);
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
	Frame f;

	/* expect at least two args */
	finit(&f, 2, 0, 0);
	fset(&f, Ocl);
	fsetarg(&f, 0);
	fsetarg(&f, 1);
	ode = mkode("apply");
	L = genlabel(ode, "apply");
	emitlabel(L, 0);
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Iapply;

	/* one extra insn for ra2size in case Iapply throws an error */
	i = nextinsn(ode, 0);
	i->kind = Iabort;

	code = mkvmcode(ode, 0);
	return mkcl(code, 0);
}

Closure*
stkunderflowthunk(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 0, 0, 0);
	fset(&f, Ocl);
	ode = mkode("$stkunderflow");
	L = genlabel(ode, "$stkunderflow");
	emitlabel(L, 0);
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Iunderflow;

	/* one extra insn for ra2size in case Iunderflow throws an error */
	i = nextinsn(ode, 0);
	i->kind = Iabort;

	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);
	return cl;
}

Closure*
mkraiseinterrupt(void)
{
	Ctl *L;
	Insn *i;
	Ode *ode;
	Code *code;
	Closure *cl;
	Frame f;
	Imm nfp;

	finit(&f, 2, 1, 0);
	fset(&f, Ocl);
	
	/* frame is:

	   	ra
	   	cl
	   	"caller's" fsz
	   	handler
	   	saved AC

	   we might consider saving VC as well.
	*/
	nfp = Onfrhd+3;

	ode = mkode("$raiseinterrupt");
	L = genlabel(ode, "$raiseinterrupt");
	emitlabel(L, 0);
	femit(&f, ode);
	fpushlm(&f);

	i = nextinsn(ode, 0);
	i->kind = Imov;
	randloc(&i->op1, AC);
	randframeloc(&i->dst, Oarg0+2);
	fset(&f, Oarg0);
	femit(&f, ode);
	
	/* set up call to handler (arg 1) */

	i = nextinsn(ode, 0);
	i->kind = Imov;
	randloc(&i->op1, CL);
	randframeloc(&i->dst, nfp+Ocl);
	fbumpfsz(&f, Onfrhd);
	fset(&f, nfp+Ocl);
	femit(&f, ode);

	i = nextinsn(ode, 0);
	i->kind = Imovvc;
	randimm(&i->op1, 0);

	i = nextinsn(ode, 0);
	i->kind = Imov;
	randframeloc(&i->op1, Oarg0+1);
	randloc(&i->dst, AC);

	i = nextinsn(ode, 0);
	i->kind = Iaddfp;
	i->scnt = nfp;

	i = nextinsn(ode, 0);
	i->kind = Icall;
	randloc(&i->op1, AC);
	fpoplm(&f);
	femit(&f, ode);
	
	i = nextinsn(ode, 0);
	i->kind = Iaddfp;
	i->scnt = -nfp;

	i = nextinsn(ode, 0);
	i->kind = Imov;
	randframeloc(&i->op1, Oarg0+2);
	randloc(&i->dst, AC);

	i = nextinsn(ode, 0);
	i->kind = Iiret; /* reads fsz from Oarg0 */

	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);

	return cl;
}

/*
    kresume expects to be the code for a closure with 2
    elements -- Cont k and Closure fn -- that is called
    on one argument x.  the effect is to apply fn to x,
    then return the result to k.

    an alternative would be to graft a call to fn(x)
    onto k and then resume k, ensuring that fn(x) is
    evaluated with the new continuation.

    kp implicitly accesses the closure to get k, and
    implicitly reads ac for the result of fn.  maybe
    these should be explicit.

    it's unfortunate that we need to replicate the
    calling convention here.
 */
Code*
kresumecode(void)
{
	Insn *i;
	Ode *ode;
	Code *code;
	Frame f;
	Imm nfp;

	finit(&f, 0, 0, 0);
	fset(&f, Ocl);
	ode = mkode("kresume");
	femit(&f, ode);

	i = nextinsn(ode, 0);
	i->kind = Iargc;
	randimm(&i->op1, 1);

	fpushlm(&f);

	nfp = Onfrhd+1;
	i = nextinsn(ode, 0);
	i->kind = Imov;
	randloc(&i->op1, CL);
	randframeloc(&i->dst, nfp+Ocl);
	fbumpfsz(&f, Onfrhd);
	fset(&f, nfp+Ocl);
	femit(&f, ode);

	i = nextinsn(ode, 0);
	i->kind = Imov;
	randframeloc(&i->op1, Oarg0);
	randframeloc(&i->dst, nfp+Oarg0);
	fbumpfsz(&f, 1);
	fset(&f, nfp+Oarg0);
	femit(&f, ode);

	i = nextinsn(ode, 0);
	i->kind = Iaddfp;
	i->scnt = nfp;

	i = nextinsn(ode, 0);
	i->kind = Icall;
	randdisploc(&i->op1, 1, 0);

	fpoplm(&f);
	femit(&f, ode);

	i = nextinsn(ode, 0);
	i->kind = Iaddfp;
	i->scnt = -nfp;

	i = nextinsn(ode, 0);
	i->kind = Ikp;

	/* one extra insn for ra2size in case Ikp throws an error */
	i = nextinsn(ode, 0);
	i->kind = Iabort;

	code = mkvmcode(ode, 2);
	return code;
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
