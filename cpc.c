#include "sys.h"
#include "util.h"
#include "syscqct.h"

/* expand syntax of various C forms for subsequent stages */
static Expr*
expandc(U *ctx, Expr *e)
{
	Expr *se, *p;

	if(e == 0)
		return e;
	switch(e->kind){
	case Earef: /* for compile_rval */
		/* rewrite: E1[E2] => *(E1+E2) */
		se = newexpr(Ederef,
			     Zadd(expandc(ctx, e->e1), expandc(ctx, e->e2)),
			     0, 0, 0);
		putsrc(se, e->src);
		return se;
	case Earrow: /* for compile_rval */
		/* rewrite: E->field => (*E).field */
		se = newexpr(Edot,
			     newexpr(Ederef, expandc(ctx, e->e1), 0, 0, 0),
			     e->e2, 0, 0);
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, expandc(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, expandc(ctx, e->e1));
		sete2(e, expandc(ctx, e->e2));
		sete3(e, expandc(ctx, e->e3));
		sete4(e, expandc(ctx, e->e4));
		return e;
	}
}

Expr*
docompilec(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return 0;
	expandc(ctx, e);
	return e;
}
