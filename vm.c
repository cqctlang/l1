#include "sys.h"
#include "util.h"
#include "syscqct.h"

char *qname[Qnkind] = {
	[Qnil]=		"nil",
	[Qas]=		"addressspace",
	[Qbox]=		"box",
	[Qcid]=		"cid",
	[Qcl]=		"closure",
	[Qcode]=	"code",
	[Qcont]=	"stack",
	[Qctype]=	"ctype",
	[Qcval]=	"cvalue",
	[Qdom]=		"domain",
	[Qexpr]=	"stx",
	[Qlist]=	"list",
	[Qns]=		"namespace",
	[Qpair]=	"pair",
	[Qprecode]=	"precode",
	[Qrange]=	"range",
	[Qrec]=		"record",
	[Qrd]=		"recorddescriptor",
	[Qstr]=		"string",
	[Qtab]=		"table",
	[Qvec]=		"vector",
};

unsigned isfloat[Vnallbase] = {
	[Vfloat] = 1,
	[Vdouble] = 1,
	[Vlongdouble] = 1,
	[Vcomplex] = 1,
	[Vdoublex] = 1,
	[Vlongdoublex] = 1,
};

unsigned isunsigned[Vnallbase] = {
	[Vbool] = 1,
	[Vuchar] = 1,
	[Vushort] = 1,
	[Vuint] = 1,
	[Vulong] = 1,
	[Vuvlong] = 1,
};

static unsigned isbigendian[Rnrep] = {
	[Ru08be]=	1,
	[Ru16be]=	1,
	[Ru32be]=	1,
	[Ru64be]=	1,
	[Rs08be]=	1,
	[Rs16be]=	1,
	[Rs32be]=	1,
	[Rs64be]=	1,
};

static unsigned isrelop[Iopmax+1] = {
	[Icmpeq]=	1,
	[Icmpneq]=	1,
	[Icmpgt]=	1,
	[Icmpge]=	1,
	[Icmplt]=	1,
	[Icmple]=	1,
};

static Ctype* dolooktype(VM *vm, Ctype *t, Ns *ns);
static Env mktopenv(void);
static void l1_sort(VM *vm, Imm argc, Val *argv, Val *rv);
static Cont* kcapture(VM *vm);
static void checkoverflow(VM *vm, unsigned m);

/* global roots */
Val Xnil;
Val syms;
Val typecache;
Dom *litdom;
Closure *halt, *vabort, *stkunderflow;
Cval *cvalnull, *cval0, *cval1, *cvalminus1;

VM *vms[1];			/* active VMs (at most one) */
u32 nvm;			/* number of active VMs */

static int lasterrno;

static char *opstr[Iopmax+1] = {
	[Iadd] = "+",
	[Iand] = "&",
	[Idiv] = "/",
	[Iinv] = "~",
	[Imod] = "%",
	[Imul] = "*",
	[Ineg] = "-",
	[Inot] = "!",
	[Ior] = "|",
	[Ishl] = "<<",
	[Ishr] = ">>",
	[Isub] = "-",
	[Icmpeq] = "==",
	[Icmpneq] = "!=",
	[Icmpgt] = ">",
	[Icmpge] = ">=",
	[Icmplt] = "<",
	[Icmple] = "<=",
};

static u32 hashptr(Val);
static u32 hashrange(Range *r);
static int eqptr(Val, Val);
static int equalrange(Range *ra, Range *rb);

void
setlasterrno(int no)
{
	lasterrno = no;
}

int
getlasterrno()
{
	return lasterrno;
}

Imm
valimm(Val v)
{
	Cval *cv;
	if(Vkind(v) != Qcval)
		fatal("valimm on non-cval");
	cv = (Cval*)v;
	return cvalu(cv);
}

Val
valboxed(Val v)
{
	Box *b;
	if(Vkind(v) != Qbox)
		fatal("valboxed on non-box");
	b = (Box*)v;
	return b->v;
}

Head*
valhead(Val v)
{
	if(v == 0)
		return 0;
	switch(Vkind(v)) {
	case Qnil:
		return 0;
		break;
	default:
		return (Head*)v;
		break;
	}
}

Closure*
mkcl(Code *code, unsigned len)
{
	Closure *cl;
	cl = (Closure*)malv(Qcl, sizeof(Closure)+len*sizeof(Val));
	cl->code = code;
	cl->dlen = len;
	return cl;
}

Closure*
mkcfn(char *id, Cfn *cfn)
{
	Code *code;
	code = mkcode(Ccfn, trampsize);
//	memcpy(codetramp(code), trampentry->buf, trampentry->n);
	code->cfn = cfn;
	code->id = mkcid0(id);
	return mkcl(code, 0);
}

Closure*
cqctmkcfn(char *id, Cfn *cfn)
{
	return mkcfn(id, cfn);
}

Closure*
mkccl(char *id, Ccl *ccl, unsigned dlen, ...)
{
	Code *code;
	Closure *cl;
	va_list args;
	Val vp;
	unsigned m;

	code = mkcode(Cccl, trampsize);
//	memcpy(codetramp(code), trampentry->buf, trampentry->n);
	code->ccl = ccl;
	code->id = mkcid0(id);
	cl = mkcl(code, dlen);
	va_start(args, dlen);
	for(m = 0; m < dlen; m++) {
		vp = va_arg(args, Val);
		cldisp(cl)[m] = vp;
	}
	va_end(args);
	return cl;
}

static Cont*
mkcont(void *base, u32 sz, void *ra, Closure *cl, Cont *link,
       u32 level, u64 gen)
{
	Cont *k;
	k = (Cont*)malq(Qcont, sizeof(Cont));
	k->base = base;
	k->sz = sz;
	k->ra = ra;
	k->cl = cl;
	k->link = link;
	k->level = level;
	k->gen = gen;
	return k;
}

int
eqval(Val v1, Val v2)
{
	if(Vkind(v1) != Vkind(v2))
		return 0;
	switch(Vkind(v1)) {
	case Qnil:
		return 1;
	case Qas:
	case Qbox:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qns:
	case Qrd:
	case Qtab:
	case Qexpr:
	case Qctype:
	case Qlist:
	case Qpair:
	case Qrange:
	case Qrec:
	case Qstr:
	case Qvec:
	case Qprecode:
	case Qcode:
	case Qcont:
		return eqptr(v1, v2);
	case Qcval:
		return eqvcval(valcval(v1), valcval(v2));
	}
	bug();
}

u32
hashqval(Val v)
{
	switch(Vkind(v)) {
	case Qnil:
		return hashp(Xnil);
	case Qas:
	case Qbox:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qns:
	case Qrd:
	case Qtab:
	case Qexpr:
	case Qctype:
	case Qlist:
	case Qpair:
	case Qrange:
	case Qrec:
	case Qstr:
	case Qvec:
	case Qprecode:
	case Qcode:
	case Qcont:
		return hashptr(v);
	case Qcval:
		return hashqvcval(valcval(v));
	}
	bug();
}

int
eqvval(Val v1, Val v2)
{
	if(Vkind(v1) != Vkind(v2))
		return 0;
	switch(Vkind(v1)) {
	case Qnil:
		return 1;
	case Qas:
	case Qbox:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qns:
	case Qrd:
	case Qtab:
	case Qprecode:
	case Qcode:
	case Qcont:
		return eqptr(v1, v2);
	case Qexpr:
		return eqvstx(valexpr(v1), valexpr(v2));		
	case Qctype:
		return eqvctype(valctype(v1), valctype(v2));
	case Qcval:
		return eqvcval(valcval(v1), valcval(v2));
	case Qlist:
		return equallist(vallist(v1), vallist(v2));
	case Qpair:
		return equalpair(valpair(v1), valpair(v2));
	case Qrange:
		return equalrange(valrange(v1), valrange(v2));
	case Qrec:
		return equalrec(valrec(v1), valrec(v2));
	case Qstr:
		return equalstr(valstr(v1), valstr(v2));
	case Qvec:
		return equalvec(valvec(v1), valvec(v2));
	}
	bug();
}

u32
hashqvval(Val v)
{
	switch(Vkind(v)) {
	case Qnil:
		return hashp(Xnil);
	case Qas:
	case Qbox:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qns:
	case Qrd:
	case Qtab:
	case Qprecode:
	case Qcode:
	case Qcont:
		return hashptr(v);
	case Qexpr:
		return hashqvstx(valexpr(v));
	case Qctype:
		return hashqvctype(valctype(v));
	case Qcval:
		return hashqvcval(valcval(v));
	case Qlist:
		return hashlist(vallist(v));
	case Qpair:
		return hashpair(valpair(v));
	case Qrange:
		return hashrange(valrange(v));
	case Qrec:
		return hashrec(valrec(v));
	case Qstr:
		return hashstr(valstr(v));
	case Qvec:
		return hashvec(valvec(v));
	}
	bug();
}

int
equalval(Val v1, Val v2)
{
	if(Vkind(v1) != Vkind(v2))
		return 0;
	switch(Vkind(v1)) {
	case Qnil:
		return 1;
	case Qas:
	case Qbox:		/* FIXME: be structural? */
	case Qcid:
	case Qcl:
	case Qdom:
	case Qns:
	case Qrd:
	case Qtab:
	case Qprecode:
	case Qcode:
	case Qcont:
		return eqptr(v1, v2);
	case Qexpr:
		return equalstx(valexpr(v1), valexpr(v2));
	case Qctype:
		return equalctype(valctype(v1), valctype(v2));
	case Qcval:
		return equalcval(valcval(v1), valcval(v2));
	case Qlist:
		return equallist(vallist(v1), vallist(v2));
	case Qpair:
		return equalpair(valpair(v1), valpair(v2));
	case Qrange:
		return equalrange(valrange(v1), valrange(v2));
	case Qrec:
		return equalrec(valrec(v1), valrec(v2));
	case Qstr:
		return equalstr(valstr(v1), valstr(v2));
	case Qvec:
		return equalvec(valvec(v1), valvec(v2));
	}
	bug();
}

u32
hashval(Val v)
{
	switch(Vkind(v)) {
	case Qnil:
		return hashp(Xnil);
	case Qas:
	case Qbox:		/* FIXME: be structural? */
	case Qcid:
	case Qcl:
	case Qdom:
	case Qns:
	case Qrd:
	case Qtab:
	case Qprecode:
	case Qcode:
	case Qcont:
		return hashptr(v);
	case Qexpr:
		return hashstx(valexpr(v));
	case Qctype:
		return hashctype(valctype(v));
	case Qcval:
		return hashcval(valcval(v));
	case Qlist:
		return hashlist(vallist(v));
	case Qpair:
		return hashpair(valpair(v));
	case Qrange:
		return hashrange(valrange(v));
	case Qrec:
		return hashrec(valrec(v));
	case Qstr:
		return hashstr(valstr(v));
	case Qvec:
		return hashvec(valvec(v));
	}
	bug();
}

static u32
hashptr(Val val)
{
	return hashp(valhead(val));
}

static int
eqptr(Val a, Val b)
{
	return valhead(a)==valhead(b);
}

static u32
hashrange(Range *r)
{
	return hashx(hashu64(cvalu(r->beg)), hashu64(cvalu(r->len)));
}

static int
equalrange(Range *ra, Range *rb)
{
	return cvalu(ra->beg)==cvalu(rb->beg) && cvalu(ra->len)==cvalu(rb->len);
}

static As*
mkas(void)
{
	As *as;
	as = (As*)malq(Qas, sizeof(As));
	return as;
}

Dom*
mkdom(Ns *ns, As *as, Str *name)
{
	Dom *dom;
	dom = (Dom*)malq(Qdom, sizeof(Dom));
	dom->ns = ns;
	dom->as = as;
	dom->name = name;
	return dom;
}

static Ns*
mkns(void)
{
	Ns *ns;
	ns = (Ns*)malq(Qns, sizeof(Ns));
	return ns;
}

static Env
mkenv(void)
{
	return mktab();
}

int
envbinds(Env env, Cid *id)
{
	if(tabget(env, mkvalcid(id)))
		return 1;
	else
		return 0;
}

void
envdefine(Env env, Cid *id, Val v)
{
	if(envbinds(env, id))
		bug();
	tabput(env, mkvalcid(id), v);
}

Pair*
envgetkv(Env env, Cid *id)
{
	return tabgetkv(env, mkvalcid(id));
}

Val
envget(Env env, Cid *id)
{
	return tabget(env, mkvalcid(id));
}

void
envput(Env env, Cid *id, Val v)
{
	tabput(env, mkvalcid(id), v);
}

void
envbind(Env env, char *id, Val val)
{
	Cid *cid;
	cid = mkcid0(id);
	if(envbinds(env, cid)) {
		xprintf("warning: rebinding %s via envbind\n", id);
		envput(env, cid, val);
	}else
		envdefine(env, cid, val);
}

void
envdel(Env env, char *id)
{
	tabdel(env, mkvalcid(mkcid0(id)));
}

static Val
envlookup(Env env, char *id)
{
	return envget(env, mkcid0(id));
}

Env
mktoplevel()
{
	return mktopenv();
}

Cval*
mkcval(Dom *dom, Ctype *type, Imm val)
{
	Cval *cv;
	cv = (Cval*)malq(Qcval, sizeof(Cval));
	cv->dom = dom;
	cv->type = type;
	cvalu(cv) = val;
	return cv;
}

Cval*
mkcvalenc(Dom *dom, Ctype *type, Enc v)
{
	Cval *cv;
	cv = (Cval*)malq(Qcval, sizeof(Cval));
	cv->dom = dom;
	cv->type = type;
	cv->v = v;
	return cv;
}

Cval*
mklitcval(Cbase base, Imm val)
{
	return mkcval(litdom, litdom->ns->base[base], val);
}

Val
mkvalcval(Dom *dom, Ctype *t, Imm imm)
{
	return (Val)mkcval(dom, t, imm);
}

Val
mkvallitcval(Cbase base, Imm imm)
{
	return (Val)mklitcval(base, imm);
}

Val
mkvallitcvalenc(Cbase base, Enc v)
{
	return (Val)mkcvalenc(litdom, litdom->ns->base[base], v);
}

Val
mkvalcval2(Cval *cv)
{
	return (Val)cv;
}

Val
mkvalbox(Val boxed)
{
	Box *box;
	box = (Box*)malbox();
	box->v = boxed;
	return (Val)box;
}

Range*
mkrange(Cval *beg, Cval *len)
{
	Range *r;
	r = (Range*)malq(Qrange, sizeof(Range));
	r->beg = beg;
	r->len = len;
	return r;
}

static Val
mkvalrange2(Cval *beg, Cval *len)
{
	Range *r;
	r = mkrange(beg, len);
	return (Val)r;
}

static void
putbox(Val box, Val boxed)
{
	Box *b;
	gcwb(box);
	b = (Box*)box;
	b->v = boxed;
}

static void
kbacktrace(VM *vm, Cont *k)
{
	Val *fp;
	Insn *pc;
	Closure *cl;
	Code *cp;
	u64 sz;

	if(k == 0)
		return;
	if(k->base == 0)
		/* defunct leftover in restored heap */
		return;
	pc = k->ra;
	cl = k->cl;
	fp = k->base+k->sz;
	while((void*)fp >= k->base) {
		cp = cl->code;
		sz = ra2size(pc, cp);
		printframe(vm, pc, cp);
		fp -= sz;
		if((void*)fp >= k->base) {
			pc = stkp(fp[Ora]);
			cl = valcl(fp[Ocl]);
		}
	}
	kbacktrace(vm, k->link);
}

static List*
stackframes(VM *vm, Cont *k)
{
	List *l;
	Val *fp;
	Insn *pc;
	Closure *cl;
	Code *cp;
	u64 sz;

	l = mklist();
	while(1) {
		if(k == 0)
			return l;
		if(k->base == 0)
			/* defunct leftover in restored heap */
			return l;
		pc = k->ra;
		cl = k->cl;
		fp = k->base+k->sz;
		while((void*)fp >= k->base) {
			cp = cl->code;
			sz = ra2size(pc, cp);
			_listappend(l, mkvallist(frameinfo(cl, pc)));
			fp -= sz;
			if((void*)fp >= k->base) {
				pc = stkp(fp[Ora]);
				cl = valcl(fp[Ocl]);
			}
		}
		k = k->link;
	}
}

static Vec*
liveinframe(VM *vm, Cont *k, u32 fno)
{
	Val *fp, *lp;
	Insn *pc;
	Closure *cl;
	Code *cp;
	u64 i, sz, lm, *mp, o;
	u32 fcnt;
	Vec *v;

	fcnt = 0;
	while(1) {
		if(k == 0)
			vmerr(vm, "no such frame");
		if(k->base == 0)
			/* defunct leftover in restored heap */
			vmerr(vm, "defunct stack");
		pc = k->ra;
		cl = k->cl;
		fp = k->base+k->sz;
		while((void*)fp >= k->base) {
			cp = cl->code;
			sz = ra2size(pc, cp);
			fp -= sz;
			if(fcnt == fno) {
				lm = ra2mask(pc, cp);
				v = mkvecinit(sz, Xnil);

				/* FIXME: merge with identical code in mem */
				lp = fp;
				if((lm>>(mwbits-1))&1) {
					o = lm&~(1ULL<<(mwbits-1));
					mp = (u64*)strdata(cp->lm)+o;
					while(sz > 0) {
						lm = *mp++;
						for(i = 0;
						    sz > 0 && i < mwbits;
						    i++, sz--, lp++)
							if((lm>>i)&1)
								_vecset(v, i, *lp);
					}
				}else
					for(i = 0; i < sz; i++, lp++)
						if((lm>>i)&1)
							_vecset(v, i, *lp);
				return v;
			}
			if((void*)fp >= k->base) {
				pc = stkp(fp[Ora]);
				cl = valcl(fp[Ocl]);
			}
			fcnt++;
		}
		k = k->link;
	}
}

static void
vmint(VM *vm)
{
	Val v;
	Closure *cl;
	Imm fsz;

	v = envlookup(vm->top, "$raiseinterrupt");
	if(v == 0)
		bug();
	if(Vkind(v) != Qcl)
		bug();
	cl = valcl(v);
	v = envlookup(vm->top, "interrupthandler");
	if(v == 0)
		vmerr(vm, "no default interrupt handler");
	if(Vkind(v) != Qcl)
		vmerr(vm, "default interrupt handler is not a function");

	/* $raiseinterrupt expects the following stack:

	  ofp->	ra0
	        cl0
	   	locals
           fp->	ra1
	   	cl1
	   	fsz
	   	v

           the depicted stack grows down.
	   the frame labeled by ofp is that of the interrupted activation.
	   e.g., ra0/cl0 is the return address for that call.
	   ra1/cl1 is one past the interrupt insn.
	   fsz is the size of the ofp frame at that insn.
	   it is passed to $raiseinterrupt so that it can restore ofp.
	   v is the user's interrupt handler.
	*/

	vm->flags &= ~VMirq;
	fsz = ra2size(vm->pc, vm->cl->code);
	vm->fp += fsz;
	vm->fp[Ora] = (Val)(uptr)vm->pc;
	vm->fp[Ocl] = mkvalcl(vm->cl);
	vm->fp[Oarg0] = (Val)(uptr)fsz;
	vm->fp[Oarg0+1] = v;
	vm->pc = codeentry(cl->code);
	vm->cl = cl;
}

static void
vvmwarn(VM *vm, char *fmt, va_list args)
{
	static char buf[4096];
	Val argv[1];
	Val err;
	vsnprint(buf, sizeof(buf), fmt, args);
	argv[0] = mkvalstr(mkstr0(buf));
	err = envlookup(vm->top, "currentwarning");
	if(err == 0)
		fatal("no warning handler");
	ccall(vm, valcl(err), 1, argv);
}

void
vmwarn(VM *vm, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vvmwarn(vm, fmt, args);
	va_end(args);
}

static void
vvmerr(VM *vm, char *fmt, va_list args)
{
	static char buf[4096];
	Val argv[1];
	Val err;
	vsnprint(buf, sizeof(buf), fmt, args);
	argv[0] = mkvalstr(mkstr0(buf));
	err = envlookup(vm->top, "errorhandler");
	if(err == 0)
		fatal("no default error handler");
	ccall(vm, valcl(err), 1, argv);
	fatal("return from default error handler");
}

void
vmerr(VM *vm, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vvmerr(vm, fmt, args);
	va_end(args);
	abort();
}

void
cqctvmerr(VM *vm, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vvmerr(vm, fmt, args);
	va_end(args);
}

static Val
getvalrand(VM *vm, Operand *r)
{
	Val v;
	switch(OKIND(r->mode)) {
	case Oreg:
		switch(OIDX(r->mode)) {
		case Rac:
			return vm->ac;
		case Rfp:
			return (Val)(uptr)vm->fp;
		case Rcl:
			return mkvalcl(vm->cl);
		case Rvc:
		case Rpc:
		default:
			bug();
		}
	case Oframe:
		v = vm->fp[OIDX(r->mode)];
		if(OBOX(r->mode))
			return valboxed(v);
		else
			return v;
	case Odisp:
		v = cldisp(vm->cl)[OIDX(r->mode)];
		if(OBOX(r->mode))
			return valboxed(v);
		else
			return v;
	case Otopl:
		v = envget(vm->top, valcid(r->val));
		if(v == 0)
			vmerr(vm, "reference to unbound variable: %s",
			      ciddata(valcid(r->val)));
		return v;
	case Otopr:
		return cdr(r->val);
	case Oval:
		/* copy mutable values */
		switch(Vkind(r->val)) {
		case Qstr:
			return mkvalstr(strcopy(valstr(r->val)));
		default:
			return r->val;
		}
	case Onil:
		return Xnil;
	case Oimm:
	default:
		bug();
	}
}

static Imm
getimmrand(Operand *r)
{
	switch(OKIND(r->mode)) {
	case Oimm:
		return r->imm;
	default:
		bug();
	}
}


static void
putvalrand(VM *vm, Val v, Operand *r)
{
	Val *dst;

	switch(OKIND(r->mode)) {
	case Oreg:
		switch(OIDX(r->mode)) {
		case Rac:
			vm->ac = v;
			break;
		case Rcl:
			vm->cl = valcl(v);
			break;
		case Rfp:
		case Rvc:
		case Rpc:
		default:
			bug();
		}
		break;
	case Oframe:
		dst = vm->fp+OIDX(r->mode);
		if(OBOX(r->mode))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Odisp:
		dst = &cldisp(vm->cl)[OIDX(r->mode)];
		if(OBOX(r->mode))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Otopl:
		envput(vm->top, valcid(r->val), v);
		break;
	case Otopr:
		setcdr(r->val, v);
		break;
	default:
		fatal("bug");
	}
}

static Imm
getbeint(char *s, unsigned nb)
{
	unsigned i;
	Imm w;
	w = 0;
	for(i = 0; i < nb; i++) {
		w <<= 8;
		w |= (s[i]&0xff);
	}
	return w;
}

// FIXME: don't assume little-endian
static Enc
str2enc(VM *vm, Ctype *t, Str *str)
{
	Enc v;
	char *s;

	s = strdata(str);
	switch(typerep(chasetype(t))) {
	case Rs08le:
		v.u = *(s8*)s;
		break;
	case Rs16le:
		v.u = *(s16*)s;
		break;
	case Rs32le:
		v.u = *(s32*)s;
		break;
	case Rs64le:
		v.u = *(s64*)s;
		break;
	case Ru08le:
		v.u = *(u8*)s;
		break;
	case Ru16le:
		v.u = *(u16*)s;
		break;
	case Ru32le:
		v.u = *(u32*)s;
		break;
	case Ru64le:
		v.u = *(u64*)s;
		break;
	case Ru08be:
		v.u = (u8)getbeint(s, 1);
		break;
	case Ru16be:
		v.u = (u16)getbeint(s, 2);
		break;
	case Ru32be:
		v.u = (u32)getbeint(s, 4);
		break;
	case Ru64be:
		v.u = (u64)getbeint(s, 8);
		break;
	case Rs08be:
		v.u = (s8)getbeint(s, 1);
		break;
	case Rs16be:
		v.u = (s16)getbeint(s, 2);
		break;
	case Rs32be:
		v.u = (s32)getbeint(s, 4);
		break;
	case Rs64be:
		v.u = (s64)getbeint(s, 8);
		break;
	case Rf32:
		v.f = *(f32*)s;
		break;
	case Rf64:
		v.d = *(f64*)s;
		break;
	default:
		vmerr(vm, "attempt to access memory with incomplete type");
	}
	return v;
}

