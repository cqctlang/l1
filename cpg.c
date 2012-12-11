#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
compileg(U *ctx, Expr* e)
{
	Expr *p;
	Expr *se;
	Kind op;

	if(e == 0)
		return 0;
	switch(e->kind){
	case EGOP:
		se = Zblock(Zlocals(1, "$tmp"),
			    Zset(doid("$tmp"), compileg(ctx, e->e2)),
			    Zset(e->e1, Zgbinop(e->kind, e->e1, doid("$tmp"))),
			    NULL);
		putsrc(se, e->src);
		return se;
	case Epostinc:
	case Epostdec:
		op = e->kind == Epostinc ? Eadd : Esub;
		se = Zblock(Zlocals(1, "$tmp"),
			    Zset(doid("$tmp"), e->e1),
			    Zset(e->e1, Zbinop(op, doid("$tmp"), Zint(1))),
			    doid("$tmp"),
			    NULL);
		putsrc(se, e->src);
		return se;
	case Epreinc:
	case Epredec:
		op = e->kind == Epreinc ? Eadd : Esub;
		se = Zset(e->e1, Zbinop(op, e->e1, Zint(1)));
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, compileg(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, compileg(ctx, e->e1));
		sete2(e, compileg(ctx, e->e2));
		sete3(e, compileg(ctx, e->e3));
		sete4(e, compileg(ctx, e->e4));
		return e;
	}
}

Expr*
docompileg(U *ctx, Expr *e)
{
	compileg(ctx, e);
	return e;
}

void
l1_cpg(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpg");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompileg(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
