#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
compilel(U *ctx, Expr* e)
{
	Expr *p;
	Expr *se;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Eland:
		se = Zifelse(compilel(ctx, e->e1),
			     Zifelse(compilel(ctx, e->e2),
				     Zint(1),
				     Zint(0)),
			     Zint(0));
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Elor:
		se = Zifelse(compilel(ctx, e->e1),
			     Zint(1),
			     Zifelse(compilel(ctx, e->e2),
				     Zint(1),
				     Zint(0)));
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = compilel(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = compilel(ctx, e->e1);
		e->e2 = compilel(ctx, e->e2);
		e->e3 = compilel(ctx, e->e3);
		e->e4 = compilel(ctx, e->e4);
		return e;
	}
}

int
docompilel(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return -1;	/* error */
	compilel(ctx, e);
	return ctx->errors;
}