static void
putbeint(char *p, Imm w, unsigned nb)
{
	int i;
	for(i = nb-1; i >= 0; i--) {
		p[i] = w&0xff;
		w >>= 8;
	}
}

static Str*
enc2str(VM *vm, Ctype *t, Enc v)
{
	Str *str;
	char *s;

	switch(typerep(chasetype(t))) {
	case Rs08le:
		str = mkstrn(sizeof(s8));
		s = strdata(str);
		*(s8*)s = (s8)v.u;
		return str;
	case Rs16le:
		str = mkstrn(sizeof(s16));
		s = strdata(str);
		*(s16*)s = (s16)v.u;
		return str;
	case Rs32le:
		str = mkstrn(sizeof(s32));
		s = strdata(str);
		*(s32*)s = (s32)v.u;
		return str;
	case Rs64le:
		str = mkstrn(sizeof(s64));
		s = strdata(str);
		*(s64*)s = (s64)v.u;
		return str;
	case Ru08le:
		str = mkstrn(sizeof(u8));
		s = strdata(str);
		*(u8*)s = (u8)v.u;
		return str;
	case Ru16le:
		str = mkstrn(sizeof(u16));
		s = strdata(str);
		*(u16*)s = (u16)v.u;
		return str;
	case Ru32le:
		str = mkstrn(sizeof(u32));
		s = strdata(str);
		*(u32*)s = (u32)v.u;
		return str;
	case Ru64le:
		str = mkstrn(sizeof(u64));
		s = strdata(str);
		*(u64*)s = (u64)v.u;
		return str;
	case Rs08be:
		str = mkstrn(sizeof(s8));
		s = strdata(str);
		putbeint(s, v.u, 1);
		return str;
	case Rs16be:
		str = mkstrn(sizeof(s16));
		s = strdata(str);
		putbeint(s, v.u, 2);
		return str;
	case Rs32be:
		str = mkstrn(sizeof(s32));
		s = strdata(str);
		putbeint(s, v.u, 4);
		return str;
	case Rs64be:
		str = mkstrn(sizeof(s64));
		s = strdata(str);
		putbeint(s, v.u, 8);
		return str;
	case Ru08be:
		str = mkstrn(sizeof(u8));
		s = strdata(str);
		putbeint(s, v.u, 1);
		return str;
	case Ru16be:
		str = mkstrn(sizeof(u16));
		s = strdata(str);
		putbeint(s, v.u, 2);
		return str;
	case Ru32be:
		str = mkstrn(sizeof(u32));
		s = strdata(str);
		putbeint(s, v.u, 4);
		return str;
	case Ru64be:
		str = mkstrn(sizeof(u64));
		s = strdata(str);
		putbeint(s, v.u, 8);
		return str;
	case Rf32:
		str = mkstrn(sizeof(f32));
		s = strdata(str);
		*(f32*)s = v.f;
		return str;
	case Rf64:
		str = mkstrn(sizeof(f64));
		s = strdata(str);
		*(f64*)s = v.d;
		return str;
	case Rundef:
		vmerr(vm, "attempt to access memory with incomplete type");
	default:
		bug();
	}
}

/* transform representation of VAL used for type OLD to type NEW.
   OLD and NEW must be chased types with defined representations. */
static Enc
_rerep(Enc v, Ctype *old, Ctype *new)
{
	/* FIXME: non-trivial cases are : real <-> int,
	   real <-> alternate real
	   integer truncation
	   (so div and shr work)
	*/
	switch((typerep(new)<<5)|typerep(old)) {
		#include "rerep.switch" /* re-cast val */
	}
 	return v;
}

static Enc
rerep(Enc v, Ctype *old, Ctype *new)
{
	old = chasetype(old);
	new = chasetype(new);
	if(typerep(old) == Rundef || typerep(new) == Rundef)
		fatal("undef!");
	return _rerep(v, old, new);
}

static Ctype*
fixrep(Ctype *t, Rkind rep)
{
	Ctypeenum *te;
	switch(t->tkind) {
	case Ttypedef:
		return mkctypedef(typetid(t), fixrep(subtype(t), rep));
	case Tenum:
		te = (Ctypeenum*)t;
		return mkctypeenum(typetag(t),
				   fixrep(subtype(t), rep),
				   te->konst);
	case Tptr:
		return mkctypeptr(subtype(t), rep);
	default:
		bug();
	}
}

static int
isvoidstar(Ctype *t)
{
	if(t->tkind != Tptr)
		return 0;
	t = subtype(t);
	if(t->tkind != Tvoid)
		return 0;
	return 1;
}

Cval*
typecast(VM *vm, Ctype *t, Cval *cv)
{
	Ctype *old, *new;
	old = chasetype(cv->type);
	new = chasetype(t);
	if(typerep(new) == Rundef) {
		/* if new is void* (or alias), steal
		 * pointer representation from old */
		if(typerep(old) == Rundef)
			bug();
		if(old->tkind != Tptr || !isvoidstar(new))
			vmerr(vm, "attempt to cast to undefined type");
		t = fixrep(t, typerep(old));
		return mkcvalenc(cv->dom, t, rerep(cvalenc(cv), old, t));
	}else
		return mkcvalenc(cv->dom, t, _rerep(cvalenc(cv), old, new));
}

static Cval*
domcastbase(VM *vm, Dom *dom, Cval *cv)
{
	Ctype *t, *old, *new;
	Str *es;

	t = cv->type;
	if(dom == litdom)
		t = chasetype(t);
	if(t->tkind != Tbase)
		vmerr(vm, "operand must be of base type");

	/* FIXME: do we really want to lookup the type in the new domain? */
	t = dom->ns->base[typecbase(t)];
	if(t == 0) {
		es = fmtctype(cv->type);
		vmerr(vm, "cast to domain that does not define %.*s",
		      (int)es->len, strdata(es));
	}
	old = chasetype(cv->type);
	new = chasetype(t);
	if(typerep(old) == Rundef || typerep(new) == Rundef)
		vmerr(vm, " attempt to cast to type "
		      "with undefined representation");
	return mkcvalenc(dom, t, _rerep(cvalenc(cv), old, new));
}

Cval*
domcast(VM *vm, Dom *dom, Cval *cv)
{
	Ctype *t, *old, *new;
	Str *es;

	/* FIXME: do we really want to lookup the type in the new domain? */
	if(dom == litdom)
		t = dolooktype(vm, chasetype(cv->type), dom->ns);
	else
		t = dolooktype(vm, cv->type, dom->ns);
	if(t == 0) {
		es = fmtctype(cv->type);
		vmerr(vm, "cast to domain that does not define %.*s",
		      (int)es->len, strdata(es));
	}
	old = chasetype(cv->type);
	new = chasetype(t);
	if(typerep(old) == Rundef || typerep(new) == Rundef)
		vmerr(vm, " attempt to cast to type "
		      "with undefined representation");
	return mkcvalenc(dom, t, _rerep(cvalenc(cv), old, new));
}

static void
dompromote(VM *vm, ikind op, Cval *op1, Cval *op2, Cval **rv1, Cval **rv2)
{
	Ctype *b1, *b2;
	static char *domerr
		= "attempt to combine cvalues of incompatible domains";

	if(op1->dom == op2->dom)
		goto out;

	if(op1->dom == litdom)
		op1 = domcast(vm, op2->dom, op1);
	else if(op2->dom == litdom)
		op2 = domcast(vm, op1->dom, op2);
	else{
		b1 = chasetype(op1->type);
		b2 = chasetype(op2->type);
		if(b1->tkind != Tptr && b2->tkind != Tptr) {
			op1 = domcastbase(vm, litdom, op1);
			op2 = domcastbase(vm, litdom, op2);
		}else if(b1->tkind == Tptr && b2->tkind == Tptr) {
			if(!isrelop[op])
				vmerr(vm, domerr);
			op1 = domcast(vm, litdom,
				      typecast(vm, op1->dom->ns->base[Vptr],
					       op1));
			op2 = domcast(vm, litdom,
				      typecast(vm, op2->dom->ns->base[Vptr],
					       op2));
		}else if(b1->tkind == Tptr) {
			if(op == Iadd || op == Isub || isrelop[op])
				op2 = domcast(vm, op1->dom, op2);
			else
				vmerr(vm, domerr);
		}else /* b2->tkind == Tptr */ {
			if(op == Iadd || op == Isub || isrelop[op])
				op1 = domcast(vm, op2->dom, op1);
			else
				vmerr(vm, domerr);
		}
	}
out:
	*rv1 = op1;
	*rv2 = op2;
}

static Cval*
intpromote(VM *vm, Cval *cv)
{
	Ctype *base;

	base = chasetype(cv->type);
	if(base->tkind != Tbase)
		return cv;
	switch(typecbase(base)) {
	case Vbool: // FIXME: is this case right?
	case Vuchar:
	case Vushort:
	case Vchar:
	case Vshort:
		return typecast(vm, cv->dom->ns->base[Vint], cv);
	default:
		break;
	}
	return cv;
}

static Ctype*
commontype(Ctype *t1, Ctype *t2)
{
	Ctype *p1, *p2;
	p1 = t1;
	while(1) {
		p2 = t2;
		while(1) {
			if(equalctype(p1, p2))
				return p1;
			if(p2->tkind != Ttypedef && p2->tkind != Tenum)
				break;
			p2 = subtype(p2);
		}
		if(p1->tkind != Ttypedef && p1->tkind != Tenum)
			break;
		p1 = subtype(p1);
	}
	fatal("no common type");
}

static void
usualconvs(VM *vm, Cval *op1, Cval *op2, Cval **rv1, Cval **rv2)
{
	/* FIXME: why assign ranks to types below int promotion? */
	static unsigned rank[Vnbase] = {
		[Vbool] = 0,
		[Vchar] = 1,
		[Vuchar] = 1,
		[Vshort] = 2,
		[Vushort] = 2,
		[Vint] = 3,
		[Vuint] = 3,
		[Vlong] = 4,
		[Vulong] = 4,
		[Vvlong] = 5,
		[Vuvlong] = 5,
		[Vfloat] = 6,
		[Vdouble] = 7,
		/* FIXME: complex */
	};
	static unsigned uvariant[Vnbase] = {
		[Vchar] = Vuchar,
		[Vshort] = Vushort,
		[Vint] = Vuint,
		[Vlong] = Vulong,
		[Vvlong] = Vuvlong,
	};
	Cbase c1, c2, nc;
	Rkind r1, r2;
	Ctype *t1, *t2, *b1, *b2, *nt;

	op1 = intpromote(vm, op1);
	op2 = intpromote(vm, op2);

	t1 = op1->type;
	t2 = op2->type;
	b1 = chasetype(t1);
	b2 = chasetype(t2);
	if(b1->tkind != Tbase || b2->tkind != Tbase) {
		/* presumably one of them is a Tptr; nothing else is sane. */
		*rv1 = op1;
		*rv2 = op2;
		return;
	}

	c1 = typecbase(b1);
	c2 = typecbase(b2);
	r1 = typerep(b1);
	r2 = typerep(b2);
	if(c1 == c2 && r1 == r2) {
		/* combinations of distinct typedefs
		   and/or enums yield the first type
		   they have in common (not necessarily
		   the base type). */
		if(t1->tkind == Ttypedef || t2->tkind == Ttypedef) {
			nt = commontype(t1, t2);
			if(t1 != nt)
				op1 = typecast(vm, nt, op1);
			if(t2 != nt)
				op2 = typecast(vm, nt, op2);
		}
		*rv1 = op1;
		*rv2 = op2;
		return;
	}

	if(isfloat[c1] || isfloat[c2]) {
		if(rank[c1] < rank[c2])
			nc = c2;
		else
			nc = c1;
	}else if((isunsigned[c1] && isunsigned[c2])
	   || (!isunsigned[c1] && !isunsigned[c2])) {
		if(rank[c1] < rank[c2])
			nc = c2;
		else
			nc = c1;
	}else if(isunsigned[c1] && rank[c1] >= rank[c2])
		nc = c1;
	else if(isunsigned[c2] && rank[c2] >= rank[c1])
		nc = c2;
	else if(!isunsigned[c1] && repsize[typerep(b1)] > repsize[typerep(b2)])
		nc = c1;
	else if(!isunsigned[c2] && repsize[typerep(b2)] > repsize[typerep(b1)])
		nc = c2;
	else if(!isunsigned[c1])
		nc = uvariant[c1];
	else
		nc = uvariant[c2];

	nt = op1->dom->ns->base[nc];          /* op2 has the same domain */
	if(c1 != nc)
		op1 = typecast(vm, nt, op1);
	if(c2 != nc)
		op2 = typecast(vm, nt, op2);
	*rv1 = op1;
	*rv2 = op2;
}

Closure *xentry;

void
calln(VM *vm)
{
	Xentry *x;
	Closure *cl;
	if(xentry == 0)
		bug();
	x = (Xentry*)codeinsn(xentry->code);
	cl = vm->cl;
	vm->cl = xentry;

	/* assist entry with argument sliding */
	memmove(vm->fp+Oarg0+Oarg0, vm->fp+Oarg0, vm->vc*sizeof(Val));

	x(vm, cl, vm->vc);
}

static void
callalien(Code *c)
{
	void (*x)(void);
	switch(Vkind(c->alien)) {
	case Qstr:
		x = (void*)strdata(valstr(c->alien));
		break;
	case Qcval:
		x = (void*)(uptr)cvalu(valcval(c->alien));
		break;
	default:
		bug();
	}
	x();
}

static Val
_ccall(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	Code *c;
	Val rv;
	Imm m;

	rv = Xnil;
	c = cl->code;
	switch(c->kind) {
	case Cvm:
		if((void*)(vm->fp+Onfrhd+argc) >= vm->stk+vm->stksz)
			/* compare current stack to definition of Redline */
			bug();

		/* insert a halt frame */
		vm->fp += Onfrhd;
		vm->fp[Ocl] = mkvalcl(halt);
		vm->fp[Ora] = (Val)(uptr)codeentry(halt->code);

		/* write arguments onto stack */
		for(m = 0; m < argc; m++)
			vm->fp[Oarg0+m] = argv[m];

		vm->vc = argc;
		vm->cl = cl;
		vm->pc = codeentry(c);
		rv = dovm(vm);
		vm->fp -= Onfrhd;

		break;
	case Ccfn:
	case Cccl:
		vm->cl = cl;
		vm->pc = 0;
		switch(c->kind) {
		case Ccfn:
			c->cfn(vm, argc, argv, &rv);
			break;
		case Cccl:
			c->ccl(vm, argc, argv, cldisp(vm->cl), &rv);
			break;
		default:
			bug();
		}
		vm->ac = rv;
		break;
	case Calien:
		callalien(c);
		vm->ac = Xnil;
		break;
	case Cnative:
#if 0
		/* broken */
		fsz = Onfrhd+vm->vc;

		vm->fp += fsz;
		if(vm->cl && vm->cl->code->kind == Cvm)
			bug();
		vm->fp[Ocl] = mkvalcl(vm->cl); /* will be C */
		vm->fp[Ora] = (Val)(uptr)fsz;

		for(m = 0; m < argc; m++)
			vm->fp[Oarg0+m] = argv[m];
		vm->vc = argc;
		vm->cl = cl;

		calln(vm);

		fsz = (Imm)(uptr)vm->fp[Ora];
		vm->cl = valcl(vm->fp[Ocl]);
		vm->vc = fsz-Onfrhd;
		vm->fp -= fsz;
		rv = vm->ac;
#endif
		break;
	default:
		bug();
	}

	return rv;
}

Val
ccall(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	Imm fsz;
	Val rv;

	if(!vm->cl) {
		/* first frame on the stack */
		rv = _ccall(vm, cl, argc, argv);
		vm->cl = 0;
		return rv;
	}

	switch(vm->cl->code->kind) {
	case Cvm:
		/* we are in the middle of a VM operation. */
		fsz = ra2size(vm->pc, vm->cl->code);
		if((void*)(vm->fp+fsz+Onfrhd) >= vm->stk+vm->stksz)
			/* compare current stack to definition of Redline */
			bug();
		vm->fp += fsz;
		vm->fp[Ora] = (Val)(uptr)vm->pc;
		vm->fp[Ocl] = mkvalcl(vm->cl);

		rv = _ccall(vm, cl, argc, argv);

		vm->pc = stkp(vm->fp[Ora]);
		vm->cl = valcl(vm->fp[Ocl]);
		vm->fp -= fsz;
		return rv;
	case Ccfn:
	case Cccl:
		/* we are in a call to a builtin,
		   either from VM or a previous call to
		   a builtin. */
		fsz = Onfrhd+vm->vc;
		if((void*)(vm->fp+fsz+Onfrhd) >= vm->stk+vm->stksz)
			/* compare current stack to definition of Redline */
			bug();
		vm->fp += fsz;
		vm->fp[Ocl] = mkvalcl(vm->cl); /* will be C */
		vm->fp[Ora] = (Val)(uptr)fsz;
		vm->vc = argc;

		rv = _ccall(vm, cl, argc, argv);

		fsz = (Imm)(uptr)vm->fp[Ora]; /* fsz already has this value */
		vm->vc = fsz-Onfrhd;
		vm->cl = valcl(vm->fp[Ocl]);
		vm->fp -= fsz;

		return rv;
	case Cnative:
	case Calien:
		bug();
	default:
		bug();
	}

	return rv;
}

static unsigned showvcall;

static void
vcall(VM *vm)
{
	Code *c;
	Val rv;

	if(showvcall)
		printf("vcall %s\n", ciddata(vm->cl->code->id));
	c = vm->cl->code;
	switch(c->kind) {
	case Cvm:
		vm->pc = codeentry(c);
		break;
	case Ccfn:
	case Cccl:
		rv = Xnil;
		switch(c->kind) {
		case Ccfn:
			c->cfn(vm, vm->vc, vm->fp+Oarg0, &rv);
			break;
		case Cccl:
			c->ccl(vm, vm->vc, vm->fp+Oarg0, cldisp(vm->cl), &rv);
			break;
		default:
			bug();
		}
		vm->ac = rv;
		vm->cl = valcl(vm->fp[Ocl]);
		vm->pc = stkp(vm->fp[Ora]);
		break;
	case Calien:
		callalien(c);
		vm->ac = Xnil;
		vm->cl = valcl(vm->fp[Ocl]);
		vm->pc = stkp(vm->fp[Ora]);
		break;
	case Cnative:
#if 0
		/* broken dregs for native call */
		calln(vm);
		vm->cl = valcl(vm->fp[Ocl]);
		vm->pc = stkp(vm->fp[Ora]);
#endif
		break;
	default:
		bug();
	}
}

static void
vapply(VM *vm)
{
	Imm i, m, oarg, sarg;
	Val *fp;
	Val fn, lv;
	List *l;
	Code *c;

/*
          shuffle stack and vm state.

	  program did

	  	apply(fn, arg0, ..., argn-1)

	  vm on entry:

	    ac = unused
	    cl = apply builtin
	    pc = non-existent insn after Iapply in apply builtin
	    vc = n+1
	    fp -> ra     (apply caller)
	          cl     (apply caller)
	          fn     (to be applied)
	          arg0
	          ...
	          argn-1 (must be list of k >= 0 elements)

	  vm on exit (when fn is bytecode):

	    ac = nil (default return value for builtins)
	    cl = fn
	    pc = codeentry(fn->code)
	    vc = n
	    fp -> ra
	          cl
	          arg0
	          ...
	          argn-1
	          argn
		  ...
		  argn+k
*/
	oarg = vm->vc;
	if(oarg < 2)
		vmerr(vm, "wrong number of arguments to apply");
	fp = vm->fp;
	fn = fp[Oarg0];
	lv = fp[Oarg0+oarg-1];
	if(Vkind(fn) != Qcl)
		vmerr(vm, "first argument to apply must be a procedure");
	if(Vkind(lv) != Qlist)
		vmerr(vm, "final argument to apply must be a list");
	l = vallist(lv);
	m = listlen(l);
	sarg = oarg-2; /* arguments already on stack */
	if(sarg+m > Maxargs)
		vmerr(vm, "too many arguments to apply (limit is %u)", Maxargs);
	checkoverflow(vm, Onfrhd+sarg+m);
	fp = vm->fp;
	memmove(fp+Oarg0, fp+Oarg0+1, sarg*sizeof(Val));
	for(i = 0; i < m; i++)
		fp[Oarg0+sarg+i] = listref(l, i);
	vm->vc = sarg+m;
	vm->cl = valcl(fn);
	vm->ac = Xnil;

	/* make the call */
	c = vm->cl->code;
	switch(c->kind) {
	case Cvm:
		vm->pc = codeentry(c);
		/* vm will continue into call, then return to apply's caller */
		break;
	case Ccfn:
		c->cfn(vm, vm->vc, vm->fp+Oarg0, &vm->ac);
		/* return to apply's caller */
		vm->pc = stkp(vm->fp[Ora]);
		vm->cl = stkp(vm->fp[Ocl]);
		break;
	case Cccl:
		c->ccl(vm, vm->vc, vm->fp+Oarg0, cldisp(vm->cl), &vm->ac);
		/* return to apply's caller */
		vm->pc = stkp(vm->fp[Ora]);
		vm->cl = stkp(vm->fp[Ocl]);
		break;
	case Calien:
		vmerr(vm, "attempt to apply alien code");
		break;
	case Cnative:
/*
  dregs for native code
		calln(vm);
*/
		break;
	default:
		bug();
	}
}

#if 0
void
ncallstat(VM *vm)
{
	Val v, k;
	Tab *t;
	Env env;
	env = vm->top;
	v = envlookup(env, "ncallstat");
	if(v == 0) {
		t = mktab();
		envbind(env, "ncallstat", mkvaltab(t));
	}else
		t = valtab(v);
	k = mkvalcid(vm->cl->code->id);
	v = tabget(t, k);
	if(v == 0) {
		tabput(t, k, mkvalcval2(cval1));
		return;
	}else
		tabput(t, k, xbinop(vm, Iadd, v, mkvalcval2(cval1)));
}
#endif

void
ncall(VM *vm)
{
	Code *c;
	Val rv;

//	ncallstat(vm);

	c = vm->cl->code;
	switch(c->kind) {
	case Cvm:
		/* push a halt frame */
		memmove(vm->fp+Oarg0+Oarg0,
			vm->fp+Oarg0,
			vm->vc*sizeof(Val));
		vm->fp += Onfrhd;
		vm->fp[Ocl] = mkvalcl(halt);
		vm->fp[Ora] = (Val)(uptr)codeentry(halt->code);
		vm->pc = codeentry(c);
		dovm(vm);
		vm->fp -= Onfrhd;
		vm->cl = valcl(vm->fp[Ocl]);
		break;
	case Ccfn:
	case Cccl:
		rv = Xnil;
		switch(c->kind) {
		case Ccfn:
			c->cfn(vm, vm->vc, vm->fp+Oarg0, &rv);
			break;
		case Cccl:
			c->ccl(vm, vm->vc, vm->fp+Oarg0, cldisp(vm->cl), &rv);
			break;
		default:
			bug();
		}
		vm->ac = rv;
		break;
	case Calien:
		/* probably could be implemented */
	case Cnative:
		bug();
	}
}

