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
	expandc(ctx, e);
	return e;
}

void
l1_cpc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpc");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompilec(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
