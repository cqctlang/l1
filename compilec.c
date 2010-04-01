#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct Vs
{
	Expr *vs;
} Vs;

static void
vsinsert(Expr *e, Vs *vs)
{
	vs->vs = vinsert(e, vs->vs);
}

static void
vsinit(Vs *vs)
{
	vs->vs = nullelist();
}

static void
vsfree(Vs *vs)
{
	freeexpr(vs->vs);
	vs->vs = 0;
}

static Expr*
letrec(U *ctx, Expr *e)
{
	Expr *p, *u, *se;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Elambda:
		u = uniqid("f");
		e->e2 = letrec(ctx, e->e2);
		se = Zletrec(Zcons(Zbind(u, e), nullelist()), copyexpr(u));
		putsrc(se, &e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = letrec(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = letrec(ctx, e->e1);
		e->e2 = letrec(ctx, e->e2);
		e->e3 = letrec(ctx, e->e3);
		e->e4 = letrec(ctx, e->e4);
		return e;
	}
}

static Expr*
free(U *ctx, Expr *e, Xenv env, Vs *fs)
{
	Expr *p, *r, *s;
	Vs nfs;

	char *id;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eid:
		if(!xenvlook(lex, id))
			vsinsert(e, fs);
		return e;
	case Elambda:
		return 0;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = free(ctx, p->e1, env, fs);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = free(ctx, e->e1, env, fs);
		e->e2 = free(ctx, e->e2, env, fs);
		e->e3 = free(ctx, e->e3, env, fs);
		e->e4 = free(ctx, e->e4, env, fs);
		return e;
	}

}

Expr*
docompilec(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = letrec(ctx, e);
	return e;
}