static Imm
xstrcmp(VM *vm, ikind op, Str *s1, Str *s2)
{
	int x;

	switch(op) {
	case Icmpeq:
		return equalstr(s1, s2);
	case Icmpneq:
		return !equalstr(s1, s2);
	case Icmpgt:
		x = Strcmp(s1, s2);
		return x > 0 ? 1 : 0;
	case Icmpge:
		x = Strcmp(s1, s2);
		return x >= 0 ? 1 : 0;
	case Icmplt:
		x = Strcmp(s1, s2);
		return x < 0 ? 1 : 0;
	case Icmple:
		x = Strcmp(s1, s2);
		return x <= 0 ? 1 : 0;
	default:
		vmerr(vm, "attempt to apply %s to string operands", opstr[op]);
	}
	return 0; /* not reached */
}

static Cval*
xunopfp(VM *vm, ikind op, Cbase cb, Cval *cv)
{
	float f;
	double d;
	Enc v;
	switch(cb) {
	case Vfloat:
		f = cvalf(cv);
		switch(op) {
		case Ineg:
			f = -f;
			break;
		default:
			vmerr(vm, "attempt to perform %s "
			      "on a floating point value", opstr[op]);
		}
		v.f = f;
		break;
	case Vdouble:
		d = cvald(cv);
		switch(op) {
		case Ineg:
			d = -d;
			break;
		default:
			vmerr(vm, "attempt to perform %s "
			      "on a floating point value", opstr[op]);
		}
		v.d = d;
		break;
	default:
		bug();
	}
	return mkcvalenc(cv->dom, cv->type, v);
}

Val
xunop(VM *vm, ikind op, Val v)
{
	Cval *cv, *cvr;
	Imm imm, nv;
	Ctype *t;
	Cbase cb;

	if(op == Inot) {
		if(Vkind(v) == Qcval)
			goto cval;
		else if(Vkind(v) == Qnil)
			cvr = cval1;
		else
			cvr = cval0;
		goto out;
	}
	if(Vkind(v) != Qcval)
		vmerr(vm, "incompatible operand for unary %s", opstr[op]);
cval:
	cv = intpromote(vm, valcval(v));
	t = chasetype(cv->type);
	cb = typecbase(t);
	if(isfloat[cb]) {
		cvr = xunopfp(vm, op, cb, cv);
		goto out;
	}

	imm = cvalu(cv);

	switch(op) {
	case Ineg:
		nv = -imm;
		cvr = mkcval(cv->dom, cv->type, nv);
		break;
	case Iinv:
		nv = ~imm;
		cvr = mkcval(cv->dom, cv->type, nv);
		break;
	case Inot:
		if(imm)
			cvr = cval0;
		else
			cvr = cval1;
		break;
	default:
		fatal("unknown unary operator %d", op);
	}
out:
	return mkvalcval2(cvr);
}

static Imm
truncimm(Imm v, Rkind rep)
{
	switch(rep) {
	case Rs08be:
	case Rs08le:
		return (s8)v;
	case Rs16be:
	case Rs16le:
		return (s16)v;
	case Rs32be:
	case Rs32le:
		return (s32)v;
	case Rs64be:
	case Rs64le:
		return (s64)v;
	case Ru08be:
	case Ru08le:
		return (u8)v;
	case Ru16be:
	case Ru16le:
		return (u16)v;
	case Ru32be:
	case Ru32le:
		return (u32)v;
	case Ru64be:
	case Ru64le:
		return (u64)v;
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Cval*
xcvalptralu(VM *vm, ikind op, Cval *op1, Cval *op2, Ctype *t1, Ctype *t2)
{
	Ctype *sub, *pt;
	Cval *ptr;
	Imm sz, osz, n;

	if(op != Iadd && op != Isub)
		vmerr(vm, "attempt to apply %s to pointer operand", opstr[op]);

	if(t1->tkind == Tptr && t2->tkind == Tptr) {
		if(op != Isub)
			vmerr(vm, "attempt to apply %s to pointer operands",
			      opstr[op]);
		sub = chasetype(subtype(t1));
		/* special case: sizeof(void)==1 for pointer arith */
		if(sub->tkind == Tvoid)
			sz = 1;
		else
			sz = typesize(vm, sub);
		sub = chasetype(subtype(t2));
		if(sub->tkind == Tvoid)
			osz = 1;
		else
			osz = typesize(vm, sub);
		if(sz != osz)
			vmerr(vm, "attempt to subtract pointers to "
			      "objects of different sizes");
		if(sz == 0)
			vmerr(vm, "attempt to subtract pointers to "
			      "zero-sized objects");
		n = (cvalu(op1)-cvalu(op2))/sz;
		n = truncimm(n, typerep(t1));
		/* FIXME: define ptrdiff_t? */
		return mkcval(litdom, litdom->ns->base[Vlong], n);
	}

	/* exactly one operand is a pointer */

	if(t1->tkind == Tptr) {
		sub = chasetype(subtype(t1));
		ptr = op1;
		n = cvalu(op2);
	}else if(op == Isub) {
		vmerr(vm, "invalid right-hand pointer operand to -");
		return 0; /* not reached */
	}else{
		sub = chasetype(subtype(t2));
		ptr = op2;
		n = cvalu(op1);
	}

	/* special case: sizeof(void)==1 for pointer arith */
	if(sub->tkind == Tvoid)
		sz = 1;
	else
		sz = typesize(vm, sub);
	/* FIXME: what about undefined sub types? */
	if(op == Iadd)
		n = cvalu(ptr)+n*sz;
	else
		n = cvalu(ptr)-n*sz;
	pt = chasetype(ptr->type);
	n = truncimm(n, typerep(pt));
	return mkcval(ptr->dom, ptr->type, n);
}

static Cval*
xcvalfpalu(VM *vm, ikind op, Cval *op1, Cval *op2, Ctype *t1, Ctype *t2)
{
	float f1, f2, fr;
	double d1, d2, dr;
	Enc v;
	Cbase cb;

	cb = typecbase(t1);
	if(cb != typecbase(t2))
		bug();
	switch(cb) {
	case Vfloat:
		f1 = cvalf(op1);
		f2 = cvalf(op2);
		switch(op) {
		case Iadd:
			fr = ((float)f1+(float)f2);
			break;
		case Isub:
			fr = ((float)f1-(float)f2);
			break;
		case Imul:
			fr = ((float)f1*(float)f2);
			break;
		case Idiv:
			fr = ((float)f1/(float)f2);
			break;
		default:
			vmerr(vm, "attempt to perform %s "
			      "on floating point values",
			      opstr[op]);
		}
		v.f = fr;
		break;
	case Vdouble:
		d1 = cvald(op1);
		d2 = cvald(op2);
		switch(op) {
		case Iadd:
			dr = ((double)d1+(double)d2);
			break;
		case Isub:
			dr = ((double)d1-(double)d2);
			break;
		case Imul:
			dr = ((double)d1*(double)d2);
			break;
		case Idiv:
			dr = ((double)d1/(double)d2);
			break;
		default:
			vmerr(vm, "attempt to perform %s "
			      "on floating point values",
			      opstr[op]);
		}
		v.d = dr;
		break;
	default:
		fatal("bug");
	}
	return mkcvalenc(op1->dom, op1->type, v);
}

static Cval*
xcvalalu1dom(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	Imm i1, i2, rv;
	Ctype *t1, *t2;
	Cbase b1, b2;

	if(op1->dom != op2->dom)
		fatal("bug");
	usualconvs(vm, op1, op2, &op1, &op2);
	t1 = chasetype(op1->type);
	t2 = chasetype(op2->type);
	if(t1->tkind == Tptr || t2->tkind == Tptr)
		return xcvalptralu(vm, op, op1, op2, t1, t2);
	b1 = typecbase(t1);
	b2 = typecbase(t2);
	if(isfloat[b1])
		return xcvalfpalu(vm, op, op1, op2, t1, t2);

	i1 = cvalu(op1);
	i2 = cvalu(op2);

	switch(op) {
	case Iadd:
		rv = i1+i2;
		break;
	case Idiv:
		if(i2 == 0)
			vmerr(vm, "divide by zero");
		if(isunsigned[b1] && isunsigned[b2])
			rv = i1/i2;
		else if(isunsigned[b1] || isunsigned[b2])
			fatal("i am dumb");
		else
			rv = (s64)i1/(s64)i2;
		break;
	case Imod:
		if(i2 == 0)
			vmerr(vm, "divide by zero");
		if(isunsigned[b1] && isunsigned[b2])
			rv = i1%i2;
		else if(isunsigned[b1] || isunsigned[b2])
			fatal("i am dumb");
		else
			rv = (s64)i1%(s64)i2;
		break;
	case Imul:
		rv = i1*i2;
		break;
	case Isub:
		rv = i1-i2;
		break;
	case Iand:
		rv = i1&i2;
		break;
	case Ixor:
		rv = i1^i2;
		break;
	case Ior:
		rv = i1|i2;
		break;
	default:
		fatal("bug");
	}

	rv = truncimm(rv, typerep(t1));
	return mkcval(op1->dom, op1->type, rv);
}

Cval*
xcvalalu(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	dompromote(vm, op, op1, op2, &op1, &op2);
	return xcvalalu1dom(vm, op, op1, op2);
}

static Cval*
xcvalshift(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	Imm i1, i2, rv;
	Ctype *t1, *t2;
	Cbase b1, b2;

	/* no need to rationalize domains */
	op1 = intpromote(vm, op1);
	op2 = intpromote(vm, op2);
	i1 = cvalu(op1);
	i2 = cvalu(op2);
	t1 = chasetype(op1->type);
	t2 = chasetype(op2->type);

	if(t1->tkind == Tptr || t2->tkind == Tptr)
		vmerr(vm, "invalid pointer operand to shift operator");

	b1 = typecbase(t1);
	b2 = typecbase(t2);
	if(isfloat[b1] || isfloat[b2])
		vmerr(vm, "invalid floating point operand to shift operator");

	/* following C99:
	   - (both) if op2 is negative or >= width of op1,
	            the result is undefined;
	   - (<<)   if op1 is signed and op1*2^op2 is not representable in
	            typeof(op1), the result is undefined;
	   - (>>)   if op1 is signed and negative, the result is
                    implementation-defined.
		    here, "implementation-defined" means whatever
		    your compiler says.  gcc and microsoft
		    performs sign extension.
	*/
	if(isunsigned[b1])
		switch(op) {
		case Ishl:
			rv = i1<<i2;
			rv = truncimm(rv, typerep(t1));
			break;
		case Ishr:
			rv = i1>>i2;
			break;
		default:
			fatal("bug");
		}
	else
		switch(op) {
		case Ishl:
			rv = (s64)i1<<i2;
			rv = truncimm(rv, typerep(t1));
			break;
		case Ishr:
			rv = (s64)i1>>i2;
			break;
		default:
			fatal("bug");
		}
	return mkcval(op1->dom, op1->type, rv);
}

static int
cvalfpcmp(VM *vm, Cval *op1, Cval *op2, Ctype *t1, Ctype *t2)
{
	float f1, f2;
	double d1, d2;
	Cbase cb;

	cb = typecbase(t1);
	if(cb != typecbase(t2))
		bug();
	switch(cb) {
	case Vfloat:
		f1 = cvalf(op1);
		f2 = cvalf(op2);
		if(f1<f2)
			return -1;
		else if(f1>f2)
			return 1;
		else
			return 0;
	case Vdouble:
		d1 = cvald(op1);
		d2 = cvald(op2);
		if(d1<d2)
			return -1;
		else if(d1>d2)
			return 1;
		else
			return 0;
	default:
		bug();
	}
}

static int
cvalcmp(VM *vm, Cval *op1, Cval *op2)
{
	Imm i1, i2;
	Ctype *t1, *t2;
	Cbase b1;

	/* We're intentionally relaxed about whether one operand is
	   pointer so that expressions like (p == 0x<addr>) can be
	   written without cast clutter. */

	/* op to dompromote does not matter as long as it is
	   not Iadd or Isub */

	dompromote(vm, Icmpeq, op1, op2, &op1, &op2);
	usualconvs(vm, op1, op2, &op1, &op2);
	i1 = cvalu(op1);
	i2 = cvalu(op2);
	t1 = chasetype(op1->type);
	t2 = chasetype(op2->type);
	b1 = typecbase(t1);
	if(isfloat[b1])
		return cvalfpcmp(vm, op1, op2, t1, t2);
	if(t1->tkind == Tptr || t2->tkind == Tptr || isunsigned[b1]) {
		if(i1<i2)
			return -1;
		else if(i1>i2)
			return 1;
		else
			return 0;
	}else{
		if((s64)i1<(s64)i2)
			return -1;
		else if((s64)i1>(s64)i2)
			return 1;
		else
			return 0;
	}
}

static Cval*
xcvalcmp(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	switch(cvalcmp(vm, op1, op2)) {
	case -1:
		switch(op) {
		case Icmpeq:
		case Icmpge:
		case Icmpgt:
			return cval0;
		case Icmple:
		case Icmplt:
		case Icmpneq:
			return cval1;
		default:
			fatal("bug");
		}
	case 0:
		switch(op) {
		case Icmpeq:
		case Icmpge:
		case Icmple:
			return cval1;
		case Icmpgt:
		case Icmplt:
		case Icmpneq:
			return cval0;
		default:
			fatal("bug");
		}
	case 1:
		switch(op) {
		case Icmpeq:
		case Icmple:
		case Icmplt:
			return cval0;
		case Icmpge:
		case Icmpgt:
		case Icmpneq:
			return cval1;
		default:
			fatal("bug");
		}
	default:
		fatal("bug");
	}
}

static Val
xbinop(VM *vm, ikind op, Val v1, Val v2)
{
	Cval *cv1, *cv2, *cvr;
	Str *s1, *s2;
	Imm nv;

	if(Vkind(v1) == Qcval && Vkind(v2) == Qcval) {
		cv1 = valcval(v1);
		cv2 = valcval(v2);
		switch(op) {
		case Iadd:
		case Iand:
		case Idiv:
		case Imod:
		case Imul:
		case Ior:
		case Isub:
		case Ixor:
			cvr = xcvalalu(vm, op, cv1, cv2);
			break;
		case Ishl:
		case Ishr:
			cvr = xcvalshift(vm, op, cv1, cv2);
			break;
		case Icmplt:
		case Icmple:
		case Icmpgt:
		case Icmpge:
		case Icmpeq:
		case Icmpneq:
			cvr = xcvalcmp(vm, op, cv1, cv2);
			break;
		default:
			fatal("bug");
		}
		return mkvalcval2(cvr);
	}

	if(Vkind(v1) == Qstr && Vkind(v2) == Qstr) {
		s1 = valstr(v1);
		s2 = valstr(v2);
dostr:
		if(op == Iadd)
			return mkvalstr(strconcat(s1, s2));
		else{
			nv = xstrcmp(vm, op, s1, s2);
			if(nv)
				return mkvalcval2(cval1);
			else
				return mkvalcval2(cval0);
		}
	}

	if(Vkind(v1) == Qstr && Vkind(v2) == Qcval) {
		cv2 = valcval(v2);
		if(ischarcval(cv2)) {
			char v = cvalu(cv2);
			s2 = mkstr(&v,1);
			s1 = valstr(v1);
			goto dostr;
		}
		if(isstrcval(cv2)) {
			s2 = stringof(vm, cv2);
			s1 = valstr(v1);
			goto dostr;
		}
		/* fall through */
	}

	if(Vkind(v2) == Qstr && Vkind(v1) == Qcval) {
		cv1 = valcval(v1);
		if(ischarcval(cv1)) {
			char v = cvalu(cv1);
			s1 = mkstr(&v,1);
			s2 = valstr(v2);
			goto dostr;
		}
		if(isstrcval(cv1)) {
			s1 = stringof(vm, cv1);
			s2 = valstr(v2);
			goto dostr;
		}
		/* fall through */
	}

	/* all other types support only == and != */
	if(op != Icmpeq && op != Icmpneq)
		vmerr(vm, "incompatible operands for binary %s", opstr[op]);

	nv = equalval(v1, v2);
	if(op == Icmpneq)
		nv = !nv;
	if(nv)
		return mkvalcval2(cval1);
	else
		return mkvalcval2(cval0);
}

static Cont*
kcapture(VM *vm)
{
	Cont *k;
	u32 sz;

	if(vm->fp != vm->stk) {
		sz = (void*)vm->fp-vm->stk;
		k = mkcont(vm->stk, sz, vm->fp[Ora], valcl(vm->fp[Ocl]),
			   vm->klink, vm->level, vm->levgen[vm->level]);
		vm->klink = k;
		vm->stk = vm->fp;
		vm->stksz -= sz;
		vm->fp[Ora] = codeentry(stkunderflow->code);
		vm->fp[Ocl] = mkvalcl(stkunderflow);
	}
	return vm->klink;
}

static void
vkcapture(VM *vm)
{
	Cont *c;

	if(vm->vc != 1)
		vmerr(vm, "wrong number of arguments to kcapture");
	if(Vkind(vm->fp[Oarg0]) != Qcl)
		vmerr(vm, "argument 1 to kcapture must be a procedure");
	c = kcapture(vm);
	vm->cl = valcl(vm->fp[Oarg0]);
	vm->pc = codeentry(vm->cl->code);
	vm->fp[Oarg0] = mkvalcont(c);
}

static void
koverflow(VM *vm)
{
	Cont *k;
	u32 sz;
	Val *fp;
	Imm fsz;
	void *ra;
	Closure *cl;

	sz = (void*)vm->fp-vm->stk;
	ra = vm->fp[Ora];
	cl = valcl(vm->fp[Ocl]);
	fsz = ra2size(vm->pc, vm->cl->code);
	k = mkcont(vm->stk, sz, ra, cl, vm->klink,
		   vm->level, vm->levgen[vm->level]);
	vm->klink = k;
	vm->stk = malstack(Maxstk);
	vm->stksz = Maxstk;
	fp = vm->stk;
	fp[Ora] = codeentry(stkunderflow->code);
	fp[Ocl] = mkvalcl(stkunderflow);
	memcpy(fp+Oarg0, vm->fp+Oarg0, (fsz-Oarg0)*sizeof(Val));
	vm->fp = vm->stk;
}

static int
ishalt(Closure *cl)
{
	return cl == halt;
}

static void
kunderflow(VM *vm)
{
	Cont *k, *nk;
	static u32 lim = 1024; /* arbitrary bound on continuation copy */
	Val *fp;
	void *top;
	u32 fsz;
	Insn *ra;
	Closure *cl;

	k = vm->klink;
	if(k == 0)
		bug();
	if(k->base == 0 || k->gen != vm->levgen[k->level])
		vmerr(vm, "attempt to return to stale context");

	/* try to split the continuation if it exceeds our copy limit.
	   FIXME: this code corrupts the stack in the case that the
	   first frame in the continuation is a halt frame (top == fp).
	   we don't split in that case.
	*/
	if(k->sz > lim) {
		top = k->base+k->sz;
		fp = (Val*)top;
		ra = k->ra;
		cl = k->cl;
		fsz = ra2size(ra, cl->code);
		if(fsz*sizeof(Val) >= lim)
			/* FIXME: we assume frame size never exceeds limit, but
			   this should be enforced by the compiler */
			bug();
		while(top - (void*)(fp-fsz) < lim) {
			if(ishalt(cl))
				/* don't split across non-vm frames -- we don't
				   have a strategy for updating vm level in the
				   split continuation. */
				break;
			fp -= fsz;
			ra = stkp(fp[Ora]);
			cl = valcl(fp[Ocl]);
			fsz = ra2size(ra, cl->code);
		}
		if((void*)fp < top) {
			/* fp points to base of last frame that will fit in
			   a lim-sized segment */
			nk = mkcont(k->base, (void*)fp-k->base, ra, cl, k->link,
				    k->level, k->gen);
			k->link = nk;
			k->base = fp;
			k->sz = top-k->base;
			if(k->sz > lim)
				bug();
			gcwb(mkvalcont(k));
			fp[Ora] = codeentry(stkunderflow->code);
			fp[Ocl] = mkvalcl(stkunderflow);
		}
	}

	/* allocate a new stack if current one is too small */
	if(vm->stksz < k->sz) {
		vm->stk = malstack(Maxstk);
		vm->stksz = Maxstk;
	}

	/* copy continuation */
	memcpy(vm->stk, k->base, k->sz);

	/* reinstate */
	vm->pc = k->ra;
	vm->cl = k->cl;
	vm->fp = vm->stk+k->sz;	/* returned-to code shall first reset fp */
	vm->klink = k->link;
	vm->level = k->level;
	longjmp(vm->dovm[vm->level], 0);
}

static void
vkresume(VM *vm)
{
	Val k;
	k = cldisp(vm->cl)[0];
	if(Vkind(k) != Qcont)
		bug();
	vm->klink = valcont(k);
//	vm->ac = vm->fp[Oarg0];
	kunderflow(vm);
}

static int
isfalse(Val v)
{
	Cval *cv;

	switch(Vkind(v)) {
	case Qcval:
		cv = valcval(v);
		return cvalu(cv) == 0;
	case Qnil:
		return 1;
	default:
		return 0;
	}
}

static void
checkoverflow(VM *vm, unsigned m)
{
	if((void*)vm->fp < vm->stk)
		/* stack is insane */
		bug();
	if((void*)vm->fp >= vm->stk+vm->stksz)
		/* stack is insane */
		bug();
	if((void*)(vm->fp+m) > vm->stk+vm->stksz-Redline) {
		koverflow(vm);
		/* eventually unnecessary sanity checks */
		if((void*)vm->fp < vm->stk)
			bug();
		if((void*)vm->fp >= vm->stk+vm->stksz-Redline)
			/* stack is insane */
			bug();
		if((void*)(vm->fp+m) > vm->stk+vm->stksz-Redline)
			bug();
	}
}

static void
l1_cref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t, *b, *pt;
	Dom *d;
	Cval *cv;
	Imm imm;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to cref");
	if(Vkind(argv[0]) != Qdom)
		vmerr(vm, "attempt to derive location from non-domain");
	checkarg(vm, argv, 1, Qctype);
	checkarg(vm, argv, 2, Qcval);
	d = valdom(argv[0]);
	t = valctype(argv[1]);
	cv = valcval(argv[2]);
	b = chasetype(t);
	switch(b->tkind) {
	case Tvoid:
	case Tbase:
	case Tundef:
	case Tptr:
	case Tstruct:
	case Tunion:
	case Tfun:
	case Tenum:
	case Tbitfield: /* bitfield bonus: C wouldn't let you do this */
		/* construct pointer */
		pt = mkctypeptr(t, typerep(d->ns->base[Vptr]));
		imm = truncimm(cvalu(cv), typerep(pt));
		*rv = mkvalcval(d, pt, imm);
		break;
	case Tarr:
		/* construct pointer to first element */
		pt = mkctypeptr(subtype(t), typerep(d->ns->base[Vptr]));
		imm = cvalu(cv);
		imm = truncimm(imm, typerep(pt));
		*rv = mkvalcval(d, pt, imm);
		break;
	case Tconst:
		vmerr(vm, "attempt to use enumeration constant as location");
	default:
	case Ttypedef:
		bug();
	}
}

static int
dobitfieldgeom(Ctype *t, BFgeom *bfg)
{
	Cval *bit0, *bs;
	Ctype *bb;
	Ctypebitfield *b;

	b = (Ctypebitfield*)t;
	if(Vkind(b->bit0) != Qcval)
		return -1;
	bit0 = valcval(b->bit0);
	bs = valcval(b->cnt);
	bfg->bp = cvalu(bit0);
	bfg->bs = cvalu(bs);
	bb = chasetype(subtype(t));
	bfg->isbe = isbigendian[typerep(bb)];
	return bitfieldgeom(bfg);
}

static Str*
_callget(VM *vm, As *as, Imm off, Imm len)
{
	Val rv, argv[2];
	Str *s;

	argv[0] = mkvalas(as);
	argv[1] = mkvalrange2(mkcval(litdom, litdom->ns->base[Vptr], off),
			      mkcval(litdom, litdom->ns->base[Vptr], len));
	rv = ccall(vm, as->get, 2, argv);
	if(Vkind(rv) != Qstr)
		vmerr(vm, "get method returned non-string");
	s = valstr(rv);
	if(s->len != len)
		vmerr(vm, "get method returned wrong number of bytes");
	return s;
}

static void
l1_cval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm imm;
	Enc v;
	Dom *d;
	Ctype *t, *b, *sub, *pt;
	Cval *cv;
	Str *s, *es;
	BFgeom bfg;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to cval");
	if(Vkind(argv[0]) != Qdom && Vkind(argv[0]) != Qns)
		vmerr(vm, "operand 1 to cval must be a domain or namespace");
	checkarg(vm, argv, 1, Qctype);
	checkarg(vm, argv, 2, Qcval);

	t = valctype(argv[1]);
	b = chasetype(t);
	cv = valcval(argv[2]);

	/* special case: enum constants can be referenced through namespace */
	if(b->tkind == Tconst) {
		switch(Vkind(argv[0])) {
		case Qdom:
			d = valdom(argv[0]);
			*rv = mkvalcval(d, subtype(b), cvalu(cv));
			break;
		case Qns:
			d = mkdom(valns(argv[0]), litdom->as, 0);
			*rv = mkvalcval(d, subtype(b), cvalu(cv));
			break;
		default:
			bug();
		}
		return;
	}

	if(Vkind(argv[0]) != Qdom)
		vmerr(vm, "attempt to access address space through non-domain");
	d = valdom(argv[0]);
	switch(b->tkind) {
	case Tbitfield:
		sub = subtype(b);
		if(sub->tkind == Tundef) {
			sub = subtype(sub);
			es = fmtctype(sub);
			vmerr(vm, "attempt to read object of undefined type: "
			      "%.*s", (int)es->len, strdata(es));
		}
		if(0 > dobitfieldgeom(b, &bfg))
			vmerr(vm, "invalid bitfield access");
		s = _callget(vm, d->as, cvalu(cv)+bfg.addr, bfg.cnt);
		imm = bitfieldget(strdata(s), &bfg);
		/* d, t, and b may have moved */
		d = valdom(argv[0]);
		t = valctype(argv[1]);
		b = chasetype(t);
		*rv = mkvalcval(d, subtype(b), imm);
		break;
	case Tbase:
	case Tptr:
		/* FIXME: check type of cv */
		s = _callget(vm, d->as, cvalu(cv), typesize(vm, t));
		/* d and t may have moved */
		d = valdom(argv[0]);
		t = valctype(argv[1]);
		v = str2enc(vm, t, s);
		*rv = mkvalcval2(mkcvalenc(d, t, v));
		break;
	case Tarr:
		/* construct pointer to first element */
		pt = mkctypeptr(subtype(t), typerep(d->ns->base[Vptr]));
		imm = truncimm(cvalu(cv), typerep(pt));
		*rv = mkvalcval(d, pt, imm);
		break;
	case Tvoid:
	case Tfun:
	case Tstruct:
	case Tunion:
		vmerr(vm,
		      "attempt to read %s-valued object from address space",
		      tkindstr[b->tkind]);
	case Tundef:
		es = fmtctype(subtype(b));
		vmerr(vm, "attempt to read object of undefined type: %.*s",
		      (int)es->len, strdata(es));
	case Tenum:
	case Ttypedef:
	case Tconst:
		bug();
	}
}

