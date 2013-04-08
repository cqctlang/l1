#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct CGEnv {
	Ctlidx Return;
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

typedef
enum
{
	/* these need to match prefix of Okind */
	Lreg,
	Lframe,
	Ldisp,
	Ltopl,
	Ltopr,
} Lkind;

typedef
struct Location {
	unsigned loc;		/* access with LOC macros */
	Val v;			/* topl/topr */
} Location;

#define LOC(idx,box,kind)	(((idx)<<4)|((box&1)<<3)|((kind)&0x7))
#define LOCIDX(loc)		((loc)>>4)
#define LOCBOX(loc)		(((loc)>>3)&0x1)
#define LOCKIND(loc)		((loc)&0x7)

enum { Maxtoploc = 3 };
static Location toploc[Maxtoploc];
static Location *Effect, *AC, *CL;
static Code* cglambda(Expr *el, char *id);

static void
newloc(Location *loc, unsigned kind, unsigned idx, unsigned indirect)
{
	loc->loc = LOC(idx,indirect,kind);
}

static Ctlidx
mklabel(Precode *code)
{
	if(code->nctl >= code->mctl){
		code->ctl = strrealloc(code->ctl, 2*code->mctl*sizeof(Ctl));
		code->mctl *= 2;
	}
	return code->nctl++;
}

static Ctl*
idx2ctl(Precode *code, Ctlidx idx)
{
	return ((Ctl*)strdata(code->ctl))+idx;
}

void
resetlabels()
{
	resetcpilabel();
}

static Ctlidx
genlabel(Precode *code)
{
	Ctlidx idx;
	Ctl *ctl;

	idx = mklabel(code);
	ctl = idx2ctl(code, idx);
	ctl->ckind = Clabel;
	return idx;
}

static Ctlidx
genlabelpair(Precode *code, Ctlidx l1, Ctlidx l2)
{
	Ctlidx idx;
	Ctl *ctl;

	idx = mklabel(code);
	ctl = idx2ctl(code, idx);
	ctl->ckind = Clabelpair;
	ctl->l1 = l1;
	ctl->l2 = l2;
	return idx;
}

static void
emitlabel(Precode *code, Ctlidx idx)
{
	Ctl *ctl;
	ctl = idx2ctl(code, idx);
	if(ctl->ckind != Clabel)
		fatal("attempt to emit label pair");
	ctl->insn = code->ninsn;
}

static Precode*
mkprecode(char *id)
{
	Precode *code;
	code = (Precode*)malq(Qprecode, sizeof(Precode));
	code->id = mkcid0(id);
	code->ninsn = 0;
	code->maxinsn = InsnAlloc;
	code->insn = mkstrn(code->maxinsn*sizeof(Insn));
	code->nctl = 0;
	code->mctl = 128;
	code->ctl = mkstrn(code->mctl*sizeof(Ctl));
	code->src = mkvec(code->maxinsn);
	code->nlm = 0;
	code->mlm = 128;
	code->lm = mkstrn(code->mlm*sizeof(u64));
	code->ndbg = 0;
	code->mdbg = 128;
	code->dbg = mkstrn(code->mdbg*sizeof(Dbg));
	code->lex = mkvec(128);
	code->nlex = 0;
	code->rlex = -1;
	return code;
}

