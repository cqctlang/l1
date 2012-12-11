#include "sys.h"
#include "util.h"
#include "syscqct.h"

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
	e = stxquote(e);
	return e;
}
