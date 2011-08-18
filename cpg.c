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
			p->e1 = compileg(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = compileg(ctx, e->e1);
		e->e2 = compileg(ctx, e->e2);
		e->e3 = compileg(ctx, e->e3);
		e->e4 = compileg(ctx, e->e4);
		return e;
	}
}

Expr*
docompileg(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	compileg(ctx, e);
	return e;
}