static int
iscvaltype(Ctype *t)
{
	static int okay[Tntkind] = {
		[Tbase] = 1,
		[Tptr] = 1,
		[Tbitfield] = 1,
	};

	t = safechasetype(t);
	return t && okay[t->tkind];
}

static int
isptrtype(Ctype *t)
{
	t = chasetype(t);
	return t->tkind == Tptr;
}

void
vargs(VM *vm, Imm m)
{
	List *l;
	Imm i;
	l = mklist();
	for(i = m; i < vm->vc; i++)
		_listappend(l, vm->fp[Oarg0+i]);
	vm->fp[Oarg0+m] = mkvallist(l);
	vm->vc = m+1;
}

static Ctype*
nsvoidstar(Ns *ns)
{
	return mkctypeptr(ns->base[Vvoid], typerep(ns->base[Vptr]));
}

static Cval*
str2voidstar(Ns *ns, Str *s)
{
	Dom *d;
	d = mkdom(ns, mksas(s), 0);
	return mkcval(d, nsvoidstar(ns), 0);
}

static Val
xxcast(VM *vm, Val tv, Val ov)
{
	Val rv;
	Cval *cv;
	Dom *d;
	Ctype *t;

	rv = 0;
	if(Vkind(ov) != Qcval && Vkind(ov) != Qstr)
		vmerr(vm, "operand 2 to extended cast operator must be a"
		      " cvalue or string");
	if(Vkind(ov) == Qstr)
		cv = str2voidstar(litdom->ns, valstr(ov));
	else
		cv = valcval(ov);
	if(Vkind(tv) != Qctype && Vkind(ov) == Qstr)
		vmerr(vm, "illegal conversion");
	if(Vkind(tv) == Qctype) {
		t = valctype(tv);
		if(!iscvaltype(t))
			vmerr(vm, "bad type conversion");
		rv = mkvalcval2(typecast(vm, t, cv));
	}else if(Vkind(tv) == Qdom) {
		d = valdom(tv);
		rv = mkvalcval2(domcast(vm, d, cv));
	}else if(Vkind(tv) == Qns) {
		d = mkdom(valns(tv), cv->dom->as, 0);
		rv = mkvalcval2(domcast(vm, d, cv));
	}else if(Vkind(tv) == Qas) {
		d = mkdom(cv->dom->ns, valas(tv), 0);
		rv = mkvalcval2(domcast(vm, d, cv));
	}else
		vmerr(vm, "bad type for operand 1 to extended cast operator");
	return rv;
}

static void
l1_sizeof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm imm;
	Ctype *t;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to sizeof");
	switch(Vkind(argv[0])) {
	case Qcval:
		cv = valcval(argv[0]);
		t = cv->type;
		break;
	case Qctype:
		t = valctype(argv[0]);
		break;
	default:
		vmerr(vm, "operand 1 to sizeof must be a type or cvalue");
	}
	imm = typesize(vm, t);
	*rv = mkvalcval(litdom, litdom->ns->base[Vuint], imm);
}

/* function from any arguments to nil */
static void
nilfn(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(vm);
	USED(argc);
	USED(argv);
	USED(rv);
}

static void
nasbad(VM *vm, Imm argc, Val *argv, Val *rv)
{
	vmerr(vm, "attempt to access null address space");
}

static void
nasmap(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to map method");
	*rv = mkvalvec(mkvec(0));
}

static void
nasismapped(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to ismapped method");
	*rv = mkvalcval2(cval0);
}


static As*
mknas(void)
{
	Tab *mtab;

	mtab = mktab();
	tabput(mtab, mkvalstr(mkstr0("get")),
	       mkvalcl(mkcfn("nasget", nasbad)));
	tabput(mtab, mkvalstr(mkstr0("put")),
	       mkvalcl(mkcfn("nasput", nasbad)));
	tabput(mtab, mkvalstr(mkstr0("map")),
	       mkvalcl(mkcfn("nasmap", nasmap)));
	tabput(mtab, mkvalstr(mkstr0("ismapped")),
	       mkvalcl(mkcfn("nasismapped", nasismapped)));

	return mkastab(mtab, mkstr0("nullas"));
}

/* is [rb,rl) in [b,l)? */
static int
checkrange(Imm b, Imm l, Imm rb, Imm rl)
{
	Imm e, re;
	e = b+l;
	if(e < b)
		fatal("bug");
	if(rb < b || rb > e)
		return 0;
	re = rb+rl;
	if(re < rb || re > e)
		/* FIXME: depending on whether ranges can be empty,
		   also check re==rb */
		return 0;
	return 1;
}

static void
sasget(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s;
	Range *r;
	Imm rb, rl;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to get method");
	checkarg(vm, argv, 1, Qrange);
	s = valstr(disp[0]);
	r = valrange(argv[1]);
	rb = cvalu(r->beg);
	rl = cvalu(r->len);
	if(!checkrange(0, s->len, rb, rl))
		vmerr(vm, "address space access out of bounds");
	*rv = mkvalstr(strslice(s, rb, rb+rl));
}

static void
sasput(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s, *dat;
	Range *r;
	Imm rb, rl;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to put method");
	checkarg(vm, argv, 1, Qrange);
	checkarg(vm, argv, 2, Qstr);
	s = valstr(disp[0]);
	r = valrange(argv[1]);
	rb = cvalu(r->beg);
	rl = cvalu(r->len);
	if(!checkrange(0, s->len, rb, rl))
		vmerr(vm, "address space access out of bounds");
	dat = valstr(argv[2]);
	if(dat->len < rl)
		vmerr(vm, "not enough bytes for address space update");
	/* FIXME: rationalize with l1_strput */
	memcpy(strdata(s)+rb, strdata(dat), rl);
	USED(rv);
}

static void
sasmap(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Val val;
	Vec *v;
	Str *s;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to map method");
	USED(argv);
	s = valstr(disp[0]);
	v = mkvec(1);
	val = mkvalrange2(cvalnull,
			  mkcval(litdom,
				 litdom->ns->base[Vptr], s->len));
	_vecset(v, 0, val);
	*rv = mkvalvec(v);
}

static void
sasismapped(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Range *r;
	Imm rb, rl;
	Str *s;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to ismapped method");
	s = valstr(disp[0]);
	r = valrange(argv[1]);
	rb = cvalu(r->beg);
	rl = cvalu(r->len);
	if(!checkrange(0, s->len, rb, rl))
		*rv = mkvalcval2(cval0);
	else
		*rv = mkvalcval2(cval1);
}

As*
mksas(Str *s)
{
	Tab *mtab;
	mtab = mktab();
	tabput(mtab, mkvalstr(mkstr0("get")),
		mkvalcl(mkccl("sasget", sasget, 1, mkvalstr(s))));
	tabput(mtab, mkvalstr(mkstr0("put")),
		mkvalcl(mkccl("sasput", sasput, 1, mkvalstr(s))));
	tabput(mtab, mkvalstr(mkstr0("map")),
		mkvalcl(mkccl("sasmap", sasmap, 1, mkvalstr(s))));
	tabput(mtab, mkvalstr(mkstr0("ismapped")),
		mkvalcl(mkccl("sasismapped", sasismapped, 1, mkvalstr(s))));
	return mkastab(mtab, 0);
}

static void
masget(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *dat;
	Range *r, *mr;
	Imm rb, rl, mb, ml;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to get method");
	checkarg(vm, argv, 1, Qrange);
	mr = valrange(disp[0]);
	r = valrange(argv[1]);
	rb = cvalu(r->beg);
	rl = cvalu(r->len);
	mb = cvalu(mr->beg);
	ml = cvalu(mr->len);
	if(!checkrange(mb, ml, rb, rl))
		vmerr(vm, "address space access out of bounds");
	dat = mkstr((char*)(uptr)rb, rl);
	*rv = mkvalstr(dat);
}

static void
masput(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *dat;
	Range *r, *mr;
	Imm rb, rl, mb, ml;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to put method");
	checkarg(vm, argv, 1, Qrange);
	checkarg(vm, argv, 2, Qstr);
	mr = valrange(disp[0]);
	r = valrange(argv[1]);
	rb = cvalu(r->beg);
	rl = cvalu(r->len);
	mb = cvalu(mr->beg);
	ml = cvalu(mr->len);
	if(!checkrange(mb, ml, rb, rl))
		vmerr(vm, "address space access out of bounds");
	dat = valstr(argv[2]);
	if(dat->len < rl)
		vmerr(vm, "not enough bytes for address space update");
	memcpy((char*)(uptr)rb, strdata(dat), rl);
	USED(rv);
}

static void
roput(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *dat;
	Range *r, *mr;
	Imm rb, rl, mb, ml;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to put method");
	checkarg(vm, argv, 1, Qrange);
	checkarg(vm, argv, 2, Qstr);
	mr = valrange(disp[0]);
	r = valrange(argv[1]);
	rb = cvalu(r->beg);
	rl = cvalu(r->len);
	mb = cvalu(mr->beg);
	ml = cvalu(mr->len);
	if(!checkrange(mb, ml, rb, rl))
		vmerr(vm, "address space access out of bounds");
	dat = valstr(argv[2]);
	if(dat->len < rl)
		vmerr(vm, "not enough bytes for address space update");

	vmerr(vm, "memory access fault");
	USED(rv);
}

static void
masmap(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Vec *v;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to map method");
	USED(argv);
	v = mkvec(1);
	_vecset(v, 0, disp[0]);
	*rv = mkvalvec(v);
}

static void
masismapped(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Range *r, *mr;
	Imm rb, rl, mb, ml;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to ismapped method");
	checkarg(vm, argv, 1, Qrange);
	mr = valrange(disp[0]);
	r = valrange(argv[1]);
	rb = cvalu(r->beg);
	rl = cvalu(r->len);
	mb = cvalu(mr->beg);
	ml = cvalu(mr->len);
	if(!checkrange(mb, ml, rb, rl))
		*rv = mkvalcval2(cval0);
	else
		*rv = mkvalcval2(cval1);
}

static As*
mkmas(VM *vm, Range *r, unsigned x)
{
	Tab *mtab;
	As *as;

	mtab = mktab();
	tabput(mtab, mkvalstr(mkstr0("get")),
		mkvalcl(mkccl("masget", masget, 1, mkvalrange(r))));
	if(x&2) {
		tabput(mtab, mkvalstr(mkstr0("put")),
			mkvalcl(mkccl("roput", roput, 1, mkvalrange(r))));
	} else {
		tabput(mtab, mkvalstr(mkstr0("put")),
			mkvalcl(mkccl("masput", masput, 1, mkvalrange(r))));
	}
	tabput(mtab, mkvalstr(mkstr0("map")),
		mkvalcl(mkccl("masmap", masmap, 1, mkvalrange(r))));
	tabput(mtab, mkvalstr(mkstr0("ismapped")),
		mkvalcl(mkccl("masmap", masismapped, 1, mkvalrange(r))));
	as = mkastab(mtab, 0);
	/* check if asked for intersection with managed
	   range, but *after* any potential heap churn
	   of this call. */
	if((x&1) && ismanagedrange((void*)(uptr)cvalu(r->beg), cvalu(r->len)))
		vmerr(vm, "range includes managed address space");
	return as;
}

As*
mkzas(Imm len)
{
	return mksas(mkstrn(len));
}

/* call looktype operator of NS on ctype T.
   FIXME: consider adding a new return parameter that,
   if result is 0, points to the undefined type name */
static Ctype*
_dolooktype(VM *vm, Ctype *t, Ns *ns)
{
	Val argv[2], rv;
	Ctype *sub, *tmp;
	Vec *v1, *v2;
	Imm i;
	Rkind rep;
	Ctypearr *ta;
	Ctypefunc *tf;
	Ctypebitfield *tw;

	switch(t->tkind) {
	case Tvoid:
		return mkctypevoid();
	case Tbase:
		return ns->base[typecbase(t)];
	case Ttypedef:
	case Tstruct:
	case Tunion:
	case Tenum:
		argv[0] = mkvalns(ns);
		argv[1] = mkvalctype(t);
		if(ns->looktype == 0)
			return 0;
		rv = ccall(vm, ns->looktype, 2, argv);
		if(Vkind(rv) == Qnil)
			return 0;
		return valctype(rv);
	case Tptr:
		rep = typerep(ns->base[Vptr]);
		sub = _dolooktype(vm, subtype(t), ns);
		if(sub == 0)
			return 0;
		return mkctypeptr(sub, rep);
	case Tarr:
		sub = _dolooktype(vm, subtype(t), ns);
		if(sub == 0)
			return 0;
		ta = (Ctypearr*)t;
		return mkctypearr(sub, ta->cnt);
	case Tfun:
		sub = _dolooktype(vm, subtype(t), ns);
		if(sub == 0)
			return 0;
		tf = (Ctypefunc*)t;
		v1 = mkvec(tf->param->len);
		for(i = 0; i < tf->param->len; i++) {
			v2 = veccopy(valvec(vecref(tf->param, i)));
			vecset(v1, i, mkvalvec(v2));
			tmp = _dolooktype(vm,
					  valctype(vecref(v2, Typepos)), ns);
			if(tmp == 0)
				return 0;
			vecset(v2, Typepos, mkvalctype(tmp));
		}
		return mkctypefunc(sub, v1);
	case Tundef:
		/* FIXME: do we want this? */
		return _dolooktype(vm, subtype(t), ns);
	case Tconst:
		vmerr(vm, "looktype is undefined on enumeration constants");
	case Tbitfield:
		sub = _dolooktype(vm, subtype(t), ns);
		if(sub == 0)
			return 0;
		tw = (Ctypebitfield*)t;
		return mkctypebitfield(sub, tw->cnt, tw->bit0);
	}
	bug();
}

/* NS must have initialized base cache */
static Ctype*
dolooktype(VM *vm, Ctype *t, Ns *ns)
{
	switch(t->tkind) {
	case Tvoid:
		return mkctypevoid();
	case Tbase:
		return ns->base[typecbase(t)];
	default:
		return _dolooktype(vm, t, ns);
	}
}

static void
nscache1base(VM *vm, Ns *ns, Cbase cb)
{
	Val rv, argv[2];
	argv[0] = mkvalns(ns);
	argv[1] = mkvalctype(mkctypebase(cb, Rundef));
	rv = ccall(vm, ns->looktype, 2, argv);
	if(Vkind(rv) == Qnil)
		vmerr(vm, "name space does not define %s", cbasename[cb]);
	ns->base[cb] = valctype(rv);
}

static void
nscachebase(VM *vm, Ns *ns)
{
	Cbase cb;
	Val rv, argv[2];

	for(cb = Vlo; cb < Vnbase; cb++)
		nscache1base(vm, ns, cb);

	/* use result of looktype(void*) to define Vptr base */
	argv[0] = mkvalns(ns);
	argv[1] = mkvalctype(mkctypeptr(mkctypevoid(), Rundef));
	rv = ccall(vm, ns->looktype, 2, argv);
	if(Vkind(rv) == Qnil) {
		argv[1] = mkvalctype(mkctypebase(Vptr, Rundef));
		rv = ccall(vm, ns->looktype, 2, argv);
	}
	if(Vkind(rv) == Qnil)
		vmerr(vm, "name space does not define void*");
	ns->base[Vptr] = mkctypebase(Vptr, typerep(valctype(rv)));
}

/* enumsym for namespaces constructed by @names */
static void
stdenumsym(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to enumsym");
	USED(argv);
	*rv = disp[0];
}

/* enumtype for namespaces constructed by @names */
static void
stdenumtype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to enumtype");
	USED(argv);
	*rv = disp[0];
}

/* looksym for namespaces constructed by @names */
static void
stdlooksym(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Tab *sym;
	Val vp;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looksym");
	checkarg(vm, argv, 1, Qcid);
	sym = valtab(disp[0]);
	vp = tabget(sym, argv[1]);
	if(vp)
		*rv = vp;
}

/* looktype for namespaces constructed by @names */
static void
stdlooktype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Tab *type;
	Val vp;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looktype");
	checkarg(vm, argv, 1, Qctype);
	type = valtab(disp[0]);
	vp = tabget(type, argv[1]);
	if(vp)
		*rv = vp;
}

/* lookaddr for namespaces constructed by @names */
static void
stdlookaddr(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	List *l;
	Cval *cv, *a;
	Imm addr, m, i, b, n;
	Vec *sym;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to lookaddr");
	checkarg(vm, argv, 1, Qcval);
	l = vallist(disp[0]);
	cv = valcval(argv[1]);
	addr = cvalu(cv);
	n = listlen(l);
	if(n == 0) {
		*rv = Xnil;
		return;
	}
	sym = valvec(listref(l, 0));
	a = valcval(attroff(vecref(sym, Attrpos)));
	if(cvalu(a) > addr) {
		*rv = Xnil;
		return;
	}

	/* binary search */
	b = 0;
	while(1) {
		i = n/2;
		m = b+i;
		if(i == 0)
			break;
		sym = valvec(listref(l, m));
		a = valcval(attroff(vecref(sym, Attrpos)));
		if(addr < cvalu(a))
			n = i;
		else{
			b = m;
			n = n-i;
		}
	}
	sym = valvec(listref(l, m));
	*rv = mkvalvec(sym);
}

typedef
struct NSctx {
	Ns *ons;		/* name space being extended */
	Tab *otype, *osym;	/* bindings passed to @names */
	Tab *rawtype, *rawsym;	/* @names declarations */
	Tab *type, *sym;	/* resulting bindings */
	Tab *undef;		/* undefined type names */
	Rkind ptrrep;		/* pointer representation */
} NSctx;

static Ctype* resolvetypename(VM *vm, Ctype *t, NSctx *ctx);

static Ctype*
resolvetid(VM *vm, Val tv, NSctx *ctx)
{
	Val rv;
	Ctype *t, *new, *def;
	Cid *id;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, tv);
	if(rv)
		return valctype(rv);

	/* do we have an unprocessed definition for the type? */
	rv = tabget(ctx->rawtype, tv);
	if(rv) {
		if(Vkind(rv) != Qctype)
			vmerr(vm, "invalid raw type table");
		t = valctype(tv);
		new = mkctypedef(typetid(t), 0);

		/* bind before recursive resolve call to stop cycles */
		tabput(ctx->type, tv, mkvalctype(new));

		def = valctype(rv);
		if(def->tkind != Ttypedef)
			vmerr(vm, "invalid typedef in raw type table");
		setsubtype(new, resolvetypename(vm, subtype(def), ctx));
		if(subtype(new) == new) {
			id = typetid(def);
			vmerr(vm, "circular definition in name space: "
			      "typedef %.*s",
			      (int)id->len, ciddata(id));
		}
		return new;
	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, tv);
	if(rv) {
		tabput(ctx->type, tv, rv);
		return valctype(rv);
	}

	tabput(ctx->undef, tv, tv);
	return mkctypeundef(valctype(tv));
}

/* determine a common type for all constants defined in an enumeration;
   cast the constants to this type in place.  return the type. */
static Ctype*
doenconsts(VM *vm, Vec *v, Ns *ns)
{
	Dom *d;
	Vec *e;
	Imm m;
	Cval *a, *cv;
	Ctype *t;
	Val val;

	/* abstract domain for name space being extended */
	d = mkdom(ns, mknas(), 0);

	/* determine type that contains all constant values */
	a = mkcval(d, d->ns->base[Vint], 0);
	for(m = 0; m < v->len; m++) {
		e = valvec(vecref(v, m)); /* FIXME check sanity */
		a = xcvalalu1dom(vm, Iadd, a,
				 domcastbase(vm, d, valcval(vecref(e, 1))));
	}
	t = a->type;

	/* cast all values to new type */
	for(m = 0; m < v->len; m++) {
		e = valvec(vecref(v, m)); /* FIXME check sanity */
		cv = typecast(vm, t, domcastbase(vm, d, valcval(vecref(e, 1))));
		val = mkvalcval2(cv);
		vecset(e, 1, val);
	}

	return t;
}

