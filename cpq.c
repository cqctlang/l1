#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*	stxquasi(Expr *e);

static Expr*
inquasi(Expr *e)
{
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estxunquote:
		return stxquasi(e->e1);
	case Eid:
		return putsrc(Zcall(G("mkstxid"), 1, Zkon(e->aux)),
			      &e->src);
	case Ekon:
		return putsrc(Zcall(G("mkstxval"), 1, Zkon(e->aux)),
			      &e->src);
	default:
		return putsrc(Zcall(G("mkstx"), 5,
				    Zkon(mkvalcid(mkcid0(S[e->kind]))),
				    inquasi(e->e1) ?: Znil(),
				    inquasi(e->e2) ?: Znil(),
				    inquasi(e->e3) ?: Znil(),
				    inquasi(e->e4) ?: Znil()),
			      &e->src);
	}
}

static Expr*
stxquasi(Expr *e)
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
			p->e1 = stxquasi(p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = stxquasi(e->e1);
		e->e2 = stxquasi(e->e2);
		e->e3 = stxquasi(e->e3);
		e->e4 = stxquasi(e->e4);
		return e;
	}
}

static Expr*
stxquote(Expr *e)
{
	Expr *p;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estxquote:
		return putsrc(Zkon(mkvalexpr(e->e1)), &e->src);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = stxquote(p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = stxquote(e->e1);
		e->e2 = stxquote(e->e2);
		e->e3 = stxquote(e->e3);
		e->e4 = stxquote(e->e4);
		return e;
	}
}

Expr*
docompileq(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = stxquote(e);
	e = stxquasi(e);
	return e;
}
