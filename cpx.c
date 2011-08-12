#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
stx(Expr *e)
{
	Expr *p;
	char *id;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Ematch:
		/* syntax in patterns is expanded later */
		return e;
	case Estx:
		id = idsym(e->e1);
		if(!strcmp(id, "id"))
			return Zcall(G("mkstxid"), 1, Zid2sym(e->e2));
		else if(!strcmp(id, "val"))
			return Zcall(G("mkstxval"), 1, stx(e->e2));
		else
			return Zapply(G("mkstx"),
				      Zcons(Zid2sym(e->e1), stx(e->e2)));
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = stx(p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = stx(e->e1);
		e->e2 = stx(e->e2);
		e->e3 = stx(e->e3);
		e->e4 = stx(e->e4);
		return e;
	}
}

Expr*
docompilex(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = stx(e);
	return e;
}
