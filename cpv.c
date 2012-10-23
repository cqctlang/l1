#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct Initset
{
	Var **var;
	unsigned max, n;
} Initset;

typedef struct Exprs Exprs;
struct Exprs
{
	Expr *e;
	Exprs *link;
};

static Expr*
uniqids(Expr *e, Xenv *xe)
{
	Expr *p, *u;

	switch(e->kind){
	case Eid:
		u = uniqid(idsym(e));
		xenvbind(xe, idsym(e), idsym(u));  /* idsym(e) is vuln to gc */
		putsrc(u, e->src);
		return u;
	case Eellipsis:
		return e;
	case Enull:
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, uniqids(p->e1, xe));
			p = p->e2;
		}
		return e;
	default:
		fatal("bug");
	}
}

/* alpha: rename each variable with a unique name */
static Expr*
alpha(Expr *e, Xenv *lex)
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
		sete1(e, uniqids(e->e1, rib));
		sete2(e, alpha(e->e2, rib));
		freexenv(rib);
		return e;
	case Egoto:
	case Elabel:
	case E_tid:
		return e;
	case Eid:
		if(lex == 0)
			fatal("bug");
		u = xenvlook(lex, idsym(e));
		if(u == 0)
			fatal("bug (%s)", idsym(e));
		se = doid(u);
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, alpha(p->e1, lex));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, alpha(e->e1, lex));
		sete2(e, alpha(e->e2, lex));
		sete3(e, alpha(e->e3, lex));
		sete4(e, alpha(e->e4, lex));
		return e;
	}
}

/* mkvars: allocate lambda and block variable descriptors */
static void
mkvars(Expr *e)
{
	Lambda *l;
	Block *b;
	Expr *p;
	Var *v;
	unsigned m;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		e->xp = l = emalloc(sizeof(Lambda));
		if(e->e3)
			l->id = idsym(e->e3);
		p = e->e1;
		if(p == 0)
			fatal("bug");
		if(hasvarg(p)){
			l->isvarg = 1;
			l->nparam = elistlen(p)-1; /* don't count ellipsis */
			v = l->param = emalloc(l->nparam*sizeof(Var));
			p = e->e1;
			m = 0;
			while(m < l->nparam-1){
				v->id = idsym(p->e1);
				v->where = Vparam;
				v->idx = m++;
				v++;
				p = p->e2;
			}
			/* by convention varg is first local stack variable */
			v->id = idsym(p->e1);
			v->where = Vlocal;
			v->idx = 0;
		}else{
			l->nparam = elistlen(p);
			v = l->param = emalloc(l->nparam*sizeof(Var));
			p = e->e1;
			m = 0;
			while(p->kind == Eelist){
				v->id = idsym(p->e1);
				v->where = Vparam;
				v->idx = m++;
				v++;
				p = p->e2;
			}
		}
		mkvars(e->e2);
		break;
	case Eblock:
		e->xp = b = emalloc(sizeof(Block));
		p = e->e1;
		m = 0;
		while(p->kind == Eelist){
			m++;
			p = p->e2;
		}
		b->nloc = m;
		v = b->loc = emalloc(m*sizeof(Var));
		p = e->e1;
		while(p->kind == Eelist){
			v->id = idsym(p->e1);
			v->where = Vlocal;
			v++;
			p = p->e2;
		}
		mkvars(e->e2);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			mkvars(p->e1);
			p = p->e2;
		}
		break;
	default:
		mkvars(e->e1);
		mkvars(e->e2);
		mkvars(e->e3);
		mkvars(e->e4);
		break;
	}
}

static void
bindvars(Xenv *xe, Var *v, unsigned n)
{
	unsigned m;
	for(m = 0; m < n; m++){
		xenvbind(xe, v->id, v);
		v++;
	}
}

/* idboxes: identify vars to be boxed */
static void
idboxes(Expr *e, Xenv *lex)
{
	Lambda *l;
	Block *b;
	Xenv *rib;
	Expr *p;
	Var *v;
	unsigned m;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
		l = (Lambda*)e->xp;
		rib = mkxenv(lex);
		bindvars(rib, l->param, l->nparam);
		idboxes(e->e2, rib);
		freexenv(rib);
		break;
	case Escope:
		b = (Block*)e->e1->xp;
		rib = mkxenv(lex);
		/* Box all locals because they might
		   be captured in a continuation. */
		for(m = 0, v = b->loc; m < b->nloc; m++, v++)
			v->box = 1;
		bindvars(rib, b->loc, b->nloc);
		idboxes(e->e1->e2, rib);
		freexenv(rib);
		break;
	case Eg:
		/* Box all variables that are assigned */
		v = xenvlook(lex, idsym(e->e1));
		if(v)
			v->box = 1;
		idboxes(e->e2, lex);
		break;
	case E_tg:
		idboxes(e->e2, lex);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			idboxes(p->e1, lex);
			p = p->e2;
		}
		break;
	default:
		idboxes(e->e1, lex);
		idboxes(e->e2, lex);
		idboxes(e->e3, lex);
		idboxes(e->e4, lex);
		break;
	}
}

