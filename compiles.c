#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
locals(U *ctx, Expr *e, Vs *vs)
{
	Expr *p;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Eblock:
		vsappend(e->e1, vs);
		p = locals(ctx, e->e2, vs);
		e->e2 = 0;
		freeexpr(e);
		return p;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = locals(ctx, p->e1, vs);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = locals(ctx, e->e1, vs);
		e->e2 = locals(ctx, e->e2, vs);
		e->e3 = locals(ctx, e->e3, vs);
		e->e4 = locals(ctx, e->e4, vs);
		return e;
	}
}

static Expr*
locals0(U *ctx, Expr *e)
{
	Vs vs;
	Expr *p, *q, *b;

	if(e->kind != Eletrec)
		fatal("bug");
	p = e->e1;
	while(p->kind == Eelist){
		q = p->e1;
		vsinit(&vs);
		b = locals(ctx, q->e2->e1, &vs);
		q->e2->e1 = Zblock(copyexpr(vs.vs), b);
		vsfree(&vs);
		p = p->e2;
	}
	vsinit(&vs);
	b = locals(ctx, e->e2, &vs);
	e->e2 = Zblock(copyexpr(vs.vs), b);
	vsfree(&vs);
	return e;
}

Expr*
docompiles(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = locals0(ctx, e);
	return e;
}