static Ctype*
resolvetag(VM *vm, Val tv, NSctx *ctx)
{
	Val rv, attr;
	Vec *vec, *fld, *nfld, *fv;
	Imm i;
	Str *es;
	Ctype *t, *new, *tmp;
	Ctypesu *ts;
	Ctypeenum *te;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, tv);
	if(rv)
		return valctype(rv);

	/* do we have an unprocessed definition for the type? */
	rv = tabget(ctx->rawtype, tv);
	if(rv) {
		if(Vkind(rv) != Qctype)
			vmerr(vm, "invalid raw type table");
		t = valctype(rv);
		switch(t->tkind) {
		case Tstruct:
		case Tunion:
			ts = (Ctypesu*)t;
			fld = ts->field;
			attr = ts->attr;
			if(fld == 0 || Vkind(attr) == Qnil)
				goto error;
			nfld = mkvec(fld->len);
			new = mkctypesu(t->tkind, ts->tag, nfld, attr);

			/* bind before recursive resolve call to stop cycles */
			tabput(ctx->type, tv, mkvalctype(new));

			for(i = 0; i < fld->len; i++) {
				vec = valvec(vecref(fld, i));
				tmp = fieldtype(vec);
				tmp = resolvetypename(vm, tmp, ctx);
				fv = mkvec(3);
				_vecset(fv, Typepos, mkvalctype(tmp));
				_vecset(fv, Idpos, vecref(vec, Idpos));
				_vecset(fv, Attrpos, vecref(vec, Attrpos));
				_vecset(nfld, i, mkvalvec(fv));
			}
			return new;
		case Tenum:
			te = (Ctypeenum*)t;
			if(te->konst == 0)
				goto error;
			tmp = doenconsts(vm, valvec(te->konst), ctx->ons);
			new = mkctypeenum(te->tag, 0, te->konst);
			tabput(ctx->type, tv, mkvalctype(new));
			setsubtype(new, resolvetypename(vm, tmp, ctx));
			return new;
		default:
		error:
			es = fmtctype(t);
			vmerr(vm, "bad definition for tagged type: %.*s",
			      (int)es->len, strdata(es));
		}
	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, tv);
	if(rv) {
		tabput(ctx->type, tv, rv);
		return valctype(rv);
	}

	tabput(ctx->undef, tv, tv);
	return mkctypeundef(valctype(tv));
}

static Ctype*
resolvebase(VM *vm, Val tv, NSctx *ctx)
{
	Val rv;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, tv);
	if(rv)
		return valctype(rv);

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, tv);
	if(rv) {
		tabput(ctx->type, tv, rv);
		return valctype(rv);
	}

	vmerr(vm, "undefined base type %s", cbasename[typecbase(valctype(tv))]);
}

static Ctype*
resolvetypename(VM *vm, Ctype *t, NSctx *ctx)
{
	Vec *vec, *pv, *np;
	Imm i;
	Ctype *tmp;
	Ctypearr *ta;
	Ctypefunc *tf;
	Ctypebitfield *tw;

	switch(t->tkind) {
	case Tvoid:
		return mkctypevoid();
	case Tbase:
		if(typerep(t) != Rundef)
			/* FIXME: can we avoid this if we use a table equality
			   function that does not discriminate on rep? */
			t = mkctypebase(typecbase(t), Rundef);
		return resolvebase(vm, mkvalctype(t), ctx);
	case Tptr:
		return mkctypeptr(resolvetypename(vm, subtype(t), ctx),
				  ctx->ptrrep);
	case Ttypedef:
		return resolvetid(vm, mkvalctype(t), ctx);
	case Tenum:
	case Tstruct:
	case Tunion:
		return resolvetag(vm, mkvalctype(t), ctx);
	case Tbitfield:
		tw = (Ctypebitfield*)t;
		return mkctypebitfield(resolvetypename(vm, subtype(t), ctx),
				       tw->cnt, tw->bit0);
	case Tconst:
		return mkctypeconst(resolvetypename(vm, subtype(t), ctx));
	case Tarr:
		ta = (Ctypearr*)t;
		return mkctypearr(resolvetypename(vm, subtype(t), ctx),
				  ta->cnt);
	case Tfun:
		tf = (Ctypefunc*)t;
		np = mkvec(tf->param->len);
		for(i = 0; i < tf->param->len; i++) {
			vec = valvec(vecref(tf->param, i));
			tmp = paramtype(vec);
			tmp = resolvetypename(vm, tmp, ctx);
			pv = mkvec(2);
			_vecset(pv, Typepos, mkvalctype(tmp));
			_vecset(pv, Idpos, vecref(vec, Idpos));
			_vecset(np, i, mkvalvec(pv));
		}
		return mkctypefunc(resolvetypename(vm, subtype(t), ctx), np);
	case Tundef:
		return mkctypeundef(subtype(t));
	}
	bug();
}

static void
calldispatch(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Closure *dcl;
	Vec *xargv;
	Imm i;

	dcl = valcl(disp[0]);
	xargv = mkvec(1+argc);
	_vecset(xargv, 0, argv[0]); /* this */
	_vecset(xargv, 1, disp[1]); /* name of method to call */
	for(i = 0; i < argc-1; i++)
		_vecset(xargv, i+2, argv[i+1]);
	*rv = ccall(vm, dcl, argc+1, vecdata(xargv));
}

static void
ascache1method(As *as, char *id, Closure **p)
{
	Val v;
	Str *ids;

	ids = mkstr0(id);

	/* is method in method table? */
	v = tabget(as->mtab, mkvalstr(ids));
	if(v) {
		*p = valcl(v);
		return;
	}

	/* no dispatch, no dice */
	if(as->dispatch == 0) {
		*p = 0;
		return;
	}

	/* bind a new caller to dispatch and hope for the best */
	*p = mkccl(id, calldispatch, 2, mkvalcl(as->dispatch), mkvalstr(ids));
}

static void
ascachemethod(As *as)
{
	/* dispatch must be resolved first */
	ascache1method(as, "dispatch", &as->dispatch);

	ascache1method(as, "get", &as->get);
	ascache1method(as, "put", &as->put);
	ascache1method(as, "ismapped", &as->ismapped);
	ascache1method(as, "map", &as->map);
}

static int
validateas(As *as)
{
	if(as->get && as->put && as->ismapped && as->map)
		return 1;
	if(as->dispatch)
		/* FIXME: we don't know if dispatch defines
		   all necessary methods */
		return 1;
	return 0;
}

As*
mkastab(Tab *mtab, Str *name)
{
	As *as;
	as = mkas();
	as->mtab = mtab;
	as->name = name;
	ascachemethod(as);
	return as;
}

static void
nscache1method(Ns *ns, char *id, Closure **p)
{
	Val v;
	Str *ids;

	ids = mkstr0(id);

	/* is method in method table? */
	v = tabget(ns->mtab, mkvalstr(ids));
	if(v) {
		*p = valcl(v);
		return;
	}

	/* no dispatch, no dice */
	if(ns->dispatch == 0) {
		*p = 0;
		return;
	}

	/* bind a new caller to dispatch and hope for the best */
	*p = mkccl(id, calldispatch, 2, mkvalcl(ns->dispatch), mkvalstr(ids));
}

static void
nscachemethod(Ns *ns)
{
	/* dispatch must be resolved first */
	nscache1method(ns, "dispatch", &ns->dispatch);

	nscache1method(ns, "looktype", &ns->looktype);
	nscache1method(ns, "enumtype", &ns->enumtype);
	nscache1method(ns, "looksym", &ns->looksym);
	nscache1method(ns, "enumsym", &ns->enumsym);
	nscache1method(ns, "lookaddr", &ns->lookaddr);
}

static int
validatens(Ns *ns)
{
	if(ns->looktype && ns->enumtype
	   && ns->looksym && ns->enumsym && ns->lookaddr)
		return 1;
	if(ns->dispatch)
		/* FIXME: we don't know if dispatch defines
		   all necessary methods */
		return 1;
	return 0;
}

Ns*
mknstab(Tab *mtab, Str *name)
{
	Ns *ns;
	ns = mkns();
	ns->mtab = mtab;
	ns->name = name;
	nscachemethod(ns);
	return ns;
}

static Ns*
mknsfn(Closure *looktype, Closure *enumtype,
       Closure *looksym, Closure *enumsym,
       Closure *lookaddr, Str *name)
{
	Tab *mtab;
	mtab = mktab();
	tabput(mtab, mkvalstr(mkstr0("looktype")), mkvalcl(looktype));
	tabput(mtab, mkvalstr(mkstr0("enumtype")), mkvalcl(enumtype));
	tabput(mtab, mkvalstr(mkstr0("looksym")), mkvalcl(looksym));
	tabput(mtab, mkvalstr(mkstr0("enumsym")), mkvalcl(enumsym));
	tabput(mtab, mkvalstr(mkstr0("lookaddr")), mkvalcl(lookaddr));
	return mknstab(mtab, name);
}

static Ns*
mknstype(Tab *type, Str *name)
{
	return mknsfn(mkccl("looktype", stdlooktype, 1, mkvaltab(type)),
		      mkccl("enumtype", stdenumtype, 1, mkvaltab(type)),
		      mkcfn("looksym", nilfn),
		      mkccl("enumsym", stdenumsym, 1, mkvaltab(mktab())),
		      mkcfn("lookaddr", nilfn),
		      name);
}

static void
symcmp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *sa, *sb;
	Cval *a, *b;
	USED(vm);
	USED(argc);
	sa = valvec(argv[0]);
	sb = valvec(argv[1]);
	a = valcval(attroff(vecref(sa, Attrpos)));
	b = valcval(attroff(vecref(sb, Attrpos)));
	if(cvalu(a) < cvalu(b))
		*rv = mkvalcval2(cvalminus1);
	else if(cvalu(a) > cvalu(b))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static Ns*
mknstypesym(VM *vm, Tab *type, Tab *sym, Str *name)
{
	Val rv, vp, op;
	Vec *vec, *s;
	List *ls;
	Imm m, len;
	Val argv[2];

	/* create sorted list of symbols with offsets for lookaddr*/
	vec = tabenum(sym);
	len = vec->len/2;
	ls = mklist();
	for(m = 0; m < len; m++) {
		vp = vecref(vec, len+m);	/* symbol #m */
		s = valvec(vp);
		op = vecref(s, Attrpos);
		if(Vkind(op) == Qnil)
			continue;
		_listappend(ls, vp);
	}
	argv[0] = mkvallist(ls);
	argv[1] = mkvalcl(mkcfn("symcmp", symcmp));
	l1_sort(vm, 2, argv, &rv);
	return mknsfn(mkccl("looktype", stdlooktype, 1, mkvaltab(type)),
		      mkccl("enumtype", stdenumtype, 1, mkvaltab(type)),
		      mkccl("looksym", stdlooksym, 1, mkvaltab(sym)),
		      mkccl("enumsym", stdenumsym, 1, mkvaltab(sym)),
		      mkccl("lookaddr", stdlookaddr, 1, mkvallist(ls)),
		      name);
}

Ns*
mknsraw(VM *vm, Ns *ons, Tab *rawtype, Tab *rawsym, Str *name)
{
	Val v, idv, vecv, vp, x, rv, argv[2];
	Vec *vec, *kvec, *nvec;
	Tab *t;
	Ctype *tt, *tmp;
	Ctypeenum *te;
	NSctx ctx;
	u32 i, j;
	Ns *ns;
	Imm m;
	Str *as;
	Val xargv[1];
	Pair *lnk;

	ctx.rawtype = rawtype;
	ctx.rawsym = rawsym;
	ctx.type = mktab();
	ctx.sym = mktab();
	ctx.undef = mktab();
	ctx.ons = ons;
	xargv[0] = mkvalns(ons);
	if(ons->enumtype == 0)
		vmerr(vm, "parent name space does not define enumtype");
	if(ons->enumsym == 0)
		vmerr(vm, "parent name space does not define enumsym");
	ctx.otype = valtab(ccall(vm, ons->enumtype, 1, xargv));
	ctx.osym = valtab(ccall(vm, ons->enumsym, 1, xargv));

	/* get pointer representation from parent name space */
	tt = mkctypebase(Vptr, Rundef);
	vp = tabget(ctx.otype, mkvalctype(tt));
	if(vp == 0)
		vmerr(vm,
		      "derived name space must define pointer representation");
	ctx.ptrrep = typerep(valctype(vp));

	t = ctx.rawtype;
	for(i = 0; i < t->sz; i++) {
		x = vecref(t->ht, i);
		while(islink(x)) {
			lnk = (Pair*)x;
			v = linkkey(lnk);
			x = linknext(lnk);
			if(Vkind(v) != Qctype)
				vmerr(vm, "invalid raw type table");
			resolvetypename(vm, valctype(v), &ctx);
		}
	}

	t = ctx.rawsym;
	for(i = 0; i < t->sz; i++) {
		x = vecref(t->ht, i);
		while(islink(x)) {
			/* id -> [ ctype, id, attr ] */
			lnk = (Pair*)x;
			idv = linkkey(lnk);
			vecv = linkval(lnk);
			x = linknext(lnk);
			if(Vkind(idv) != Qcid)
				vmerr(vm, "invalid raw symbol table");
			if(Vkind(vecv) != Qvec)
				vmerr(vm, "invalid raw symbol table");
			vec = valvec(vecv);
			if(!issym(vec))
				vmerr(vm, "invalid raw symbol table");
			tt = symtype(vec);
			tt = resolvetypename(vm, tt, &ctx);
			if(tt != 0) {
				/* reuse vec */
				vecset(vec, Typepos, mkvalctype(tt));
				tabput(ctx.sym, idv, vecv);
			}
		}
	}

	/* inherit sym and type definitions not shadowed by @names */
	t = ctx.otype;
	for(i = 0; i < t->sz; i++) {
		x = vecref(t->ht, i);
		while(islink(x)) {
			lnk = (Pair*)x;
			vp = linkkey(lnk);
			if(!tabget(ctx.type, vp))
				tabput(ctx.type, vp, linkval(lnk));
			x = linknext(lnk);
		}
	}
	t = ctx.osym;
	for(i = 0; i < t->sz; i++) {
		x = vecref(t->ht, i);
		while(islink(x)) {
			lnk = (Pair*)x;
			vp = linkkey(lnk);
			if(!tabget(ctx.sym, vp))
				tabput(ctx.sym, vp, linkval(lnk));
			x = linknext(lnk);
		}
	}

	/* add enumeration constants to symtab */
	t = ctx.type;
	for(i = 0; i < t->sz; i++) {
		x = vecref(t->ht, i);
		while(islink(x)) {
			lnk = (Pair*)x;
			tt = valctype(linkval(lnk));
			if(tt->tkind != Tenum)
				goto next;
			te = (Ctypeenum*)tt;
			tmp = mkctypeconst(tt);
			vec = valvec(te->konst);
			for(j = 0; j < vec->len; j++) {
				kvec = valvec(vecref(vec, j));
				nvec = mkvec(3);
				_vecset(nvec, Typepos, mkvalctype(tmp));
				_vecset(nvec, Idpos, vecref(kvec, 0));
				_vecset(nvec, Attrpos, mkattr(vecref(kvec, 1)));
				tabput(ctx.sym,
				       vecref(kvec, 0), mkvalvec(nvec));
			}
		next:
			x = linknext(lnk);
		}
	}

	vec = tabenum(ctx.undef);
	m = vec->len/2;
	if(m > 0 && cqctflags['w']) {
		nvec = mkvec(m);
		for(i = 0; i < m; i++)
			vecset(nvec, i, vecref(vec, i));
		argv[0] = mkvalvec(nvec);
		argv[1] = envlookup(vm->top, "typenamecmp");
		l1_sort(vm, 2, argv, &rv);
		xprintf("warning: name space references undefined "
			"type%s:\n", m > 1 ? "s" : "");
		for(i = 0; i < m; i++) {
			vp = vecref(nvec, i);
			as = fmtctype(valctype(vp));
			xprintf("\t%.*s\n", (int)as->len, strdata(as));
		}
	}

	ns = mknstypesym(vm, ctx.type, ctx.sym, name);
	nscachebase(vm, ns);

	return ns;
}

static char*
myroot(void)
{
	u32 x;
	char *p;
	int longsz;
	int ptrsz;
	int flags;
	enum {
		lw = 1<<0,
		lp = 1<<1,
		be = 1<<2,
	};
	static char *root[] = {
		[0] 		= "c32le",
		[lw]		= "c64le",
		[lp|lw]		= "clp64le",
		[be] 		= "c32be",
		[be|lw]		= "c64be",
		[be|lp|lw]	= "clp64be",
	};
	char *r;

	flags = 0;
	longsz = 8*sizeof(long);
	if(longsz == 64)
		flags |= lw;
	ptrsz = 8*sizeof(void*);
	if(ptrsz == 64)
		flags |= lp;
	x = 0x01020304;
	p = (char*)&x;
	if(*p == 0x01)
		flags |= be;

	r = root[flags];
	if(r == 0)
		return "unsupported";
	return r;
}

static void* gotab[Iopmax+1];

void
setgo(Code *c)
{
	Insn *i, *e;
	i = codeinsn(c);
	e = codeend(c);
	for(; i < e; i++)
		i->go = gotab[i->kind];
}

static void
builtincode(Env env, char *name, Code *c)
{
	Val val;
	val = mkvalcode(c);
	envbind(env, name, val);
}

void
builtinnil(Env env, char *name)
{
	envbind(env, name, Xnil);
}

void
builtinfd(Env env, char *name, Fd *fd)
{
	Val val;
	val = mkvalfd(fd);
	envbind(env, name, val);
}

void
builtinfn(Env env, char *name, Closure *cl)
{
	Val val;
	val = mkvalcl(cl);
	envbind(env, name, val);
	if(name[0] == '%')
		envbind(env, name+1, val);
}

void
cqctbuiltinfn(VM* vm, char *name, Closure *cl)
{
	builtinfn(vm->top, name, cl);
}

static void
builtinns(Env env, char *name, Ns *ns)
{
	Val val;
	val = mkvalns(ns);
	envbind(env, name, val);
}

static void
builtindom(Env env, char *name, Dom *dom)
{
	Val val;
	val = mkvaldom(dom);
	envbind(env, name, val);
}

static void
builtintab(Env env, char *name, Tab *tab)
{
	Val val;
	val = mkvaltab(tab);
	envbind(env, name, val);
}

static void
builtincval(Env env, char *name, Cval *cv)
{
	Val val;
	val = mkvalcval2(cv);
	envbind(env, name, val);
}

static void
vmresetctl(VM *vm)
{
	vm->stk = malstack(Maxstk);
	vm->stksz = Maxstk;
	vm->klink = 0;
	vm->fp = vm->stk;
	vm->ac = Xnil;
	vm->cl = 0;
	vm->vc = 0;
	vm->fp[Ocl] = mkvalcl(vabort);
	vm->fp[Ora] = (Val)(uptr)codeentry(vabort->code);
}

void
dogc(VM *vm, u32 g, u32 tg)
{
	Val v;
	u64 b;
	b = usec();
	_gc(g, tg);
	vm->gctime += usec()-b;
	b = usec();
	v = cqctenvlook(vm, "callpostgc");
	if(v && Vkind(v) == Qcl)
		/* we need to preserve the current
		   value of AC.  this dance does so
		   without requiring a special gc-safe
		   save mechanism. */
		vm->ac = ccall(vm, valcl(v), 1, &vm->ac);
	vm->postgctime += usec()-b;
}

Val
__attribute__ ((noclone))
dovm(VM *vm)
{
	Insn *i;
	Val val;
	Imm m;
	Val v1, v2, rv;

#ifdef THREADED
	if(!vm) {
		/* we've been called by initvm.
		   we're only here to initialize gotab. */
		gotab[Iabort] 	= &&Iabort;
		gotab[Iadd]	= &&Iadd;
		gotab[Iaddfp]	= &&Iaddfp;
		gotab[Iand]	= &&Iand;
		gotab[Iapply]	= &&Iapply;
		gotab[Iargc]	= &&Iargc;
		gotab[Ibox]	= &&Ibox;
		gotab[Ibox0]	= &&Ibox0;
		gotab[Icall]	= &&Icall;
		gotab[Icall2]	= &&Icall2;
		gotab[Icallt]	= &&Icallt;
		gotab[Ichkcl]	= &&Ichkcl;
		gotab[Ichkint]	= &&Ichkint;
		gotab[Ichksp]	= &&Ichksp;
		gotab[Icmpeq] 	= &&Icmpeq;
		gotab[Icmpgt] 	= &&Icmpgt;
		gotab[Icmpge] 	= &&Icmpge;
		gotab[Icmplt] 	= &&Icmplt;
		gotab[Icmple] 	= &&Icmple;
		gotab[Icmpneq] 	= &&Icmpneq;
		gotab[Idiv] 	= &&Idiv;
		gotab[Igcpoll] 	= &&Igcpoll;
		gotab[Ihalt] 	= &&Ihalt;
		gotab[Iinv] 	= &&Iinv;
		gotab[Iiret] 	= &&Iiret;
		gotab[Ijmp] 	= &&Ijmp;
		gotab[Ijnz] 	= &&Ijnz;
		gotab[Ijz] 	= &&Ijz;
		gotab[Ikg] 	= &&Ikg;
		gotab[Ikp] 	= &&Ikp;
		gotab[Imod] 	= &&Imod;
		gotab[Imov] 	= &&Imov;
		gotab[Imovvc] 	= &&Imovvc;
		gotab[Imul] 	= &&Imul;
		gotab[Ineg] 	= &&Ineg;
		gotab[Inot] 	= &&Inot;
		gotab[Ior] 	= &&Ior;
		gotab[Inop] 	= &&Inop;
		gotab[Iret] 	= &&Iret;
		gotab[Ishl] 	= &&Ishl;
		gotab[Ishr] 	= &&Ishr;
		gotab[Isub] 	= &&Isub;
		gotab[Iunderflow] 	= &&Iunderflow;
		gotab[Ivargc]	= &&Ivargc;
		gotab[Ixcast] 	= &&Ixcast;
		gotab[Ixor] 	= &&Ixor;
		return 0;
	}
#endif

	vm->level++;
	vm->levgen[vm->level] = vm->gen++;
	setjmp(vm->dovm[vm->level]);
	while(1) {
		NEXTLABEL(i = vm->pc++) {
		LABEL Iabort:
			fatal("vm reached abort instruction");
		LABEL Inop:
			continue;
		LABEL Iinv:
		LABEL Ineg:
		LABEL Inot:
			v1 = getvalrand(vm, &i->op1);
			rv = xunop(vm, i->kind, v1);
			putvalrand(vm, rv, &i->dst);
			continue;
		LABEL Iadd:
		LABEL Iand:
		LABEL Idiv:
		LABEL Imod:
		LABEL Imul:
		LABEL Ior:
		LABEL Ishl:
		LABEL Ishr:
		LABEL Isub:
		LABEL Ixor:
		LABEL Icmplt:
		LABEL Icmple:
		LABEL Icmpgt:
		LABEL Icmpge:
		LABEL Icmpeq:
		LABEL Icmpneq:
			v1 = getvalrand(vm, &i->op1);
			v2 = getvalrand(vm, &i->op2);
			rv = xbinop(vm, i->kind, v1, v2);
			putvalrand(vm, rv, &i->dst);
			continue;
		LABEL Iaddfp:
			vm->fp += i->scnt;
			continue;
		LABEL Ichkcl:
			val = getvalrand(vm, &i->op1);
			if(Vkind(val) != Qcl)
				vmerr(vm, "attempt to call non-procedure");
			continue;
		LABEL Ichkint:
			if(vm->flags&VMirq)
				vmint(vm);
			continue;
		LABEL Ichksp:
			m = getimmrand(&i->op1); /* max frame size */
			checkoverflow(vm, m);
			continue;
		LABEL Imovvc:
			m = getimmrand(&i->op1);
			vm->vc = m;
			continue;
		LABEL Imov:
			v1 = getvalrand(vm, &i->op1);
			putvalrand(vm, v1, &i->dst);
			continue;
		LABEL Iargc:
			m = getimmrand(&i->op1);
			if(vm->vc != m)
				vmerr(vm, "wrong number of arguments to %s"
				      " (expects %u, passed %u)",
				      ciddata(vm->cl->code->id),
				      m, vm->vc);
			continue;
		LABEL Ivargc:
			m = getimmrand(&i->op1);
			if(vm->vc < m)
				vmerr(vm, "not enough arguments to %s"
				      " (expects at least %u, passed %u)",
				      ciddata(vm->cl->code->id),
				      m, vm->vc);
			vargs(vm, m);
			continue;
		LABEL Igcpoll:
			gcpoll(vm);
			continue;
		LABEL Icall2:
			vm->fp += i->scnt;
		LABEL Icall:
			val = getvalrand(vm, &i->op1);
			vm->fp[Ora] = (Val)(uptr)vm->pc;
			vm->cl = valcl(val);
			vcall(vm);
			continue;
		LABEL Icallt:
			val = getvalrand(vm, &i->op1);
			vm->cl = valcl(val);
			vcall(vm);
			continue;
		LABEL Iapply:
			vapply(vm);
			continue;
		LABEL Ihalt:
			/* Ihalt is exactly like Iret... */
			vm->cl = valcl(vm->fp[Ocl]);
			vm->pc = stkp(vm->fp[Ora]);
			/* ... except that it returns from dovm */
			vm->level--;
			return vm->ac;
		LABEL Iret:
			vm->cl = valcl(vm->fp[Ocl]);
			vm->pc = stkp(vm->fp[Ora]);
			continue;
		LABEL Iiret:
			vm->cl = valcl(vm->fp[Ocl]);
			vm->pc = stkp(vm->fp[Ora]);
			vm->fp -= (Imm)(uptr)vm->fp[Oarg0];
			continue;
		LABEL Ijmp:
			vm->pc += i->scnt;
			continue;
		LABEL Ijnz:
			if(!isfalse(getvalrand(vm, &i->op1)))
				vm->pc += i->scnt;
			continue;
		LABEL Ijz:
			if(isfalse(getvalrand(vm, &i->op1)))
				vm->pc += i->scnt;
			continue;
		LABEL Ikg:
			vkcapture(vm);
			continue;
		LABEL Ikp:
			vkresume(vm);
			continue;
		LABEL Iunderflow:
			kunderflow(vm);
			continue;
		LABEL Ibox:
			v1 = getvalrand(vm, &i->op1);
			putvalrand(vm, mkvalbox(v1), &i->op1);
			continue;
		LABEL Ibox0:
			putvalrand(vm, mkvalbox(Xnil), &i->op1);
			continue;
		LABEL Ixcast:
			v1 = getvalrand(vm, &i->op1);
			v2 = getvalrand(vm, &i->op2);
			rv = xxcast(vm, v1, v2);
			putvalrand(vm, rv, &i->dst);
			continue;
		}
	}
}

