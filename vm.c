#include "sys.h"
#include "util.h"
#include "syscqct.h"

char *qname[Qnkind] = {
	[Qundef]=	"undefined",
	[Qnil]=		"nil",
	[Qas]=		"address space",
	[Qbox]=		"box",
	[Qcid]=		"cid",
	[Qcl]=		"closure",
	[Qcode]=	"code",
	[Qctype]=	"ctype",
	[Qcval]=	"cvalue",
	[Qdom]=		"domain",
	[Qexpr]=	"stx",
	[Qfd]=		"fd",
	[Qlist]=	"list",
	[Qns]=		"name space",
	[Qpair]=	"pair",
	[Qrange]=	"range",
	[Qrec]=		"record",
	[Qrd]=		"record descriptor",
	[Qstr]=		"string",
	[Qtab]=		"table",
	[Qvec]=		"vector",
};

unsigned isfloat[Vnbase] = {
	[Vfloat] = 1,
	[Vdouble] = 1,
	[Vlongdouble] = 1,
};

unsigned isunsigned[Vnbase] = {
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

static Ctype* dolooktype(VM *vm, Ctype *t, Ns *ns);
static Env* mktopenv(void);
static void l1_sort(VM *vm, Imm argc, Val *argv, Val *rv);

void *GCiterdone;
Val Xundef;
Val Xnil;
Dom *litdom;
static Closure *halt;
Cval *cvalnull, *cval0, *cval1, *cvalminus1;

VM *vms[Maxvms];
static unsigned nvms;
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

Code *kcode, *cccode, *tcccode;

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
	switch(Vkind(v)){
	case Qundef:
	case Qnil:
		return 0;
		break;
	default:
		return (Head*)v;
		break;
	}
}

Code*
newcode()
{
	return (Code*)malcode();
}

Closure*
mkcl(Code *code, unsigned long entry, unsigned len, char *id)
{
	Closure *cl;
	cl = (Closure*)malv(Qcl, sizeof(Closure)+len*sizeof(Val));
	cl->code = code;
	cl->entry = &code->insn[entry];
	cl->dlen = len;
	cl->id = mkcid0(id);
	return cl;
}

Closure*
mkxfn(Str *code)
{
	Closure *cl;
	cl = mkcl(cccode, 0, 0, "*asm*");
	cl->xfn = code;
	return cl;
}

Closure*
mkcfn(char *id, Cfn *cfn)
{
	Closure *cl;
	cl = mkcl(cccode, 0, 0, id);
	cl->cfn = cfn;
	return cl;
}

Closure*
mktcfn(char *id, Cfn *cfn)
{
	Closure *cl;
	cl = mkcl(tcccode, 0, 0, id);
	cl->cfn = cfn;
	return cl;
}

Closure*
mkccl(char *id, Ccl *ccl, unsigned dlen, ...)
{
	Closure *cl;
	va_list args;
	Val vp;
	unsigned m;

	va_start(args, dlen);
	cl = mkcl(cccode, 0, dlen, id);
	cl->ccl = ccl;
	for(m = 0; m < dlen; m++){
		vp = va_arg(args, Val);
		cldisp(cl)[m] = vp;
	}
	va_end(args);
	return cl;
}

Fd*
mkfdfn(Str *name, int flags, Xfd *xfd)
{
	Fd *fd;
//	if(read == 0)
//		flags &= ~Fread;
//	if(write == 0)
//		flags &= ~Fwrite;
	fd = (Fd*)malq(Qfd, sizeof(Fd));
	fd->name = name;
	fd->u.fn = *xfd;
	fd->flags = flags|Ffn;
	quard((Val)fd);
	return fd;
}

Fd*
mkfdcl(Str *name, int flags,
       Closure *read,
       Closure *write,
       Closure *close)
{
	Fd *fd;
	if(read == 0)
		flags &= ~Fread;
	if(write == 0)
		flags &= ~Fwrite;
	fd = (Fd*)malq(Qfd, sizeof(Fd));
	fd->name = name;
	fd->u.cl.read = read;
	fd->u.cl.write = write;
	fd->u.cl.close = close;
	fd->flags = flags&~Ffn;
	quard((Val)fd);
	return fd;
}

