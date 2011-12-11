#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*	stxquasi(U *ctx, Expr *e);

static Expr*
inquasi(U *ctx, Expr *e)
{
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estxunquote:
		return putsrc(Zcall(G("stxliftval"), 1, stxquasi(ctx, e->e1)),
			      e->src);
	case Eid:
		return putsrc(Zcall(G("mkstxid"), 1, Zval(e->aux)), e->src);
	case Eval:
		return putsrc(Zcall(G("mkstxval"), 1, Zval(e->aux)), e->src);
	default:
		return putsrc(Zcall(G("mkstx"), 5,
				    Zval(mkvalcid(mkcid0(S[e->kind]))),
				    inquasi(ctx, e->e1) ?: Znil(),
				    inquasi(ctx, e->e2) ?: Znil(),
				    inquasi(ctx, e->e3) ?: Znil(),
				    inquasi(ctx, e->e4) ?: Znil()),
			      e->src);
	}
}

static Expr*
stxquasi(U *ctx, Expr *e)
{
	Expr *p;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estxunquote:
		cerror(ctx, e, "unquote outside quasiquote");
	case Estxquasi:
		return inquasi(ctx, e->e1);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, stxquasi(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, stxquasi(ctx, e->e1));
		sete2(e, stxquasi(ctx, e->e2));
		sete3(e, stxquasi(ctx, e->e3));
		sete4(e, stxquasi(ctx, e->e4));
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
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = stxquote(e);
	e = stxquasi(ctx, e);
	return e;
}
