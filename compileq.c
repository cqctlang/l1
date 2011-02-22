#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
quote(U *ctx, Expr *e)
{
	Expr *p;
	Val v;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Equote:
		v = expr2syntax(e->e1);
		p = Zkon(v);
		freeexpr(e);
		return p;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = quote(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = quote(ctx, e->e1);
		e->e2 = quote(ctx, e->e2);
		e->e3 = quote(ctx, e->e3);
		e->e4 = quote(ctx, e->e4);
		return e;
	}
}

Expr*
docompileq(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = quote(ctx, e);
	return e;
}