int
eqvval(Val v1, Val v2)
{
	if(Vkind(v1) != Vkind(v2))
		return 0;
	switch(Vkind(v1)){
	case Qundef:
	case Qbox:
		bug();
	case Qnil:
		return 1;
	case Qas:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qfd:
	case Qns:
	case Qrd:
	case Qtab:
	case Qexpr:
		return eqptr(v1, v2);
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
	switch(Vkind(v)){
	case Qundef:
	case Qbox:
		bug();
	case Qnil:
		return hashp(Xnil);
	case Qas:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qfd:
	case Qns:
	case Qrd:
	case Qtab:
	case Qexpr:
		return hashptr(v);
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
	switch(Vkind(v1)){
	case Qundef:
	case Qbox:
		bug();
	case Qnil:
		return 1;
	case Qas:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qfd:
	case Qns:
	case Qrd:
	case Qtab:
	case Qexpr:
		return eqptr(v1, v2);
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
	switch(Vkind(v)){
	case Qundef:
	case Qbox:
		bug();
	case Qnil:
		return hashp(Xnil);
	case Qas:
	case Qcid:
	case Qcl:
	case Qdom:
	case Qfd:
	case Qns:
	case Qrd:
	case Qtab:
	case Qexpr:
		return hashptr(v);
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

static Env*
mkenv(void)
{
	Env *env;
	env = emalloc(sizeof(Env));
	env->var = mktab();
	env->rd = mkhts();
	return env;
}

int
envbinds(Env *env, Cid *id)
{
	if(tabget(env->var, mkvalcid(id)))
		return 1;
	else
		return 0;
}

void
envdefine(Env *env, Cid *id, Val v)
{
	if(envbinds(env, id))
		bug();
	tabput(env->var, mkvalcid(id), v);
}

Pair*
envgetkv(Env *env, Cid *id)
{
	return tabgetkv(env->var, mkvalcid(id));
}

Val
envget(Env *env, Cid *id)
{
	return tabget(env->var, mkvalcid(id));
}

void
envput(Env *env, Cid *id, Val v)
{
	tabput(env->var, mkvalcid(id), v);
}

static void
envbind(Env *env, char *id, Val val)
{
	Cid *cid;
	cid = mkcid0(id);
	if(envbinds(env, cid)){
		xprintf("warning: rebinding %s via envbind\n", id);
		envput(env, cid, val);
	}else
		envdefine(env, cid, val);
}

static Val
envlookup(Env *env, char *id)
{
	return envget(env, mkcid0(id));
}

static void
freerd(void *u, char *id, void *v)
{
	USED(u);
	USED(v);
	efree(id);
}

void
freeenv(Env *env)
{
	hforeach(env->rd, freerd, 0);
	freeht(env->rd);
	efree(env);
}

Toplevel*
mktoplevel(Xfd *in, Xfd *out, Xfd *err)
{
	Toplevel *top;

	top = emalloc(sizeof(Toplevel));
	top->env = mktopenv();

	/* persistent storage for stdio xfds */
	top->in = *in;
	top->out = *out;
	top->err = *err;

	builtinfd(top->env, "stdin",
		  mkfdfn(mkstr0("<stdin>"), Fread, &top->in));
	builtinfd(top->env, "stdout",
		  mkfdfn(mkstr0("<stdout>"), Fwrite, &top->out));
	builtinfd(top->env, "stderr",
		  mkfdfn(mkstr0("<stderr>"), Fwrite, &top->err));
	return top;
}

void
freetoplevel(Toplevel *top)
{
	freeenv(top->env);
	efree(top);
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
putval(VM *vm, Val v, Location *loc)
{
	Val *dst;

	switch(LOCKIND(loc->loc)){
	case Lreg:
		switch(LOCIDX(loc->loc)){
		case Rac:
			vm->ac = v;
			break;
		case Rsp:
		case Rfp:
		case Rpc:
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		dst = &vm->stack[(vm->fp+1)+LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Llocal:
		dst = &vm->stack[(vm->fp-1)-LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Ldisp:
		dst = &cldisp(vm->cl)[LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Ltopl:
		envput(vm->top->env, valcid(loc->v), v);
		break;
	case Ltopr:
		setcdr(loc->v, v);
		break;
	default:
		fatal("bug");
	}
}

Src*
addr2line(Code *code, Insn *pc)
{
	return &pc->src;
}

static void
printsrc(Xfd *xfd, Closure *cl, Insn *pc)
{
	Code *code;
	Src *src;
	char *fn;

	code = cl->code;
	if(cl->cfn || cl->ccl){
		cprintf(xfd, "%20s\t(builtin %s)\n", ciddata(cl->id),
			cl->cfn ? "function" : "closure");
		return;
	}
	if(cl->xfn){
		cprintf(xfd, "%20s\t(code)\n", ciddata(cl->id));
		return;
	}

	src = addr2line(code, pc);
	fn = src->filename;
	if(fn == syssrcfile){
		cprintf(xfd, "%20s\t(no source information)\n",
			ciddata(cl->id));
		return;
	}
	if(fn == 0)
		fn = "<stdin!!!>";
	cprintf(xfd, "%20s\t(%s:%u)\n", ciddata(cl->id), fn, src->line);
}

void
fvmbacktrace(VM *vm)
{
	Imm fp, narg;
	Insn *pc;
	Closure *cl;
	Xfd *xfd;

	xfd = &vm->top->out;

	pc = vm->pc;
	fp = vm->fp;
	cl = vm->cl;
	while(pc != 0){
		pc--;		/* vm loop increments pc after fetch */
		if(strcmp(ciddata(cl->id), "$halt")){
//			cprintf(xfd, "fp=%05lld pc=%08lld ", fp, pc);
			printsrc(xfd, cl, pc);
		}
		narg = stkimm(vm->stack[fp]);
		pc = stkp(vm->stack[fp+narg+1]);
		cl = valcl(vm->stack[fp+narg+2]);
		fp = stkimm(vm->stack[fp+narg+3]);
	}
}

static void
vmbacktrace(VM *vm)
{
	fvmbacktrace(vm);
}

void
vmerr(VM *vm, char *fmt, ...)
{
	va_list args;
	cprintf(&vm->top->out, "error: ");
	va_start(args, fmt);
	cvprintf(&vm->top->out, fmt, args);
	va_end(args);
	cprintf(&vm->top->out, "\n");
	fvmbacktrace(vm);
	nexterror(vm);
}

static Val
getval(VM *vm, Location *loc)
{
	Val p;

	switch(LOCKIND(loc->loc)){
	case Lreg:
		switch(LOCIDX(loc->loc)){
		case Rac:
			return vm->ac;
		case Rsp:
		case Rfp:
		case Rcl:
		case Rpc:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		p = vm->stack[(vm->fp+1)+LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxed(p);
		else
			return p;
	case Llocal:
		p = vm->stack[(vm->fp-1)-LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxed(p);
		else
			return p;
	case Ldisp:
		p = cldisp(vm->cl)[LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxed(p);
		else
			return p;
	case Ltopl:
		p = envget(vm->top->env, valcid(loc->v));
		if(p == 0 || p == Xundef)
			vmerr(vm, "reference to unbound variable: %s",
			      ciddata(valcid(loc->v)));
		return p;
	case Ltopr:
		p = cdr(loc->v);
		if(p == Xundef)
			vmerr(vm, "reference to unbound variable: %s",
			      ciddata(valcid(car(loc->v))));
		return p;
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Cval*
getcval(VM *vm, Location *loc)
{
	Val p;

	switch(LOCKIND(loc->loc)){
	case Lreg:
		switch(LOCIDX(loc->loc)){
		case Rac:
			return valcval(vm->ac);
		case Rsp:
		case Rfp:
		case Rpc:
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		p = vm->stack[(vm->fp+1)+LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxedcval(p);
		return valcval(p);
	case Llocal:
		p = vm->stack[(vm->fp-1)-LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxedcval(p);
		return valcval(p);
	case Ldisp:
		p = cldisp(vm->cl)[LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxedcval(p);
		return valcval(p);
	case Ltopl:
		p = envget(vm->top->env, valcid(loc->v));
		if(p == 0 || p == Xundef)
			vmerr(vm, "reference to unbound variable: %s",
			      ciddata(valcid(loc->v)));
		return valcval(p);
	case Ltopr:
		p = cdr(loc->v);
		if(p == Xundef)
			vmerr(vm, "reference to unbound variable: %s",
			      ciddata(valcid(car(loc->v))));
		return valcval(p);
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Val
getvalrand(VM *vm, Operand *r)
{
	Val v;
	switch(r->okind){
	case Oloc:
		return getval(vm, &r->u.loc);
	case Okon:
		v = r->u.kon;
		/* any mutable vals must be copied */
		switch(Vkind(v)){
		case Qstr:
			v = mkvalstr(strcopy(valstr(v)));
			break;
		default:
			break;
		}
		return v;
	case Onil:
		return Xnil;
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Cval*
getcvalrand(VM *vm, Operand *r)
{
	switch(r->okind){
	case Oloc:
		return getcval(vm, &r->u.loc);
	case Okon:
		/* FIXME: check for cval? */
		return valcval(r->u.kon);
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static void
putvalrand(VM *vm, Val v, Operand *r)
{
	if(r->okind != Oloc)
		fatal("bad destination");
	putval(vm, v, &r->u.loc);
}

static void
putcvalrand(VM *vm, Cval *cv, Operand *r)
{
	if(r->okind != Oloc)
		fatal("bad destination");
	putval(vm, mkvalcval2(cv), &r->u.loc);
}

static Imm
getbeint(char *s, unsigned nb)
{
	unsigned i;
	Imm w;
	w = 0;
	for(i = 0; i < nb; i++){
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
	switch(typerep(chasetype(t))){
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
	for(i = nb-1; i >= 0; i--){
		p[i] = w&0xff;
		w >>= 8;
	}
}

static Str*
enc2str(VM *vm, Ctype *t, Enc v)
{
	Str *str;
	char *s;

	switch(typerep(chasetype(t))){
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
	switch((typerep(new)<<5)|typerep(old)){
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
	switch(t->tkind){
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
	if(typerep(new) == Rundef){
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
	if(t == 0){
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
	if(t == 0){
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
		if(b1->tkind != Tptr && b2->tkind != Tptr){
			op1 = domcastbase(vm, litdom, op1);
			op2 = domcastbase(vm, litdom, op2);
		}else if(b1->tkind == Tptr && b2->tkind == Tptr)
			vmerr(vm, domerr);
		else if(b1->tkind == Tptr){
			if(op == Iadd || op == Isub)
				op2 = domcast(vm, op1->dom, op2);
			else
				vmerr(vm, domerr);
		}else /* b2->tkind == Tptr */ {
			if(op == Iadd || op == Isub)
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
	switch(typecbase(base)){
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
	while(1){
		p2 = t2;
		while(1){
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
	if(b1->tkind != Tbase || b2->tkind != Tbase){
		/* presumably one of them is a Tptr; nothing else is sane. */
		*rv1 = op1;
		*rv2 = op2;
		return;
	}

	c1 = typecbase(b1);
	c2 = typecbase(b2);
	r1 = typerep(b1);
	r2 = typerep(b2);
	if(c1 == c2 && r1 == r2){
		/* combinations of distinct typedefs
		   and/or enums yield the first type
		   they have in common (not necessarily
		   the base type). */
		if(t1->tkind == Ttypedef || t2->tkind == Ttypedef){
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

	if(isfloat[c1] || isfloat[c2]){
		if(rank[c1] < rank[c2])
			nc = c2;
		else
			nc = c1;
	}else if((isunsigned[c1] && isunsigned[c2])
	   || (!isunsigned[c1] && !isunsigned[c2])){
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

static void
xcallc(VM *vm)
{
	Imm argc;
	Val *argv;
	Val rv;
	Cfn *x;

	if(vm->cl->cfn == 0 && vm->cl->ccl == 0 && vm->cl->xfn == 0)
		vmerr(vm, "bad closure for builtin call");

	rv = Xnil;
	argc = stkimm(vm->stack[vm->fp]);
	argv = &vm->stack[vm->fp+1];
	vm->pc += 2; // skip livemask
	if(vm->cl->cfn)
		vm->cl->cfn(vm, argc, argv, &rv);
	else if(vm->cl->ccl)
		vm->cl->ccl(vm, argc, argv, cldisp(vm->cl), &rv);
	else{
		x = (Cfn*)strdata(vm->cl->xfn);
		x(vm, argc, argv, &rv);
	}
	vm->ac = rv;
}

static void
xcalltc(VM *vm)
{
	Imm argc;
	Val *argv;
	Closure *cl;
	Cfn *x;

	if(vm->cl->cfn == 0 && vm->cl->ccl == 0 && vm->cl->xfn == 0)
		vmerr(vm, "bad closure for builtin call");
	cl = vm->cl;

	/* return from cinquecento frame ahead of call */
	argc = stkimm(vm->stack[vm->fp]);
	argv = &vm->stack[vm->fp+1];
	vm->sp = vm->fp+stkimm(vm->stack[vm->fp])+1; /* narg+1 */
	vm->fp = stkimm(vm->stack[vm->sp+2]);
	vm->cl = valcl(vm->stack[vm->sp+1]);
	vm->pc = stkp(vm->stack[vm->sp]);
	vm->sp += 3; /* should be vmpop(vm, 3), but only declared below */

	vm->ac = Xnil; /* FIXME: okay?  why doesn't callc do this? */
	if(cl->cfn)
		cl->cfn(vm, argc, argv, &vm->ac);
	else if(cl->ccl)
		cl->ccl(vm, argc, argv, cldisp(cl), &vm->ac);
	else{
		x = (Cfn*)strdata(cl->xfn);
		x(vm, argc, argv, &vm->ac);
	}
}

static Imm
xstrcmp(VM *vm, ikind op, Str *s1, Str *s2)
{
	int x;

	switch(op){
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
	switch(cb){
	case Vfloat:
		f = cvalf(cv);
		switch(op){
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
		switch(op){
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

static void
xunop(VM *vm, ikind op, Operand *op1, Operand *dst)
{
	Val v;
	Cval *cv, *cvr;
	Imm imm, nv;
	Ctype *t;
	Cbase cb;

	v = getvalrand(vm, op1);
	if(op == Inot){
		if(Vkind(v) == Qcval)
			goto cval;
		if(Vkind(v) != Qnil)
			vmerr(vm, "incompatible operand for unary %s",
			      opstr[op]);
		cvr = cval1;
		goto out;
	}
	if(Vkind(v) != Qcval)
		vmerr(vm, "incompatible operand for unary %s", opstr[op]);
cval:
	cv = intpromote(vm, valcval(v));
	t = chasetype(cv->type);
	cb = typecbase(t);
	if(isfloat[cb]){
		cvr = xunopfp(vm, op, cb, cv);
		goto out;
	}

	imm = cvalu(cv);

	switch(op){
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
		return; /* not reached */
	}
out:
	putcvalrand(vm, cvr, dst);
}

static Imm
truncimm(Imm v, Rkind rep)
{
	switch(rep){
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

	if(t1->tkind == Tptr && t2->tkind == Tptr){
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

	if(t1->tkind == Tptr){
		sub = chasetype(subtype(t1));
		ptr = op1;
		n = cvalu(op2);
	}else if(op == Isub){
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
	switch(cb){
	case Vfloat:
		f1 = cvalf(op1);
		f2 = cvalf(op2);
		switch(op){
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
		switch(op){
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

	switch(op){
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
		switch(op){
		case Ishl:
			rv = i1<<i2;
			break;
		case Ishr:
			rv = i1>>i2;
			break;
		default:
			fatal("bug");
		}
	else
		switch(op){
		case Ishl:
			rv = (s64)i1<<i2;
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
	switch(cb){
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
	if(t1->tkind == Tptr || t2->tkind == Tptr || isunsigned[b1]){
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
	switch(cvalcmp(vm, op1, op2)){
	case -1:
		switch(op){
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
		switch(op){
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
		switch(op){
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

static void
xbinop(VM *vm, ikind op, Operand *op1, Operand *op2, Operand *dst)
{
	Val v1, v2;
	Cval *cv1, *cv2, *cvr;
	Str *s1, *s2;
	Imm nv;

	v1 = getvalrand(vm, op1);
	v2 = getvalrand(vm, op2);

	if(Vkind(v1) == Qcval && Vkind(v2) == Qcval){
		cv1 = valcval(v1);
		cv2 = valcval(v2);
		switch(op){
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
			return; /* not reached */
		}
		putcvalrand(vm, cvr, dst);
		return;
	}

	if(Vkind(v1) == Qstr && Vkind(v2) == Qstr){
		s1 = valstr(v1);
		s2 = valstr(v2);
dostr:
		if(op == Iadd)
			putvalrand(vm, mkvalstr(strconcat(s1, s2)), dst);
		else{
			nv = xstrcmp(vm, op, s1, s2);
			if(nv)
				putvalrand(vm, mkvalcval2(cval1), dst);
			else
				putvalrand(vm, mkvalcval2(cval0), dst);
		}
		return;
	}

	if(Vkind(v1) == Qstr && Vkind(v2) == Qcval){
		cv2 = valcval(v2);
		if(isstrcval(cv2)){
			s2 = stringof(vm, cv2);
			s1 = valstr(v1);
			goto dostr;
		}
		/* fall through */
	}

	if(Vkind(v2) == Qstr && Vkind(v1) == Qcval){
		cv1 = valcval(v1);
		if(isstrcval(cv1)){
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
		putvalrand(vm, mkvalcval2(cval1), dst);
	else
		putvalrand(vm, mkvalcval2(cval0), dst);
}

static void
xclo(VM *vm, Operand *dl, Ctl *label, Operand *dst)
{
	Closure *cl;
	Cval *cv;
	Imm m, len;

	/* dl is number of values to copy from stack into display */
	/* label points to instruction in current closure's code */
	/* captured variables are in display order on stack,
	   from low to high stack address */

	cv = getcvalrand(vm, dl);
	len = cvalu(cv);

	cl = mkcl(vm->cl->code, label->insn, len, label->label);
	for(m = 0; m < len; m++)
		cldisp(cl)[m] = vm->stack[vm->sp+m];
	vm->sp += m;

	putvalrand(vm, mkvalcl(cl), dst);
}

static void
xkg(VM *vm, Operand *dst)
{
	Closure *k;
	Imm len;

	len = Maxstk-vm->sp;
	k = mkcl(kcode, 0, len, "kg");
	memcpy(cldisp(k), &vm->stack[vm->sp], len*sizeof(Val));
	k->fp = vm->fp;

	putvalrand(vm, mkvalcl(k), dst);
}

static void
xkp(VM *vm)
{
	Closure *k;
	k = vm->cl;
	vm->fp = k->fp;
	vm->sp = Maxstk-k->dlen;
	memcpy(&vm->stack[vm->sp], cldisp(k), k->dlen*sizeof(Val));
}

static void
xmov(VM *vm, Operand *src, Operand *dst)
{
	Val v;
	v = getvalrand(vm, src);
	putvalrand(vm, v, dst);
}

static int
falseval(Val v)
{
	Cval *cv;

	switch(Vkind(v)){
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
checkoverflow(VM *vm, unsigned dec)
{
	if(dec > vm->sp)
		vmerr(vm, "stack overflow");
}

static void
vmpush(VM *vm, Val v)
{
	checkoverflow(vm, 1);
	vm->stack[--vm->sp] = v;
}

static void
vmpushi(VM *vm, Imm imm)
{
	checkoverflow(vm, 1);
	vm->stack[--vm->sp] = (Val)(uptr)imm;
}

static void
vmpushp(VM *vm, void *p)
{
	checkoverflow(vm, 1);
	vm->stack[--vm->sp] = (Val)(uptr)p;
}

static void
vmpop(VM *vm, unsigned n)
{
	vm->sp += n;
}

static void
xpush(VM *vm, Operand *op)
{
	Val v;
	v = getvalrand(vm, op);
	vmpush(vm, v);
}

static void
xpushi(VM *vm, Operand *op)
{
	Val v;
	Cval *cv;

	v = getvalrand(vm, op);
	cv = valcval(v);
	vmpushi(vm, cvalu(cv));
}

static void
xbox(VM *vm, Operand *op)
{
	Val v;
	v = getvalrand(vm, op);
	// specializer can trigger redundant boxing of parameters
	// when jumping from original to new function
	if(Vkind(v) == Qbox)
		return;
	putvalrand(vm, mkvalbox(v), op);
}

static void
xbox0(VM *vm, Operand *op)
{
	putvalrand(vm, mkvalbox(Xnil), op);
}

// Operand *x, Operand *type, Operand *cval, Operand *dst)
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
	checkarg(vm, "cref", argv, 1, Qctype);
	checkarg(vm, "cref", argv, 2, Qcval);
	d = valdom(argv[0]);
	t = valctype(argv[1]);
	cv = valcval(argv[2]);
	b = chasetype(t);
	switch(b->tkind){
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
	rv = dovm(vm, as->get, 2, argv);
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
	checkarg(vm, "cval", argv, 1, Qctype);
	checkarg(vm, "cval", argv, 2, Qcval);

	t = valctype(argv[1]);
	b = chasetype(t);
	cv = valcval(argv[2]);

	/* special case: enum constants can be referenced through namespace */
	if(b->tkind == Tconst){
		switch(Vkind(argv[0])){
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
	switch(b->tkind){
	case Tbitfield:
		sub = subtype(b);
		if(sub->tkind == Tundef){
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

static void
xxcast(VM *vm, Operand *typeordom, Operand *o, Operand *dst)
{
	Val tv, ov, rv;
	Cval *cv;
	Dom *d;
	Ctype *t;

	rv = 0;
	ov = getvalrand(vm, o);
	if(Vkind(ov) != Qcval && Vkind(ov) != Qstr)
		vmerr(vm, "operand 2 to extended cast operator must be a"
		      " cvalue or string");
	tv = getvalrand(vm, typeordom);
	if(Vkind(ov) == Qstr)
		cv = str2voidstar(litdom->ns, valstr(ov));
	else
		cv = valcval(ov);
	if(Vkind(tv) != Qctype && Vkind(ov) == Qstr)
		vmerr(vm, "illegal conversion");
	if(Vkind(tv) == Qctype){
		t = valctype(tv);
		if(!iscvaltype(t))
			vmerr(vm, "bad type conversion");
		rv = mkvalcval2(typecast(vm, t, cv));
	}else if(Vkind(tv) == Qdom){
		d = valdom(tv);
		rv = mkvalcval2(domcast(vm, d, cv));
	}else if(Vkind(tv) == Qns){
		d = mkdom(valns(tv), cv->dom->as, 0);
		rv = mkvalcval2(domcast(vm, d, cv));
	}else if(Vkind(tv) == Qas){
		d = mkdom(cv->dom->ns, valas(tv), 0);
		rv = mkvalcval2(domcast(vm, d, cv));
	}else
		vmerr(vm, "bad type for operand 1 to extended cast operator");
	putvalrand(vm, rv, dst);
}

static void
xlist(VM *vm, Operand *op1, Operand *op2, Operand *dst)
{
	Val v;
	Imm sp, n, m, i;
	List *lst;
	Val rv;

	v = getvalrand(vm, op1);
	sp = vm->fp+valimm(v);
	n = stkimm(vm->stack[sp]);
	v = getvalrand(vm, op2);
	m = valimm(v);

	lst = mklist();
	for(i = m; i < n; i++)
		listappend(vm, lst, vm->stack[sp+1+i]);
	rv = mkvallist(lst);
	putvalrand(vm, rv, dst);
}

static void
l1_sizeof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm imm;
	Ctype *t;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to sizeof");
	switch(Vkind(argv[0])){
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
	checkarg(vm, "sasget", argv, 1, Qrange);
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
	checkarg(vm, "sasput", argv, 1, Qrange);
	checkarg(vm, "sasput", argv, 2, Qstr);
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
	checkarg(vm, "masget", argv, 1, Qrange);
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
	checkarg(vm, "masput", argv, 1, Qrange);
	checkarg(vm, "masput", argv, 2, Qstr);
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
	checkarg(vm, "ismapped", argv, 1, Qrange);
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
	tabput(mtab, mkvalstr(mkstr0("put")),
		mkvalcl(mkccl("masput", masput, 1, mkvalrange(r))));
	tabput(mtab, mkvalstr(mkstr0("map")),
		mkvalcl(mkccl("masmap", masmap, 1, mkvalrange(r))));
	tabput(mtab, mkvalstr(mkstr0("ismapped")),
		mkvalcl(mkccl("masmap", masismapped, 1, mkvalrange(r))));
	as = mkastab(mtab, 0);
	/* check if asked for intersection with managed
	   range, but *after* any potential heap churn
	   of this call. */
	if(x && ismanagedrange((void*)(uptr)cvalu(r->beg), cvalu(r->len)))
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

	switch(t->tkind){
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
		rv = safedovm(vm, ns->looktype, 2, argv);
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
		for(i = 0; i < tf->param->len; i++){
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
	switch(t->tkind){
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
	rv = safedovm(vm, ns->looktype, 2, argv);
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
	rv = safedovm(vm, ns->looktype, 2, argv);
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
	checkarg(vm, "looksym", argv, 1, Qcid);
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
	checkarg(vm, "looktype", argv, 1, Qctype);
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
	checkarg(vm, "lookaddr", argv, 1, Qcval);
	l = vallist(disp[0]);
	cv = valcval(argv[1]);
	addr = cvalu(cv);
	n = listlen(l);
	if(n == 0){
		*rv = Xnil;
		return;
	}
	sym = valvec(listref(vm, l, 0));
	a = valcval(attroff(vecref(sym, Attrpos)));
	if(cvalu(a) > addr){
		*rv = Xnil;
		return;
	}

	/* binary search */
	b = 0;
	while(1){
		i = n/2;
		m = b+i;
		if(i == 0)
			break;
		sym = valvec(listref(vm, l, m));
		a = valcval(attroff(vecref(sym, Attrpos)));
		if(addr < cvalu(a))
			n = i;
		else{
			b = m;
			n = n-i;
		}
	}
	sym = valvec(listref(vm, l, m));
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
	if(rv){
		if(Vkind(rv) != Qctype)
			vmerr(vm, "invalid raw type table");
		t = valctype(tv);
		new = mkctypedef(typetid(t), 0);

		/* bind before recursive resolve call to stop cycles */
		tabput(ctx->type, tv, mkvalctype(new));

		def = valctype(rv);
		if(def->tkind != Ttypedef)
			vmerr(vm, "invalid typedef in raw type table");
		if(def == t)
			vmerr(vm, "invalid typedef in raw type table");
		setsubtype(new, resolvetypename(vm, subtype(def), ctx));
		if(subtype(new) == new){
			id = typetid(def);
			vmerr(vm, "circular definition in name space: "
			      "typedef %.*s",
			      (int)id->len, ciddata(id));
		}
		return new;
	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, tv);
	if(rv){
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
	for(m = 0; m < v->len; m++){
		e = valvec(vecref(v, m)); /* FIXME check sanity */
		a = xcvalalu1dom(vm, Iadd, a,
				 domcastbase(vm, d, valcval(vecref(e, 1))));
	}
	t = a->type;

	/* cast all values to new type */
	for(m = 0; m < v->len; m++){
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
	if(rv){
		if(Vkind(rv) != Qctype)
			vmerr(vm, "invalid raw type table");
		t = valctype(rv);
		switch(t->tkind){
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

			for(i = 0; i < fld->len; i++){
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
	if(rv){
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
	if(rv){
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

	switch(t->tkind){
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
		for(i = 0; i < tf->param->len; i++){
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
	Val *xargv;

	dcl = valcl(disp[0]);
	xargv = emalloc((1+argc)*sizeof(Val));
	xargv[0] = argv[0];	/* this */
	xargv[1] = disp[1];	/* name of method to call */
	memcpy(xargv+2, argv+1, (argc-1)*sizeof(Val));
	if(waserror(vm)){
		efree(xargv);
		nexterror(vm);
	}
	*rv = dovm(vm, dcl, argc+1, xargv);
	efree(xargv);
	poperror(vm);
}

static void
ascache1method(As *as, char *id, Closure **p)
{
	Val v;
	Str *ids;

	ids = mkstr0(id);

	/* is method in method table? */
	v = tabget(as->mtab, mkvalstr(ids));
	if(v){
		*p = valcl(v);
		return;
	}

	/* no dispatch, no dice */
	if(as->dispatch == 0){
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
	if(v){
		*p = valcl(v);
		return;
	}

	/* no dispatch, no dice */
	if(ns->dispatch == 0){
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
	for(m = 0; m < len; m++){
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

static Ns*
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
	ctx.otype = valtab(safedovm(vm, ons->enumtype, 1, xargv));
	ctx.osym = valtab(safedovm(vm, ons->enumsym, 1, xargv));

	/* get pointer representation from parent name space */
	tt = mkctypebase(Vptr, Rundef);
	vp = tabget(ctx.otype, mkvalctype(tt));
	if(vp == 0)
		vmerr(vm,
		      "derived name space must define pointer representation");
	ctx.ptrrep = typerep(valctype(vp));

	t = ctx.rawtype;
	for(i = 0; i < t->sz; i++){
		x = vecref(t->ht, i);
		while(islink(x)){
			lnk = (Pair*)x;
			v = linkkey(lnk);
			x = linknext(lnk);
			if(Vkind(v) != Qctype)
				vmerr(vm, "invalid raw type table");
			resolvetypename(vm, valctype(v), &ctx);
		}
	}

	t = ctx.rawsym;
	for(i = 0; i < t->sz; i++){
		x = vecref(t->ht, i);
		while(islink(x)){
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
			if(tt != 0){
				/* reuse vec */
				vecset(vec, Typepos, mkvalctype(tt));
				tabput(ctx.sym, idv, vecv);
			}
		}
	}

	/* inherit sym and type definitions not shadowed by @names */
	t = ctx.otype;
	for(i = 0; i < t->sz; i++){
		x = vecref(t->ht, i);
		while(islink(x)){
			lnk = (Pair*)x;
			vp = linkkey(lnk);
			if(!tabget(ctx.type, vp))
				tabput(ctx.type, vp, linkval(lnk));
			x = linknext(lnk);
		}
	}
	t = ctx.osym;
	for(i = 0; i < t->sz; i++){
		x = vecref(t->ht, i);
		while(islink(x)){
			lnk = (Pair*)x;
			vp = linkkey(lnk);
			if(!tabget(ctx.sym, vp))
				tabput(ctx.sym, vp, linkval(lnk));
			x = linknext(lnk);
		}
	}

	/* add enumeration constants to symtab */
	t = ctx.type;
	for(i = 0; i < t->sz; i++){
		x = vecref(t->ht, i);
		while(islink(x)){
			lnk = (Pair*)x;
			tt = valctype(linkval(lnk));
			if(tt->tkind != Tenum)
				goto next;
			te = (Ctypeenum*)tt;
			tmp = mkctypeconst(tt);
			vec = valvec(te->konst);
			for(j = 0; j < vec->len; j++){
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
	if(m > 0 && cqctflags['w']){
		nvec = mkvec(m);
		for(i = 0; i < m; i++)
			vecset(nvec, i, vecref(vec, i));
		argv[0] = mkvalvec(nvec);
		argv[1] = envlookup(vm->top->env, "typenamecmp");
		l1_sort(vm, 2, argv, &rv);
		xprintf("warning: name space references undefined "
			"type%s:\n", m > 1 ? "s" : "");
		for(i = 0; i < m; i++){
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
	e = &c->insn[c->ninsn];
	for(i = c->insn; i < e; i++)
		i->go = gotab[i->kind];
}

jmp_buf*
_pusherror(VM *vm)
{
	Err *ep;
	if(vm->edepth >= vm->emax){
		vm->err = erealloc(vm->err, vm->emax*sizeof(Err),
				   2*vm->emax*sizeof(Err));
		vm->emax *= 2;
	}
	ep = &vm->err[vm->edepth++];
	ep->fp = vm->fp;
	ep->sp = vm->sp;
	ep->pc = vm->pc;
	ep->cl = vm->cl;
	return &ep->esc;
}

void
nexterror(VM *vm)
{
	Err *ep;
	if(vm->edepth == 0)
		fatal("bad error stack discipline");
	vm->edepth--;
	ep = &vm->err[vm->edepth];
	vm->fp = ep->fp;
	vm->sp = ep->sp;
	vm->pc = ep->pc;
	vm->cl = ep->cl;
	longjmp(ep->esc, 1);
}

void
poperror(VM *vm)
{
	if(vm->edepth == 0)
		fatal("bad error stack discipline");
	vm->edepth--;
}

Val
cqctgcprotect(VM *vm, Val v)
{
	return gclock(v);
}

Val
cqctgcunprotect(VM *vm, Val v)
{
	return gcunlock(v);
}

Val
cqctgcpersist(VM *vm, Val v)
{
	return gclock(v);
}

Val
cqctgcunpersist(VM *vm, Val v)
{
	return gcunlock(v);
}

void
builtinnil(Env *env, char *name)
{
	envbind(env, name, Xnil);
}

void
builtinfd(Env *env, char *name, Fd *fd)
{
	Val val;
	val = mkvalfd(fd);
	envbind(env, name, val);
}

void
builtinfn(Env *env, char *name, Closure *cl)
{
	Val val;
	val = mkvalcl(cl);
	envbind(env, name, val);
	if(name[0] == '%')
		envbind(env, name+1, val);
}

static void
builtinns(Env *env, char *name, Ns *ns)
{
	Val val;
	val = mkvalns(ns);
	envbind(env, name, val);
}

static void
builtindom(Env *env, char *name, Dom *dom)
{
	Val val;
	val = mkvaldom(dom);
	envbind(env, name, val);
}

static void
builtintab(Env *env, char *name, Tab *tab)
{
	Val val;
	val = mkvaltab(tab);
	envbind(env, name, val);
}

static void
builtincval(Env *env, char *name, Cval *cv)
{
	Val val;
	val = mkvalcval2(cv);
	envbind(env, name, val);
}

static void
builtinval(Env *env, char *name, Val val)
{
	envbind(env, name, val);
}

static void
vmresetctl(VM *vm)
{
	vm->edepth = 0;
	vm->fp = 0;
	vm->sp = Maxstk;
	vm->ac = Xnil;
	vm->cl = panicthunk();
}

Fd*
vmstdout(VM *vm)
{
	Val v;
	v = envlookup(vm->top->env, "stdout");
	if(v == 0)
		vmerr(vm, "stdout is undefined");
	if(Vkind(v) != Qfd)
		vmerr(vm, "stdout not bound to a file descriptor");
	return valfd(v);
}

Val
safedovm(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	Val rv;
	gcdisable();
	if(waserror(vm)){
		gcenable();
		nexterror(vm);
	}
	rv = dovm(vm, cl, argc, argv);
	poperror(vm);
	gcenable();
	return rv;
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
	v = cqctenvlook(vm->top, "callpostgc");
	if(v && Vkind(v) == Qcl)
		/* we need to preserve the current
		   value of AC.  this dance does so
		   without requiring a special gc-safe
		   save mechanism. */
		vm->ac = dovm(vm, valcl(v), 1, &vm->ac);
	vm->postgctime += usec()-b;
}

Val
dovm(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	Insn *i;
	Cval *cv;
	Val val;
	Imm m, narg, onarg;

#ifdef THREADED
	if(!vm){
		gotab[Iadd]	= &&Iadd;
		gotab[Iand]	= &&Iand;
		gotab[Iargc]	= &&Iargc;
		gotab[Ibox]	= &&Ibox;
		gotab[Ibox0]	= &&Ibox0;
		gotab[Icall]	= &&Icall;
		gotab[Icallc]	= &&Icallc;
		gotab[Icallt]	= &&Icallt;
		gotab[Icalltc]	= &&Icalltc;
		gotab[Iclo]	= &&Iclo;
		gotab[Icmpeq] 	= &&Icmpeq;
		gotab[Icmpgt] 	= &&Icmpgt;
		gotab[Icmpge] 	= &&Icmpge;
		gotab[Icmplt] 	= &&Icmplt;
		gotab[Icmple] 	= &&Icmple;
		gotab[Icmpneq] 	= &&Icmpneq;
		gotab[Icode]	= &&Icode;
		gotab[Idiv] 	= &&Idiv;
		gotab[Iframe] 	= &&Iframe;
		gotab[Ifmask]	= &&Ifmask;
		gotab[Ifsize]	= &&Ifsize;
		gotab[Ihalt] 	= &&Ihalt;
		gotab[Iinv] 	= &&Iinv;
		gotab[Ijmp] 	= &&Ijmp;
		gotab[Ijnz] 	= &&Ijnz;
		gotab[Ijz] 	= &&Ijz;
		gotab[Ikg] 	= &&Ikg;
		gotab[Ikp] 	= &&Ikp;
		gotab[Ilist]	= &&Ilist;
		gotab[Imod] 	= &&Imod;
		gotab[Imov] 	= &&Imov;
		gotab[Imul] 	= &&Imul;
		gotab[Ineg] 	= &&Ineg;
		gotab[Inot] 	= &&Inot;
		gotab[Ior] 	= &&Ior;
		gotab[Inop] 	= &&Inop;
		gotab[Ipanic] 	= &&Ipanic;
		gotab[Ipush] 	= &&Ipush;
		gotab[Ipushi] 	= &&Ipushi;
		gotab[Iret] 	= &&Iret;
		gotab[Ishl] 	= &&Ishl;
		gotab[Ishr] 	= &&Ishr;
		gotab[Isub] 	= &&Isub;
		gotab[Isubsp] 	= &&Isubsp;
		gotab[Ivargc]	= &&Ivargc;
		gotab[Ixcast] 	= &&Ixcast;
		gotab[Ixor] 	= &&Ixor;
		return 0;
	}
#endif

	/* for recursive entry, store current context */
	vmpushi(vm, vm->fp);		/* fp */
	vmpush(vm, mkvalcl(vm->cl));	/* cl */
	vmpushp(vm, vm->pc);		/* pc */
	vmpushi(vm, 0);			/* narg */
	vm->fp = vm->sp;

	/* push frame for halt thunk */
	vmpushi(vm, vm->fp);		/* fp */
	vmpush(vm, mkvalcl(halt));	/* cl */
	vmpushp(vm, halt->entry);	/* pc */
	for(m = argc; m > 0; m--)
		vmpush(vm, argv[m-1]);
	vmpushi(vm, argc);		/* narg */
	vm->fp = vm->sp;

	/* switch to cl */
	vm->cl = cl;
	vm->pc = vm->cl->entry;

	while(1){
		NEXTLABEL(i = vm->pc++){
		LABEL Inop:
			continue;
		LABEL Iinv:
		LABEL Ineg:
		LABEL Inot:
			xunop(vm, i->kind, &i->op1, &i->dst);
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
			xbinop(vm, i->kind, &i->op1, &i->op2, &i->dst);
			continue;
		LABEL Isubsp:
			val = getvalrand(vm, &i->op2);  /* max frame size */
			checkoverflow(vm, valimm(val));
			val = getvalrand(vm, &i->op1);
			vm->sp -= valimm(val);
			continue;
		LABEL Imov:
			xmov(vm, &i->op1, &i->dst);
			continue;
		LABEL Ipush:
			xpush(vm, &i->op1);
			continue;
		LABEL Ipushi:
			xpushi(vm, &i->op1);
			continue;
		LABEL Iargc:
			val = getvalrand(vm, &i->op1);
			cv = valcval(val);
			if(stkimm(vm->stack[vm->fp]) != cvalu(cv))
				vmerr(vm, "wrong number of arguments to %s",
				      ciddata(vm->cl->id));
			continue;
		LABEL Ivargc:
			val = getvalrand(vm, &i->op1);
			cv = valcval(val);
			if(stkimm(vm->stack[vm->fp]) < cvalu(cv))
				vmerr(vm, "insufficient arguments to %s",
				      ciddata(vm->cl->id));
			continue;
		LABEL Icall:
			val = getvalrand(vm, &i->op1);
			if(Vkind(val) != Qcl)
				vmerr(vm, "attempt to call non-procedure");
			vm->cl = valcl(val);
			vm->pc = vm->cl->entry;
			vm->fp = vm->sp;
			gcpoll(vm);
			continue;
		LABEL Icallt:
			val = getvalrand(vm, &i->op1);
			if(Vkind(val) != Qcl)
				vmerr(vm, "attempt to call non-procedure");
			vm->cl = valcl(val);
			/* shift current arguments over previous arguments */
			narg = stkimm(vm->stack[vm->sp]);
			onarg = stkimm(vm->stack[vm->fp]);
			vm->fp = vm->fp+onarg-narg;
			memmove(&vm->stack[vm->fp], &vm->stack[vm->sp],
				(narg+1)*sizeof(Val));
			vm->sp = vm->fp;
			vm->pc = vm->cl->entry;
			gcpoll(vm);
			continue;
		LABEL Icalltc:
			xcalltc(vm);
			continue;
		LABEL Icallc:
			xcallc(vm);
			continue;
		LABEL Iframe:
			vmpushi(vm, vm->fp);
			vmpush(vm, mkvalcl(vm->cl));
			vmpushp(vm, i->targ);
			continue;
		LABEL Ipanic:
			fatal("vm panic");
		LABEL Ihalt:
			/* Ihalt is exactly like Iret... */
			vm->sp = vm->fp+stkimm(vm->stack[vm->fp])+1;/* narg+1 */
			vm->fp = stkimm(vm->stack[vm->sp+2]);
			vm->cl = valcl(vm->stack[vm->sp+1]);
			vm->pc = stkp(vm->stack[vm->sp]);
			vmpop(vm, 3);

			/* ...except that it returns from dovm */
			return vm->ac;
		LABEL Iret:
			vm->sp = vm->fp+stkimm(vm->stack[vm->fp])+1;/* narg+1 */
			vm->fp = stkimm(vm->stack[vm->sp+2]);
			vm->cl = valcl(vm->stack[vm->sp+1]);
			vm->pc = stkp(vm->stack[vm->sp]);
			vmpop(vm, 3);
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			continue;
		LABEL Ijmp:
			vm->pc = i->targ;
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			continue;
		LABEL Ijnz:
			if(!falseval(getvalrand(vm, &i->op1)))
				vm->pc = i->targ;
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			continue;
		LABEL Ijz:
			if(falseval(getvalrand(vm, &i->op1)))
				vm->pc = i->targ;
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			continue;
		LABEL Iclo:
			xclo(vm, &i->op1, i->dstlabel, &i->dst);
			/* vm->sp has been updated */
			continue;
		LABEL Ikg:
			xkg(vm, &i->dst);
			continue;
		LABEL Ikp:
			xkp(vm);
			/* vm->sp, vm->fp have been updated */
			continue;
		LABEL Ibox:
			xbox(vm, &i->op1);
			continue;
		LABEL Ibox0:
			xbox0(vm, &i->op1);
			continue;
		LABEL Ixcast:
			xxcast(vm, &i->op1, &i->op2, &i->dst);
			continue;
		LABEL Ilist:
			xlist(vm, &i->op1, &i->op2, &i->dst);
			continue;
		LABEL Ifsize:
			fatal("attempt to execute frame size");
		LABEL Ifmask:
			fatal("attempt to execute frame mask");
		LABEL Icode:
			fatal("attempt to execute code pointer");
		}
	}
}

void
checkarg(VM *vm, char *fn, Val *argv, unsigned arg, Qkind qkind)
{
	if(Vkind(argv[arg]) != qkind)
		vmerr(vm, "operand %d to %s must be a %s",
		      arg+1, fn, qname[qkind]);
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
	rv = safedovm(vm, cmp, 2, argv);
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
	while(1){
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
	checkarg(vm, "sort", argv, 1, Qcl);
	cmp = valcl(argv[1]);
	o = argv[0];
	switch(Vkind(o)){
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
	if(n < 2){
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
	while(n > 1){
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
	checkarg(vm, "bsearch", argv, 2, Qcl);
	cmp = valcl(argv[2]);
	switch(Vkind(argv[1])){
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
	else if(Vkind(argv[0]) == Qdom){
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
	else if(Vkind(argv[0]) == Qdom){
		dom = valdom(argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to looksym must be a namespace or domain");

	if(Vkind(argv[1]) != Qcid)
		vmerr(vm, "operand 2 to looksym must be an identifier");
	*rv = dovm(vm, ns->looksym, argc, argv);
}

static void
l1_domof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to domof");
	arg0 = argv[0];
	if(Vkind(arg0) == Qcval){
		cv = valcval(arg0);
		*rv = mkvaldom(cv->dom);
	}else if(Vkind(arg0) == Qstr)
		*rv = mkvaldom(litdom);
	else
		vmerr(vm,
		      "operand 1 to domof must be a cvalue or string");
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
	else if(Vkind(argv[arg]) == Qcval){
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
	else if(Vkind(argv[arg]) == Qcval){
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
	safedovm(vm, as->put, 3, argv);
}

Str*
callget(VM *vm, As *as, Imm off, Imm len)
{
	Val rv, argv[2];
	Str *s;

	argv[0] = mkvalas(as);
	argv[1] = mkvalrange2(mkcval(litdom, litdom->ns->base[Vptr], off),
			      mkcval(litdom, litdom->ns->base[Vptr], len));
	rv = safedovm(vm, as->get, 2, argv);
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
	rv = safedovm(vm, as->map, 1, argv);
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
	rv = safedovm(vm, as->ismapped, 2, argv);
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

	for(m = 0; m < map->len; m++){
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
	Imm l, m, n, o;

	/* effectively a call to unit */
	v = callmap(vm, cv->dom->as);
	r = mapstab(vm, v, cvalu(cv), 0);	/* FIXME: type sanity */
	if(r == 0)
		vmerr(vm, "address space access out of bounds");

	l = 0;
	m = cvalu(r->beg)+cvalu(r->len)-cvalu(cv);
	o = cvalu(cv);
	buf = 0;
	n = MIN(m, PAGESZ-o%PAGESZ);
	if(!ismapped(vm, cv->dom->as, o, 1))
		vmerr(vm, "address space access out of bounds");
	while(m > 0){
		if(buf == 0)
			buf = emalloc(n);
		else
			buf = erealloc(buf, l, l+n);
		s = callget(vm, cv->dom->as, o, n);
		memcpy(buf+l, strdata(s), s->len);
		q = strnchr(buf+l, '\0', s->len);
		if(q){
			l += q-(buf+l);
			break;
		}
		l += s->len;
		o += s->len;
		m -= s->len;
		n = MIN(m, PAGESZ);
		if(!ismapped(vm, cv->dom->as, o, 1))
			break;
	}
	s = mkstr(buf, l);	/* FIXME: mkstr copies buf; should steal */
	efree(buf);
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
l1_backtrace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to backtrace");
	USED(argv);
	vmbacktrace(vm);
	USED(rv);
}

static void
l1_myrootns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *r;
	USED(argc);
	USED(argv);
	r = myroot();
	*rv = envlookup(vm->top->env, r);
	if(*rv == 0)
		vmerr(vm, "my root name space is undefined: %s", r);
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
	if(Vkind(argv[0]) == Qcval){
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
	checkarg(vm, "put", iargv, 0, Qdom);
	checkarg(vm, "put", iargv, 1, Qcval);
	checkarg(vm, "put", iargv, 2, Qctype);
	checkarg(vm, "put", iargv, 3, Qcval);
	d = valdom(iargv[0]);
	addr = valcval(iargv[1]);
	t = valctype(iargv[2]);
	cv = valcval(iargv[3]);
	b = chasetype(t);
	switch(b->tkind){
	case Tbase:
	case Tptr:
		cv = typecast(vm, t, cv);
		bytes = enc2str(vm, t, cvalenc(cv));
		callput(vm, d->as, cvalu(addr), typesize(vm, t), bytes);
		*rv = mkvalcval2(cv);
		break;
	case Tbitfield:
		s = subtype(b);
		if(s->tkind == Tundef){
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
l1_close(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to close");
	checkarg(vm, "close", argv, 0, Qfd);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		return;
	fd->flags |= Fclosed;
	if(fd->flags&Ffn){
		if(fd->u.fn.close)
			fd->u.fn.close(&fd->u.fn);
	}else
		if(fd->u.cl.close)
			dovm(vm, fd->u.cl.close, 0, 0);
	USED(rv);
}

static void
l1_fdname(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fdname");
	checkarg(vm, "fdname", argv, 0, Qfd);
	fd = valfd(argv[0]);
	*rv = mkvalstr(fd->name);
}

static void
l1_mkfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Closure *r, *w, *c;
	Str *n;

	if(argc != 3 && argc != 4)
		vmerr(vm, "wrong number of arguments to mkfd");
	if(Vkind(argv[0]) != Qcl && Vkind(argv[0]) != Qnil)
		vmerr(vm, "argument 1 to mkfd must be a function or nil");
	if(Vkind(argv[1]) != Qcl && Vkind(argv[1]) != Qnil)
		vmerr(vm, "argument 2 to mkfd must be a function or nil");
	if(Vkind(argv[2]) != Qcl && Vkind(argv[2]) != Qnil)
		vmerr(vm, "argument 3 to mkfd must be a function or nil");

	r = w = c = 0;
	if(Vkind(argv[0]) == Qcl)
		r = valcl(argv[0]);
	if(Vkind(argv[1]) == Qcl)
		w = valcl(argv[1]);
	if(Vkind(argv[2]) == Qcl)
		c = valcl(argv[2]);
	if(argc == 4){
		checkarg(vm, "mkfd", argv, 3, Qstr);
		n = valstr(argv[3]);
	}else
		n = mkstr0("");
	fd = mkfdcl(n, Fread|Fwrite, r, w, c);
	*rv = mkvalfd(fd);
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
	checkarg(vm, "mksas", argv, 0, Qstr);
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
	switch(argc){
	case 0:
		r = mkrange(mklitcval(Vptr, 0), mklitcval(Vptr, (uptr)-1));
		break;
	case 2:
		checkarg(vm, "mksas", argv, 0, Qcval);
		checkarg(vm, "mksas", argv, 1, Qcval);
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
	checkarg(vm, "mkzas", argv, 0, Qcval);
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
	checkarg(vm, "mkas", argv, 0, Qtab);
	name = 0;
	if(argc == 2){
		checkarg(vm, "mkas", argv, 1, Qstr);
		name = valstr(argv[1]);
	}
	mtab = valtab(argv[0]);
	as = mkastab(mtab, name);
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
	checkarg(vm, "mkns", argv, 0, Qns);
	checkarg(vm, "mkns", argv, 1, Qtab);
	checkarg(vm, "mkns", argv, 2, Qtab);
	name = 0;
	if(argc == 4){
		checkarg(vm, "mknsraw", argv, 3, Qstr);
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
	if(Vkind(argv[0]) != Qcval && Vkind(argv[0]) != Qtab)
		vmerr(vm, "argument 1 to mkattr must be a table or cvalue");
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
	checkarg(vm, "mkns", argv, 0, Qtab);
	name = 0;
	if(argc == 2){
		checkarg(vm, "mkns", argv, 1, Qstr);
		name = valstr(argv[1]);
	}
	mtab = valtab(argv[0]);
	ns = mknstab(mtab, name);
	nscachebase(vm, ns);
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
	checkarg(vm, "mkdom", argv, 0, Qns);
	checkarg(vm, "mkdom", argv, 1, Qas);
	name = 0;
	if(argc == 3){
		checkarg(vm, "mkdom", argv, 2, Qstr);
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
	if(Vkind(argv[0]) == Qdom){
		dom = valdom(argv[0]);
		name = dom->name;
	}else if(Vkind(argv[0]) == Qns){
		ns = valns(argv[0]);
		name = ns->name;
	}else if(Vkind(argv[0]) == Qas){
		as = valas(argv[0]);
		name = as->name;
	}else
		vmerr(vm, "operand 1 to nameof must be a domain, name space"
		      " or address space");
	if(name)
		*rv = mkvalstr(name);
}

static void
l1_setname(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	As *as;
	Str *name;

	name = 0;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to setname");
	if(Vkind(argv[1]) != Qnil && Vkind(argv[1]) != Qstr)
		vmerr(vm, "argument 2 to setname must be a string or nil");
	if(Vkind(argv[1]) == Qstr)
		name = valstr(argv[1]);
	else
		name = 0;
	if(Vkind(argv[0]) == Qdom){
		dom = valdom(argv[0]);
		dom->name = name;
	}else if(Vkind(argv[0]) == Qns){
		ns = valns(argv[0]);
		ns->name = name;
	}else if(Vkind(argv[0]) == Qas){
		as = valas(argv[0]);
		as->name = name;
	}else
		vmerr(vm, "operand 1 to nameof must be a domain, name space"
		      " or address space");
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
	else if(Vkind(argv[0]) == Qcval){
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
	else if(Vkind(argv[0]) == Qcval){
		cv = valcval(argv[0]);
		dom = cv->dom;
	}else
		vmerr(vm, "operand 1 to nsof must be a domain or cvalue");
	*rv = mkvalns(dom->ns);
}

static void
l1_callmethod(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val this, id, args, v, *xargv;
	Dom *dom;
	As *as;
	Ns *ns;
	Str *s;
	Closure *cl, *dcl;
	Imm ll, xargc;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to callmethod");
	checkarg(vm, "callmethod", argv, 1, Qstr);
	checkarg(vm, "callmethod", argv, 2, Qlist);
	this = argv[0];
	id = argv[1];
	args = argv[2];
	dcl = 0;
	v = 0;

	if(Vkind(this) == Qas){
		as = valas(this);
		v = tabget(as->mtab, id);
		if(v == 0)
			dcl = as->dispatch;
	}else if(Vkind(this) == Qns){
		ns = valns(this);
		v = tabget(ns->mtab, id);
		if(v == 0)
			dcl = ns->dispatch;
	}else if(Vkind(this) == Qdom){
		dom = valdom(this);
		/* search as, then ns */
		v = tabget(dom->as->mtab, id);
		if(v == 0)
			v = tabget(dom->ns->mtab, id);
		if(v == 0)
			dcl = dom->as->dispatch;
		if(dcl == 0)
			dcl = dom->ns->dispatch;
	}else
		vmerr(vm,
		      "operand 2 to callmethod must be an address space, "
		      "name space, or domain");

	if(v == 0 && dcl == 0){
		s = valstr(id);
		vmerr(vm, "callmethod target must define %.*s or dispatch",
		      (int)s->len, strdata(s));
	}

	ll = listlen(vallist(args));
	if(v){
		cl = valcl(v);
		xargc = ll+1;
		xargv = emalloc(xargc*sizeof(Val));
		xargv[0] = this;
		listcopyv(vallist(args), 0, ll, xargv+1);
	}else{
		cl = dcl;
		xargc = ll+2;
		xargv = emalloc(xargc*sizeof(Val));
		xargv[0] = this;
		xargv[1] = id;
		listcopyv(vallist(args), 0, ll, xargv+2);
	}
	if(waserror(vm)){
		efree(xargv);
		nexterror(vm);
	}
	*rv = dovm(vm, cl, xargc, xargv);
	poperror(vm);
	efree(xargv);
}

static void
l1_callmethodx(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val this, id, v, *xargv;
	Dom *dom;
	As *as;
	Ns *ns;
	Str *s;
	Closure *cl, *dcl;
	Imm xargc;

	if(argc < 2)
		vmerr(vm, "wrong number of arguments to callmethod");
	checkarg(vm, "callmethod", argv, 1, Qstr);
	this = argv[0];
	id = argv[1];
	dcl = 0;
	v = 0;

	if(Vkind(this) == Qas){
		as = valas(this);
		v = tabget(as->mtab, id);
		if(v == 0)
			dcl = as->dispatch;
	}else if(Vkind(this) == Qns){
		ns = valns(this);
		v = tabget(ns->mtab, id);
		if(v == 0)
			dcl = ns->dispatch;
	}else if(Vkind(this) == Qdom){
		dom = valdom(this);
		/* search as, then ns */
		v = tabget(dom->as->mtab, id);
		if(v == 0)
			v = tabget(dom->ns->mtab, id);
		if(v == 0)
			dcl = dom->as->dispatch;
		if(dcl == 0)
			dcl = dom->ns->dispatch;
	}else
		vmerr(vm,
		      "operand 2 to callmethod must be an address space, "
		      "name space, or domain");

	if(v == 0 && dcl == 0){
		s = valstr(id);
		vmerr(vm, "callmethod target must define %.*s or dispatch",
		      (int)s->len, strdata(s));
	}

	if(v){
		cl = valcl(v);
		xargc = argc-1;
		xargv = argv+1;
		xargv[0] = this;
	}else{
		cl = dcl;
		xargc = argc;
		xargv = argv;
	}
	*rv = dovm(vm, cl, xargc, xargv);
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
	checkarg(vm, "nsreptype", argv, 1, Qcval);
	if(Vkind(argv[0]) == Qns)
		ns = valns(argv[0]);
	else if(Vkind(argv[0]) == Qdom){
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
		if(typerep(ns->base[cb]) == rep){
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
	checkarg(vm, "mkrange", argv, 0, Qcval);
	checkarg(vm, "mkrange", argv, 1, Qcval);
	/* FIXME: check sanity */
	*rv = mkvalrange2(valcval(argv[0]), valcval(argv[1]));
}

static void
l1_rangebeg(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Range *r;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rangebeg");
	checkarg(vm, "rangebeg", argv, 0, Qrange);
	r = valrange(argv[0]);
	*rv = mkvalcval2(r->beg);
}

static void
l1_rangelen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Range *r;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rangelen");
	checkarg(vm, "rangelen", argv, 0, Qrange);
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

	checkarg(vm, "memcpy", argv, 0, Qcval);
	checkarg(vm, "memcpy", argv, 1, Qcval);
	checkarg(vm, "memcpy", argv, 2, Qcval);
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
	checkarg(vm, "stringof", argv, 0, Qcval);
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
	if(argc == 3){
		checkarg(vm, "split", argv, 2, Qcval);
		lim = valcval(argv[2]);
		if(!isnatcval(lim))
			vmerr(vm, "split expects a non-negative limit");
		if(cvalu(lim) == 0){
			*rv = mkvallist(r);
			return;
		}
	}

	/* delimiter set */
	if(argc > 1 && (s = valstrorcvalornil(vm, "split", argv, 1))){
		set = str2cstr(s);
		mflag = 0;
	}else{
		set = xstrdup(" \t\n");
		mflag = 1;
	}

	if(!mflag){
		n = 0;
		q = p;
		while(q < e && (!lim || n < cvalu(lim))){
			if(strchr(set, *q)){
				listins(vm, r, n++, mkvalstr(mkstr(p, q-p)));
				p = q+1;
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
		while(q < e && (!lim || n < cvalu(lim))){
			if(strchr(set, *q)){
				if(intok)
					listins(vm, r, n++,
						mkvalstr(mkstr(p, q-p)));
				intok = 0;
				p = q+1;
				while(p < e && strchr(set, *p))
					p++;
				if(p >= e)
					break;
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
	checkarg(vm, "ismapped", argv, 0, Qcval);
	addr = valcval(argv[0]);
	sz = 0;
	if(argc == 2){
		checkarg(vm, "ismapped", argv, 1, Qcval);
		len = valcval(argv[1]);
		if(!isnatcval(len))
			vmerr(vm, "ismapped expects a non-negative length");
		sz = cvalu(len);
	}
	if(sz == 0){
		if(!isptrtype(addr->type))
			vmerr(vm, "ismapped expects a pointer cvalue");
		sz = typesize(vm, subtype(chasetype(addr->type)));
	}
	if(ismapped(vm, addr->dom->as, cvalu(addr), sz))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_getbytes(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Cval *addr, *len;
	Imm n;
	Ctype *t;

	if(iargc != 1 && iargc != 2)
		vmerr(vm, "wrong number of arguments to getbytes");
	checkarg(vm, "getbytes", iargv, 0, Qcval);
	addr = valcval(iargv[0]);
	t = chasetype(addr->type);
	if(t->tkind != Tptr)
		vmerr(vm, "operand 1 to getbytes must be a pointer");
	if(iargc == 2){
		checkarg(vm, "getbytes", iargv, 1, Qcval);
		len = valcval(iargv[1]);
		n = cvalu(len);
	}else
		n = typesize(vm, subtype(t));
	*rv = mkvalstr(getbytes(vm, addr, n));
}

static void
l1_putbytes(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Cval *addr;
	Str *str;
	Ctype *t;

	if(iargc != 2)
		vmerr(vm, "wrong number of arguments to putbytes");
	checkarg(vm, "putbytes", iargv, 0, Qcval);
	checkarg(vm, "putbytes", iargv, 1, Qstr);
	addr = valcval(iargv[0]);
	str = valstr(iargv[1]);
	t = chasetype(addr->type);
	if(t->tkind != Tptr)
		vmerr(vm, "operand 1 to putbytes must be a pointer");
	callput(vm, addr->dom->as, cvalu(addr), str->len, str);
	USED(rv);
}

static Imm
cntrlen(VM *vm, char *name, Val v)
{
	List *lst;
	Vec *vec;
	Str *str;
	Tab *tab;
	Imm len;

	switch(Vkind(v)){
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
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to count must be a list, string, "
		      "or vector");
	case Qlist:
		lst = vallist(argv[0]);
		len = listlen(lst);
		for(i = 0; i < len; i++)
			if(equalval(v, listref(vm, lst, i)))
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
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to index must be a list, string, "
		      "or vector");
	case Qlist:
		lst = vallist(argv[0]);
		len = listlen(lst);
		for(i = 0; i < len; i++)
			if(equalval(v, listref(vm, lst, i)))
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
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to ismember must be a list, string, "
		      "table, or vector");
	case Qlist:
		lst = vallist(argv[0]);
		len = listlen(lst);
		for(i = 0; i < len; i++)
			if(equalval(v, listref(vm, lst, i)))
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
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to delete must be a list or table");
	case Qlist:
		lst = vallist(argv[0]);
		i = 0;
		while(i < listlen(lst)){
			if(!equalval(v, listref(vm, lst, i)))
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
	if(Vkind(argv[0]) == Qlist && Vkind(argv[1]) == Qlist){
		lst = listconcat(vm, vallist(argv[0]), vallist(argv[1]));
		*rv = mkvallist(lst);
	}else if(Vkind(argv[0]) == Qstr && Vkind(argv[1]) == Qstr){
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
	switch(Vkind(argv[0])){
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
	}
}

static void
l1_cntrput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to cntrput");
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to cntrput must be a container");
	case Qlist:
		l1_listset(vm, argc, argv, rv);
		*rv = argv[2];
		break;
	case Qstr:
		l1_strput(vm, argc, argv, rv);
		if(Vkind(argv[2]) == Qcval){
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
	}
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
l1_isfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isfd", Qfd);
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
l1_gc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	u32 g, tg;
	if(argc == 0)
		gc(vm);
	else if(argc == 1){
		checkarg(vm, "gc", argv, 0, Qcval);
		g = cvalu(valcval(argv[0]));
		dogc(vm, g, g+1);
	}else if(argc == 2){
		checkarg(vm, "gc", argv, 0, Qcval);
		checkarg(vm, "gc", argv, 1, Qcval);
		g = cvalu(valcval(argv[0]));
		tg = cvalu(valcval(argv[1]));
		if(g != tg && g != tg-1)
			vmerr(vm, "invalid arguments to gc");
		dogc(vm, g, tg);
	}else
		vmerr(vm, "wrong number of arguments to gc");
}

static void
l1_compact(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to compact");
	compact(vm);
}

static void
l1_gclock(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to gclock");
	gclock(argv[0]);
}

static void
l1_gcunlock(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to gcunlock");
	gcunlock(argv[0]);
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
	checkarg(vm, "instguard", argv, 0, Qpair);
	p = valpair(argv[0]);
	/* FIXME: we trust that cdr(p) is a valid guardian tconc */
	instguard(p);
}

static void
l1_eval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *str;
	Val cl;
	char *s;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to eval");
	checkarg(vm, "eval", argv, 0, Qstr);
	str = valstr(argv[0]);
	s = str2cstr(str);
	cl = cqctcompile(s, "<eval-input>", vm->top, 0);
	efree(s);
	if(cl == 0)
		return;
	*rv = dovm(vm, valcl(cl), 0, 0);
}

/* ks[0] and ks[1] are success and failure continuations.
   they must be protected from collection, such as by
   being located on a scanned portion of the stack.
   apply cl to argv and call the appropriate continuation.
*/
static void
applyk(VM *vm, Val cl, Val *ks, Imm argc, Val *argv, Val *rv)
{
	if(waserror(vm)){
		*rv = dovm(vm, valcl(ks[1]), 0, 0);
		return;
	}
	*rv = dovm(vm, valcl(cl), argc, argv);
	poperror(vm);
	*rv = dovm(vm, valcl(ks[0]), 1, rv);
}

static void
l1_evalk(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *str;
	Val cl;
	char *s;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to evalk");
	checkarg(vm, "evalk", argv, 0, Qstr);
	checkarg(vm, "evalk", argv, 1, Qcl);
	checkarg(vm, "evalk", argv, 2, Qcl);
	str = valstr(argv[0]);
	s = str2cstr(str);
	cl = cqctcompile(s, "<eval-input>", vm->top, 0);
	efree(s);
	if(cl == 0)
		return;
	applyk(vm, cl, argv+1, 0, 0, rv);
}

static void
l1_apply(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Imm ll, argc, m;
	Val *argv, *ap, *ip, vp;
	Closure *cl;
	List *lst;

	if(iargc < 2)
		vmerr(vm, "wrong number of arguments to apply");
	checkarg(vm, "apply", iargv, 0, Qcl);
	cl = valcl(iargv[0]);
	checkarg(vm, "apply", iargv, iargc-1, Qlist);
	ll = listlen(vallist(iargv[iargc-1]));
	argc = iargc-2+ll;
	argv = emalloc(argc*sizeof(Val));
	ap = argv;
	ip = &iargv[1];
	for(m = 0; m < iargc-2; m++)
		*ap++ = *ip++;
	lst = vallist(*ip);
	for(m = 0; m < ll; m++){
		vp = listref(vm, lst, m);
		*ap++ = vp;
	}
	if(waserror(vm)){
		efree(argv);
		nexterror(vm);
	}
	vp = dovm(vm, cl, argc, argv);
	*rv = vp;
	efree(argv);
	poperror(vm);
}

static void
l1_applyk(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Imm ll, argc, m;
	Val *argv, *ap, *ip, vp;
	List *lst;

	if(iargc < 4)
		vmerr(vm, "wrong number of arguments to applyk");
	checkarg(vm, "applyk", iargv, 0, Qcl);
	checkarg(vm, "applyk", iargv, 1, Qcl);
	checkarg(vm, "applyk", iargv, 2, Qcl);
	checkarg(vm, "applyk", iargv, iargc-1, Qlist);
	ll = listlen(vallist(iargv[iargc-1]));
	argc = iargc-4+ll;
	argv = emalloc(argc*sizeof(Val));
	ap = argv;
	ip = &iargv[3];
	for(m = 0; m < iargc-4; m++)
		*ap++ = *ip++;
	lst = vallist(*ip);
	for(m = 0; m < ll; m++){
		vp = listref(vm, lst, m);
		*ap++ = vp;
	}
	if(waserror(vm)){
		efree(argv);
		nexterror(vm);
	}
	applyk(vm, iargv[0], iargv+1, argc, argv, rv);
	efree(argv);
	poperror(vm);
}

static void
l1_resettop(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(argc);
	USED(argv);
	freeenv(vm->top->env);
	vm->top->env = mktopenv();
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
	while(*lp){
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
	checkarg(vm, "setloadpath", argv, 0, Qlist);
	l = vallist(argv[0]);
	m = listlen(l);
	for(i = 0; i < m; i++){
		v = listref(vm, l, i);
		if(Vkind(v) != Qstr)
			vmerr(vm, "argument 1 to setloadpath "
			      "must be a list of strings");
	}
	lp = emalloc((m+1)*sizeof(char*));
	for(i = 0; i < m; i++)
		lp[i] = str2cstr(valstr(listref(vm, l, i)));
	efree(cqctloadpath);
	cqctloadpath = copystrv(lp);
	for(i = 0; i < m; i++)
		efree(lp[i]);
	efree(lp);
	USED(rv);
}

Val
expr2syntax(Expr *e)
{
	List *l;
	Expr *p;

	if(e == 0)
		return Xnil;
	l = mklist();
	_listappend(l, mkvalstr(mkstr0(S[e->kind])));
//	_listappend(l, mkvalstr(mkstr0(e->src.filename)));
//	_listappend(l, mkvallitcval(Vuint, e->src.line));
	switch(e->kind){
	case E_tid:
	case Eid:
	case Elabel:
	case Egoto:
	case Ekon:
		_listappend(l, e->aux);
		break;
	case Ebinop:
	case Egop:
		_listappend(l, mkvalstr(mkstr0(S[e->op])));
		_listappend(l, expr2syntax(e->e1));
		_listappend(l, expr2syntax(e->e2));
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			_listappend(l, expr2syntax(p->e1));
			p = p->e2;
		}
		break;
	default:
		_listappend(l, expr2syntax(e->e1));
		_listappend(l, expr2syntax(e->e2));
		_listappend(l, expr2syntax(e->e3));
		_listappend(l, expr2syntax(e->e4));
		break;
	}
	return mkvallist(l);
}

static Expr*
syntax2expr(VM *vm, Val a)
{
	Val v;
	Imm i;
	Expr *e;
	Cid *id;
	Str *s;
	Kind k;
	List *l;
	static unsigned skip = 0;

	if(a == Xnil)
		return 0;
	if(Vkind(a) != Qlist)
		goto bad;
	l = (List*)a;
	v = listref(vm, l, 0);
	if(Vkind(v) != Qstr)
		goto bad;
	s = (Str*)v;
	k = s2kind(s);
	if(k == Ebad)
		goto bad;
	switch(k){
	case E_tid:
		v = listref(vm, l, skip+1);
		if(Vkind(v) != Qcid)
			goto bad;
		id = (Cid*)v;
		return Ztidn(ciddata(id), strlen(ciddata(id)));
	case Egoto:
		v = listref(vm, l, skip+1);
		if(Vkind(v) != Qcid)
			goto bad;
		id = (Cid*)v;
		return Zgoton(ciddata(id), strlen(ciddata(id)));
	case Elabel:
		v = listref(vm, l, skip+1);
		if(Vkind(v) != Qcid)
			goto bad;
		id = (Cid*)v;
		return Zlabeln(ciddata(id), strlen(ciddata(id)));
	case E_tg:
		v = listref(vm, l, skip+1);
		if(Vkind(v) != Qcid)
			goto bad;
		id = (Cid*)v;
		return Ztgn(ciddata(id), strlen(ciddata(id)),
			    syntax2expr(vm, listref(vm, l, skip+2)));
	case Eid:
		v = listref(vm, l, skip+1);
		if(Vkind(v) != Qcid)
			goto bad;
		id = (Cid*)v;
		return doidnsrc(0, ciddata(id), strlen(ciddata(id)));
	case Ebinop:
		v = listref(vm, l, skip+1);
		if(Vkind(v) != Qstr)
			goto bad;
		k = s2kind((Str*)v);
		return newbinopsrc(0,
				   k,
				   syntax2expr(vm, listref(vm, l, skip+2)),
				   syntax2expr(vm, listref(vm, l, skip+3)));
	case Egop:
		v = listref(vm, l, skip+1);
		if(Vkind(v) != Qstr)
			goto bad;
		k = s2kind((Str*)v);
		return newgopsrc(0,
				 k,
				 syntax2expr(vm, listref(vm, l, skip+2)),
				 syntax2expr(vm, listref(vm, l, skip+3)));
	case Eelist:
		e = nullelist();
		i = listlen(l)-1;
		while(i > skip){
			e = Zcons(syntax2expr(vm, listref(vm, l, i)), e);
			i--;
		}
		return e;
	default:
		switch(listlen(l)-(skip+1)){
		case 0:
			return Z0(k);
		case 1:
			return Z1(k,
				  syntax2expr(vm, listref(vm, l, skip+1)));
		case 2:
			return Z2(k,
				  syntax2expr(vm, listref(vm, l, skip+1)),
				  syntax2expr(vm, listref(vm, l, skip+2)));
		case 3:
			return Z3(k,
				  syntax2expr(vm, listref(vm, l, skip+1)),
				  syntax2expr(vm, listref(vm, l, skip+2)),
				  syntax2expr(vm, listref(vm, l, skip+3)));
		case 4:
			return Z4(k,
				  syntax2expr(vm, listref(vm, l, skip+1)),
				  syntax2expr(vm, listref(vm, l, skip+2)),
				  syntax2expr(vm, listref(vm, l, skip+3)),
				  syntax2expr(vm, listref(vm, l, skip+4)));
		default:
			break;
		}
	}
bad:
	vmerr(vm, "invalid syntax expression");
}

static void
l1_parse(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	char *whence;
	char *buf;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to parse");
	checkarg(vm, "parse", argv, 0, Qstr);
	if(argc == 2)
		checkarg(vm, "parse", argv, 1, Qstr);
	buf = str2cstr(valstr(argv[0]));
	whence = "<stdin>";
	if(argc == 2)
		whence = str2cstr(valstr(argv[1]));
	e = cqctparse(buf, vm->top, whence);
	efree(buf);
	if(argc == 2)
		efree(whence);
	if(e == 0)
		return;
	*rv = expr2syntax(e);
}

static void
l1_compile(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	Val v;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to compile");
	checkarg(vm, "compile", argv, 0, Qexpr);
	e = valexpr(argv[0]);
	e = Zcons(e, nullelist()); /* wrap in "begin" just in case */
	v = cqctcompile0(e, vm->top, 0);
	if(v != 0)
		*rv = v;
}

static void
l1_front(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to compile");
	checkarg(vm, "front", argv, 0, Qlist);
	e = syntax2expr(vm, argv[0]);
	e = Zcons(e, nullelist()); /* wrap in "begin" just in case */
	e = cqctcompilex(e, vm->top, 0);
	if(e == 0)
		vmerr(vm, "compilex error");
	*rv = expr2syntax(e);
}

static void
l1_pp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to pp");
	checkarg(vm, "front", argv, 0, Qexpr);
	printcqct(valexpr(argv[0]));
	xprintf("\n");
}

static void
l1_mkcl(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Closure *cl;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkcl");
	checkarg(vm, "mkcl", argv, 0, Qstr);
	cl = mkxfn(valstr(argv[0]));
	*rv = mkvalcl(cl);
}

static void
l1_cvalcmp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int r;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cvalcmp");
	checkarg(vm, "cvalcmp", argv, 0, Qcval);
	checkarg(vm, "cvalcmp", argv, 1, Qcval);
	r = cvalcmp(vm, valcval(argv[0]), valcval(argv[1]));
	switch(r){
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
	checkarg(vm, "cval2str", argv, 0, Qcval);
	cv = valcval(argv[0]);
	s = enc2str(vm, cv->type, cvalenc(cv));
	*rv = mkvalstr(s);
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

char*
cqctsprintval(VM *vm, Val v)
{
	Val argv[2], rv;

	Str *s;
	s = mkstrk("%a", 2, Sperm);
	argv[0] = mkvalstr(s);
	argv[1] = v;
	l1_sprintfa(vm, 2, argv, &rv);
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
	[Vbool]=	Ru08le,
	[Vchar]=	Rs08le,
	[Vshort]=	Rs16le,
	[Vint]=		Rs32le,
	[Vlong]=	Rs32le,
	[Vvlong]=	Rs64le,
	[Vuchar]=	Ru08le,
	[Vushort]=	Ru16le,
	[Vuint]=	Ru32le,
	[Vulong]=	Ru32le,
	[Vuvlong]=	Ru64le,
	[Vfloat]=	Rf32,
	[Vdouble]=	Rf64,
	[Vlongdouble]=	Rundef,
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
	[Vbool]=	Ru08le,
	[Vchar]=	Rs08be,
	[Vshort]=	Rs16be,
	[Vint]=		Rs32be,
	[Vlong]=	Rs32be,
	[Vvlong]=	Rs64be,
	[Vuchar]=	Ru08be,
	[Vushort]=	Ru16be,
	[Vuint]=	Ru32be,
	[Vulong]=	Ru32be,
	[Vuvlong]=	Ru64be,
	[Vfloat]=	Rf32,
	[Vdouble]=	Rf64,
	[Vlongdouble]=	Rundef,
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
	[Vbool]=	Ru08le,
	[Vchar]=	Rs08le,
	[Vshort]=	Rs16le,
	[Vint]=		Rs32le,
	[Vlong]=	Rs64le,
	[Vvlong]=	Rs64le,
	[Vuchar]=	Ru08le,
	[Vushort]=	Ru16le,
	[Vuint]=	Ru32le,
	[Vulong]=	Ru64le,
	[Vuvlong]=	Ru64le,
	[Vfloat]=	Rf32,
	[Vdouble]=	Rf64,
	[Vlongdouble]=	Rundef,
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
	[Vbool]=	Ru08le,
	[Vchar]=	Rs08be,
	[Vshort]=	Rs16be,
	[Vint]=		Rs32be,
	[Vlong]=	Rs64be,
	[Vvlong]=	Rs64be,
	[Vuchar]=	Ru08be,
	[Vushort]=	Ru16be,
	[Vuint]=	Ru32be,
	[Vulong]=	Ru64be,
	[Vuvlong]=	Ru64be,
	[Vfloat]=	Rf32,
	[Vdouble]=	Rf64,
	[Vlongdouble]=	Rundef,
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
	[Vbool]=	Ru08le,
	[Vchar]=	Rs08le,
	[Vshort]=	Rs16le,
	[Vint]=		Rs32le,
	[Vlong]=	Rs64le,
	[Vvlong]=	Rs64le,
	[Vuchar]=	Ru08le,
	[Vushort]=	Ru16le,
	[Vuint]=	Ru32le,
	[Vulong]=	Ru64le,
	[Vuvlong]=	Ru64le,
	[Vfloat]=	Rf32,
	[Vdouble]=	Rf64,
	[Vlongdouble]=	Rundef,
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
	[Vbool]=	Ru08le,
	[Vchar]=	Rs08be,
	[Vshort]=	Rs16be,
	[Vint]=		Rs32be,
	[Vlong]=	Rs64be,
	[Vvlong]=	Rs64be,
	[Vuchar]=	Ru08be,
	[Vushort]=	Ru16be,
	[Vuint]=	Ru32be,
	[Vulong]=	Ru64be,
	[Vuvlong]=	Ru64be,
	[Vfloat]=	Rf32,
	[Vdouble]=	Rf64,
	[Vlongdouble]=	Rundef,
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

static void
l1_hash(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to hash");
	*rv = mkvalcval(litdom, litdom->ns->base[Vuint], hashval(argv[0]));
}

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
	for(cb = Vlo; cb < Vnbase; cb++){
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

static Env*
mktopenv(void)
{
	Env *env;

	env = mkenv();

	builtinfn(env, "halt", halt);
	builtinfn(env, "callcc", callcc());

	FN(apply);
	FN(applyk);
	FN(asof);
	FN(backtrace);
	FN(bsearch);
	FN(callmethod);
	FN(callmethodx);
	FN(close);
	FN(compact);
	FN(cntrget);
	FN(cntrput);
	FN(compile);
	FN(concat);
	FN(copy);
	FN(count);
	FN(cref);
	FN(cval);
	FN(cvalcmp);
	FN(cval2str);
	FN(delete);
	FN(domof);
	FN(equal);
	FN(eqv);
	FN(eval);
	FN(evalk);
	FN(error);
	FN(fault);
	FN(fdname);
	FN(front);
	FN(gc);
	FN(gclock);
	FN(gcstats);
	FN(gcunlock);
	FN(getbytes);
	FN(hash);
	FN(index);
	FN(instguard);  // FIXME: make system routine
	FN(isas);
	FN(iscid);
	FN(isctype);
	FN(iscvalue);
	FN(isdom);
	FN(isempty);
	FN(isfd);
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
	FN(istable);
	FN(isvector);
	FN(length);
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
	FN(mkfd);
	FN(mkmas);
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
	FN(putbytes);
	FN(rangebeg);
	FN(rangelen);
	FN(resettop);
	FN(setname);
	FN(setloadpath);
	FN(sizeof);		/* cannot be called directly by user code */
	FN(sort);
	FN(split);
	FN(statistics);
	FN(stringof);

	fnch(env);
	fncid(env);
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
	builtinval(env, "nil", Xnil);
	builtindom(env, "litdom", litdom);
	builtintab(env, "toplevel", env->var);
	builtinns(env, "c32le", mkrootns(&c32le));
	builtinns(env, "c32be", mkrootns(&c32be));
	builtinns(env, "c64le", mkrootns(&c64le));
	builtinns(env, "c64be", mkrootns(&c64be));
	builtinns(env, "clp64le", mkrootns(&clp64le));
	builtinns(env, "clp64be", mkrootns(&clp64be));
	builtincval(env, "NULL", cvalnull);
	builtinnil(env, "$$");

	/* expanded source may call these magic functions */
	builtinfn(env, "$put", mkcfn("$put", l1_put));
	builtinfn(env, "$typeof", mkcfn("$typeof", l1_typeof));

	return env;
}

VM*
cqctmkvm(Toplevel *top)
{
	VM *vm, **vmp;
	Val rv;
	char fb, fp, fq, fT;

	vm = emalloc(sizeof(VM));
	vm->top = top;
	vm->emax = Errinitdepth;
	vm->err = emalloc(vm->emax*sizeof(Err));

	vmresetctl(vm);

	/* register vm with fault handler */
	vmp = vms;
	while(*vmp){
		vmp++;
		if(vmp > vms+Maxvms)
			fatal("too many vms");
	}
	*vmp = vm;
	nvms++;

	/* load prelude (unless suppressed with -d)
	   suppress IR printing and stats */
	if(!cqctflags['d']){
		fb = cqctflags['b'];
		fp = cqctflags['p'];
		fq = cqctflags['q'];
		fT = cqctflags['T'];
		cqctflags['b'] = 0;
		cqctflags['p'] = 0;
		cqctflags['q'] = 0;
		cqctflags['T'] = 0;
		cqcteval(vm, "@include <prelude.cqct>", "<prelude>", &rv);
		cqctflags['b'] = fb;
		cqctflags['p'] = fp;
		cqctflags['q'] = fq;
		cqctflags['T'] = fT;
		resetlabels();
	}

	/* vm is now callable */
	return vm;
}

void
cqctfreevm(VM *vm)
{
	VM **vmp;
	efree(vm->err);
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp == vm){
			*vmp = 0;
			break;
		}
		vmp++;
	}
	efree(vm);
	nvms--;
}

static void
vmfaulthook()
{
	VM **vmp;
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp){
			xprintf("backtrace of vm %p:\n", *vmp);
			fvmbacktrace(*vmp);
			xprintf("\n");
		}
		vmp++;
	}
}

void
initvm()
{
	dovm(0, 0, 0, 0); /* initialize gotab */
	Xundef = gclock(malq(Qundef, sizeof(Head)));
	cccode = gclock(callccode());
	tcccode = gclock(calltccode());
	kcode = gclock(contcode());
	litdom = gclock(mklitdom());
	cvalnull = gclock(mkcval(litdom, litdom->ns->base[Vptr], 0));
	cval0 = gclock(mkcval(litdom, litdom->ns->base[Vint], 0));
	cval1 = gclock(mkcval(litdom, litdom->ns->base[Vint], 1));
	cvalminus1 = gclock(mkcval(litdom, litdom->ns->base[Vint], -1));
	halt = gclock(haltthunk());
	cqctfaulthook(vmfaulthook, 1);
	GCiterdone = emalloc(1); /* unique pointer */
}

void
finivm(void)
{
	gcunlock(Xundef);
	gcunlock(tcccode);
	gcunlock(cccode);
	gcunlock(kcode);
	gcunlock(litdom);
	gcunlock(cvalnull);
	gcunlock(cval0);
	gcunlock(cval1);
	gcunlock(cvalminus1);
	gcunlock(halt);
	cqctfaulthook(vmfaulthook, 0);
	efree(GCiterdone);
}

int
cqctcallfn(VM *vm, Val cl, int argc, Val *argv, Val *rv)
{
	int r;
	r = -1;
	vm->exelast = usec();
	if(waserror(vm))
		goto out;
	if(Vkind(cl) != Qcl)
		goto out;
	vm->flags &= ~VMirq;
	*rv = dovm(vm, valcl(cl), argc, argv);
	poperror(vm);
	r = 0;
out:
	updatestats(vm);
	return r;
}

int
cqctcallthunk(VM *vm, Val cl, Val *rv)
{
	return cqctcallfn(vm, cl, 0, 0, rv);
}

Cbase
cqctvalcbase(Val v)
{
	Cval *cv;
	Ctype *t;
	if(Vkind(v) != Qcval)
		return (Cbase)-1;
	cv = valcval(v);
	t = chasetype(cv->type);
	switch(t->tkind){
	case Tbase:
		return typecbase(t);
	case Tptr:
		return typecbase(cv->dom->ns->base[Vptr]);
	default:
		/* only scalar types in cvalues */
		bug();
	}
}

Val
cqctmkfd(Xfd *xfd, char *name)
{
	Fd *fd;
	Str *n;

	if(name)
		n = mkstr0(name);
	else
		n = mkstr0("");
	fd = mkfdfn(n, Fread|Fwrite, xfd);
	return mkvalfd(fd);
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

char*
cqctvalcstr(Val v)
{
	if(Vkind(v) != Qstr)
		return 0;
	return str2cstr(valstr(v));
}

char*
cqctvalcstrshared(Val v)
{
	Str *s;
	if(Vkind(v) != Qstr)
		return 0;
	s = valstr(v);
	return strdata(s);
}

uint64_t
cqctvalcstrlen(Val v)
{
	Str *s;
	if(Vkind(v) != Qstr)
		return 0;
	s = valstr(v);
	return (uint64_t)s->len;
}

Val
cqctcstrval(char *s)
{
	return mkvalstr(mkstr0(s));
}

Val
cqctcstrnval(char *s, uint64_t len)
{
	return mkvalstr(mkstr(s, len));
}

Val
cqctcstrvalshared(char *s)
{
	return mkvalstr(mkstrk(s, strlen(s), Sperm));
}

Val
cqctcstrnvalshared(char *s, uint64_t len)
{
	return mkvalstr(mkstrk(s, len, Sperm));
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

void
cqctfreecstr(char *s)
{
	efree(s);
}

void
cqctenvbind(Toplevel *top, char *name, Val v)
{
	envbind(top->env, name, v);
}

Val
cqctenvlook(Toplevel *top, char *name)
{
	return envget(top->env, mkcid0(name));
}

uint64_t
cqctlength(Val v)
{
	List *lst;
	Vec *vec;
	Str *str;
	Tab *tab;

	switch(Vkind(v)){
	default:
		return (uint64_t)-1;
	case Qlist:
		lst = vallist(v);
		return listlen(lst);
	case Qstr:
		str = valstr(v);
		return str->len;
	case Qvec:
		vec = valvec(v);
		return vec->len;
	case Qtab:
		tab = valtab(v);
		return tab->nent;
	}
}

Val*
cqctlistvals(Val v)
{
	List *l;
	if(Vkind(v) != Qlist)
		return 0;
	l = vallist(v);
	return &listdata(l)[l->h];
}

Val*
cqctvecvals(Val v)
{
	Vec *vec;
	if(Vkind(v) != Qvec)
		return 0;
	vec = valvec(v);
	return vecdata(vec);
}
