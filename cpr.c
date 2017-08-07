#include "sys.h"
#include "util.h"
#include "syscqct.h"

static int
Zcontains(Expr *id, Expr *l)
{
	if(l->kind == Enull)
		return 0;
	if(l->kind != Eelist)
		bug();
	if(l->e1->kind != Eid)
		bug();
	if(l->e1->aux == id->aux)
		return 1;
	return Zcontains(id, l->e2);
}

static Expr*
record(U *ctx, Expr *e, Expr *s)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Escope:
		sete1(e, record(ctx, e->e1, e));
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, record(ctx, p->e1, s));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, record(ctx, e->e1, s));
		sete2(e, record(ctx, e->e2, s));
		sete3(e, record(ctx, e->e3, s));
		sete4(e, record(ctx, e->e4, s));
		return e;
	}
}

Expr*
docompiler(U *ctx, Expr *e)
{
	record(ctx, e, 0);
	return e;
}

void
l1_cpr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpr");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompiler(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
