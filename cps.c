#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
gccheck(Expr *e)
{
	Expr *p, *b;

	if(e == 0)
		return e;
	switch(e->kind){
	case Elambda:
		sete2(e, gccheck(e->e2));
		/* FIXME: need to simplify
		   this part of IR language */
		b = e->e2;
		if(b->kind == Escope)
			b = b->e1;
		if(b->kind != Eblock)
			bug();
		sete2(b,
		      putsrc(Zcons(Zcall(G("gcpoll"), 0),
				   b->e2),
			     b->src));
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, gccheck(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, gccheck(e->e1));
		sete2(e, gccheck(e->e2));
		sete3(e, gccheck(e->e3));
		sete4(e, gccheck(e->e4));
		return e;
	}
}

Expr*
docompiles(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;
	e = gccheck(e);
	return e;
}
