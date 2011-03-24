#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* names(U *ctx, Expr *e);


static void
do1name(U *ctx, Expr *e)
{
	switch(e->kind){
	case Etypedef:
		
	case Edecls:

	default:
		fatal("bug");
	}
}

static Expr*
donames(U *ctx, Expr *e)
{
	Expr *l, *te, *p;

	l = Zlocals(2, "$typetab", "$symtab");
	te = Znull();
	te = Zcons(Zset(doid("$typetab"), Zcall(G("mktab"), 0)), te);
	te = Zcons(Zset(doid("$symtab"), Zcall(G("mktab"), 0)), te);

	p = e->e2;
	while(!isnull(p)){
		do1name(ctx, p->e1);
		p = p->e2;
	}

	te = Zcons(Zcall(G("mknsraw"), 3,
			 names(ctx, e->e1),
			 doid("$typetab"),
			 doid("$symtab")),
		   te);
	te = Zscope(Zblock(l, invert(te), NULL));
	putsrc(te, &e->src);
	return te;
}

static Expr*
names(U *ctx, Expr *e)
{
	Expr *p;
	switch(e->kind){
	case Enames:
		return donames(ctx, e);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = names(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = names(ctx, e->e1);
		e->e2 = names(ctx, e->e2);
		e->e3 = names(ctx, e->e3);
		e->e4 = names(ctx, e->e4);
		return e;
	}
	return 0;
}

static Expr*
inc(Expr *e)
{
	return Zadd(e, Zuint(1));
}

/* substitute enum references with expressions inside @names */
static Expr*
enumsub(U *ctx, HT *tab, Expr *e)
{
	Expr *v, *p, *en;
	HT *nt;
	char *s;

	switch(e->kind){
	case Enames:
		e->e1 = enumsub(ctx, tab, e->e1);
		nt = mkhts();
		e->e2 = enumsub(ctx, nt, e->e2);
		freeht(nt);
		return e;
	case Eid:
		/* matching any identifier may be too generous.
		   instead maybe restrict to constant
		   expressions in type definitions. */
		if(tab == 0)
			/* not in @names expression */
			return e;
		v = hgets(tab, idsym(e), strlen(idsym(e)));
		if(v)
			return v;
		else
			return e;
	case Eenum:
		if(e->e2 == 0 || isnull(e->e2))
			return e;
		p = e->e2;
		while(!isnull(p)){
			en = p->e1;
			p = p->e2;
			en->e2 = enumsub(ctx, tab, en->e2);
			s = idsym(en->e1);
			hputs(tab, s, strlen(s), en->e2);
		}
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = enumsub(ctx, tab, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = enumsub(ctx, tab, e->e1);
		e->e2 = enumsub(ctx, tab, e->e2);
		e->e3 = enumsub(ctx, tab, e->e3);
		e->e4 = enumsub(ctx, tab, e->e4);
		return e;
	}
}


/* expand implicit enum constant definitions */
static Expr*
enumincs(U *ctx, Expr *e)
{
	Expr *p, *c, *en;

	switch(e->kind){
	case Eenum:
		if(e->e2 == 0 || isnull(e->e2))
			return e;
		c = Zuint(0);
		p = e->e2;
		while(!isnull(p)){
			en = p->e1;
			p = p->e2;
			if(en->e2){
				en->e2 = enumincs(ctx, en->e2);
				c = inc(en->e2);
			}else{
				en->e2 = c;
				c = inc(en->e2);
			}
		}
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = enumincs(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = enumincs(ctx, e->e1);
		e->e2 = enumincs(ctx, e->e2);
		e->e3 = enumincs(ctx, e->e3);
		e->e4 = enumincs(ctx, e->e4);
		return e;
	}
}

Expr*
compilen(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;
	e = enumincs(ctx, e);
	e = enumsub(ctx, 0, e);
	e = names(ctx, e);
	return e;
}
