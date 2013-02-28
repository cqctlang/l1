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
	while(d < e-1){
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

	switch(code->kind){
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
		lm = (1UL<<fsz)-1;
		lm &= ~(1ULL<<Ora); /* assume everything else is live */
		return lm;
	case Cxfn:
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

	switch(code->kind){
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
	case Cxfn:
	default:
		bug();
	}
}

Code*
mkncode(Imm nbytes)
{
	Code *c;
	c = mkcode(Cxfn, nbytes);
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

Src
addr2line(Code *code, Insn *pc)
{
	return (Src)vecref(code->src, pc-(Insn*)codeinsn(code));
}

static void
printsrc(Xfd *xfd, Code *c, Insn *pc)
{
	Src src;

	switch(c->kind){
	case Ccfn:
		cprintf(xfd, "%20s\t(builtin function)\n", ciddata(c->id));
		break;
	case Cccl:
		cprintf(xfd, "%20s\t(builtin closure)\n", ciddata(c->id));
		break;
	case Cxfn:
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
	if(Vkind(argv[0]) != Qstr && Vkind(argv[0]) != Qcval)
		vmerr(vm, "operand 1 to mkaliencode must be a string or cvalue");
	code = mkcode(Cxfn, 0);
	code->id = mkcid0("*alien*");
	code->xfn = argv[0];
	*rv = mkvalcode(code);
}

void
fncode(Env env)
{
	FN(mkaliencode);
}
