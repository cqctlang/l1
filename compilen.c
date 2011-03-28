#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* names(U *ctx, Expr *e);

typedef
struct Seen
{
	HT *sym;
	HT *tag;
	HT *tid;
} Seen;

static void
initseen(Seen *s)
{
	s->sym = mkhts();
	s->tag = mkhts();
	s->tid = mkhts();
}

static void
finiseen(Seen *s)
{
	freeht(s->sym);
	freeht(s->tag);
	freeht(s->tid);
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

	if(e == 0)
		return e;
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

	if(e == 0)
		return e;
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

static Expr* lift(U *ctx, Expr *e);
static Expr* liftspec(U *ctx, Seen *s, Expr *e, Expr **te);

static Expr*
liftdtors(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *p, *a;
	if(e == 0)
		return e;
	switch(e->kind){
	case Eid:
		return e;
	case Earr:
		e->e1 = liftdtors(ctx, s, e->e1, te);
		if(e->e2)
			e->e2 = lift(ctx, e->e2);
		return e;
	case Eptr:
		e->e1 = liftdtors(ctx, s, e->e1, te);
		return e;
	case Efun:
		e->e1 = liftdtors(ctx, s, e->e1, te);
		/* parameters are list of declarations */
		p = e->e2;
		while(!isnull(p)){
			a = p->e1;
			p = p->e2;
			a->e1 = liftspec(ctx, s, a->e1, te);
			a->e2 = liftdtors(ctx, s, a->e2, te);
			a->e3 = lift(ctx, a->e3); /* optional attribute */
		}
		return e;
	case Eelist:
		p = e;
		while(isnull(p)){
			p->e1 = liftdtors(ctx, s, p->e1, te);
			p = p->e2;
		}
		return e;
	default:
		fatal("bug");
	}
}

static void
liftspecenum(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *p, *en;
	p = e->e2;
	while(!isnull(p)){
		en = p->e1;
		p = p->e2;
		if(en->e2)
			en->e2 = lift(ctx, en->e2);
	}
}

static void
liftspecsu(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *p, *f;

	/* fields */
	p = e->e2;
	while(!isnull(p)){
		f = p->e1;
		p = p->e2;
		f->e1 = liftspec(ctx, s, f->e1, te);
		f->e2 = liftdtors(ctx, s, f->e2, te);
		f->e3 = lift(ctx, f->e3); /* optional field attribute */
		f->e4 = lift(ctx, f->e4); /* width (bitfields only) */
	}

	/* optional su attribute */
	e->e3 = lift(ctx, e->e3);
}

static Expr*
liftspec(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *q;
	char *id;

	switch(e->kind){
	case Ebase:
	case Eid:
		return e;
	case Estruct:
	case Eunion:
	case Eenum:
		if(e->e2 == 0)
			return e; /* not a definition */
		id = idsym(e->e1);
		q = Z1(e->kind, e->e1);
		putsrc(q, &e->src);
		if(hgets(s->tag, id, strlen(id)))
			return q; /* already defined */
		hputs(s->tag, id, strlen(id), id);
		if(e->kind == Eenum)
			liftspecenum(ctx, s, e, te);
		else
			liftspecsu(ctx, s, e, te);
		*te = Zcons(e, *te);
		return q;
	default:
		fatal("bug");
	}
}

static void
lift1name(U *ctx, Seen *s, Expr *e, Expr **te)
{
	switch(e->kind){
	case Etypedef:
	case Edecls:
		e->e1 = liftspec(ctx, s, e->e1, te);
		if(e->e2 && !isnull(e->e2)){
			/* translate declarators and emit declarations */
			e->e2 = liftdtors(ctx, s, e->e2, te);
			*te = Zcons(e, *te);
		}
		break;
	default:
		fatal("bug");
	}
}

static Expr*
liftnames(U *ctx, Expr *e)
{
	Seen s;
	Expr *p, *te;

	if(isnull(e))
		return e;
	initseen(&s);
	te = Znull();
	p = e;
	while(!isnull(p)){
		lift1name(ctx, &s, p->e1, &te);
		p = p->e2;
	}
	finiseen(&s);
	return invert(te);
}

static Expr*
lift(U *ctx, Expr *e)
{
	Expr *p;
	if(e == 0)
		return e;
	switch(e->kind){
	case Enames:
		e->e1 = lift(ctx, e->e1);
		e->e2 = liftnames(ctx, e->e2);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = lift(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = lift(ctx, e->e1);
		e->e2 = lift(ctx, e->e2);
		e->e3 = lift(ctx, e->e3);
		e->e4 = lift(ctx, e->e4);
		return e;
	}
}

static void
dotie(Expr *t, Expr *dtor, Expr **type, Expr **id)
{
	Expr *t;
	t = spec;
	while(dtor){
		switch(dtor->kind){
		case Eid:
			**type = t;
			**id = dtor;
			break;
		case Earr:
			break;
		case Efun:
			break;
		case Eptr:
			break;
		default:
			fatal("bug");
		}
	}
}

static void
tie1sufield(U *ctx, Expr *e, Expr **fs)
{
	Expr *t;
	if(e->kind == Ebitfield){
		/* only one declarator */
		t = e->e1;
		
	}

	if(e->kind != E
}

static void
tie1name(U *ctx, Expr *e, Expr **te)
{
	Expr *p, *fs;

	switch(e->kind){
	case Estruct:
	case Eunion:
		fs = Znull();
		p = e->e2;
		while(!isnull(p)){
			p = p->e2;
			tie1sufield(ctx, p->e1, &fs);
		}
		e->e2 = invert(fs);
		*te = Zcons(e, *te);
		break;
	case Eenum:
		*te = Zcons(e, *te);
		break;
	case Etypedef:
		
		break;
	case Edecls:
		/* replace typedef or sym declaration list */
		break;
	default:
		fatal("bug");
	}
}

static Expr*
tienames(U *ctx, Expr *e)
{
	Expr *p, *te;

	if(isnull(e))
		return e;
	te = Znull();
	p = e;
	while(!isnull(p)){
		tie1name(ctx, p->e1, &te);
		p = p->e2;
	}
	return invert(te);
}

static Expr*
tie(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;
	switch(e->kind){
	case Enames:
		e->e1 = tie(ctx, e->e1);
		e->e2 = tienames(ctx, e->e2);
		return e;
	case Etypename:
		return tietypename(ctx, e);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = tie(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = tie(ctx, e->e1);
		e->e2 = tie(ctx, e->e2);
		e->e3 = tie(ctx, e->e3);
		e->e4 = tie(ctx, e->e4);
		return e;
	}
}

static void
do1name(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *q;
	char *s;

	switch(e->kind){
	case Estruct:
	case Eunion:
	case Eenum:
		break;
	case Etypedef:
		s = idsym(e->e1);
		if(hgets(s->tid, s, strlen(s)))
			return;
		hputs(s->tid, s, strlen(s), s);
		q = Zcall(G("tabinsert"), 3,
			  doid("$typetab"),
			  Z1(Etypedef, e->e1),
			  
			  Zcall(G("mkctype_typedef"), 1, Zstr(s)),
			  Zcall(G("mkctype_typedef"), 2, Zstr(s),
				typedefn(ctx, s, e->e2)));
		*te = Zcons(q, *te);
		break;
	case Edecls:
		break;
	default:
		fatal("bug");
	}
}

static Expr*
donames(U *ctx, Expr *e)
{
	Expr *l, *te, *p;
	Seen seen;

	l = Zlocals(2, "$typetab", "$symtab");
	te = Znull();
	te = Zcons(Zset(doid("$typetab"), Zcall(G("mktab"), 0)), te);
	te = Zcons(Zset(doid("$symtab"), Zcall(G("mktab"), 0)), te);

	initseen(&seen);
	p = e->e2;
	while(!isnull(p)){
		do1name(ctx, &seen, p->e1, &te);
		p = p->e2;
	}
	finiseen(&seen);

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

Expr*
docompilen(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;
	e = enumincs(ctx, e);
	e = enumsub(ctx, 0, e);
	e = lift(ctx, e);    /* lift sues from interior of names decls */
	e = names(ctx, e);
	return e;
}
