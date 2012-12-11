#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
expandarrow(U *ctx, Expr *e)
{
	Expr *se, *p;

	if(e == 0)
		return e;
	switch(e->kind){
	case Earrow: /* for compile_rval */
		/* rewrite: E->field => (*E).field */
		se = newexpr(Edot,
			     newexpr(Ederef, expandarrow(ctx, e->e1), 0, 0, 0),
			     e->e2, 0, 0);
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, expandarrow(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, expandarrow(ctx, e->e1));
		sete2(e, expandarrow(ctx, e->e2));
		sete3(e, expandarrow(ctx, e->e3));
		sete4(e, expandarrow(ctx, e->e4));
		return e;
	}
}

// find multiple return value contexts and
// expand into ordinary assignment.
static Expr*
expandm(U *ctx, Expr *e)
{
	Expr *p, *se;
	Imm i;

	if(e == 0)
		return e;

	switch(e->kind){
	case Eg:
		if(e->e1->kind != Elist){
			sete1(e, expandm(ctx, e->e1));
			sete2(e, expandm(ctx, e->e2));
			return e;
		}
		p = e->e1->e1;
		while(p->kind == Eelist){
			if(p->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			p = p->e2;
		}
		sete2(e, expandm(ctx, e->e2));
		se = nullelist();
		i = 0;
		p = e->e1->e1;
		while(p->kind == Eelist){
			se = Zcons(Zset(p->e1,
					Zcall(G("cntrget"), 2,
					      doid("$tmp"),
					      Zint(i++))),
				   se);
			p = p->e2;
		}
		se = Zcons(Znil(), se);
		se = invert(se);
		se = Zcons(Zset(doid("$tmp"), expandm(ctx, e->e2)), se);
		se = Zblock(Zlocals(1, "$tmp"), se, NULL);
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, expandm(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, expandm(ctx, e->e1));
		sete2(e, expandm(ctx, e->e2));
		sete3(e, expandm(ctx, e->e3));
		sete4(e, expandm(ctx, e->e4));
		return e;
	}
}

Expr*
docompilew(U *ctx, Expr *e)
{
	expandm(ctx, e);
	expandarrow(ctx, e);
	return e;
}

void
l1_cpw(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpw");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompilew(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
