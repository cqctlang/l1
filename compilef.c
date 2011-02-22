#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
targets(U *ctx, Expr *e, HT *ls)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Egoto:
		id = e->id;
		hput(ls, id, strlen(id), e);
		targets(ctx, e->e2, ls);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			targets(ctx, p->e1, ls);
			p = p->e2;
		}
		break;
	default:
		targets(ctx, e->e1, ls);
		targets(ctx, e->e2, ls);
		targets(ctx, e->e3, ls);
		targets(ctx, e->e4, ls);
		break;
	}
}

static Expr*
rmunused(U *ctx, Expr *e, HT *ls)
{
	char *id;
	Expr *p, *se;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Elabel:
		id = e->id;
		if(hget(ls, id, strlen(id)))
			return e;
		se = Znop();
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = rmunused(ctx, p->e1, ls);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = rmunused(ctx, e->e1, ls);
		e->e2 = rmunused(ctx, e->e2, ls);
		e->e3 = rmunused(ctx, e->e3, ls);
		e->e4 = rmunused(ctx, e->e4, ls);
		return e;
	}
}

static Expr*
rmunused0(U *ctx, Expr *e)
{
	HT *ls;
	ls = mkht();
	targets(ctx, e, ls);
	e = rmunused(ctx, e, ls);
	freeht(ls);
	return e;
}

static Expr*
append(Expr *e1, Expr *e2)
{
	if(e1->kind == Eelist || e1->kind == Enull)
		return invert(Zcons(e2, invert(e1)));
	else
		return Zcons(e1, Zcons(e2, nullelist()));
}

static Expr*
delabel(U *ctx, Expr *e, Expr **bs, Expr **nl)
{
	Expr *se, *u;
	Expr *xl, *cl, *al;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Elabel:
		se = Zlambda(nullelist(), Zcons(Znop(), nullelist()));
		e->kind = Eid;
		*bs = Zcons(Zbind(e, se), *bs);
		*nl = se;
		se = Zcall(copyexpr(e), 0);
		return se;
	case Eif:
		xl = cl = al = 0;
		e->e1 = delabel(ctx, e->e1, bs, &xl); // FIXME: consider xl
		e->e2 = delabel(ctx, e->e2, bs, &cl);
		e->e3 = delabel(ctx, e->e3, bs, &al);
		if(cl == 0 && al == 0)
			return e;
		u = uniqid("ff");
		se = Zlambda(nullelist(), Zcons(Znop(), nullelist()));
		*bs = Zcons(Zbind(u, se), *bs);
		*nl = se;
		if(cl)
			cl->e2 = append(cl->e2, Zcall(copyexpr(u), 0));
		else
			e->e2 = append(e->e2, Zcall(copyexpr(u), 0));
		if(al)
			al->e2 = append(al->e2, Zcall(copyexpr(u), 0));
		else
			e->e3 = append(e->e3, Zcall(copyexpr(u), 0));
		return e;
	case Eelist:
		xl = 0;
		*nl = 0;
		e->e1 = delabel(ctx, e->e1, bs, &xl);
		e->e2 = delabel(ctx, e->e2, bs, nl);
		if(xl == 0)
			return e;
		xl->e2 = append(xl->e2, e->e2);
		e->e2 = nullelist();
		if(*nl == 0)
			*nl = xl;
		return e;
	default:
		e->e1 = delabel(ctx, e->e1, bs, nl);
		e->e2 = delabel(ctx, e->e2, bs, nl);
		e->e3 = delabel(ctx, e->e3, bs, nl);
		e->e4 = delabel(ctx, e->e4, bs, nl);
		return e;
	}
}

static Expr*
delabel0(U *ctx, Expr *e)
{
	Expr *p, *q, **bs;
	Expr *nl;

	if(e->kind != Eletrec)
		fatal("bug");
	bs = &e->e1;
	p = e->e1;
	while(p->kind == Eelist){
		q = p->e1;
		nl = 0;
		q->e2->e1 = delabel(ctx, q->e2->e1, bs, &nl);
		p = p->e2;
	}
	nl = 0;
	e->e2 = delabel(ctx, e->e2, bs, &nl);
	return e;
}

static int
markescape(U *ctx, Expr *e)
{
	Expr *p;
	int rv;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Egoto:
		return 1;
	case Eret:
		markescape(ctx, e->e1);
		return 1;
	case Eif:
		if(markescape(ctx, e->e1)
		   || markescape(ctx, e->e2)
		   || markescape(ctx, e->e3))
			return 1;
		else
			return 0;
	case Eelist:
		p = e;
		rv = 0;
		while(p->kind == Eelist){
			if(markescape(ctx, p->e1)){
				p->xp = (void*)1;
				rv = 1;
			}
			p = p->e2;
		}
		return rv;
	default:
		rv = 0;
		rv |= markescape(ctx, e->e1);
		rv |= markescape(ctx, e->e2);
		rv |= markescape(ctx, e->e3);
		rv |= markescape(ctx, e->e4);
		return rv;
	}
}

static Expr*
tailify(U *ctx, Expr *e, Expr *nxt, Expr **bs)
{
	Expr *p, *q, *se;
	Expr *u;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Econst:
		if(nxt)
			return Zcons(e, Zcons(nxt, nullelist()));
		else
			return e;
	case Eblock:
	case Elambda:
		e->e2 = tailify(ctx, e->e2, nxt, bs);
		return e;
	case Ecall:
		e->e1 = tailify(ctx, e->e1, 0, bs);
		p = e->e2;
		while(p->kind == Eelist){
			p->e1 = tailify(ctx, p->e1, 0, bs);
			p = p->e2;
		}
		return e;
	case Eelist:
		q = invert(e);
		p = q;
		if(nxt && nxt->kind != Eelist)
			nxt = Zcons(nxt, nullelist());
		while(p->kind == Eelist){
			if(p->xp){
				p->xp = 0;
				u = uniqid("ff");
				se = Zlambda(nullelist(), nxt);
				*bs = Zcons(Zbind(u, se), *bs);
				nxt = tailify(ctx, p->e1,
					      Zcall(copyexpr(u), 0), bs);
			}else{
				nxt = Zcons(tailify(ctx, p->e1, 0, bs), nxt);
			}
			p = p->e2;
		}
		return nxt;
	case Eif:
		/* FIXME: it cannot be right to ignore this form */
	default:
		e->e1 = tailify(ctx, e->e1, nxt, bs);
		e->e2 = tailify(ctx, e->e2, nxt, bs);
		e->e3 = tailify(ctx, e->e3, nxt, bs);
		e->e4 = tailify(ctx, e->e4, nxt, bs);
		return e;
	}
	return 0;
}

static Expr*
tailify0(U *ctx, Expr *e)
{
	Expr *p, *q, **bs;
	Expr *nl;

	if(e->kind != Eletrec)
		fatal("bug");
	bs = &e->e1;
	p = e->e1;
	while(p->kind == Eelist){
		q = p->e1;
		nl = 0;
		q->e2->e1 = tailify(ctx, q->e2->e1, 0, bs);
		p = p->e2;
	}
	nl = 0;
	e->e2 = tailify(ctx, e->e2, 0, bs);
	return e;
}

Expr*
docompilef(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	e = rmunused0(ctx, e);
	e = delabel0(ctx, e);
	markescape(ctx, e);
	e = tailify0(ctx, e);
	return e;
}
