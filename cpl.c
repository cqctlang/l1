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
		putsrc(se, e->src);
		return se;
	case Elor:
		se = Zifelse(compilel(ctx, e->e1),
			     Zint(1),
			     Zifelse(compilel(ctx, e->e2),
				     Zint(1),
				     Zint(0)));
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, compilel(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, compilel(ctx, e->e1));
		sete2(e, compilel(ctx, e->e2));
		sete3(e, compilel(ctx, e->e3));
		sete4(e, compilel(ctx, e->e4));
		return e;
	}
}

Expr*
docompilel(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	compilel(ctx, e);
	return e;
}
