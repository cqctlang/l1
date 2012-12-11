#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
stx(U *ctx, Expr *e)
{
	Expr *p;
	char *id;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Ematch:
		/* syntax in patterns is expanded later */
		sete2(e, stx(ctx, e->e2));
		return e;
	case Estx:
		id = idsym(e->e1);
		if(!strcmp(id, "id")){
			if(elistlen(e->e2) != 1)
				cerror(ctx, e, "wrong number of arguments to #id");
			return putsrc(Zcall(G("mkstxid"), 1, stx(ctx, e->e2->e1)),
				      e->src);
		}else if(!strcmp(id, "val")){
			if(elistlen(e->e2) != 1)
				cerror(ctx, e, "wrong number of arguments to #val");
			return putsrc(Zcall(G("mkstxval"), 1, stx(ctx, e->e2->e1)),
				      e->src);
		}else
			return putsrc(Zapply(G("mkstx"),
					     Zcons(Zid2sym(e->e1), stx(ctx, e->e2))),
				      e->src);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, stx(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, stx(ctx, e->e1));
		sete2(e, stx(ctx, e->e2));
		sete3(e, stx(ctx, e->e3));
		sete4(e, stx(ctx, e->e4));
		return e;
	}
}

Expr*
docompilex(U *ctx, Expr *e)
{
	e = stx(ctx, e);
	return e;
}

void
l1_cpx(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpx");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompilex(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