static Insn*
nextinsn(Precode *code, Src src)
{
	Insn *in;
	/* plan for one extra instruction for emitlabel */
	if(code->ninsn+1 >= code->maxinsn){
		code->insn = strrealloc(code->insn, 2*code->maxinsn*sizeof(Insn));
		code->src = vecrealloc(code->src, 2*code->maxinsn);
		code->maxinsn *= 2;
	}
	in = &((Insn*)strdata(code->insn))[code->ninsn];
	if(src == 0)
		src = mksrcfake("system code");
	vecdata(code->src)[code->ninsn] = mkvalvec(src);
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
	switch(OKIND(r->mode)){
	case Oreg:
		xprintf("%s", regtos(OIDX(r->mode)));
		break;
	case Oframe:
		if(OBOX(r->mode))
			xprintf("[");
		xprintf("%d(%s)", OIDX(r->mode), regtos(Rfp));
		if(OBOX(r->mode))
			xprintf("]");
		break;
	case Odisp:
		if(OBOX(r->mode))
			xprintf("[");
		xprintf("%d(%s)", OIDX(r->mode), regtos(Rcl));
		if(OBOX(r->mode))
			xprintf("]");
		break;
	case Otopl:
		xprintf("<%s>", ciddata(valcid(r->val)));
		break;
	case Otopr:
		xprintf("<%s>", ciddata(valcid(car(r->val))));
		break;
	case Oval:
		printval(r->val);
		break;
	case Oimm:
		xprintf("%lu", r->imm);
		break;
	case Onil:
		xprintf("nil");
		break;
	default:
		fatal("unknown operand kind %d", OKIND(r->mode));
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
	switch(OKIND(r->mode)){
	case Oval:
	case Otopl:
	case Otopr:
		setrelocobj(c, &r->val);
		break;
	default:
		break;
	}
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
setinsn(Precode *c)
{
	Ctl *ctl;
	Insn *i, *s, *t, *e;
	s = (Insn*)strdata(c->insn);
	e = s+c->ninsn;
	for(i = s; i < e; i++)
		switch(i->kind){
		case Ijmp:
		case Ijnz:
		case Ijz:
			ctl = idx2ctl(c, i->dstidx);
			t = s+ctl->insn;
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
	return c;
}

static Code*
mkvmcode(Precode *o, u32 nfree)
{
	Code *c;
	Imm n;

	setinsn(o);
	n = o->ninsn;
	c = mkcode(Cvm, trampsize+n*sizeof(Insn));
	c->id = o->id;
//	memcpy(codetramp(c), trampentry->buf, trampentry->n);
	memcpy(codeinsn(c), strdata(o->insn), o->ninsn*sizeof(Insn));
	c->lm = strrealloc(o->lm, o->nlm*sizeof(u64));
	c->dbg = strrealloc(o->dbg, o->ndbg*sizeof(Dbg));
	c->src = vecrealloc(o->src, n);
	if(o->nlex)
		c->lex = vecrealloc(o->lex, o->nlex);
	c->ndbg = o->ndbg;
	c->reloc = mkstrn(128);
	c->nreloc = 0;
	c->nfree = nfree;

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
reclabels(Expr *e, Precode *code, HT *ls)
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
		hputs(ls, id, strlen(id), (void*)(uptr)genlabel(code));
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
labels(Expr *e, Precode *code)
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
	rand->mode = OMODE(LOCIDX(loc->loc),
			   LOCBOX(loc->loc),
			   LOCKIND(loc->loc));
	switch(OKIND(rand->mode)){
	case Otopl:
	case Otopr:
		rand->val = loc->v;
		break;
	default:
		break;
	}
}

static void
randkon(Operand *rand, Val v)
{
	rand->mode = OMODE(0, 0, Oval);
	rand->val = v;
}

static void
randnil(Operand *rand)
{
	rand->mode = OMODE(0, 0, Onil);
}

static void
randimm(Operand *rand, Imm i)
{
	rand->mode = OMODE(0, 0, Oimm);
	rand->imm = i;
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
	if(deref && !v->box)
		bug();
	deref = deref && v->box;
	switch(v->where){
	case Vparam:
		rand->mode = OMODE(Oarg0+v->idx, deref, Oframe);
		break;
	case Vlocal:
		rand->mode = OMODE(Oarg0+f->narg+v->idx, deref, Oframe);
		break;
	case Vdisp:
		rand->mode = OMODE(v->idx, deref, Odisp);
		break;
	case Vtopl:
		rand->mode = OMODE(0, 0, Otopl);
		rand->val = mkvalcid(v->sym);
		break;
	case Vtopr:
		rand->mode = OMODE(0, 0, Otopr);
		rand->val = mkvalpair(v->kv);
		break;
	default:
		fatal("bug");
	}
}

static void
randframeloc(Operand *rand, unsigned idx)
{
	rand->mode = OMODE(idx, 0, Oframe);
}

static void
randdisploc(Operand *rand, unsigned idx, int deref)
{
	rand->mode = OMODE(idx, deref, Odisp);
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
cgjmp(Precode *code, CGEnv *p, Ctlidx ctl, Ctlidx nxt, Src src)
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
		i->dstidx = ctl;
	}
}

static int
returnlabel(CGEnv *p, Ctlidx ctl)
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

static Ctlidx
escapectl(Expr *e, CGEnv *p)
{
	Ctlidx rv;
	Expr *kind;

	rv = 0;
	kind = escaping(e);
	if(kind == 0)
		fatal("not an escaping expression");
	// FIXME: need to emit box init prologue; then enable goto in escaping
	// FIXME harder: is the above still relevant now that box init
	//               is emitted in cpv?
	else if(kind->kind == Egoto)
		rv = (Ctlidx)(uptr)hgets(p->labels, idsym(kind), strlen(idsym(kind)));
	else
		fatal("not an escaping expression");
//	if(rv == 0)
//		fatal("escaping expression has undefined target");
	return rv;
}

static void
cgbranch(Precode *code, CGEnv *p, Ctlidx idx, Ctlidx nxt, Src src)
{
	Insn *i;
	Ctlidx l1, l2;
	Ctl *ctl;

	ctl = idx2ctl(code, idx);
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
		i->dstidx = l1;
		cgjmp(code, p, l2, nxt, src);
	}else{
		i = nextinsn(code, src);
		i->kind = Ichkint;
		i = nextinsn(code, src);
		i->kind = Ijz;
		randloc(&i->op1, AC);
		i->dstidx = l2;
		cgjmp(code, p, l1, nxt, src);
	}
}

static void
cgctl(Precode *code, CGEnv *p, Ctlidx idx, Ctlidx nxt, Src src)
{
	Ctl *ctl;
	ctl = idx2ctl(code, idx);
	if(ctl->ckind == Clabel)
		cgjmp(code, p, idx, nxt, src);
	else if(ctl->ckind == Clabelpair)
		cgbranch(code, p, idx, nxt, src);
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
	switch(OKIND(r->mode)){
	case Oframe:
		if(!OBOX(r->mode))
			fset(f, OIDX(r->mode));
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
femit(Frame *f, Precode *c)
{
	Dbg *dbg;
	u32 nw;

	if(c->ndbg >= c->mdbg){
		c->dbg = strrealloc(c->dbg, 2*c->mdbg*sizeof(Dbg));
		c->mdbg *= 2;
	}
	dbg = &((Dbg*)strdata(c->dbg))[c->ndbg++];
	dbg->off = c->ninsn*sizeof(Insn);
	dbg->fsz = f->fsz;
	if(c->nlex == 0)
		dbg->lex = (u32)-1;
	else
		dbg->lex = c->nlex-1;
	if(f->fsz < mwbits-1)
		/* live mask fits in debug record */
		dbg->lm = (*f->l)&((1UL<<f->fsz)-1);
	else{
		/* live mask goes in live mask buffer */
		nw = roundup(f->fsz, mwbits)/mwbits;
		if(c->nlm+nw >= c->mlm){
			c->lm = strrealloc(c->lm, 2*c->mlm*sizeof(u64));
			c->mlm *= 2;
		}
		dbg->lm = (1ULL<<(mwbits-1))|c->nlm; /* offset into lm */
		memcpy(strdata(c->lm)+c->nlm, f->l, nw*sizeof(u64));
		c->nlm += nw;
	}
}

static void
bindvars(Xenv *xe, Var *v, unsigned n)
{
	unsigned m;
	for(m = 0; m < n; m++){
		xenvbind(xe, v->id, v);
		v++;
	}
}

static void
cntuservar(void *u, char *k, void *v)
{
	unsigned *nvp;
	nvp = u;
	if(k[0] != '$')
		(*nvp)++;
}

typedef
struct D
{
	List *l;
	Frame *f;
} D;

static Cid*
origid(char *id)
{
	char *p;
	p = strchr(id, '.');
	if(p == 0)
		return mkcid0(id);
	else
		return mkcid(id, p-id);
}

static void
adduservar(void *u, char *k, void *w)
{
	List *l, *e;
	D *d;
	Var *v;
	Frame *f;
	Cid *id;
	Cid *kind;
	Cval *idx;
	Cval *box;

	d = u;
	l = d->l;
	f = d->f;
	v = w;
	if(k[0] == '$')
		return;
	switch(v->where){
	case Vparam:
		id = origid(v->id);
		kind = mkcid0("arg");
		idx = mklitcval(Vuint, Oarg0+v->idx);
		box = mklitcval(Vuint, v->box);
		break;
	case Vlocal:
		id = origid(v->id);
		kind = mkcid0("local");
		idx = mklitcval(Vuint, Oarg0+f->narg+v->idx);
		box = mklitcval(Vuint, v->box);
		break;
	case Vdisp:
		id = origid(v->id);
		kind = mkcid0("free");
		idx = mklitcval(Vuint, v->idx);
		box = mklitcval(Vuint, v->box);
		break;
	case Vtopl:
	case Vtopr:
		return;
	default:
		bug();
	}

	e = mklist();
	_listappend(e, mkvalcid(id));
	_listappend(e, mkvalcid(kind));
	_listappend(e, mkvalcid(idx));
	_listappend(e, mkvalcid(box));
	_listappend(l, mkvallist(e));
}

static u32
pushlex(Precode *code, Xenv *lex, Frame *f)
{
	unsigned nv, m, rlex;
	List *l;
	D d;

	rlex = code->rlex;
	m = xenvsize(lex);
	if(m == 0)
		return rlex;
	nv = 0;
	xenvforeach(lex, cntuservar, &nv);
	if(nv == 0)
		return rlex;
	l = mklist();
	if(rlex == -1)
		_listappend(l, Xnil);
	else
		_listappend(l, mkvallitcval(Vuint, rlex));
	if(code->nlex >= veclen(code->lex))
		code->lex = vecrealloc(code->lex, 2*veclen(code->lex));
	code->rlex = code->nlex;
	d.l = l;
	d.f = f;
	xenvforeach(lex, adduservar, &d);
	_vecset(code->lex, code->nlex++, mkvallist(l));
	return rlex;
}

static void
poplex(Precode *code, u32 rlex)
{
	code->rlex = rlex;
}

static void
cg(Expr *e, Precode *code, CGEnv *p, Location *loc, Ctlidx ctl, Ctlidx nxt,
   Frame *f, Xenv *lex)
{
	Ctlidx L0, L, Lthen, Lelse, lpair;
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
	u32 rlex;

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
				cg(ep->e1, code, p, loc, ctl, nxt, f, lex);
				break;
			}
			L = genlabel(code);
			cg(ep->e1, code, p, Effect, L, L, f, lex);
			emitlabel(code, L);
			femit(f, code);
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
			L = genlabel(code);
			cg(e->e2, code, p, &dst, L, L, f, lex);
			emitlabel(code, L);
			femit(f, code);
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randloc(&i->op1, &dst);
			randloc(&i->dst, loc);
			fsetrand(f, &i->dst);
			femit(f, code);
			cgctl(code, p, ctl, nxt, e->src);
		}else
			cg(e->e2, code, p, &dst, ctl, nxt, f, lex);
		break;
	case E_tg:
	case Eg:
		varloc(f, &dst, e->e1->xp, 0);
		if(loc != Effect){
			L = genlabel(code);
			cg(e->e2, code, p, &dst, L, L, f, lex);
			emitlabel(code, L);
			femit(f, code);
			i = nextinsn(code, e->src);
			i->kind = Imov;
			randloc(&i->op1, &dst);
			randloc(&i->dst, loc);
			fsetrand(f, &i->dst);
			femit(f, code);
			cgctl(code, p, ctl, nxt, e->src);
		}else
			cg(e->e2, code, p, &dst, ctl, nxt, f, lex);
		break;
	case Euminus:
	case Eutwiddle:
	case Eunot:
	case Euplus:
		if(issimple(e->e1))
			cgrand(f, &r1, e->e1);
		else{
			L = genlabel(code);
			cg(e->e1, code, p, AC, L, L, f, lex);
			emitlabel(code, L);
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
			L = genlabel(code);
			cg(e->e2, code, p, AC, L, L, f, lex);
			emitlabel(code, L);
			femit(f, code);
			randloc(&r2, AC);
		}else if(issimple(e->e2)){
			L = genlabel(code);
			cg(e->e1, code, p, AC, L, L, f, lex);
			emitlabel(code, L);
			femit(f, code);
			randloc(&r1, AC);
			cgrand(f, &r2, e->e2);
		}else{
			L0 = genlabel(code);
			fpushlm(f);
			newloc(&dst, Lframe, f->tmp++, 0);
			randloc(&r1, &dst);
			cg(e->e1, code, p, &dst, L0, L0, f, lex);
			emitlabel(code, L0);
			femit(f, code);
			L = genlabel(code);
			cg(e->e2, code, p, AC, L, L, f, lex);
			f->tmp--;
			fpoplm(f);
			emitlabel(code, L);
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
		lex = mkxenv(lex);
		bindvars(lex, b->loc, b->nloc);
		rlex = pushlex(code, lex, f);
		fpushlm(f);
		cg(e->e2, code, p, loc, ctl, nxt, f, lex);
		fpoplm(f);
		poplex(code, rlex);
		freexenv(lex);
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
		for(m = 0, q = e->e2; m < narg; m++, q = q->e2){
			if(issimple(q->e1)){
				cgrand(f, &r1, q->e1);
				i = nextinsn(code, q->e1->src);
				i->kind = Imov;
				i->op1 = r1;
				randframeloc(&i->dst, nap+m);
			}else{
				L = genlabel(code);
				cg(q->e1, code, p, AC, L, L, f, lex);
				emitlabel(code, L);
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
		}

		if(issimple(e->e1)){
			cgrand(f, &r1, e->e1);
		}else {
			femit(f, code);
			L = genlabel(code);
			cg(e->e1, code, p, AC, L, L, f, lex);
			emitlabel(code, L);
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
			   FIXME: a block move might be nice */
			if(nap != Oarg0)
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
			cg(e->e1, code, p, AC, p->Return, nxt, f, lex);
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
				Lelse = genlabel(code);
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     Lelse);
				cg(e->e1, code, p, AC, lpair, Lelse, f, lex);
				emitlabel(code, Lelse);
				femit(f, code);
				cg(e->e3, code, p, loc, ctl,
				   nxt, f, lex);
			}else if(!escaping(e->e2) && escaping(e->e3)){
				Lthen = genlabel(code);
				lpair = genlabelpair(code, Lthen,
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, Lthen, f, lex);
				emitlabel(code, Lthen);
				femit(f, code);
				cg(e->e2, code, p, loc, ctl,
				   nxt, f, lex);
			}else if(escaping(e->e2) && escaping(e->e3)){
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     escapectl(e->e3, p));
				cg(e->e1, code, p, AC, lpair, nxt, f, lex);
			}else{
				Lthen = genlabel(code);
				Lelse = genlabel(code);
				lpair = genlabelpair(code, Lthen, Lelse);
				cg(e->e1, code, p, AC, lpair, Lthen, f, lex);
				emitlabel(code, Lthen);
				femit(f, code);
				cg(e->e2, code, p, loc, ctl,
				   Lelse, f, lex);
				emitlabel(code, Lelse);
				femit(f, code);
				cg(e->e3, code, p, loc, ctl,
				   nxt, f, lex);
			}
		}else{
			if(escaping(e->e2)){
				lpair = genlabelpair(code, escapectl(e->e2, p),
						     ctl);
				cg(e->e1, code, p, AC, lpair, nxt, f, lex);
			}else{
				Lthen = genlabel(code);
				lpair = genlabelpair(code, Lthen, ctl);
				cg(e->e1, code, p, AC, lpair, Lthen, f, lex);
				emitlabel(code, Lthen);
				femit(f, code);
				cg(e->e2, code, p, loc, ctl,
				   nxt, f, lex);
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
		L = (Ctlidx)(uptr)hgets(p->labels, idsym(e->e1), strlen(idsym(e->e1)));
//		if(L == 0)
//			fatal("goto bug");
		cgjmp(code, p, L, nxt, e->src);
		break;
	case Elabel:
		L = (Ctlidx)(uptr)hgets(p->labels, idsym(e->e1), strlen(idsym(e->e1)));
//		if(L == 0)
//			fatal("goto bug");
		emitlabel(code, L);
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
	Precode *ode;
	Code *code;
	Insn *i;
	CGEnv p;
	unsigned m;
	Src src;
	Frame f;
	unsigned long idx;
	Xenv *lex;
	u32 rlex;

	if(e->kind != Elambda)
		fatal("cglambda on non-lambda");
	l = (Lambda*)e->xp;

	if(id == 0)
		id = "anon";
	ode = mkprecode(id);

	finit(&f, l->narg, l->nloc, l->ntmp);
	fset(&f, Ocl);
	for(m = 0; m < l->narg; m++)
		fsetarg(&f, l->arg[m].idx);
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

	femit(&f, ode);

	p.Return = genlabel(ode);
	p.labels = labels(e->e2, ode);

	lex = mkxenv(0);
	bindvars(lex, l->arg, l->narg);
	bindvars(lex, l->disp, l->ndisp);
	rlex = pushlex(ode, lex, &f);
	cg(e->e2, ode, &p, AC, p.Return, p.Return, &f, lex);
	poplex(ode, rlex);
	freexenv(lex);

	/* finish Ichksp */
	i = &((Insn*)strdata(ode->insn))[idx];
	randimm(&i->op1, f.maxfsz);

	/* FIXME: is this redundant wrt p.Return?
	   (I bet not always; I bet we've tried this before.) */

	emitlabel(ode, p.Return);
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
	freevars(e);
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
	Insn *i;
	Precode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 0, 0, 0);
	fset(&f, Ocl);
	ode = mkprecode("$halt");
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Ihalt;
	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);
	ffini(&f);
	return cl;
}

