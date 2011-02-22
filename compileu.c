#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
uniqids(Expr *e, Xenv *xe)
{
	Expr *p, *u;

	switch(e->kind){
	case Eid:
		u = uniqid(e->id);
		xenvbind(xe, e->id, u->id);  /* steal */
		e->id = 0;
		putsrc(u, &e->src);
		freeexpr(e);
		return u;
	case Eellipsis:
		return e;
	case Enull:
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = uniqids(p->e1, xe);
			p = p->e2;
		}
		return e;
	default:
		fatal("bug");
	}
}

static Expr*
alpha(U *ctx, Expr *e, Xenv *lex)
{
	Expr *p, *se;
	char *u;
	Xenv *rib;

	if(e == 0)
		return e;

	switch(e->kind){
	case Elambda:
	case Eblock:
		rib = mkxenv(lex);
		e->e1 = uniqids(e->e1, rib);
		e->e2 = alpha(ctx, e->e2, rib);
		freexenv(rib);
		return e;
	case Eid:
		if(lex == 0)
			fatal("bug");
		u = xenvlook(lex, e->id);
		if(u == 0)
			fatal("bug");
		se = doid(u);
		putsrc(se, &e->src);
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = alpha(ctx, p->e1, lex);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = alpha(ctx, e->e1, lex);
		e->e2 = alpha(ctx, e->e2, lex);
		e->e3 = alpha(ctx, e->e3, lex);
		e->e4 = alpha(ctx, e->e4, lex);
		return e;
	}
}

Expr*
docompileu(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = alpha(ctx, e, 0);
	return e;
}