char*
vmfnid(VM *vm)
{
	return ciddata(vm->cl->code->id);
}

void
checkarg(VM *vm, Val *argv, unsigned arg, Qkind qkind)
{
	if(Vkind(argv[arg]) != qkind)
		vmerr(vm, "operand %d to %s must be a %s",
		      arg+1, vmfnid(vm), qname[qkind]);
}

void
cqctcheckarg(VM *vm, Val *argv, unsigned arg, Qkind qkind)
{
	checkarg(vm, argv, arg, qkind);
}

int
isbasecval(Cval *cv)
{
	Ctype *t;
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		return 0;
	return 1;
}

int
isnegcval(Cval *cv)
{
	Ctype *t;
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		return 0;
	if(isunsigned[typecbase(t)])
		return 0;
	return (s64)cvalu(cv) < 0;
}

int
isnatcval(Cval *cv)
{
	Ctype *t;
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		return 0;
	if(isunsigned[typecbase(t)])
		return 1;
	return (s64)cvalu(cv) >= 0;
}

int
iszerocval(Cval *cv)
{
	return cvalu(cv) == 0;
}

static int
docmp(VM *vm, Val a, Val b, Closure *cmp)
{
	Val argv[2], rv;
	Cval *cv;

	argv[0] = a;
	argv[1] = b;
	rv = ccall(vm, cmp, 2, argv);
	if(Vkind(rv) != Qcval)
		vmerr(vm, "comparison function must return an integer cvalue");
	cv = valcval(rv);
	if(!isbasecval(cv))
		vmerr(vm, "comparison function must return an integer cvalue");
	if(isnegcval(cv))
		return -1;
	if(iszerocval(cv))
		return 0;
	return 1;
}

static void
doswap(Val *vs, Imm i, Imm j)
{
	Val t;
	t = vs[i];
	vs[i] = vs[j];
	vs[j] = t;
}

static void
dosort(VM *vm, Val *vs, Imm n, Closure *cmp)
{
	Val pv;
	Imm lo, hi, p;
	if(n < 2)
		return;

	lo = 0;
	hi = n;
	p = n>>1;		/* weak pivot */
	doswap(vs, p, lo);
	pv = vs[0];
	while(1) {
		do
			lo++;
		while(lo < n && docmp(vm, vs[lo], pv, cmp) < 0);
		do
			hi--;
		while(hi && docmp(vm, vs[hi], pv, cmp) > 0);
		if(hi < lo)
			break;
		doswap(vs, lo, hi);
	}
	doswap(vs, 0, hi);
	dosort(vm, vs, hi, cmp);
	dosort(vm, vs+hi+1, n-hi-1, cmp);
}

static void
l1_sort(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Vec *v;
	Val *vs, o;
	Closure *cmp;
	Imm n;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to sort");
	if(Vkind(argv[0]) != Qlist && Vkind(argv[0]) != Qvec)
		vmerr(vm, "operand 1 to sort must a list or vector");
	checkarg(vm, argv, 1, Qcl);
	cmp = valcl(argv[1]);
	o = argv[0];
	switch(Vkind(o)) {
	case Qlist:
		l = vallist(o);
		n = l->t-l->h;
		vs = &listdata(l)[l->h];
		break;
	case Qvec:
		v = valvec(o);
		n = v->len;
		vs = vecdata(v);
		break;
	default:
		return;
	}
	if(n < 2) {
		*rv = o;
		return;
	}
	dosort(vm, vs, n, cmp);
	*rv = o;
}

static Val
dobsearch(VM *vm, Val key, Val *vs, Imm n, Closure *cmp)
{
	Imm m, i, b;
	int rv;

	b = 0;
	while(n > 1) {
		i = n/2;
		m = b+i;
		rv = docmp(vm, key, vs[m], cmp);
		if(rv == 0)
			return vs[m];
		if(rv < 0)
			n = i;
		else{
			b = m;
			n = n-i;
		}
	}
	if(n && docmp(vm, key, vs[b], cmp) == 0)
		return vs[b];
	return Xnil;
}

static void
l1_bsearch(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Vec *v;
	Val *vs;
	Closure *cmp;
	Imm n;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to bsearch");
	if(Vkind(argv[1]) != Qlist && Vkind(argv[1]) != Qvec)
		vmerr(vm, "operand 2 to bsearch must a list or vector");
	checkarg(vm, argv, 2, Qcl);
	cmp = valcl(argv[2]);
	switch(Vkind(argv[1])) {
	case Qlist:
		l = vallist(argv[1]);
		n = l->t-l->h;
		vs = &listdata(l)[l->h];
		break;
	case Qvec:
		v = valvec(argv[1]);
		n = v->len;
		vs = vecdata(v);
		break;
	default:
		return;
	}
	*rv = dobsearch(vm, argv[0], vs, n, cmp);
}

static void
l1_looktype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	Ctype *t;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looktype");

	ns = 0;
	if(Vkind(argv[0]) == Qns)
		ns = valns(argv[0]);
	else if(Vkind(argv[0]) == Qdom) {
		dom = valdom(argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to looktype must be a namespace or domain");

	if(Vkind(argv[1]) != Qctype)
		vmerr(vm, "operand 2 to looktype must be a typename");
	t = valctype(argv[1]);
	t = dolooktype(vm, t, ns);
	if(t)
		*rv = mkvalctype(t);
}

static void
l1_looksym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looksym");

	ns = 0;
	if(Vkind(argv[0]) == Qns)
		ns = valns(argv[0]);
	else if(Vkind(argv[0]) == Qdom) {
		dom = valdom(argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to looksym must be a namespace or domain");

	if(Vkind(argv[1]) != Qcid)
		vmerr(vm, "operand 2 to looksym must be an identifier");
	*rv = ccall(vm, ns->looksym, argc, argv);
}

static void
l1_domof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to domof");
	arg0 = argv[0];
	if(Vkind(arg0) == Qcval) {
		cv = valcval(arg0);
		*rv = mkvaldom(cv->dom);
	}else if(Vkind(arg0) == Qstr)
		*rv = mkvaldom(litdom);
	else
		vmerr(vm,
		      "operand 1 to domof must be a cvalue or string");
}

int
ischarcval(Cval *cv)
{
	Ctype *t;
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		return 0;
	if(typecbase(t) != Vchar && typecbase(t) != Vuchar)
		return 0;
	return 1;
}

int
isstrcval(Cval *cv)
{
	Ctype *t;
	t = chasetype(cv->type);
	if(t->tkind != Tptr)
		return 0;
	t = chasetype(subtype(t));
	if(t->tkind != Tbase)
		return 0;
	if(typecbase(t) != Vchar && typecbase(t) != Vuchar)
		return 0;
	return 1;
}

Str*
valstrorcval(VM *vm, char *fn, Val *argv, unsigned arg)
{
	Cval *cv;
	if(Vkind(argv[arg]) == Qstr)
		return valstr(argv[arg]);
	else if(Vkind(argv[arg]) == Qcval) {
		cv = valcval(argv[arg]);
		if(isstrcval(cv))
			return stringof(vm, cv);
	}
	vmerr(vm, "operand %d to %s must be a string, char* cvalue, "
	      "or uchar* cvalue",
	      arg+1, fn);
	return 0; /* not reached */
}

static Str*
valstrorcvalornil(VM *vm, char *fn, Val *argv, unsigned arg)
{
	Cval *cv;
	if(Vkind(argv[arg]) == Qstr)
		return valstr(argv[arg]);
	else if(Vkind(argv[arg]) == Qcval) {
		cv = valcval(argv[arg]);
		if(isstrcval(cv))
			return stringof(vm, cv);
	}else if(Vkind(argv[arg]) == Qnil)
		return 0;
	vmerr(vm, "operand %d to %s must be a string, char* cvalue, "
	      "uchar* cvalue, or nil",
	      arg+1, fn);
	return 0; /* not reached */
}

void
callput(VM *vm, As *as, Imm off, Imm len, Str *s)
{
	Val argv[3];

	argv[0] = mkvalas(as);
	argv[1] = mkvalrange2(mkcval(litdom, litdom->ns->base[Vptr], off),
			      mkcval(litdom, litdom->ns->base[Vptr], len));
	argv[2] = mkvalstr(s);
	if(s->len < len)
		vmerr(vm, "attempt to put short string into longer range");
	ccall(vm, as->put, 3, argv);
}

Str*
callget(VM *vm, As *as, Imm off, Imm len)
{
	Val rv, argv[2];
	Str *s;

	argv[0] = mkvalas(as);
	argv[1] = mkvalrange2(mkcval(litdom, litdom->ns->base[Vptr], off),
			      mkcval(litdom, litdom->ns->base[Vptr], len));
	rv = ccall(vm, as->get, 2, argv);
	if(Vkind(rv) != Qstr)
		vmerr(vm, "get method returned non-string");
	s = valstr(rv);
	if(s->len != len)
		vmerr(vm, "get method returned wrong number of bytes");
	return s;
}

Str*
getbytes(VM *vm, Cval *addr, Imm n)
{
	return callget(vm, addr->dom->as, cvalu(addr), n);
}

Vec*
callmap(VM *vm, As *as)
{
	Val argv[1], rv;
	argv[0] = mkvalas(as);
	rv = ccall(vm, as->map, 1, argv);
	if(Vkind(rv) != Qvec)
		vmerr(vm, "map method returned invalid value");
	return valvec(rv);
}

Cval*
callismapped(VM *vm, As *as, Imm off, Imm len)
{
	Val argv[2], rv;
	argv[0] = mkvalas(as);
	argv[1] = mkvalrange2(mkcval(litdom, litdom->ns->base[Vptr], off),
			      mkcval(litdom, litdom->ns->base[Vptr], len));
	rv = ccall(vm, as->ismapped, 2, argv);
	if(Vkind(rv) != Qcval)
		vmerr(vm, "ismapped method returned invalid value");
	return valcval(rv);
}

Range*
mapstab(VM *vm, Vec *map, Imm addr, Imm len)
{
	Imm m;
	Val rp;
	Range *r;

	for(m = 0; m < map->len; m++) {
		rp = vecref(map, m);
		if(Vkind(rp) != Qrange)
			vmerr(vm, "address space map returned invalid value");
		r = valrange(rp);
		if(cvalu(r->beg) > addr)
			return 0;
		if(cvalu(r->beg)+cvalu(r->len) >= addr+len)
			return r;
	}
	return 0;
}

/* assume CV has been vetted by isstrcval */
/* FIXME: we don't trust map, so we stride
   the reads by PAGESZ and verify each page
   with ismapped.
   we should not be doing this.
   we should have reliable maps.
   but we don't. */
Str*
stringof(VM *vm, Cval *cv)
{
	Str *s;
	char *buf, *q;
	Vec *v;
	Range *r;
	Imm l, m, n, o, oo;

	/* effectively a call to unit */
	v = callmap(vm, cv->dom->as);
	r = mapstab(vm, v, cvalu(cv), 0);	/* FIXME: type sanity */
	if(r == 0)
		vmerr(vm, "address space access out of bounds");

	l = 0;
	m = cvalu(r->beg)+cvalu(r->len)-cvalu(cv);
	oo = o = cvalu(cv);
	buf = 0;
	n = MIN(m, PAGESZ-o%PAGESZ);
	if(!ismapped(vm, cv->dom->as, o, 1))
		vmerr(vm, "address space access out of bounds");
	while(m > 0) {
		s = callget(vm, cv->dom->as, o, n);
		buf = strdata(s);
		q = strnchr(buf, '\0', s->len);
		if(q) {
			l += q-buf;
			break;
		}
		l += s->len;
		o += s->len;
		m -= s->len;
		n = MIN(m, PAGESZ);
		if(!ismapped(vm, cv->dom->as, o, 1))
			break;
	}
	s = callget(vm, cv->dom->as, oo, l);
	return s;
}

int
ismapped(VM *vm, As *as, Imm addr, Imm len)
{
	Cval *cv;
	cv = callismapped(vm, as, addr, len);
	return cvalu(cv) != 0;
}

static void
l1_kbacktrace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to kbacktrace");
	checkarg(vm, argv, 0, Qcont);
	kbacktrace(vm, valcont(argv[0]));
}

static void
l1_stackframes(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stackframes");
	checkarg(vm, argv, 0, Qcont);
	*rv = mkvallist(stackframes(vm, valcont(argv[0])));
}

static void
l1_liveinframe(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to liveinframe");
	checkarg(vm, argv, 0, Qcont);
	checkarg(vm, argv, 1, Qcval);
	*rv = mkvalvec(liveinframe(vm, valcont(argv[0]), cvalu(valcval(argv[1]))));
}

Val
myrootns(Env env)
{
	char *r;
	Val n;

	r = myroot();
	n = envlookup(env, r);
	if(n == 0)
		return Xnil;
	else
		return n;
}

static void
l1_myrootns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(argc);
	USED(argv);
	*rv = myrootns(vm->top);
	if(*rv == Xnil)
		vmerr(vm, "my root name space is undefined: %s", myroot());
}

/* the purpose of typeof on types is to strip the
   enum Tconst and bitfield Tbitfield status from
   lvalue expressions passed to typeof.
   for example: if A is a const of enum X in DOM,
   then we ensure that
   	typeof(dom`A) == typeof(x = dom`A) == enum X
   otherwise, typeof(dom`A) would be Tconst(enum X)

   FIXME:  perhaps this should be generalized
           so that typeof(dom`A) == typeof(x = dom`A).

   e.g.,
	currently, given
		int a[3];
	that
		typeof(a) is int[3]
*/
static void
l1_typeof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Ctype *t;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to $typeof");
	t = 0;
	if(Vkind(argv[0]) == Qcval) {
		cv = valcval(argv[0]);
		t = cv->type;
	}else if(Vkind(argv[0]) == Qctype)
		t = valctype(argv[0]);
	else
		vmerr(vm, "operand 1 to $typeof must be a cvalue or type");
	*rv = mkvalctype(t);
}

static void
l1_isnil(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isnil");
	if(Vkind(argv[0]) == Qnil)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_error(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	l1_sprintfa(vm, argc, argv, rv);
	s = valstr(*rv);
	vmerr(vm, "%.*s", (int)s->len, strdata(s));
}

static void
l1_fault(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(argc);
	USED(argv);
	vmerr(vm, "memory access fault");
	USED(rv);
}

static void
l1_put(VM *vm, Imm argc, Val *iargv, Val *rv)
{
	Dom *d;
	Ctype *t, *b, *s;
	Cval *addr, *cv;
	Str *bytes, *es;
	BFgeom bfg;
	Imm imm;

	if(argc != 4)
		vmerr(vm, "wrong number of arguments to put");
	checkarg(vm, iargv, 0, Qdom);
	checkarg(vm, iargv, 1, Qcval);
	checkarg(vm, iargv, 2, Qctype);
	checkarg(vm, iargv, 3, Qcval);
	d = valdom(iargv[0]);
	addr = valcval(iargv[1]);
	t = valctype(iargv[2]);
	cv = valcval(iargv[3]);
	b = chasetype(t);
	switch(b->tkind) {
	case Tbase:
	case Tptr:
		cv = typecast(vm, t, cv);
		bytes = enc2str(vm, t, cvalenc(cv));
		callput(vm, d->as, cvalu(addr), typesize(vm, t), bytes);
		*rv = mkvalcval2(cv);
		break;
	case Tbitfield:
		s = subtype(b);
		if(s->tkind == Tundef) {
			es = fmtctype(subtype(s));
			vmerr(vm, "attempt to write object of undefined type: "
			      "%.*s", (int)es->len, strdata(es));
		}
		if(0 > dobitfieldgeom(b, &bfg))
			vmerr(vm, "invalid bitfield access");

		/* get contents of bitfield container */
		bytes = callget(vm, d->as, cvalu(addr)+bfg.addr, bfg.cnt);

		/* update bitfield container */
		imm = bitfieldput(strdata(bytes), &bfg, cvalu(cv));

		/* put updated bitfield container */
		callput(vm, d->as, cvalu(addr)+bfg.addr, bfg.cnt, bytes);

		/* return value of bitfield (not container) */
		*rv = mkvalcval(d, s, imm);
		break;
	case Tconst:
		vmerr(vm, "attempt to use enumeration constant as location");
		break;
	case Tundef:
		es = fmtctype(subtype(b));
		vmerr(vm,
		      "attempt to write object of undefined type: %.*s",
		      (int)es->len, strdata(es));
	case Tenum:
	case Ttypedef:
	case Tvoid:
	case Tstruct:
	case Tunion:
	case Tfun:
	case Tarr:
		vmerr(vm,
		      "attempt to write %s-valued object to address space",
		      tkindstr[b->tkind]);
	}
}

static void
l1_mknas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mknas");
	USED(argv);
	as = mknas();
	*rv = mkvalas(as);
}

static void
l1_mksas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Str *s;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mksas");
	checkarg(vm, argv, 0, Qstr);
	s = valstr(argv[0]);
	as = mksas(s);
	*rv = mkvalas(as);
}

static void
domkmas(VM *vm, char *fn, Imm argc, Val *argv, Val *rv, unsigned x)
{
	As *as;
	Cval *p, *l;
	Range *r;
	switch(argc) {
	case 0:
		r = mkrange(mklitcval(Vptr, 0), mklitcval(Vptr, (uptr)-1));
		break;
	case 2:
		checkarg(vm, argv, 0, Qcval);
		checkarg(vm, argv, 1, Qcval);
		p = valcval(argv[0]);
		l = valcval(argv[1]);
		if(cvalu(l) && cvalu(p)+cvalu(l) <= cvalu(p))
			vmerr(vm, "bad range for %s", fn);
		r = mkrange(p, l);
		break;
	default:
		vmerr(vm, "wrong number of arguments to %s", fn);
	}
	as = mkmas(vm, r, x);
	*rv = mkvalas(as);
}

static void
l1_mkmas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	domkmas(vm, "mkmas", argc, argv, rv, 0);
}

static void
l1_mkmasro(VM *vm, Imm argc, Val *argv, Val *rv)
{
	domkmas(vm, "mkmasro", argc, argv, rv, 2);
}

static void
l1_mkmasx(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mkmasx");
	domkmas(vm, "mkmasx", argc, argv, rv, 1);
}

static void
l1_mkzas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Cval *cv;
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkzas");
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		vmerr(vm, "operand 1 to mkzas must be an integer cvalue");
	as = mkzas(cvalu(cv));
	*rv = mkvalas(as);
}

static void
l1_mkas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Str *name;
	Tab *mtab;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkas");
	checkarg(vm, argv, 0, Qtab);
	name = 0;
	if(argc == 2) {
		checkarg(vm, argv, 1, Qstr);
		name = valstr(argv[1]);
	}
	mtab = valtab(argv[0]);
	as = mkastab(mtab, name);
	if(!validateas(as))
		vmerr(vm, "incompletely defined address space");
	*rv = mkvalas(as);
}

static void
l1_mknsraw(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ns *ons, *ns;
	Tab *rawtype, *rawsym;
	Str *name;

	if(argc != 3 && argc != 4)
		vmerr(vm, "wrong number of arguments to mknsraw");
	checkarg(vm, argv, 0, Qns);
	checkarg(vm, argv, 1, Qtab);
	checkarg(vm, argv, 2, Qtab);
	name = 0;
	if(argc == 4) {
		checkarg(vm, argv, 3, Qstr);
		name = valstr(argv[3]);
	}
	ons = valns(argv[0]);
	rawtype = valtab(argv[1]);
	rawsym = valtab(argv[2]);
	ns = mknsraw(vm, ons, rawtype, rawsym, name);
	*rv = mkvalns(ns);
}

static void
l1_mkattr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkattr");
	if(Vkind(argv[0]) != Qcval
	   && Vkind(argv[0]) != Qtab
	   && Vkind(argv[0]) != Qnil)
		vmerr(vm, "argument 1 to mkattr must be a table, cvalue, "
		      "or nil");
	*rv = mkattr(argv[0]);
}

static void
l1_mkns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ns *ns;
	Str *name;
	Tab *mtab;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkns");
	checkarg(vm, argv, 0, Qtab);
	name = 0;
	if(argc == 2) {
		checkarg(vm, argv, 1, Qstr);
		name = valstr(argv[1]);
	}
	mtab = valtab(argv[0]);
	ns = mknstab(mtab, name);
	nscachebase(vm, ns);
	if(!validatens(ns))
		vmerr(vm, "incompletely defined name space");
	*rv = mkvalns(ns);
}

static void
l1_mkdom(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Ns *ns;
	Dom *dom;
	Str *name;

	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to mkdom");
	checkarg(vm, argv, 0, Qns);
	checkarg(vm, argv, 1, Qas);
	name = 0;
	if(argc == 3) {
		checkarg(vm, argv, 2, Qstr);
		name = valstr(argv[2]);
	}
	ns = valns(argv[0]);
	as = valas(argv[1]);
	dom = mkdom(ns, as, name);
	*rv = mkvaldom(dom);
}

static void
l1_nameof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	As *as;
	Str *name;

	name = 0;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nameof");
	if(Vkind(argv[0]) == Qdom) {
		dom = valdom(argv[0]);
		name = dom->name;
	}else if(Vkind(argv[0]) == Qns) {
		ns = valns(argv[0]);
		name = ns->name;
	}else if(Vkind(argv[0]) == Qas) {
		as = valas(argv[0]);
		name = as->name;
	}else
		vmerr(vm, "operand 1 to nameof must be a domain, name space"
		      " or address space");
	if(name)
		*rv = mkvalstr(name);
}

static void
l1_asof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to asof");
	dom = 0;
	if(Vkind(argv[0]) == Qdom)
		dom = valdom(argv[0]);
	else if(Vkind(argv[0]) == Qcval) {
		cv = valcval(argv[0]);
		dom = cv->dom;
	}else
		vmerr(vm, "operand 1 to asof must be a domain or cvalue");
	*rv = mkvalas(dom->as);
}

