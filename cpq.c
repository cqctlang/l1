#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
stxquote(Expr *e)
{
	Expr *p;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estxquote:
		return putsrc(Zval(mkvalexpr(e->e1)), e->src);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, stxquote(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, stxquote(e->e1));
		sete2(e, stxquote(e->e2));
		sete3(e, stxquote(e->e3));
		sete4(e, stxquote(e->e4));
		return e;
	}
}

Expr*
docompileq(U *ctx, Expr *e)
{
	/*
	 * resetlabels should happen before every
	 * compile.  as the start of the first pass,
	 * this is a convenient place to do it.
	 */
	resetlabels();

	return stxquote(e);
}

void
l1_cpq(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpq");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompileq(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