Closure*
abortthunk(void)
{
	Insn *i;
	Precode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 0, 0, 0);
	fset(&f, Ocl);
	ode = mkprecode("$abort");
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Iabort;
	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);
	ffini(&f);
	return cl;
}

Closure*
mkkcapture(void)
{
	Insn *i;
	Precode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 1, 0, 0);
	fset(&f, Ocl);
	fsetarg(&f, 0);
	ode = mkprecode("kcapture");
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Ikg;

	/* one extra insn for ra2size in case Ikg throws an error */
	i = nextinsn(ode, 0);
	i->kind = Iabort;

	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);
	ffini(&f);
	return cl;
}

Closure*
mkapply(void)
{
	Insn *i;
	Precode *ode;
	Code *code;
	Frame f;

	/* expect at least two args */
	finit(&f, 2, 0, 0);
	fset(&f, Ocl);
	fsetarg(&f, 0);
	fsetarg(&f, 1);
	ode = mkprecode("apply");
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Iapply;

	/* one extra insn for ra2size in case Iapply throws an error */
	i = nextinsn(ode, 0);
	i->kind = Iabort;

	code = mkvmcode(ode, 0);
	ffini(&f);
	return mkcl(code, 0);
}

Closure*
stkunderflowthunk(void)
{
	Insn *i;
	Precode *ode;
	Code *code;
	Closure *cl;
	Frame f;

	finit(&f, 0, 0, 0);
	fset(&f, Ocl);
	ode = mkprecode("$stkunderflow");
	femit(&f, ode);
	i = nextinsn(ode, 0);
	i->kind = Iunderflow;

	/* one extra insn for ra2size in case Iunderflow throws an error */
	i = nextinsn(ode, 0);
	i->kind = Iabort;

	code = mkvmcode(ode, 0);
	cl = mkcl(code, 0);
	ffini(&f);
	return cl;
}

Closure*
mkraiseinterrupt(void)
{
	Insn *i;
	Precode *ode;
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

	ode = mkprecode("$raiseinterrupt");
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
	ffini(&f);
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
	Precode *ode;
	Code *code;
	Frame f;
	Imm nfp;

	finit(&f, 1, 0, 0);
	fset(&f, Ocl);
	ode = mkprecode("kresume");
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
	ffini(&f);
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
	CL = &toploc[m++];
	newloc(CL, Lreg, Rcl, 0);
	if(m != Maxtoploc)
		bug();
}

void
finicg(void)
{
}