static void
l1_nsof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Cval *cv;

	dom = 0;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsof");
	if(Vkind(argv[0]) == Qdom)
		dom = valdom(argv[0]);
	else if(Vkind(argv[0]) == Qcval) {
		cv = valcval(argv[0]);
		dom = cv->dom;
	}else
		vmerr(vm, "operand 1 to nsof must be a domain or cvalue");
	*rv = mkvalns(dom->ns);
}

static void
l1_dispatchtab(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to dispatchtab");
	if(Vkind(argv[0]) == Qns) {
		ns = valns(argv[0]);
		*rv = mkvaltab(ns->mtab);
	}else if(Vkind(argv[0]) == Qas) {
		as = valas(argv[0]);
		*rv = mkvaltab(as->mtab);
	}else
		vmerr(vm, "operand 1 to dispatchtab must be a name space "
		      "or address space");
}

static void
l1_nsreptype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	Cval *cv;
	Rkind rep;
	Cbase cb;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to nsreptype");
	checkarg(vm, argv, 1, Qcval);
	if(Vkind(argv[0]) == Qns)
		ns = valns(argv[0]);
	else if(Vkind(argv[0]) == Qdom) {
		dom = valdom(argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to nsreptype must be a namespace or domain");
	cv = valcval(argv[1]);
	rep = cvalu(cv);
	if(rep >= Rnrep)
		vmerr(vm, "invalid representation identifier");
	for(cb = Vlo; cb < Vnallbase; cb++)
		if(typerep(ns->base[cb]) == rep) {
			*rv = mkvalctype(ns->base[cb]);
			return;
		}
}

static void
l1_nsptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsptr");
	arg0 = argv[0];
	if(Vkind(arg0) != Qns && Vkind(arg0) != Qdom)
		vmerr(vm,
		      "operand 1 to nsptr must be a namespace or domain");
	if(Vkind(arg0) == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	*rv = mkvallitcval(Vuchar, typerep(ns->base[Vptr]));
}

static void
l1_mkrange(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mkrange");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	/* FIXME: check sanity */
	*rv = mkvalrange2(valcval(argv[0]), valcval(argv[1]));
}

static void
l1_rangebeg(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Range *r;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rangebeg");
	checkarg(vm, argv, 0, Qrange);
	r = valrange(argv[0]);
	*rv = mkvalcval2(r->beg);
}

static void
l1_rangelen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Range *r;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rangelen");
	checkarg(vm, argv, 0, Qrange);
	r = valrange(argv[0]);
	*rv = mkvalcval2(r->len);
}

static void
l1_memcpy(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *ncv, *scv, *dcv;
	Str *buf;

	if(argc != 3)
		vmerr(vm, "wrong number of argument to memcpy");

	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	checkarg(vm, argv, 2, Qcval);
	dcv = valcval(argv[0]);
	scv = valcval(argv[1]);
	ncv = valcval(argv[2]);
	if(!isptrtype(dcv->type))
		vmerr(vm, "operand 1 to memcpy must be a pointer");
	if(!isptrtype(scv->type))
		vmerr(vm, "operand 2 to memcpy must be a pointer");
	buf = callget(vm, scv->dom->as, cvalu(scv), cvalu(ncv));
	callput(vm, dcv->dom->as, cvalu(dcv), cvalu(ncv), buf);
	USED(rv);
}

static void
l1_stringof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Cval *cv;
	static char *err =
		"operand 1 to stringof must be a "
		"char* or unsigned char* cvalue";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stringof");
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(!isstrcval(cv))
		vmerr(vm, err);
	s = stringof(vm, cv);
	*rv = mkvalstr(s);
}

static void
l1_split(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	char *p, *q, *e, *set;
	Cval *lim;
	Imm n;
	List *r;
	int intok, mflag;

	r = mklist();

	/* subject string */
	if(argc != 1 && argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to split");
	s = valstrorcval(vm, "split", argv, 0);
	p = strdata(s);
	e = strdata(s)+s->len;

	/* split limit */
	lim = 0;
	if(argc == 3) {
		checkarg(vm, argv, 2, Qcval);
		lim = valcval(argv[2]);
		if(!isnatcval(lim))
			vmerr(vm, "split expects a non-negative limit");
		if(cvalu(lim) == 0) {
			*rv = mkvallist(r);
			return;
		}
	}

	/* delimiter set */
	if(argc > 1 && (s = valstrorcvalornil(vm, "split", argv, 1))) {
		set = str2cstr(s);
		mflag = 0;
	}else{
		set = xstrdup(" \t\n");
		mflag = 1;
	}

	if(!mflag) {
		n = 0;
		q = p;
		while(q < e && (!lim || n < cvalu(lim))) {
			if(strchr(set, *q)) {
				listins(vm, r, n++, mkvalstr(mkstr(p, q-p)));
				p = q+1;
				if(p == e)
					listins(vm, r, n++, mkvalstr(mkstrn(0)));
				q = p;
			}else
				q++;
		}
		if(p < e)
			listins(vm, r, n, mkvalstr(mkstr(p, e-p)));
	}else{
		n = 0;
		q = p;
		intok = 0;
		while(q < e && (!lim || n < cvalu(lim))) {
			if(strchr(set, *q)) {
				if(intok)
					listins(vm, r, n++,
						mkvalstr(mkstr(p, q-p)));
				intok = 0;
				p = q+1;
				while(p < e && strchr(set, *p))
					p++;
				if(p >= e) {
					listins(vm, r, n++,
					        mkvalstr(mkstrn(0)));
					break;
				}
				q = p;
			}else{
				q++;
				intok = 1;
			}
		}
		if(intok)
			listins(vm, r, n, mkvalstr(mkstr(p, e-p)));
	}
	efree(set);
	*rv = mkvallist(r);
}

static void
l1_ismapped(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *addr, *len;
	Imm sz;
	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to ismapped");
	checkarg(vm, argv, 0, Qcval);
	addr = valcval(argv[0]);
	sz = 0;
	if(argc == 2) {
		checkarg(vm, argv, 1, Qcval);
		len = valcval(argv[1]);
		if(!isnatcval(len))
			vmerr(vm, "ismapped expects a non-negative length");
		sz = cvalu(len);
	}
	if(sz == 0) {
		if(!isptrtype(addr->type))
			vmerr(vm, "ismapped expects a pointer cvalue");
		sz = typesize(vm, subtype(chasetype(addr->type)));
	}
	if(ismapped(vm, addr->dom->as, cvalu(addr), sz))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static Imm
cntrlen(VM *vm, char *name, Val v)
{
	List *lst;
	Vec *vec;
	Str *str;
	Tab *tab;
	Pair *p;
	Imm len;


	switch(Vkind(v)) {
	default:
		vmerr(vm, "operand 1 to %s must be a container", name);
	case Qlist:
		lst = vallist(v);
		len = listlen(lst);
		break;
	case Qstr:
		str = valstr(v);
		len = str->len;
		break;
	case Qvec:
		vec = valvec(v);
		len = vec->len;
		break;
	case Qtab:
		tab = valtab(v);
		len = tab->nent;
		break;
	case Qnil:
		len = 0;
		break;
	case Qpair:
		len = 0;
		while(Vkind(v) == Qpair) {
			len++;
			p = valpair(v);
			v = p->cdr;
			if(Vkind(v) != Qpair && Vkind(v) != Qnil)
				vmerr(vm, "attempt to determine length "
				      "of improper list");
		}
		break;
	}
	return len;
}

static void
l1_isempty(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm len;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isempty");
	len = cntrlen(vm, "isempty", argv[0]);
	if(len == 0)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_length(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm len;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to length");
	len = cntrlen(vm, "length", argv[0]);
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], len);
}

static void
l1_count(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Vec *vec;
	Str *str;
	Imm len, i, m;
	char c;
	Cval *cv;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to count");
	v = argv[1];
	m = 0;
	switch(Vkind(argv[0])) {
	default:
		vmerr(vm, "operand 1 to count must be a list, string, "
		      "or vector");
	case Qlist:
		lst = vallist(argv[0]);
		len = listlen(lst);
		for(i = 0; i < len; i++)
			if(equalval(v, listref(lst, i)))
				m++;
		break;
	case Qvec:
		vec = valvec(argv[0]);
		len = vec->len;
		for(i = 0; i < len; i++)
			if(equalval(v, vecdata(vec)[i]))
				m++;
		break;
	case Qstr:
		if(Vkind(v) != Qcval)
			vmerr(vm, "operand 2 to count must a character when"
			      " operand 1 is a string");
		cv = valcval(v);
		c = (char)cvalu(cv);
		str = valstr(argv[0]);
		len = str->len;
		for(i = 0; i < len; i++)
			if(c == strdata(str)[i])
				m++;
		break;
	}
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], m);
}

static void
l1_index(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Vec *vec;
	Str *str;
	Imm len, i;
	char c;
	Cval *cv;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to index");
	v = argv[1];
	switch(Vkind(argv[0])) {
	default:
		vmerr(vm, "operand 1 to index must be a list, string, "
		      "or vector");
	case Qlist:
		lst = vallist(argv[0]);
		len = listlen(lst);
		for(i = 0; i < len; i++)
			if(equalval(v, listref(lst, i)))
				goto gotit;
		break;
	case Qvec:
		vec = valvec(argv[0]);
		len = vec->len;
		for(i = 0; i < len; i++)
			if(equalval(v, vecdata(vec)[i]))
				goto gotit;
		break;
	case Qstr:
		if(Vkind(v) != Qcval)
			vmerr(vm, "operand 2 to index must a character when"
			      " operand 1 is a string");
		cv = valcval(v);
		c = (char)cvalu(cv);
		str = valstr(argv[0]);
		len = str->len;
		for(i = 0; i < len; i++)
			if(c == strdata(str)[i])
				goto gotit;
		break;
	}
	return;   /* nil */
gotit:
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], i);
}

static void
l1_ismember(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Vec *vec;
	Str *str;
	Imm len, i;
	char c;
	Cval *cv;
	Tab *tab;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to ismember");
	*rv = mkvalcval2(cval0);
	v = argv[1];
	switch(Vkind(argv[0])) {
	default:
		vmerr(vm, "operand 1 to ismember must be a list, string, "
		      "table, or vector");
	case Qlist:
		lst = vallist(argv[0]);
		len = listlen(lst);
		for(i = 0; i < len; i++)
			if(equalval(v, listref(lst, i)))
				goto gotit;
		break;
	case Qvec:
		vec = valvec(argv[0]);
		len = vec->len;
		for(i = 0; i < len; i++)
			if(equalval(v, vecdata(vec)[i]))
				goto gotit;
		break;
	case Qstr:
		if(Vkind(v) != Qcval)
			vmerr(vm, "operand 2 to ismember must a character when"
			      " operand 1 is a string");
		cv = valcval(v);
		c = (char)cvalu(cv);
		str = valstr(argv[0]);
		len = str->len;
		for(i = 0; i < len; i++)
			if(c == strdata(str)[i])
				goto gotit;
		break;
	case Qtab:
		tab = valtab(argv[0]);
		if(tabget(tab, argv[1]))
			goto gotit;
		break;
	}
	return;
gotit:
	*rv = mkvalcval2(cval1);;
}

static void
l1_delete(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Imm i;
	Tab *tab;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to delete");
	v = argv[1];
	switch(Vkind(argv[0])) {
	default:
		vmerr(vm, "operand 1 to delete must be a list or table");
	case Qlist:
		lst = vallist(argv[0]);
		i = 0;
		while(i < listlen(lst)) {
			if(!equalval(v, listref(lst, i)))
				i++;
			else
				listdel(vm, lst, i);
		}
		break;
	case Qtab:
		tab = valtab(argv[0]);
		tabdel(tab, v);
		break;
	}
	*rv = argv[0];
}

static void
l1_delq(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Imm i;
	Tab *tab;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to delq");
	v = argv[1];
	switch(Vkind(argv[0])) {
	default:
		vmerr(vm, "operand 1 to delq must be a list or table");
	case Qlist:
		lst = vallist(argv[0]);
		i = 0;
		while(i < listlen(lst)) {
			if(!eqval(v, listref(lst, i)))
				i++;
			else
				listdel(vm, lst, i);
		}
		break;
	case Qtab:
		tab = valtab(argv[0]);
		tabdel(tab, v);
		break;
	}
	*rv = argv[0];
}

static void
l1_pop(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to pop");
	arg = argv[0];
	if(Vkind(arg) == Qlist)
		listpop(vallist(argv[0]), rv);
	else if(Vkind(arg) == Qtab)
		tabpop(valtab(argv[0]), rv);
	else
		vmerr(vm, "operand 1 to pop must be a list or table");
}

static void
l1_copy(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to append");
	if(Vkind(argv[0]) == Qlist)
		*rv = mkvallist(listcopy(vallist(argv[0])));
	else if(Vkind(argv[0]) == Qvec)
		*rv = mkvalvec(veccopy(valvec(argv[0])));
	else if(Vkind(argv[0]) == Qtab)
		*rv = mkvaltab(tabcopy(valtab(argv[0])));
	else if(Vkind(argv[0]) == Qstr)
		*rv = mkvalstr(strcopy(valstr(argv[0])));
	else
		vmerr(vm, "operand 1 to copy must be a container");
}

static void
l1_concat(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Str *str;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to concat");
	if(Vkind(argv[0]) == Qlist && Vkind(argv[1]) == Qlist) {
		lst = listconcat(vm, vallist(argv[0]), vallist(argv[1]));
		*rv = mkvallist(lst);
	}else if(Vkind(argv[0]) == Qstr && Vkind(argv[1]) == Qstr) {
		str = strconcat(valstr(argv[0]), valstr(argv[1]));
		*rv = mkvalstr(str);
	}else
		vmerr(vm, "operands to concat must be lists or strings");
}

static void
l1_cntrget(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cntrget");
	switch(Vkind(argv[0])) {
	default:
		vmerr(vm, "operand 1 to cntrget must be a container");
	case Qlist:
		l1_listref(vm, argc, argv, rv);
		break;
	case Qstr:
		l1_strref(vm, argc, argv, rv);
		break;
	case Qvec:
		l1_vecref(vm, argc, argv, rv);
		break;
	case Qtab:
		l1_tablook(vm, argc, argv, rv);
		break;
	case Qexpr:
		l1_stxref(vm, argc, argv, rv);
		break;
	}
}

static void
l1_cntrput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to cntrput");
	switch(Vkind(argv[0])) {
	default:
		vmerr(vm, "operand 1 to cntrput must be a container");
	case Qlist:
		l1_listset(vm, argc, argv, rv);
		*rv = argv[2];
		break;
	case Qstr:
		l1_strput(vm, argc, argv, rv);
		if(Vkind(argv[2]) == Qcval) {
			cv = valcval(argv[2]);
			cv = typecast(vm, cv->dom->ns->base[Vchar], cv);
			*rv = mkvalcval2(cv);
		}else
			*rv = argv[2];
		break;
	case Qvec:
		l1_vecset(vm, argc, argv, rv);
		*rv = argv[2];
		break;
	case Qtab:
		l1_tabinsert(vm, argc, argv, rv);
		*rv = argv[2];
		break;
	case Qexpr:
		vmerr(vm, "attempt to modify syntax record");
		break;
	}
}

static void
l1_unbox(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Box *b;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to unbox");
	checkarg(vm, argv, 0, Qbox);
	b = valbox(argv[0]);
	*rv = b->v;
}

static void
l1_hashq(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to hashq");
	*rv = mkvalcval(litdom, litdom->ns->base[Vuint], hashqval(argv[0]));
}

static void
l1_hashqv(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to hashqv");
	*rv = mkvalcval(litdom, litdom->ns->base[Vuint], hashqvval(argv[0]));
}

static void
l1_hash(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to hash");
	*rv = mkvalcval(litdom, litdom->ns->base[Vuint], hashval(argv[0]));
}

static void
l1_eq(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to eq");
	if(eqval(argv[0], argv[1]))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_eqv(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to eqv");
	if(eqvval(argv[0], argv[1]))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_equal(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to equal");
	if(equalval(argv[0], argv[1]))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isx(VM *vm, Imm argc, Val *argv, Val *rv, char *name, Qkind kind)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", name);
	if(Vkind(argv[0]) == kind)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isas", Qas);
}

static void
l1_iscid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "iscid", Qcid);
}

static void
l1_iscode(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "iscode", Qcode);
}

static void
l1_isctype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isctype", Qctype);
}

static void
l1_iscvalue(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "iscval", Qcval);
}

static void
l1_isdom(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isdom", Qdom);
}

static void
l1_islist(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "islist", Qlist);
}

static void
l1_isns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isns", Qns);
}

static void
l1_ispair(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "ispair", Qpair);
}

static void
l1_isprocedure(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isprocedure", Qcl);
}

static void
l1_isrange(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isrange", Qrange);
}

static void
l1_isrec(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isrec", Qrec);
}

static void
l1_isrd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isrd", Qrd);
}

static void
l1_isstring(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isstring", Qstr);
}

static void
l1_isstx(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isstx", Qexpr);
}

static void
l1_istable(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "istable", Qtab);
}

static void
l1_isvector(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isvector", Qvec);
}

static void
l1_iscallable(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to iscallable");
	if(Vkind(argv[0]) == Qcl
	   || Vkind(argv[0]) == Qas
	   || Vkind(argv[0]) == Qns
	   || Vkind(argv[0]) == Qdom)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_qtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to qtype");
	*rv = mkvalcid(mkcid0(qname[Vkind(argv[0])]));
}

static void
l1_meminuse(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(vm);
	USED(argc);
	USED(argv);
	*rv = mkvallitcval(Vuvlong, cqctmeminuse+meminuse());
}

static void
l1_memtotal(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(vm);
	USED(argc);
	USED(argv);
	*rv = mkvallitcval(Vuvlong, cqctmemtotal);
}

static void
l1_gcpoll(VM *vm, Imm argc, Val *argv, Val *rv)
{
	gcpoll(vm);
}

static void
l1_gc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	u32 g, tg;
	if(argc == 0)
		gc(vm);
	else if(argc == 1) {
		checkarg(vm, argv, 0, Qcval);
		g = cvalu(valcval(argv[0]));
		dogc(vm, g, g+1);
	}else if(argc == 2) {
		checkarg(vm, argv, 0, Qcval);
		checkarg(vm, argv, 1, Qcval);
		g = cvalu(valcval(argv[0]));
		tg = cvalu(valcval(argv[1]));
		if(g != tg && g != tg-1)
			vmerr(vm, "invalid arguments to gc");
		dogc(vm, g, tg);
	}else
		vmerr(vm, "wrong number of arguments to gc");
}

static void
l1_fullgc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	fullgc(vm);
}

static void
l1_compact(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to compact");
	compact(vm);
}

static void
l1_gcstats(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to gcstats");
	gcstats();
}

static void
l1_instguard(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Pair *p;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to instguard");
	checkarg(vm, argv, 0, Qpair);
	p = valpair(argv[0]);
	/* FIXME: we trust that cdr(p) is a valid guardian tconc */
	instguard(p);
}

static void
l1_saveheapfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to saveheapfd");
	checkarg(vm, argv, 0, Qcval);
	fd = cvalu(valcval(argv[0]));
	fullgc(vm);
	saveheapfd(vm->top, fd);
}


static void
l1_resettop(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(argc);
	USED(argv);
	vm->top = mktopenv();
	USED(rv);
}

static void
l1_loadpath(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char **lp;
	List *l;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to loadpath");
	USED(argv);
	l = mklist();
	lp = cqctloadpath;
	while(*lp) {
		listappend(vm, l, mkvalstr(mkstr0(*lp)));
		lp++;
	}
	*rv = mkvallist(l);
}

static void
l1_setloadpath(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char **lp;
	List *l;
	Imm i, m;
	Val v;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to loadpath");
	checkarg(vm, argv, 0, Qlist);
	l = vallist(argv[0]);
	m = listlen(l);
	for(i = 0; i < m; i++) {
		v = listref(l, i);
		if(Vkind(v) != Qstr)
			vmerr(vm, "argument 1 to setloadpath "
			      "must be a list of strings");
	}
	lp = emalloc((m+1)*sizeof(char*));
	for(i = 0; i < m; i++)
		lp[i] = str2cstr(valstr(listref(l, i)));
	efree(cqctloadpath);
	cqctloadpath = copystrv(lp);
	for(i = 0; i < m; i++)
		efree(lp[i]);
	efree(lp);
	USED(rv);
}

static void
l1_parse(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	unsigned line;
	U ctx;
	Str *buf, *whence;

	if(argc != 1 && argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to parse");
	checkarg(vm, argv, 0, Qstr);
	if(argc > 1) {
		checkarg(vm, argv, 1, Qstr);
		whence = valstr(argv[1]);
	}else
		whence = mkstr0("(stdin)");
	if(argc > 2) {
		checkarg(vm, argv, 2, Qcval);
		line = cvalu(valcval(argv[2]));
	}else
		line = 1;
	buf = valstr(argv[0]);
	initctx(&ctx, vm);
	e = doparse(&ctx, strdata(buf), buf->len,
		    strdata(whence), whence->len, line);
	if(e == 0)
		return;
	*rv = mkvalexpr(e);
}

static void
l1_pp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to pp");
	checkarg(vm, argv, 0, Qexpr);
	printcqct(valexpr(argv[0]));
	xprintf("\n");
}

static void
l1_mkcl(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Closure *cl;
	Code *c;
	unsigned i, dlen;

	if(argc < 1)
		vmerr(vm, "wrong number of arguments to mkcl");
	checkarg(vm, argv, 0, Qcode);
	c = valcode(argv[0]);
	dlen = argc-1;
	if(dlen != c->nfree)
		vmerr(vm, "wrong number of free variables");
	cl = mkcl(c, dlen);
	for(i = 0; i < dlen; i++)
		cldisp(cl)[i] = argv[i+1];
	*rv = mkvalcl(cl);
}

static void
l1_clref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Closure *cl;
	Cval *cv;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to clref");
	checkarg(vm, argv, 0, Qcl);
	checkarg(vm, argv, 1, Qcval);
	cl = valcl(argv[0]);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "argument 2 to clref must be non-negative");
	if(cvalu(cv) >= cl->dlen)
		vmerr(vm, "closure reference out of bounds");
	*rv = cldisp(cl)[cvalu(cv)];
}

static void
l1_clcode(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Closure *cl;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to clcode");
	checkarg(vm, argv, 0, Qcl);
	cl = valcl(argv[0]);
	*rv = mkvalcode(cl->code);
}

/* FIXME: this is crap; rewrite or ditch */
static void
l1_printcode(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Code *c;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to printcode");
	checkarg(vm, argv, 0, Qcode);
	c = valcode(argv[0]);
	printcode(c);
}

static void
l1_cvalcmp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int r;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cvalcmp");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	r = cvalcmp(vm, valcval(argv[0]), valcval(argv[1]));
	switch(r) {
	case -1:
		*rv = mkvalcval2(cvalminus1);
		break;
	case 0:
		*rv = mkvalcval2(cval0);
		break;
	case 1:
		*rv = mkvalcval2(cval1);
		break;
	default:
		fatal("bug");
	}
}

static void
l1_cval2str(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Cval *cv;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cval2str");
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	s = enc2str(vm, cv->type, cvalenc(cv));
	*rv = mkvalstr(s);
}

static void
l1_gettoplevel(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val p;
	Cid *id;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to gettoplevel");
	checkarg(vm, argv, 0, Qcid);
	id = valcid(argv[0]);
	p = envget(vm->top, id);
	if(p == 0)
		vmerr(vm, "reference to unbound variable: %s", ciddata(id));
	*rv = p;
}

static void
l1_settoplevel(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to settoplevel");
	checkarg(vm, argv, 0, Qcid);
	envput(vm->top, valcid(argv[0]), argv[1]);
	*rv = argv[1];
}

static void
updatestats(VM *vm)
{
	u64 t;
	t = usec();
	vm->exetime += t-vm->exelast;
	vm->exelast = t;
}

