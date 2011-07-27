#include "sys.h"
#include "util.h"
#include "syscqct.h"


static Expr*
inquasi(Expr *e)
{
	switch(e->kind){
	case Ekon:
		return Zcall(G("mkkon"), 1, e->aux);
	default:
		return 0;
	}
}

static Expr*
stxquasi(U *ctx, Expr *e)
{
	Expr *p;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estxquasi:
		return inquasi(e->e1);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = stxquasi(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = stxquasi(ctx, e->e1);
		e->e2 = stxquasi(ctx, e->e2);
		e->e3 = stxquasi(ctx, e->e3);
		e->e4 = stxquasi(ctx, e->e4);
		return e;
	}
}

static Expr*
stxquote(U *ctx, Expr *e)
{
	Expr *p;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estxquote:
		return Zkon(mkvalexpr(e->e1));
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = stxquote(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = stxquote(ctx, e->e1);
		e->e2 = stxquote(ctx, e->e2);
		e->e3 = stxquote(ctx, e->e3);
		e->e4 = stxquote(ctx, e->e4);
		return e;
	}
}

static Expr*
quote(U *ctx, Expr *e)
{
	Expr *p;
	Val v;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Equote:
		v = expr2syntax(e->e1);
		p = Zkon(v);
		return p;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = quote(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = quote(ctx, e->e1);
		e->e2 = quote(ctx, e->e2);
		e->e3 = quote(ctx, e->e3);
		e->e4 = quote(ctx, e->e4);
		return e;
	}
}

Expr*
docompileq(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = quote(ctx, e);
	e = stxquote(ctx, e);
//	e = stxquasi(ctx, e);
	return e;
}
