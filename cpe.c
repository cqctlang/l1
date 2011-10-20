/* default syntax expander, called only prior to definition
   of "expand" */
#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
noexpand(U *ctx, Expr *e)
{
	Expr *p;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Emcall:
		cerror(ctx, e, "macro call without expander");
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, noexpand(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, noexpand(ctx, e->e1));
		sete2(e, noexpand(ctx, e->e2));
		sete3(e, noexpand(ctx, e->e3));
		sete4(e, noexpand(ctx, e->e4));
		return e;
	}
}

Expr*
docompilee(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = noexpand(ctx, e);
	return e;
}
