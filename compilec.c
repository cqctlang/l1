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
vsunion(Vs *from, Vs *to)
{
	Expr *r;
	r = vunion(from->vs, to->vs);
	freeexpr(to->vs);
	to->vs = r;
}

static void
vsdiff(Vs *a, Expr *b)
{
	Expr *r;
	r = vdiff(a->vs, b);
	freeexpr(a->vs);
	a->vs = r;
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
uncoverfree(U *ctx, Expr *e, Vs *fs)
{
	Expr *p;
	Vs nfs;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eid:
		vsinsert(e, fs);
		return e;
	case Eblock:
		vsinit(&nfs);
		e->e2 = uncoverfree(ctx, e->e2, &nfs);
		vsdiff(&nfs, e->e1);
		vsunion(&nfs, fs);
		vsfree(&nfs);
		return e;
	case Elambda:
		vsinit(&nfs);
		e->e2 = uncoverfree(ctx, e->e2, &nfs);
		vsdiff(&nfs, e->e1);
		e->xp = copyexpr(nfs.vs);
		vsunion(&nfs, fs);
		vsfree(&nfs);

		xprintf("%s:%d @lambda(", e->src.filename, e->src.line);
		printids(e->e1);
		xprintf(") -> ");
		printids(e->xp);
		xprintf("\n");

		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = uncoverfree(ctx, p->e1, fs);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = uncoverfree(ctx, e->e1, fs);
		e->e2 = uncoverfree(ctx, e->e2, fs);
		e->e3 = uncoverfree(ctx, e->e3, fs);
		e->e4 = uncoverfree(ctx, e->e4, fs);
		return e;
	}

}

Expr*
docompilec(U *ctx, Expr *e)
{
	Vs fs;
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	vsinit(&fs);
	e = uncoverfree(ctx, e, &fs);
	if(fs.vs->kind != Enull)
		fatal("free bug");
	vsfree(&fs);
	e = letrec(ctx, e);
	return e;
}
