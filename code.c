#include "sys.h"
#include "util.h"
#include "syscqct.h"

Code*
ra2code(void *ra, Closure *cl)
{
	return cl->code;
}

static Dbg*
lookdbg(Code *c, uptr off)
{
	Dbg *d, *e, *n;
	d = (Dbg*)strdata(c->dbg);
	e = d+c->ndbg;
	if(d->off > off)
		bug();
	while(d < e-1) {
		n = d+1;
		if(n->off > off)
			return d;
		d = n;
	}
	return d;
}

Imm
ra2mask(void *ra, Code *code)
{
	uptr off;
	Imm fsz, lm;
	Dbg *d;

	switch(code->kind) {
	case Cnative:
	case Cvm:
		if(ra >= codeend(code))
			bug();
		if(ra < codeinsn(code))
			bug();
		off = ra-codeinsn(code);
		d = lookdbg(code, off);
		return d->lm;
	case Ccfn:
	case Cccl:
		fsz = (Imm)(uptr)ra;
		if(fsz >= mwbits-1)
			bug(); /* need to implement this case */
		lm = (1ULL<<fsz)-1;
		lm &= ~(1ULL<<Ora); /* assume everything else is live */
		return lm;
	case Calien:
	default:
		bug();
	}
}

Imm
ra2size(void *ra, Code *code)
{
	uptr off;
	Imm fsz;
	Dbg *d;

	switch(code->kind) {
	case Cnative:
	case Cvm:
		if(ra >= codeend(code))
			bug();
		if(ra < codeinsn(code))
			bug();
		off = ra-codeinsn(code);
		d = lookdbg(code, off);
		return d->fsz;
	case Ccfn:
	case Cccl:
		fsz = (Imm)(uptr)ra;
		return fsz;
	case Calien:
	default:
		bug();
	}
}

Code*
mkncode(Imm nbytes)
{
	Code *c;
	c = mkcode(Cnative, nbytes);
	c->reloc = mkstrn(128);
	c->nreloc = 0;
	return c;
}

void
addreloc(Code *code, uptr coff)
{
	Reloc *r;
	if(code->nreloc >= code->reloc->len/sizeof(Reloc))
		code->reloc = strrealloc(code->reloc, 2*code->reloc->len);
	r = (Reloc*)strdata(code->reloc)+code->nreloc++;
	r->coff = coff;
}

static Src
off2line(Code *code, u64 off)
{
	return (Src)vecref(code->src, off);
}

Src
addr2line(Code *code, Insn *pc)
{
	return off2line(code, pc-(Insn*)codeinsn(code));
}

static u32
off2scope(Code *c, u64 off)
{
	Dbg *d;

	switch(c->kind) {
	case Cvm:
		/* expect offset is in Insn units */
		d = lookdbg(c, off*sizeof(Insn));
		return d->lex;
	case Cnative:
	case Calien:
	case Ccfn:
	case Cccl:
	default:
		bug();
	}
}

List*
frameinfo(Closure *cl, void *pc)
{
	List *l;
	Code *c;
	l = mklist();
	_listappend(l, mkvalcl(cl));
	c = cl->code;
	switch(c->kind) {
	case Cvm:
		_listappend(l, mkvallitcval(Vuvlong,
					    (Insn*)pc-(Insn*)codeinsn(c)));
		break;
	case Cnative:
	case Calien:
	case Ccfn:
	case Cccl:
		_listappend(l, Xnil);
		break;
	default:
		bug();
	}
	return l;
}