/* rmscope: rm Escope nodes */
static Expr*
rmscope(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Escope:
		p = rmscope(ctx, e->e1);
		return p;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, rmscope(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, rmscope(ctx, e->e1));
		sete2(e, rmscope(ctx, e->e2));
		sete3(e, rmscope(ctx, e->e3));
		sete4(e, rmscope(ctx, e->e4));
		return e;
	}
}

static Exprs*
newexprs(Expr *e, Exprs *link)
{
	Exprs *es;
	es = emalloc(sizeof(Exprs));
	es->e = e;
	es->link = link;
	return es;
}

static void
freeexprs(Exprs *es)
{
	Exprs *q;
	while(es){
		q = es->link;
		efree(es);
		es = q;
	}
}

static Exprs*
copyexprs(Exprs *es)
{
	if(es == 0)
		return 0;
	return newexprs(es->e, copyexprs(es->link));
}

static void
free1exprs(Exprs *es)
{
	efree(es);
}

static void
free1ls(void *u, char *k, void *v)
{
	freeexprs(v);
}

static Initset*
newinitset(void)
{
	Initset *st;
	st = emalloc(sizeof(Initset));
	st->max = 8;
	st->var = emalloc(st->max*sizeof(Var*));
	return st;
}

static void
freeinitset(Initset *st)
{
	efree(st->var);
	efree(st);
}

static void
initsetins(Initset *st, Var *v)
{
	if(st->n >= st->max){
		st->var = erealloc(st->var, st->max*sizeof(Var*),
				   2*st->max*sizeof(Var*));
		st->max *= 2;
	}
	st->var[st->n++] = v;
}

/* collects vars declared in scope list LES, up to scope LP */
static Initset*
mkinitset(Exprs *les, Exprs *lp)
{
	Initset *st;
	Block *b;
	unsigned m;
	Var *v;

	if(les == lp)
		return newinitset();
	st = mkinitset(les->link, lp);
	if(les->e->kind != Eblock)
		fatal("bug");
	b = (Block*)les->e->xp;
	for(m = 0; m < b->nloc; m++){
		v = &b->loc[m];
		initsetins(st, v);
	}
	return st;
}

/* LES is list of scopes from fn entry to a label.
   GES is list of scopes from fn entry to a goto to that label.
   find where scopes meet (LP) and collect vars defined along
   path from LP to LES.  these vars must be initialized when
   the goto is taken. */
static Initset*
gotoplan(Exprs *les, Exprs *ges)
{
	Exprs *gp, *lp;

	/* find nearest common Expr shared by les and ges */
	lp = les;
	while(lp){
		gp = ges;
		while(gp && gp->e != lp->e)
			gp = gp->link;
		if(gp)
			/* match */
			break;
		lp = lp->link;
	}
	if(lp == 0)
		fatal("gotoplan error");

	return mkinitset(les, lp);
}

static void
gotos(Expr *e, HT *ls, Exprs *ges)
{
	char *id;
	Expr *p;
	Exprs *les;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		break;
	case Eblock:
		ges = newexprs(e, ges);
		gotos(e->e2, ls, ges);
		free1exprs(ges);
		break;
	case Egoto:
		id = idsym(e->e1);
		les = hgets(ls, id, strlen(id));
		e->xp = gotoplan(les, ges);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			gotos(p->e1, ls, ges);
			p = p->e2;
		}
		break;
	default:
		gotos(e->e1, ls, ges);
		gotos(e->e2, ls, ges);
		gotos(e->e3, ls, ges);
		gotos(e->e4, ls, ges);
		break;
	}
}

/* boxgoto: identify non-local control branches that span
   variable initialization; schedule their initialization
   FIXME: this involves too much code.  */
