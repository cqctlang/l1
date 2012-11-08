#include "sys.h"
#include "util.h"
#include "syscqct.h"

static char* primf[] = {
	[Eadd]=		"add",
	[Esub]=		"sub",
	[Emul]=		"mul",
	[Ediv]=		"div",
	[Emod]=		"mod",
	[Eshl]=		"shl",
	[Eshr]=		"shr",
	[Eband]=	"and",
	[Ebor]=		"or",
	[Ebxor]=	"xor",
	[Egt]=		"cmpgt",
	[Ege]=		"cmpge",
	[Elt]=		"cmplt",
	[Ele]=		"cmple",
	[Eeq]=		"cmpeq",
	[Eneq]=		"cmpneq",
	[Excast]=	"xcast",
	[Euminus]=	"neg",
	[Eutwiddle]=	"inv",
	[Eunot]=	"not",
};

static Expr*
prim(Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;
	switch(e->kind){
	case EBINOP:
		return putsrc(Zcall(G(primf[e->kind]),
				    2, prim(e->e1), prim(e->e2)),
			      e->src);
	case Euminus:
	case Eutwiddle:
	case Eunot:
		return putsrc(Zcall(G(primf[e->kind]), 1, prim(e->e1)),
			      e->src);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, prim(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, prim(e->e1));
		sete2(e, prim(e->e2));
		sete3(e, prim(e->e3));
		sete4(e, prim(e->e4));
		return e;
	}
}

Expr*
docompilef(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;
	e = prim(e);
	return e;
}
