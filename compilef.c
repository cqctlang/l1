#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
targets(U *ctx, Expr *e, HT *ls)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Egoto:
		id = e->id;
		hput(ls, id, strlen(id), e);
		targets(ctx, e->e2, ls);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			targets(ctx, p->e1, ls);
			p = p->e2;
		}
		break;
	default:
		targets(ctx, e->e1, ls);
		targets(ctx, e->e2, ls);
		targets(ctx, e->e3, ls);
		targets(ctx, e->e4, ls);
		break;
	}
}

static Expr*
rm(U *ctx, Expr *e, HT *ls)
{
	char *id;
	Expr *p, *se;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Elabel:
		id = e->id;
		e->e1 = rm(ctx, e->e1, ls);
		if(hget(ls, id, strlen(id)))
			return e;
		se = e->e1;
		e->e1 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = rm(ctx, p->e1, ls);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = rm(ctx, e->e1, ls);
		e->e2 = rm(ctx, e->e2, ls);
		e->e3 = rm(ctx, e->e3, ls);
		e->e4 = rm(ctx, e->e4, ls);
		return e;
	}
}

static Expr*
rmuseless(U *ctx, Expr *e)
{
	HT *ls;
	ls = mkht();
	targets(ctx, e, ls);
	e = rm(ctx, e, ls);
	freeht(ls);
	return e;
}

static Expr*
rmgoto(U *ctx, Expr *e, Expr **bs)
{
	if(e == 0)
		return 0;
	switch(e->kind){
	case Elabel:
		id = e->id;
		e->e1 = rm(ctx, e->e1, ls);
		if(hget(ls, id, strlen(id)))
			return e;
		se = e->e1;
		e->e1 = 0;
		freeexpr(e);
		return se;
	case Egoto:
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = rmgoto(ctx, p->e1, ls);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = rmgoto(ctx, e->e1, ls);
		e->e2 = rmgoto(ctx, e->e2, ls);
		e->e3 = rmgoto(ctx, e->e3, ls);
		e->e4 = rmgoto(ctx, e->e4, ls);
		return e;
	}
}

static Expr*
rmgoto0(U *ctx, Expr *e)
{
	Expr *p, *q, **bs;

	if(e->kind != Eletrec)
		fatal("bug");
	bs = &e->e1;
	p = e->e1;
	while(p->kind == Eelist){
		q = p->e1;
		q->e2->e1 = rmgoto(ctx, q->e2->e1, bs);
		p = p->e2;
	}
	e->e2 = rmgoto(ctx, e->e2, bs);
	return e;
}

Expr*
docompilef(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = rmuseless(ctx, e);
	e = rmgoto0(ctx, e);
	return e;
}