static void
l1_statistics(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Tab *t;
	updatestats(vm);
	t = mktab();
	tabput(t, mkvalcid(mkcid0("exetime")),
	       mkvallitcval(Vuvlong, vm->exetime));
	tabput(t, mkvalcid(mkcid0("collecttime")),
	       mkvallitcval(Vuvlong, vm->gctime));
	tabput(t, mkvalcid(mkcid0("postgctime")),
	       mkvallitcval(Vuvlong, vm->postgctime));
	gcstatistics(t);
	*rv = mkvaltab(t);
}

static void
l1_add(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to add");
	if(Vkind(argv[0]) != Qcval && Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to add must be a cvalue or string");
	if(Vkind(argv[1]) != Qcval && Vkind(argv[1]) != Qstr)
		vmerr(vm, "operand 2 to add must be a cvalue or string");
	*rv = xbinop(vm, Iadd, argv[0], argv[1]);
}

static void
l1_div(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to div");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Idiv, argv[0], argv[1]);
}

static void
l1_mod(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mod");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Imod, argv[0], argv[1]);
}

static void
l1_mul(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mul");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Imul, argv[0], argv[1]);
}

static void
l1_sub(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to sub");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Isub, argv[0], argv[1]);
}

static void
l1_shl(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to shl");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Ishl, argv[0], argv[1]);
}

static void
l1_shr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to shr");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Ishr, argv[0], argv[1]);
}

static void
l1_and(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to and");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Iand, argv[0], argv[1]);
}

static void
l1_or(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to or");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Ior, argv[0], argv[1]);
}

static void
l1_xor(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to xor");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	*rv = xbinop(vm, Ixor, argv[0], argv[1]);
}

static void
l1_cmpeq(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cmpeq");
	*rv = xbinop(vm, Icmpeq, argv[0], argv[1]);
}

static void
l1_cmpneq(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cmpneq");
	*rv = xbinop(vm, Icmpneq, argv[0], argv[1]);
}

static void
l1_cmple(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cmple");
	if(Vkind(argv[0]) != Qcval && Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to cmple must be a cvalue or string");
	if(Vkind(argv[1]) != Qcval && Vkind(argv[1]) != Qstr)
		vmerr(vm, "operand 2 to cmple must be a cvalue or string");
	*rv = xbinop(vm, Icmple, argv[0], argv[1]);
}

static void
l1_cmplt(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cmplt");
	if(Vkind(argv[0]) != Qcval && Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to cmplt must be a cvalue or string");
	if(Vkind(argv[1]) != Qcval && Vkind(argv[1]) != Qstr)
		vmerr(vm, "operand 2 to cmplt must be a cvalue or string");
	*rv = xbinop(vm, Icmplt, argv[0], argv[1]);
}

static void
l1_cmpge(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cmpge");
	if(Vkind(argv[0]) != Qcval && Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to cmpge must be a cvalue or string");
	if(Vkind(argv[1]) != Qcval && Vkind(argv[1]) != Qstr)
		vmerr(vm, "operand 2 to cmpge must be a cvalue or string");
	*rv = xbinop(vm, Icmpge, argv[0], argv[1]);
}

static void
l1_cmpgt(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cmpgt");
	if(Vkind(argv[0]) != Qcval && Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to cmpgt must be a cvalue or string");
	if(Vkind(argv[1]) != Qcval && Vkind(argv[1]) != Qstr)
		vmerr(vm, "operand 2 to cmpgt must be a cvalue or string");
	*rv = xbinop(vm, Icmpgt, argv[0], argv[1]);
}

static void
l1_inv(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to inv");
	*rv = xunop(vm, Iinv, argv[0]);
}

static void
l1_neg(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to neg");
	*rv = xunop(vm, Ineg, argv[0]);
}

static void
l1_not(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to not");
	*rv = xunop(vm, Inot, argv[0]);
}

static void
l1_xcast(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to xcast");
	*rv = xxcast(vm, argv[0], argv[1]);
}

static void
fnalu(Env env)
{
	FN(add);
	FN(div);
	FN(mod);
	FN(mul);
	FN(sub);

	FN(shl);
	FN(shr);

	FN(and);
	FN(or);
	FN(xor);

	FN(cmpeq);
	FN(cmpneq);
	FN(cmpge);
	FN(cmpgt);
	FN(cmple);
	FN(cmplt);

	FN(inv);
	FN(not);
	FN(neg);

	FN(xcast);

	FN(cvalcmp);
}

static void
noctl(char *which)
{
	fatal("control reached undefined default %s handler", which);
}

static void
l1_currentabort(VM *vm, Imm argc, Val *argv, Val *rv)
{
	noctl("abort");
}

static void
l1_errorhandler(VM *vm, Imm argc, Val *argv, Val *rv)
{
	noctl("error");
}

static void
l1_interrupthandler(VM *vm, Imm argc, Val *argv, Val *rv)
{
	noctl("interrupt");
}

static void
l1_currentwarning(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to warning");
	checkarg(vm, argv, 0, Qstr);
	s = valstr(argv[0]);
	cprintf(&l1stderr, "%.*s\n", (int)s->len, strdata(s));
}

static void
l1_currentreset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	noctl("reset");
}

static void
l1_currentreturn(VM *vm, Imm argc, Val *argv, Val *rv)
{
	noctl("return");
}

static void
fnctl(Env env)
{
	FN(currentabort);
	FN(currentreset);
	FN(currentreturn);
	FN(currentwarning);
	FN(errorhandler);
	FN(interrupthandler);
}

char*
cqctsprintval(VM *vm, Val v)
{
	Val argv[2], rv, cl;

	Str *s;
	s = mkstrk("%a", 2, Sperm);
	cl = envlookup(vm->top, "sprintfa");
	argv[0] = mkvalstr(s);
	argv[1] = v;
	if(0 > cqctcallfn(vm, cl, 2, argv, &rv))
		return 0;
	return str2cstr(valstr(rv));
}

typedef
struct NSroot {
	Rkind base[Vnbase];
	Cbase ptr;
	Cbase xint8, xint16, xint32, xint64;
	Cbase xuint8, xuint16, xuint32, xuint64;
	char *name;
} NSroot;

static NSroot c32le = {
.base = {
	[Vbool]=		Ru08le,
	[Vchar]=		Rs08le,
	[Vshort]=		Rs16le,
	[Vint]=			Rs32le,
	[Vlong]=		Rs32le,
	[Vvlong]=		Rs64le,
	[Vuchar]=		Ru08le,
	[Vushort]=		Ru16le,
	[Vuint]=		Ru32le,
	[Vulong]=		Ru32le,
	[Vuvlong]=		Ru64le,
	[Vfloat]=		Rf32,
	[Vdouble]=		Rf64,
	[Vlongdouble]=		Rundef,
	[Vcomplex]=		Rundef,
	[Vdoublex]=		Rundef,
	[Vlongdoublex]=		Rundef,
	},
.ptr = Vulong,
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vlong,
.xint64 = Vvlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vulong,
.xuint64 = Vuvlong,
.name = "c32le",
};

static NSroot c32be = {
.base = {
	[Vbool]=		Ru08le,
	[Vchar]=		Rs08be,
	[Vshort]=		Rs16be,
	[Vint]=			Rs32be,
	[Vlong]=		Rs32be,
	[Vvlong]=		Rs64be,
	[Vuchar]=		Ru08be,
	[Vushort]=		Ru16be,
	[Vuint]=		Ru32be,
	[Vulong]=		Ru32be,
	[Vuvlong]=		Ru64be,
	[Vfloat]=		Rf32,
	[Vdouble]=		Rf64,
	[Vlongdouble]=		Rundef,
	[Vcomplex]=		Rundef,
	[Vdoublex]=		Rundef,
	[Vlongdoublex]=		Rundef,
	},
.ptr = Vulong,
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vlong,
.xint64 = Vvlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vulong,
.xuint64 = Vuvlong,
.name = "c32be",
};

static NSroot c64le = {
.base = {
	[Vbool]=		Ru08le,
	[Vchar]=		Rs08le,
	[Vshort]=		Rs16le,
	[Vint]=			Rs32le,
	[Vlong]=		Rs64le,
	[Vvlong]=		Rs64le,
	[Vuchar]=		Ru08le,
	[Vushort]=		Ru16le,
	[Vuint]=		Ru32le,
	[Vulong]=		Ru64le,
	[Vuvlong]=		Ru64le,
	[Vfloat]=		Rf32,
	[Vdouble]=		Rf64,
	[Vlongdouble]=		Rundef,
	[Vcomplex]=		Rundef,
	[Vdoublex]=		Rundef,
	[Vlongdoublex]=		Rundef,
	},
.ptr = Vuint,
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "c64le",
};

static NSroot c64be = {
.base = {
	[Vbool]=		Ru08le,
	[Vchar]=		Rs08be,
	[Vshort]=		Rs16be,
	[Vint]=			Rs32be,
	[Vlong]=		Rs64be,
	[Vvlong]=		Rs64be,
	[Vuchar]=		Ru08be,
	[Vushort]=		Ru16be,
	[Vuint]=		Ru32be,
	[Vulong]=		Ru64be,
	[Vuvlong]=		Ru64be,
	[Vfloat]=		Rf32,
	[Vdouble]=		Rf64,
	[Vlongdouble]=		Rundef,
	[Vcomplex]=		Rundef,
	[Vdoublex]=		Rundef,
	[Vlongdoublex]=		Rundef,
	},
.ptr = Vuint,
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "c64be",
};

static NSroot clp64le = {
.base = {
	[Vbool]=		Ru08le,
	[Vchar]=		Rs08le,
	[Vshort]=		Rs16le,
	[Vint]=			Rs32le,
	[Vlong]=		Rs64le,
	[Vvlong]=		Rs64le,
	[Vuchar]=		Ru08le,
	[Vushort]=		Ru16le,
	[Vuint]=		Ru32le,
	[Vulong]=		Ru64le,
	[Vuvlong]=		Ru64le,
	[Vfloat]=		Rf32,
	[Vdouble]=		Rf64,
	[Vlongdouble]=		Rundef,
	[Vcomplex]=		Rundef,
	[Vdoublex]=		Rundef,
	[Vlongdoublex]=		Rundef,
	},
.ptr = Vulong,
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "clp64le",
};

static NSroot clp64be = {
.base = {
	[Vbool]=		Ru08le,
	[Vchar]=		Rs08be,
	[Vshort]=		Rs16be,
	[Vint]=			Rs32be,
	[Vlong]=		Rs64be,
	[Vvlong]=		Rs64be,
	[Vuchar]=		Ru08be,
	[Vushort]=		Ru16be,
	[Vuint]=		Ru32be,
	[Vulong]=		Ru64be,
	[Vuvlong]=		Ru64be,
	[Vfloat]=		Rf32,
	[Vdouble]=		Rf64,
	[Vlongdouble]=		Rundef,
	[Vcomplex]=		Rundef,
	[Vdoublex]=		Rundef,
	[Vlongdoublex]=		Rundef,
	},
.ptr = Vulong,
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "clp64be",
};

static Imm procgen = 0;

static Tab*
basetab(NSroot *def, Ctype **base)
{
	Cbase cb;
	Val kv, vv;
	Tab *type;
	Cid *tn;

	for(cb = Vlo; cb < Vnbase; cb++)
		base[cb] = mkctypebase(cb, def->base[cb]);
	base[Vptr] = base[def->ptr];

	type = mktab();
	for(cb = Vlo; cb < Vnbase; cb++) {
		kv = mkvalctype(mkctypebase(cb, Rundef));
		vv = mkvalctype(base[cb]);
		tabput(type, kv, vv);
	}

	/* map pointer to integer representation (void*) */
	kv = mkvalctype(mkctypeptr(mkctypevoid(), Rundef));
	vv = mkvalctype(mkctypeptr(mkctypevoid(), def->base[def->ptr]));
	tabput(type, kv, vv);

	/* map pointer to integer representation (base Vptr) */
	kv = mkvalctype(mkctypebase(Vptr, Rundef));
	vv = mkvalctype(base[Vptr]);
	tabput(type, kv, vv);

	/* define stdint-like integer typedefs */

	tn = mkcid0("uintptr");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->ptr]));
	tabput(type, kv, vv);

	tn = mkcid0("int8");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xint8]));
	tabput(type, kv, vv);
	tn = mkcid0("int16");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xint16]));
	tabput(type, kv, vv);
	tn = mkcid0("int32");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xint32]));
	tabput(type, kv, vv);
	tn = mkcid0("int64");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xint64]));
	tabput(type, kv, vv);

	tn = mkcid0("uint8");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xuint8]));
	tabput(type, kv, vv);
	tn = mkcid0("uint16");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xuint16]));
	tabput(type, kv, vv);
	tn = mkcid0("uint32");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xuint32]));
	tabput(type, kv, vv);
	tn = mkcid0("uint64");
	kv = mkvalctype(mkctypedef(tn, 0));
	vv = mkvalctype(mkctypedef(tn, base[def->xuint64]));
	tabput(type, kv, vv);

	return type;
}

static Ns*
mkrootns(NSroot *def)
{
	Tab *type;
	Ns *ns;
	Ctype *base[Vnallbase];

	memset(base, 0, sizeof(base)); /* values will be seen by GC */
	type = basetab(def, base);
	ns = mknstype(type, mkstr0(def->name));
	memcpy(ns->base, base, sizeof(base));
	return ns;
}

static Dom*
mklitdom(void)
{
	return mkdom(mkrootns(&clp64le), mknas(), mkstr0("litdom"));
}

static Env
mktopenv(void)
{
	Env env;

	env = mkenv();

	builtinfn(env, "halt", halt);
	builtinfn(env, "kcapture", mkkcapture());
	builtinfn(env, "apply", mkapply());
	builtinfn(env, "$raiseinterrupt", mkraiseinterrupt());

	FN(asof);
	FN(bsearch);
	FN(clcode);
	FN(clref);
	FN(compact);
	FN(cntrget);
	FN(cntrput);
	FN(concat);
	FN(copy);
	FN(count);
	FN(cref);
	FN(cval);
	FN(cval2str);
	FN(delete);
	FN(delq);
	FN(dispatchtab);
	FN(domof);
	FN(eq);
	FN(equal);
	FN(eqv);
	FN(error);
	FN(fault);
	FN(fullgc);
	FN(gc);
	FN(gcpoll);
	FN(gcstats);
	FN(gettoplevel);
	FN(hash);
	FN(hashq);
	FN(hashqv);
	FN(index);
	FN(instguard);  // FIXME: make system routine
	FN(isas);
	FN(iscallable);
	FN(iscid);
	FN(iscode);
	FN(isctype);
	FN(iscvalue);
	FN(isdom);
	FN(isempty);
	FN(islist);
	FN(ismapped);
	FN(ismember);
	FN(isnil);
	FN(isns);
	FN(ispair);
	FN(isprocedure);
	FN(isrange);
	FN(isrec);
	FN(isrd);
	FN(isstring);
	FN(isstx);
	FN(istable);
	FN(isvector);
	FN(kbacktrace);
	FN(length);
	FN(liveinframe);
	FN(loadpath);
	FN(looksym);
	FN(looktype);
	FN(memcpy);
	FN(meminuse);
	FN(memtotal);
	FN(mkas);
	FN(mkattr);
	FN(mkcl);
	FN(mkdom);
	FN(mkmas);
	FN(mkmasro);
	FN(mkmasx);
	FN(mknas);
	FN(mkns);
	FN(mknsraw);
	FN(mkrange);
	FN(mksas);
	FN(mkzas);
	FN(myrootns);
	FN(nameof);
	FN(nsof);
	FN(nsptr);
	FN(nsreptype);
	FN(parse);
	FN(pop);
	FN(pp);
	FN(printcode);
	FN(qtype);
	FN(rangebeg);
	FN(rangelen);
	FN(resettop);
	FN(saveheapfd);
	FN(setloadpath);
	FN(settoplevel);
	FN(sizeof);		/* cannot be called directly by user code */
	FN(sort);
	FN(split);
	FN(stackframes);
	FN(statistics);
	FN(stringof);
	FN(unbox);

	fnalu(env);
	fnch(env);
	fncid(env);
	fncompile(env);
	fncode(env);
	fnctl(env);
	fnctype(env);
	fncval(env);
	fnlist(env);
	fnpair(env);
	fnrec(env);
	fnstr(env);
	fnstx(env);
	fnsym(env);
	fntab(env);
	fnvec(env);
	fns(env);		/* configuration-specific functions */

	/* FIXME: these bindings should be immutable */
	builtindom(env, "litdom", litdom);
	builtintab(env, "toplevel", env);
	builtinns(env, "c32le", mkrootns(&c32le));
	builtinns(env, "c32be", mkrootns(&c32be));
	builtinns(env, "c64le", mkrootns(&c64le));
	builtinns(env, "c64be", mkrootns(&c64be));
	builtinns(env, "clp64le", mkrootns(&clp64le));
	builtinns(env, "clp64be", mkrootns(&clp64be));
	builtincval(env, "NULL", cvalnull);
	builtinnil(env, "$$"); /* FIXME: get rid of this */
	builtincode(env, "kresumecode", kresumecode());
	builtincval(env, "$procgen", mkcval(litdom, litdom->ns->base[Vuint], procgen));

	/* expanded source may call these magic functions */
	builtinfn(env, "$put", mkcfn("$put", l1_put));
	builtinfn(env, "$typeof", mkcfn("$typeof", l1_typeof));

	return env;
}

static VM*
mkvm(Env top)
{
	VM *vm;

	if(nvm != 0)
		bug();
	vm = emalloc(sizeof(VM));
	vm->top = top;
	vms[nvm++] = vm;
	vmresetctl(vm);
	vm->exelast = usec();
	return vm;
}

static void
bootvm(VM *vm)
{
	boot(vm);
	vmresetctl(vm);
}

static void
freevm(VM *vm)
{
	if(nvm != 1)
		bug();
	if(vms[0] != vm)
		bug();
	efree(vm);
	nvm = 0;
}

static void
sigint(int sig)
{
	cqctinterrupt(vms[0]);
}

static void
procincr(Env env) {
	Val pg;
	Cid *cid;

	pg = envlookup(env, "$procgen");
	if((pg != 0) && (Vkind(pg) == Qcval)) {
		procgen = cvalu(valcval(pg));
		procgen++;
		cid = mkcid0("$procgen");
		envput(env, cid, mkvalcval2(mkcval(litdom, litdom->ns->base[Vuint], procgen)));
	}
}

VM*
cqctinitxfd(char *memfile, char **loadpath, Xfd *in, Xfd *out, Xfd *err)
{
	Env top, tmp;
	VM *vm;
	struct sigaction sa;
	Xfd xfd[3];

	/* users cannot disable warnings */
	cqctflags['w'] = 1;

	if(in == 0) {
		in = &xfd[0];
		memset(in, 0, sizeof(Xfd));
		in->read = xfdread;
		in->fd = 0;
	}
	if(out == 0) {
		out = &xfd[1];
		memset(out, 0, sizeof(Xfd));
		out->write = xfdwrite;
		out->fd = 1;
	}
	if(err == 0) {
		err = &xfd[2];
		memset(err, 0, sizeof(Xfd));
		err->write = xfdwrite;
		err->fd = 2;
	}

	initos();
	initmem();
	initio(in, out, err);
	initqc();
	initparse();
	initcid();
	inittype();
	initrec();
	initcg();
//	initnc();
	initvm();

	/* restoreheap depends on the table of C
	   built-ins, which is initialized by
	   mktoplevel.  the storage will be
	   reclaimed if it is not used. */
	tmp = mktoplevel();

	vm = 0;
	if(memfile) {
		top = restoreheap(memfile);
		if(top == 0)
			return 0;
		cqctloadpath = copystrv(loadpath);
		vm = mkvm(top);
		procincr(top);
	}else if((top = restoreheap(0))) {
		cqctloadpath = copystrv(loadpath);
		vm = mkvm(top);
		procincr(top);
	}else{
		cqctloadpath = copystrv(loadpath);
		vm = mkvm(tmp);
		bootvm(vm);
	}

	if(vm) {
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler = sigint;
		sigaction(SIGINT, &sa, 0);
	}
	return vm;
}

VM*
cqctinit(char *memfile, char **loadpath)
{
	return cqctinitxfd(memfile, loadpath, 0, 0, 0);
}
void
cqctfini(VM *vm)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, 0);

	freevm(vm);
	efree(cqctloadpath);
	finivm();
	finicg();
	finirec();
	finitype();
	finicid();
	finiparse();
	finimem();
	finiqc();
	finiio();
}

void
initvm()
{
	dovm(0); /* initialize gotab */
	litdom = mklitdom();
	cvalnull = mkcval(litdom, litdom->ns->base[Vptr], 0);
	cval0 = mkcval(litdom, litdom->ns->base[Vint], 0);
	cval1 = mkcval(litdom, litdom->ns->base[Vint], 1);
	cvalminus1 = mkcval(litdom, litdom->ns->base[Vint], -1);
	vabort = abortthunk();
	halt = haltthunk();
	stkunderflow = stkunderflowthunk();

	REGFN(calldispatch);
	REGFN(equalval);
	REGFN(eqval);
	REGFN(eqvval);
	REGFN(l1_put);
	REGFN(l1_typeof);
	REGFN(hashqval);
	REGFN(hashqvval);
	REGFN(hashval);
	REGFN(masget);
	REGFN(masismapped);
	REGFN(masmap);
	REGFN(masput);
	REGFN(nasbad);
	REGFN(nasismapped);
	REGFN(nasmap);
	REGFN(nilfn);
	REGFN(roput);
	REGFN(sasget);
	REGFN(sasismapped);
	REGFN(sasmap);
	REGFN(sasput);
	REGFN(stdenumsym);
	REGFN(stdenumtype);
	REGFN(stdlookaddr);
	REGFN(stdlooksym);
	REGFN(stdlooktype);
	REGFN(symcmp);
}

void
finivm(void)
{
}

int
cqctcallfn(VM *vm, Val cl, int argc, Val *argv, Val *rv)
{
	Val call;
	Val *xargv;

	call = envlookup(vm->top, "callfn");
	if(call == 0 || Vkind(call) == 0)
		fatal("no way to call Cinquecento functions from C");
	xargv = emalloc((1+argc)*sizeof(Val));
	memcpy(xargv+1, argv, argc*sizeof(Val));
	xargv[0] = cl;
	vm->flags &= ~VMirq;
	*rv = ccall(vm, valcl(call), argc+1, xargv);
	efree(xargv);
	return 0;
}

int
cqctcallthunk(VM *vm, Val cl, Val *rv)
{
	return cqctcallfn(vm, cl, 0, 0, rv);
}

/* these cqctval<type> routines and their inverses assume litdom is clp64le */

int8_t
cqctvalint8(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint8];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

int16_t
cqctvalint16(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint16];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

int32_t
cqctvalint32(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint32];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

int64_t
cqctvalint64(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint64];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

uint8_t
cqctvaluint8(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint8];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

uint16_t
cqctvaluint16(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint16];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

uint32_t
cqctvaluint32(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint32];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

uint64_t
cqctvaluint64(Val v)
{
	Cval *cv, *rv;
	Ctype *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint64];
	rv = mkcvalenc(litdom, t, rerep(cvalenc(cv), cv->type, t));
	return cvalu(rv);
}

Val
cqctint8val(int8_t x)
{
	return mkvallitcval(clp64le.xint8, x);
}

Val
cqctint16val(int16_t x)
{
	return mkvallitcval(clp64le.xint16, x);
}

Val
cqctint32val(int32_t x)
{
	return mkvallitcval(clp64le.xint32, x);
}

Val
cqctint64val(int64_t x)
{
	return mkvallitcval(clp64le.xint64, x);
}

Val
cqctuint8val(uint8_t x)
{
	return mkvallitcval(clp64le.xuint8, x);
}

Val
cqctuint16val(uint16_t x)
{
	return mkvallitcval(clp64le.xuint16, x);
}

Val
cqctuint32val(uint32_t x)
{
	return mkvallitcval(clp64le.xuint32, x);
}

Val
cqctuint64val(uint64_t x)
{
	return mkvallitcval(clp64le.xuint64, x);
}
