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

typedef struct Bind Bind;

struct Bind
{
	Expr *id;
	Expr *exp;
	Bind* next;
};

typedef
struct Match
{
	Expr *check;
	Bind *binds;
} Match;

static Bind *
mkbind(Expr *id, Expr *exp, Bind *next)
{
	Bind *b;
	b = emalloc(sizeof(Bind));
	b->id = id;
	b->exp = exp;
	b->next = next;
	return b;
}

static int
dupbind(Bind *bind, Expr *id)
{
	if(id->kind != Eid)
		bug();
	for(; bind != 0; bind = bind->next)
		if(id->aux == bind->id->aux)
			return 1;
	return 0;
}

static void
freebinds(Bind *bind)
{
	Bind *tmp;
	for(; bind != 0; bind = tmp){
		tmp = bind->next;
		efree(bind);
	}
}

static Expr *
Zand(Expr *e1, Expr *e2)
{
	if(!e1)
		return e2;
	if(!e2)
		return e1;
	return Zifelse(e1, e2, Zint(0));
}

static int
match(U *ctx, Expr* exp, Expr* pat, Match *m)
{
	Expr *p, *e0, *k, *v;
        Match m0;
        int rv, isvarg, l;
        char *id;
        Kind op;

	memset(&m0, 0, sizeof(m0));
	rv = 0;
	isvarg = 0;
	switch(pat->kind){
	case Eid:
		id = idsym(pat);
                if(strcmp(id, "_") != 0){
                        if(dupbind(m->binds, pat))
                                cerror(ctx, pat,
                                       "duplicate pattern variable");
                        m->binds = mkbind(pat, exp, m->binds);
                }
                rv = 1; /* pretend this is a binder even if _ used */
		break;
	case Epair:
		m->check = Zand(m->check, Zcall(doid("ispair"), 1, exp));
		rv |= match(ctx, Zcall(doid("car"), 1,
                                       copyexpr(exp)), pat->e1, m);
		rv |= match(ctx, Zcall(doid("cdr"), 1,
                                      copyexpr(exp)), pat->e2, m);
		break;
	case Elist:
		m->check = Zand(m->check, Zcall(doid("islist"), 1, exp));
		p = pat->e1;
                l = elistlen(p);
                if(hasvarg(p)){
                        isvarg = 1;
                        l -= 2;  /* ignore ellipsis and last variable */
                        if(l < 0)
                                cerror(ctx, pat,
                                       "ellipsis without adjacent binder");
                        op = Ege;
                }
                else
			op = Eeq;
                m->check = Zand(m->check, Zbinop(op,
                                                 Zcall(doid("length"), 1,
                                                       copyexpr(exp)),
                                                 Zint(l)));
		e0 = exp;
                for(; l >= 0; l--){
                        if(isvarg && l == 0){
                                if(p->e1->kind != Eid)
                                        cerror(ctx, pat,
                                               "ellipsis must "
                                               "adjoin a binder");
                                rv |= match(ctx, copyexpr(e0), p->e1, m);
                        }
                        else if(l > 0){
                                rv |= match(ctx, Zcall(doid("head"), 1,
                                                       copyexpr(e0)),
                                            p->e1, m);
                                p = p->e2;
                                e0 = Zcall(doid("tail"), 1, e0);
                        }
		}
		break;
        case Etab:
                m->check = Zand(m->check, Zcall(doid("istable"), 1, exp));
                p = pat->e1;
		/* FIXME: I think I can delete this check; handled by
		   making N calls to tablook below. */
		m->check = Zand(m->check,
				Zbinop(Ege,
				       Zcall(doid("length"), 1, exp),
				       Zint(elistlen(p))));
		while(p->kind != Enull){
                        if(p->e1->kind != Eelist
                           || p->e1->e2->kind != Eelist
                           || p->e1->e2->e2->kind != Enull)
                                fatal("bug in Etab pattern");
                        k = p->e1->e1;
                        v = p->e1->e2->e1;
                        /* check whether key contains pat variables */
                        if(match(ctx, doid("$dummy"), k, &m0)){
                                freebinds(m0.binds);
                                cerror(ctx, pat,
                                       "cannot pattern-match table keys");
                        }
                        /* FIXME: does the same call twice; */
                        /* should memoize the result */
                        e0 = Zcall(doid("tablook"), 2, exp, k);
                        m->check = Zand(m->check, Zbinop(Eneq, e0, Znil()));
			rv |= match(ctx, copyexpr(e0), v, m);
			p = p->e2;
		}
		break;
	default:
		m->check = Zand(m->check, Zbinop(Eeq, pat, exp));
		break;
	}
        return rv;
}

static char*
genlabel()
{
	static unsigned long m = 0;
	char buf[32];
	snprint(buf, sizeof(buf), ".L%u", m++);
	return xstrdup(buf);
}

