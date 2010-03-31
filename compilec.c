#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
letrec(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = letrec(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = letrec(ctx, e->e1);
		e->e2 = letrec(ctx, e->e2);
		e->e3 = letrec(ctx, e->e3);
		e->e4 = letrec(ctx, e->e4);
		return e;
	}
}

Expr*
docompilec(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	return e;
}
