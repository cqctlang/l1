#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct Case
{
	char *l;
	Expr *e;
} Case;

typedef
struct Cases
{
	u32 nc, max;
	char *dflt;
	Case *cases;
} Cases;

static char*
genlabel()
{
	static unsigned long m = 0;
	char buf[32];
	snprint(buf, sizeof(buf), ".L%u", m++);
	return xstrdup(buf);
}

static char*
addcase(Expr *e, Cases *cs)
{
	char *rv;
	if(cs->nc >= cs->max){
		cs->cases = erealloc(cs->cases,
				     cs->max*sizeof(Case),
				     2*cs->max*sizeof(Case));
		cs->max *= 2;
	}
	cs->cases[cs->nc].l = genlabel();
	cs->cases[cs->nc].e = e;
	rv = cs->cases[cs->nc].l;
	cs->nc++;
	return rv;
}

static Cases*
mkcases()
{
	Cases *cs;
	cs = emalloc(sizeof(Cases));
	cs->max = 128;
	cs->cases = emalloc(cs->max*sizeof(Case));
	return cs;
}

static void
freecases(Cases *cs)
{
	u32 i;
	for(i = 0; i < cs->nc; i++)
		efree(cs->cases[i].l);
	efree(cs->dflt);
	efree(cs->cases);
	efree(cs);
}

static Expr*
cases(U *ctx, Expr* e, Cases *cs)
{
	Expr *p;
	Expr *se;
	char *l;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Eswitch:
		e->e1 = cases(ctx, e->e1, cs);
		e->xp = mkcases();
		e->e2 = cases(ctx, e->e2, e->xp);
		return e;
	case Ecase:
		l = addcase(cases(ctx, e->e1, cs), cs);
		se = Zblock(nullelist(),
			    Zlabel(l),
			    cases(ctx, e->e2, cs),
			    NULL);
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Edefault:
		cs->dflt = genlabel();
		se = Zblock(nullelist(),
			    Zlabel(cs->dflt),
			    cases(ctx, e->e1, cs),
			    NULL);
		putsrc(se, &e->src);
		e->e1 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = cases(ctx, p->e1, cs);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = cases(ctx, e->e1, cs);
		e->e2 = cases(ctx, e->e2, cs);
		e->e3 = cases(ctx, e->e3, cs);
		e->e4 = cases(ctx, e->e4, cs);
		return e;
	}
}

/* assume all break statments are for switch */
static Expr*
swtch(U *ctx, Expr *e, char *lb)
{
	Expr *p;
	Expr *se;
	Cases *cs;
	u32 i;
	char *nlb;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Ebreak:
		if(!lb)
			fatal("bug");
		se = Zgoto(lb);
		putsrc(se, &e->src);
		freeexpr(e);
		return se;
	case Eswitch:
		cs = e->xp;
		nlb = genlabel();
		se = nullelist();
		for(i = 0; i < cs->nc; i++)
			se = Zcons(Zif(Zbinop(Eeq,
					      doid("$t"),
					      swtch(ctx, cs->cases[i].e, lb)),
				       Zgoto(cs->cases[i].l)),
				   se);
		se = Zblock(Zlocals(1, "$t"),
			    Zset(doid("$t"), swtch(ctx, e->e1, lb)),
			    invert(se),
			    cs->dflt ? Zgoto(cs->dflt) : Zgoto(nlb),
			    swtch(ctx, e->e2, nlb),
			    Zlabel(nlb),
			    Znil(),
			    NULL);
		efree(nlb);
		freecases(cs);
		putsrc(se, &e->src);
		e->xp = 0;
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = swtch(ctx, p->e1, lb);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = swtch(ctx, e->e1, lb);
		e->e2 = swtch(ctx, e->e2, lb);
		e->e3 = swtch(ctx, e->e3, lb);
		e->e4 = swtch(ctx, e->e4, lb);
		return e;
	}
}

static Expr*
loops(U *ctx, Expr* e, char *lb, char *lc)
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
		putsrc(se, &e->src);
		freeexpr(e);
		return se;
	case Econtinue:
		if(!lc)
			return e;
		se = Zgoto(lc);
		putsrc(se, &e->src);
		freeexpr(e);
		return se;
	case Eswitch:
		e->e1 = loops(ctx, e->e1, lb, lc);
		e->e2 = loops(ctx, e->e2, 0, lc);
		return e;
	case Efor:
		h = genlabel();
		nlb = genlabel();
		nlc = genlabel();
		se = Zblock(nullelist(),
			    e->e1 ? loops(ctx, e->e1, lb, lc) : nullelist(),
			    Zlabel(h),
			    (e->e2 ? Zif(Znot(loops(ctx, e->e2, lb, lc)),
					 Zgoto(nlb))
			           : nullelist()),
			    loops(ctx, e->e4, nlb, nlc),
			    Zlabel(nlc),
			    e->e3 ? loops(ctx, e->e3, lb, lc) : nullelist(),
			    Zgoto(h),
			    Zlabel(nlb),
			    Znil(),
			    NULL);
		efree(h);
		efree(nlb);
		efree(nlc);
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		e->e3 = 0;
		e->e4 = 0;
		freeexpr(e);
		return se;
	case Edo:
		h = genlabel();
		nlb = genlabel();
		nlc = genlabel();
		se = Zblock(nullelist(),
			    Zlabel(h),
			    loops(ctx, e->e1, nlb, nlc),
			    Zlabel(nlc),
			    Zif(loops(ctx, e->e2, lb, lc), Zgoto(h)),
			    Zlabel(nlb),
			    Znil(),
			    NULL);
		efree(h);
		efree(nlb);
		efree(nlc);
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ewhile:
		nlb = genlabel();
		nlc = genlabel();
		se = Zblock(nullelist(),
			    Zlabel(nlc),
			    Zif(Znot(loops(ctx, e->e1, lb, lc)), Zgoto(nlb)),
			    loops(ctx, e->e2, nlb, nlc),
			    Zgoto(nlc),
			    Zlabel(nlb),
			    Znil(),
			    NULL);
		efree(nlb);
		efree(nlc);
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = loops(ctx, p->e1, lb, lc);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = loops(ctx, e->e1, lb, lc);
		e->e2 = loops(ctx, e->e2, lb, lc);
		e->e3 = loops(ctx, e->e3, lb, lc);
		e->e4 = loops(ctx, e->e4, lb, lc);
		return e;
	}
}

Expr*
docompilei(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	loops(ctx, e, 0, 0);
	cases(ctx, e, 0);
	swtch(ctx, e, 0);
	return e;
}
