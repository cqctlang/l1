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
			return putsrc(Zcall(G("mkstxid"), 1, stx(e->e2)),
				      e->src);
		else if(!strcmp(id, "val"))
			return putsrc(Zcall(G("mkstxval"), 1, stx(e->e2)),
				      e->src);
		else
			return putsrc(Zapply(G("mkstx"),
					     Zcons(Zid2sym(e->e1), stx(e->e2))),
				      e->src);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, stx(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, stx(e->e1));
		sete2(e, stx(e->e2));
		sete3(e, stx(e->e3));
		sete4(e, stx(e->e4));
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
