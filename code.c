#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum
{
	FDSZ = 10, /* bytes in a "word" of frame data */
};

Code*
ra2code(void *ra, Closure *cl)
{
	Insn *in;
	switch(cl->code->kind){
	case Cxfn:
		ra -= FDSZ*Ocode;
		return *(Code**)(ra+2);
	case Cvm:
		in = ra;
		in -= Ocode;
		if(in->kind != Icode)
			fatal("no code pointer at return target %p", ra);
		return (Code*)((void*)in-in->cnt);
	case Ccfn:
	case Cccl:
		return 0;
	default:
		bug();
	}
}

Imm
ra2mask(void *ra, Closure *cl)
{
	Insn *in;
	Imm fsz, lm;
	switch(cl->code->kind){
	case Cxfn:
		ra -= FDSZ*Omask;
		return *(Imm*)(ra+2);
	case Cvm:
		in = ra;
		in -= Omask;
		if(in->kind != Ifmask)
			fatal("no live mask at return target %p", ra);
		return in->cnt;
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
ra2size(void *ra, Closure *cl)
{
	Insn *in;
	Imm fsz;
	switch(cl->code->kind){
	case Cxfn:
		ra -= FDSZ*Ofsz;
		return *(Imm*)(ra+2);
	case Cvm:
		in = ra;
		in -= Ofsz;
		if(in->kind != Ifsize)
			fatal("no frame size at return target %p", ra);
		return in->cnt;
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
