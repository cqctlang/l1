#include "sys.h"
#include "util.h"
#include "syscqct.h"

static char*
genlabel()
{
	static unsigned long m = 0;
	char buf[32];
	snprint(buf, sizeof(buf), ".L%u", m++);
	return xstrdup(buf);
}

static Expr*
compilei(U *ctx, Expr* e, char *lb, char *lc)
{
	Expr *p;
	Expr *se;
	char *nlb, *nlc, *h;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Ebreak:
		if(!lb)
			return e;
		se = Zgoto(lb);
		freeexpr(e);
		return se;
	case Econtinue:
		if(!lc)
			return e;
		se = Zgoto(lc);
		freeexpr(e);
		return se;
	case Eswitch:
		e->e1 = compilei(ctx, e->e1, lb, lc);
		e->e2 = compilei(ctx, e->e2, 0, lc);
		return e;
	case Efor:
		e->e1 = compilei(ctx, e->e1, lb, lc);
		e->e2 = compilei(ctx, e->e2, lb, lc);
		e->e3 = compilei(ctx, e->e3, lb, lc);
		e->e4 = compilei(ctx, e->e4, 0, 0);
		return e;
	case Edo:
		h = genlabel();
		nlb = genlabel();
		nlc = genlabel();
		se = Zblock(nullelist(),
			    Zlabel(h),
			    compilei(ctx, e->e1, nlb, nlc),
			    Zlabel(nlc),
			    Zif(compilei(ctx, e->e2, lb, lc), Zgoto(h)),
			    Zlabel(nlb),
			    Znil(),
			    NULL);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ewhile:
		nlb = genlabel();
		nlc = genlabel();
		se = Zblock(nullelist(),
			    Zlabel(nlc),
			    Zif(Znot(compilei(ctx, e->e1, lb, lc)), Zgoto(nlb)),
			    compilei(ctx, e->e2, nlb, nlc),
			    Zgoto(nlc),
			    Zlabel(nlb),
			    Znil(),
			    NULL);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = compilei(ctx, p->e1, lb, lc);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = compilei(ctx, e->e1, lb, lc);
		e->e2 = compilei(ctx, e->e2, lb, lc);
		e->e3 = compilei(ctx, e->e3, lb, lc);
		e->e4 = compilei(ctx, e->e4, lb, lc);
		return e;
	}
}

int
docompilei(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return -1;	/* error */
	compilei(ctx, e, 0, 0);
	return ctx->errors;
}