static void
boxgoto(Expr *e, HT *ls, Exprs *les)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
		les = newexprs(e, 0);
		ls = mkhts();
		boxgoto(e->e2, ls, les);
		gotos(e->e2, ls, les);
		free1exprs(les);
		hforeach(ls, free1ls, 0);
		freeht(ls);
		break;
	case Eblock:
		les = newexprs(e, les);
		boxgoto(e->e2, ls, les);
		free1exprs(les);
		break;
	case Elabel:
		id = idsym(e->e1);
		hputs(ls, id, strlen(id), copyexprs(les));
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			boxgoto(p->e1, ls, les);
			p = p->e2;
		}
		break;
	default:
		boxgoto(e->e1, ls, les);
		boxgoto(e->e2, ls, les);
		boxgoto(e->e3, ls, les);
		boxgoto(e->e4, ls, les);
		break;
	}
}

/* exboxes: expose box operations */
static Expr*
exboxes(Expr *e, Xenv *lex)
{
	Lambda *l;
	Block *b;
	Initset *st;
	Expr *es, *p;
	Var *v;
	int m;
	Xenv *rib;

	if(e == 0)
		return e;

	switch(e->kind){
	case E_tid:
	case Elabel:
		/* these aren't the ids you're looking for */
		return e;
	case Eid:
		v = xenvlook(lex, idsym(e));
		if(!v)
			bug();
		if(v->box)
			return putsrc(Zboxref(e), e->src);
		else
			return e;
	case Eg:
		sete2(e, exboxes(e->e2, lex));
		v = xenvlook(lex, idsym(e->e1));
		if(!v)
			bug();
		if(v->box)
			return putsrc(Zboxset(e->e1, e->e2), e->src);
		else
			return e;
	case Elambda:
		l = (Lambda*)e->xp;
		rib = mkxenv(lex);
		bindvars(rib, l->param, l->nparam);
		es = Zcons(exboxes(e->e2, rib), Znull());
		freexenv(rib);
		for(m = l->nparam-1, v = &l->param[m]; m >= 0; m--, v--)
			if(v->box)
				es = putsrc(Zcons(Zmkbox(doid(v->id)), es),
					    e->src);
		sete2(e, es);
		return e;
	case Eblock:
		b = (Block*)e->xp;
		rib = mkxenv(lex);
		bindvars(rib, b->loc, b->nloc);
		es = Zcons(exboxes(e->e2, rib), Znull());
		freexenv(rib);
		for(m = b->nloc-1, v = &b->loc[m]; m >= 0; m--, v--)
			if(v->box)
				es = putsrc(Zcons(Zmkbox0(doid(v->id)),
						  es), e->src);
			else
				es = putsrc(Zcons(Zset(doid(v->id), Znil()),
						  es), e->src);
		sete2(e, es);
		return e;
	case Egoto:
		st = (Initset*)e->xp;
		es = Zcons(e, Znull());
		for(m = 0; m < st->n; m++){
			v = st->var[m];
			if(v->box)
				es = putsrc(Zcons(Zmkbox0(doid(v->id)),
						  es), e->src);
			else
				es = putsrc(Zcons(Zset(doid(v->id), Znil()),
						  es), e->src);
		}
		freeinitset(st);
		e->xp = 0;
		return es;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, exboxes(p->e1, lex));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, exboxes(e->e1, lex));
		sete2(e, exboxes(e->e2, lex));
		sete3(e, exboxes(e->e3, lex));
		sete4(e, exboxes(e->e4, lex));
		return e;
	}
}

/* Find free variables in E.
   LEX is lexical environment governing innermost
   lambda in which E appears.
   LOC is lexical environment within that lambda up to E.
   FREE is the set of free variable identifiers.
*/
static void
fv(Expr *e, Xenv *lex, Xenv *loc, Xenv *free)
{
	Lambda *l;
	Block *b;
	Xenv *rib;
	Expr *p;
	Var *v;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		l = (Lambda*)e->xp;
		rib = mkxenv(loc);
		bindvars(rib, l->param, l->nparam);
		fv(e->e2, lex, rib, free);
		freexenv(rib);
		break;
	case Eblock:
		b = (Block*)e->xp;
		rib = mkxenv(loc);
		bindvars(rib, b->loc, b->nloc);
		fv(e->e2, lex, rib, free);
		freexenv(rib);
		break;
	case E_tid:
	case Elabel:
	case Egoto:
		break;
	case Eid:
		v = xenvlook(lex, idsym(e));
		if(v && !xenvlook(loc, idsym(e)))
			xenvbind(free, idsym(e), v);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			fv(p->e1, lex, loc, free);
			p = p->e2;
		}
		break;
	default:
		fv(e->e1, lex, loc, free);
		fv(e->e2, lex, loc, free);
		fv(e->e3, lex, loc, free);
		fv(e->e4, lex, loc, free);
		break;
	}
}