static void
bindvars(Bind *binds, Expr **bvars, Expr **inits)
{
	Expr *i, *j;
	i = nullelist();
        j = nullelist();
	while(binds){
                j = Zcons(binds->id, j);
		i = Zcons(Zset(binds->id, binds->exp), i);
		binds = binds->next;
	}
        *bvars = j;
	*inits = i;
}

static Expr* cases(U *ctx, Expr* e, Cases *cs);

static Expr*
addcase(U *ctx, Expr *e, Expr *b, Cases *cs)
{
	Expr *se;
	char *l;
        int nc;
	Match m = {0,0};

        if(!cs)
		cerror(ctx, e, "case without switch");
        nc = cs->nc;
        b = cases(ctx,b,cs);
	if(cs->nc >= cs->max){
		cs->cases = erealloc(cs->cases,
				     cs->max*sizeof(Case),
				     2*cs->max*sizeof(Case));
		cs->max *= 2;
	}
	l = genlabel();
	match(ctx, doid("$t"), e, &m);
	if(m.binds != 0){
		Expr *inits, *decls;
                if(nc != cs->nc)
                        cerror(ctx, e,
                               "nested case when pattern matching");
		bindvars(m.binds, &decls, &inits);
		freebinds(m.binds);
		b = Zblock(decls, inits, b, NULL);
	}
	else
		m.check = Zbinop(Eeq, e, doid("$t"));
	se = Zblock(nullelist(), Zlabel(l), b, NULL);
	cs->cases[cs->nc].l = l;
	cs->cases[cs->nc].e = m.check;
	cs->nc++;
	return se;
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
smush(Expr* p, Expr *tail)
{
        if(p->kind != Eelist) fatal("bug in smush");
        switch(p->e1->kind){
        case Ecase:
                p->e1->e2 = Zblock(nullelist(), p->e1->e2, p->e2, NULL);
                p->e2 = tail;
                break;
        case Edefault:
                p->e1->e1 = Zblock(nullelist(), p->e1->e1, p->e2, NULL);
                p->e2 = tail;
                break;
        default:
                fatal("bug in smush(2)");
        }
        return p;
}

/* Blocks together statements associated with the same case statement.
   Nnecessary for binding pattern variables in the cases() phase. */
static Expr*
coalesce(U *ctx, Expr* e, Expr* q)
{
	Expr *p, *pp = 0;

	if(e == 0 || (q && e->kind == Enull))
                return e;
	switch(e->kind){
        case Eelist:
		p = e;
		while(p->kind == Eelist){
                        switch(p->e1->kind){
                        case Ecase:
                        case Edefault:
                                /* start coalescing */
                                if(!q){
                                        p = coalesce(ctx, p->e2, p);
                                        pp = 0;
                                        continue;
                                }
                                /* finish coalescing */
                                else if(pp){
                                        pp->e2 = nullelist();
                                        smush(q,p);
                                }
                                return p;
                        default:
                                p->e1 = coalesce(ctx, p->e1, 0);
                        }
                        pp = p;
                        p = p->e2;
		}
                if(q){
                        smush(q, nullelist());
                        return p;
                }
                else
                        return e;
        default:
                if(q)
                        fatal("bug in coalesce");
		e->e1 = coalesce(ctx, e->e1, 0);
		e->e2 = coalesce(ctx, e->e2, 0);
		e->e3 = coalesce(ctx, e->e3, 0);
		e->e4 = coalesce(ctx, e->e4, 0);
		return e;
	}
}

static Expr*
cases(U *ctx, Expr* e, Cases *cs)
{
	Expr *p;
	Expr *se;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Eswitch:
		e->e1 = cases(ctx, e->e1, cs);
		e->xp = mkcases();
		e->e2 = cases(ctx, e->e2, e->xp);
		return e;
	case Ecase:
		se = addcase(ctx,
                             cases(ctx, e->e1, 0),
                             e->e2, /* recursive call in addcase() */
                             cs);
		putsrc(se, &e->src);
		return se;
	case Edefault:
		cs->dflt = genlabel();
		se = Zblock(nullelist(),
			    Zlabel(cs->dflt),
			    cases(ctx, e->e1, cs),
			    NULL);
		putsrc(se, &e->src);
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
			fatal("bug in swtch");
		se = Zgoto(lb);
		putsrc(se, &e->src);
		return se;
	case Eswitch:
		cs = e->xp;
		nlb = genlabel();
		se = nullelist();
		for(i = 0; i < cs->nc; i++)
			se = Zcons(cs->cases[i].e ?
				   Zif(swtch(ctx, cs->cases[i].e, lb),
				       Zgoto(cs->cases[i].l)) :
				   Zgoto(cs->cases[i].l),
				   se);
		se = Zblock(Zcons(doid("$t"),nullelist()),
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
		return se;
	case Econtinue:
		if(!lc)
			return e;
		se = Zgoto(lc);
		putsrc(se, &e->src);
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
        coalesce(ctx, e, 0);
	cases(ctx, e, 0);
	swtch(ctx, e, 0);
	return e;
}
