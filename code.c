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
	case Cxfn:
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
	case Cxfn:
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