struct capctl {
	Var **cap;
};

void
cap1(void *u, char *id, void *v)
{
	struct capctl *ctl;
	Var *w;
	USED(id);
	ctl = u;
	w = v;
	*ctl->cap++ = w;
}

/* idfree: identify free variables */
static void
idfree(Expr *e, Xenv *lex)
{
	Lambda *l;
	Block *b;
	Xenv *loc, *free;
	Expr *p;
	unsigned m;
	Var *v, *w;
	struct capctl ctl;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		l = (Lambda*)e->xp;
		loc = mkxenv(0);
		bindvars(loc, l->param, l->nparam);

		/* find free variables (loc ends at this lambda) */
		free = mkxenv(0);
		fv(e->e2, lex, loc, free);
		freexenv(loc);

		l->ncap = xenvsize(free);
		if(l->ncap){
			/* allocate capture plan */
			l->cap = emalloc(l->ncap*sizeof(Var));
			ctl.cap = l->cap;
			xenvforeach(free, cap1, &ctl);

			/* allocate display environment */
			l->disp = emalloc(l->ncap*sizeof(Var));
			l->ndisp = l->ncap;
			for(m = 0, w = l->disp;
			    m < l->ncap;
			    m++, w++){
				v = l->cap[m];
				w->id = v->id;
				w->where = Vdisp;
				w->idx = m;
				w->box = v->box;
			}
		}

		/* recurse (loc extends lexical environment). */
		loc = mkxenv(lex);
		bindvars(loc, l->disp, l->ndisp);
		bindvars(loc, l->param, l->nparam);
		idfree(e->e2, loc);
		freexenv(loc);
		freexenv(free);
		break;
	case Eblock:
		b = (Block*)e->xp;
		loc = mkxenv(lex);
		bindvars(loc, b->loc, b->nloc);
		idfree(e->e2, loc);
		freexenv(loc);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			idfree(p->e1, lex);
			p = p->e2;
		}
		break;
	default:
		idfree(e->e1, lex);
		idfree(e->e2, lex);
		idfree(e->e3, lex);
		idfree(e->e4, lex);
		break;
	}
}

/* convcl: convert closure forms to closure construction */
static Expr*
convcl(Expr *e)
{
	Expr *se;
	Lambda *l;
	int m;

	if(e == 0)
		return e;

	switch(e->kind){
	case Elambda:
		sete2(e, convcl(e->e2));
		l = (Lambda*)e->xp;
		se = Znull();
		for(m = 0; m < l->ncap; m++)
			se = Zcons(doid(l->cap[m]->id), se);
		se = Zcons(e, Zreverse(se));
		return putsrc(Zapply(Ztid("%mkcl"), se), e->src);
	default:
		sete1(e, convcl(e->e1));
		sete2(e, convcl(e->e2));
		sete3(e, convcl(e->e3));
		sete4(e, convcl(e->e4));
		return e;
	}
}

int
issimple(Expr *e)
{
	return (e->kind == Eid
		|| e->kind == E_tid
		|| e->kind == Eboxref
		|| e->kind == Eval
		|| e->kind == Enil);
}

/* determine # of locals for lambda body E */
static unsigned
locpass(Expr *e)
{
	unsigned m;
	Expr *p;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Elambda:
		return 0;
	case Eblock:
		m = elistlen(e->e1);
		return m+locpass(e->e2);
	case Eelist:
		p = e;
		m = 0;
		while(p->kind == Eelist){
			m = max(m, locpass(p->e1));
			p = p->e2;
		}
		return m;
	default:
		m = locpass(e->e1);
		m = max(m, locpass(e->e2));
		m = max(m, locpass(e->e3));
		m = max(m, locpass(e->e4));
		return m;
	}
}