static void
printsrc(Xfd *xfd, Code *c, Insn *pc)
{
	Src src;

	switch(c->kind) {
	case Ccfn:
		cprintf(xfd, "%20s\t(builtin function)\n", ciddata(c->id));
		break;
	case Cccl:
		cprintf(xfd, "%20s\t(builtin closure)\n", ciddata(c->id));
		break;
	case Cnative:
		cprintf(xfd, "%20s\t(native code)\n", ciddata(c->id));
		break;
	case Calien:
		cprintf(xfd, "%20s\t(alien code)\n", ciddata(c->id));
		break;
	case Cvm:
		src = addr2line(c, pc);
		if(srclineval(src) == Xnil)
			cprintf(xfd, "%20s\t(%s)\n",
				ciddata(c->id), srcfile(src));
		else
			cprintf(xfd, "%20s\t(%s:%u)\n", ciddata(c->id),
				srcfile(src), srcline(src));
		break;
	default:
		bug();
	}
}

void
printframe(VM *vm, Insn *pc, Code *c)
{
	/* elide system functions */
	if(ciddata(c->id)[0] == '$')
		return;
	printsrc(&l1stdout, c, pc);
}

static void
l1_mkaliencode(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Code *code;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkaliencode");
	if(!Viskind(argv[0], Qstr) && !Viskind(argv[0], Qcval))
		vmerr(vm, "operand 1 to mkaliencode must be a string or cvalue");
	code = mkcode(Calien, 0);
	code->id = mkcid0("*alien*");
	code->alien = argv[0];
	*rv = mkvalcode(code);
}

static void
l1_codename(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Code *c;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to codename");
	checkarg(vm, argv, 0, Qcode);
	c = valcode(argv[0]);
	*rv = mkvalcid(c->id);
}

static void
l1_codekind(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Code *c;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to codekind");
	checkarg(vm, argv, 0, Qcode);
	c = valcode(argv[0]);
	switch(c->kind) {
	case Cnative:
		*rv = mkvalcid(mkcid0("native"));
		break;
	case Cvm:
		*rv = mkvalcid(mkcid0("byte"));
		break;
	case Ccfn:
	case Cccl:
		*rv = mkvalcid(mkcid0("builtin"));
		break;
	case Calien:
		*rv = mkvalcid(mkcid0("alien"));
		break;
	default:
		bug();
	}
}

static void
l1_codesrc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Code *c;
	Cval *o;
	Src s;
	u64 off;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to codesrc");
	checkarg(vm, argv, 0, Qcode);
	checkarg(vm, argv, 1, Qcval);
	c = valcode(argv[0]);
	o = valcval(argv[1]);
	switch(c->kind) {
	case Cvm:
		break;
	case Cnative:
	case Ccfn:
	case Cccl:
	case Calien:
		return;
	default:
		bug();
	}
	off = cvalu(o);
	if(off >= veclen(c->src))
		vmerr(vm, "offset out of bounds");
	s = off2line(c, off);
	*rv = mkvalvec(s);
}

static void
l1_codescope(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Code *c;
	Cval *o;
	u64 off;
	u32 idx;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to codescope");
	checkarg(vm, argv, 0, Qcode);
	checkarg(vm, argv, 1, Qcval);
	c = valcode(argv[0]);
	o = valcval(argv[1]);
	switch(c->kind) {
	case Cvm:
		break;
	case Cnative:
	case Ccfn:
	case Cccl:
	case Calien:
		return;
	default:
		bug();
	}

	off = cvalu(o);
	/* FIXME: we need a better offset bound */
	if(off >= veclen(c->src))
		vmerr(vm, "offset out of bounds");

	idx = off2scope(c, off);
	if(idx != (u32)-1)
		*rv = mkvallitcval(Vuint, idx);
}

static void
l1_codelex(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Code *c;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to codelex");
	checkarg(vm, argv, 0, Qcode);
	c = valcode(argv[0]);
	switch(c->kind) {
	case Cvm:
		if(c->lex)
			*rv = mkvalvec(c->lex);
		break;
	case Cnative:
	case Ccfn:
	case Cccl:
	case Calien:
		return;
	default:
		bug();
	}
}

void
fncode(Env env)
{
	FN(codekind);
	FN(codelex);
	FN(codename);
	FN(codescope);
	FN(codesrc);
	FN(mkaliencode);
}