/* determine # of tmps for lambda body E */
static unsigned
tmppass(Expr *e)
{
	Expr *p;
	unsigned m;

	if(e == 0)
		return 0;

	if(isbinop(e->kind)){
		if(issimple(e->e1) && issimple(e->e2))
			return 0;
		else if(issimple(e->e1))
			return tmppass(e->e2);
		else if(issimple(e->e2))
			return tmppass(e->e1);
		else
			return 1+max(tmppass(e->e1),
				     tmppass(e->e2));
	}
	switch(e->kind){
	case Elambda:
		return 0;
	case Eblock:
		return tmppass(e->e2);
	case Eelist:
		p = e;
		m = 0;
		while(p->kind == Eelist){
			m = max(m, tmppass(p->e1));
			p = p->e2;
		}
		return m;
	default:
		m = tmppass(e->e1);
		m = max(m, tmppass(e->e2));
		m = max(m, tmppass(e->e3));
		m = max(m, tmppass(e->e4));
		return m;
	}
}

/* allocfr: allocate frame locations for locals */
static void
allocfr(Expr *e, unsigned ln)
{
	Lambda *l;
	Block *b;
	Expr *p;
	unsigned n;
	Var *v;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		l = (Lambda*)e->xp;
		l->ntmp = tmppass(e->e2);
		l->nloc = l->isvarg+locpass(e->e2);
		allocfr(e->e2, l->isvarg);
		break;
	case Eblock:
		b = (Block*)e->xp;
		for(n = 0, v = b->loc; n < b->nloc; n++, v++)
			v->idx = ln++;
		allocfr(e->e2, ln);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			allocfr(p->e1, ln);
			p = p->e2;
		}
		break;
	default:
		allocfr(e->e1, ln);
		allocfr(e->e2, ln);
		allocfr(e->e3, ln);
		allocfr(e->e4, ln);
		break;
	}
}

static Var*
topvar(Env *top, Cid *id)
{
	Var *v;
	v = emalloc(sizeof(Var));
	if(envbinds(top, id)){
		v->where = Vtopr;
		v->kv = envgetkv(top, id);
	}else{
		v->where = Vtopl;
		v->sym = id;
	}
	return v;
}

static void
collect(Expr *e, Xenv *lex)
{
	Expr *p;
	Block *b;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		break;
	case Eblock:
		b = (Block*)e->xp;
		bindvars(lex, b->loc, b->nloc);
		collect(e->e2, lex);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			collect(p->e1, lex);
			p = p->e2;
		}
		break;
	default:
		collect(e->e1, lex);
		collect(e->e2, lex);
		collect(e->e3, lex);
		collect(e->e4, lex);
		break;
	}
}

/* exloc: expose locations of variable reference */
static void
exloc(Expr *e, Xenv *lex, Env *top)
{
	Lambda *l;
	Expr *p;
	Var *v;
	char *id;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		l = (Lambda*)e->xp;
		lex = mkxenv(0);
		bindvars(lex, l->disp, l->ndisp);
		bindvars(lex, l->param, l->nparam);
		collect(e->e2, lex);
		exloc(e->e2, lex, top);
		freexenv(lex);
		break;
	case Eblock:
		exloc(e->e2, lex, top);
		break;
	case Elabel:
	case Egoto:
		break;
	case E_tid:
		id = idsym(e->e1);
		if(xenvlook(lex, id))
			fatal("bug");
		v = topvar(top, idcid(e->e1));
		e->xp = v;
		break;
	case Eid:
		id = idsym(e);
		v = xenvlook(lex, id);
		if(!v)
			fatal("bug (no id %s)", id);
		e->xp = v;
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			exloc(p->e1, lex, top);
			p = p->e2;
		}
		break;
	default:
		exloc(e->e1, lex, top);
		exloc(e->e2, lex, top);
		exloc(e->e3, lex, top);
		exloc(e->e4, lex, top);
		break;
	}
}

Expr*
docompilev(U *ctx, Expr *e)
{
	if(cqctflags['q']){
		xprintf("*** enter cpv ***\n");
		printexpr(e);
		xprintf("\n");
	}
	e = alpha(e, 0);
	if(cqctflags['q']){
		xprintf("*** after alpha ***\n");
		printexpr(e);
		xprintf("\n");
	}
	mkvars(e);
	idboxes(e, 0);
	e = rmscope(ctx, e);
	boxgoto(e, 0, 0);
	e = exboxes(e, 0);
	if(cqctflags['q']){
		xprintf("*** after exboxes ***\n");
		printexpr(e);
		xprintf("\n");
	}
	idfree(e, 0);
	sete2(e, convcl(e->e2)); /* skip top-level lambda introduced by cpb */
	allocfr(e, 0);
	if(cqctflags['q']){
		xprintf("*** after allocfr ***\n");
		printexpr(e);
		xprintf("\n");
	}
	exloc(e, 0, ctx->top->env);
	return e;
}
