#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void freelambda(Lambda *l);
static void freeblock(Block *b);
static void freeboxset(Boxset *bxst);

typedef struct Exprs Exprs;
struct Exprs
{
	Expr *e;
	Exprs *link;
};

enum
{
	/* pass4 states: must be non-zero, non-pointer values */
	Vref = 1,
};

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

static void
free1exprs(Exprs *es)
{
	efree(es);
}

static Exprs*
copyexprs(Exprs *es)
{
	if(es == 0)
		return 0;
	return newexprs(es->e, copyexprs(es->link));
}

int
issimple(Expr *e)
{
	return (e->kind == Eid
		|| e->kind == E_tid
		|| e->kind == Ekon
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

/* clean up xp data.  expr nodes may be shared, so
   clear xp on the way out */
static void
freeexprx(Expr *e)
{
	Var *v;

	if(e->xp == 0)
		return;
	switch(e->kind){
	case E_tid:
		v = e->xp;
		if(v->where != Vtopl && v->where != Vtopr)
			fatal("bug");
		efree(v);
		e->xp = 0;
		break;
	case Eid:
		/* e->xp, if set, points to lambda/block vars freed elsewhere */
		e->xp = 0;
		break;
	case Elambda:
		freelambda(e->xp);
		e->xp = 0;
		break;
	case Eblock:
		freeblock(e->xp);
		e->xp = 0;
		break;
	case Egoto:
		freeboxset(e->xp);
		e->xp = 0;
		break;
	default:
		break;
	}
}

void
freexp(Expr *e)
{
	Expr *p;
	if(e == 0)
		return;
	freeexprx(e);
	switch(e->kind){
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			freexp(p->e1);
			p = p->e2;
		}
		break;
	default:
		freexp(e->e1);
		freexp(e->e2);
		freexp(e->e3);
		freexp(e->e4);
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

static void
freelambda(Lambda *l)
{
	efree(l->cap);
	efree(l->disp);
	efree(l->param);
	efree(l);
}

static void
freeblock(Block *b)
{
	efree(b->loc);
	efree(b);
}

/* pass0: allocate lambda and block variable descriptors */
static void
pass0(Expr *e)
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
		pass0(e->e2);
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
		pass0(e->e2);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			pass0(p->e1);
			p = p->e2;
		}
		break;
	default:
		pass0(e->e1);
		pass0(e->e2);
		pass0(e->e3);
		pass0(e->e4);
		break;
	}
}

/* pass 0.5: determine variables that must be boxed;
   FIXME: this is very conservative, mainly because of continuations.
   it may make more sense to do free variable analysis
   first; if so, the display allocation in free var pass should
   move to a later pass.
 */
static void
pass0_5(Expr *e, Xenv *lex)
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
		pass0_5(e->e2, rib);
		freexenv(rib);
		break;
	case Eblock:
		b = (Block*)e->xp;
		rib = mkxenv(lex);
		/* Box all locals because they might
		   be captured in a continuation. */
		for(m = 0, v = b->loc; m < b->nloc; m++, v++)
			v->box = 1;
		bindvars(rib, b->loc, b->nloc);
		pass0_5(e->e2, rib);
		freexenv(rib);
		break;
	case Eg:
		/* Box all variables that are assigned */
		v = xenvlook(lex, idsym(e->e1));
		if(v)
			v->box = 1;
		pass0_5(e->e2, lex);
		break;
	case E_tg:
		pass0_5(e->e2, lex);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			pass0_5(p->e1, lex);
			p = p->e2;
		}
		break;
	default:
		pass0_5(e->e1, lex);
		pass0_5(e->e2, lex);
		pass0_5(e->e3, lex);
		pass0_5(e->e4, lex);
		break;
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

/* pass1: compute free variables; prepare lambdas for their capture;
   FIXME: this pass is n^2 in nodes; it seems like there should
   be a linear way to do it.  one issue is getting the captures
   along a chain of lambdas separating a var binding from its ref.
*/
static void
pass1(Expr *e, Xenv *lex)
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
		pass1(e->e2, loc);
		freexenv(loc);
		freexenv(free);
		break;
	case Eblock:
		b = (Block*)e->xp;
		loc = mkxenv(lex);
		bindvars(loc, b->loc, b->nloc);
		pass1(e->e2, loc);
		freexenv(loc);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			pass1(p->e1, lex);
			p = p->e2;
		}
		break;
	default:
		pass1(e->e1, lex);
		pass1(e->e2, lex);
		pass1(e->e3, lex);
		pass1(e->e4, lex);
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

/* pass2: bind variable references */
static void
pass2(Expr *e, Xenv *lex, Env *top)
{
	Lambda *l;
	Block *b;
	Xenv *rib;
	Expr *p;
	Var *v;
	char *id;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		l = (Lambda*)e->xp;
		rib = mkxenv(lex);
		bindvars(rib, l->disp, l->ndisp);
		bindvars(rib, l->param, l->nparam);
		pass2(e->e2, rib, top);
		freexenv(rib);
		break;
	case Eblock:
		b = (Block*)e->xp;
		rib = mkxenv(lex);
		bindvars(rib, b->loc, b->nloc);
		pass2(e->e2, rib, top);
		freexenv(rib);
		break;
	case E_tid:
		id = idsym(e->e1);
		if(xenvlook(lex, id))
			fatal("bug");
		v = topvar(top, idcid(e->e1));
		e->xp = v;
		break;
	case Elabel:
	case Egoto:
		break;
	case Eid:
		id = idsym(e);
		v = xenvlook(lex, id);
		if(!v)
			fatal("bug");
		e->xp = v;
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			pass2(p->e1, lex, top);
			p = p->e2;
		}
		break;
	default:
		pass2(e->e1, lex, top);
		pass2(e->e2, lex, top);
		pass2(e->e3, lex, top);
		pass2(e->e4, lex, top);
		break;
	}
}


/* pass3: allocate stack storage.
   (the calls to tmppass and locpass do extra passes on current lambda)
*/
static void
pass3(Expr *e, unsigned ln)
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
		pass3(e->e2, l->isvarg);
		break;
	case Eblock:
		b = (Block*)e->xp;
		for(n = 0, v = b->loc; n < b->nloc; n++, v++)
			v->idx = ln++;
		pass3(e->e2, ln);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			pass3(p->e1, ln);
			p = p->e2;
		}
		break;
	default:
		pass3(e->e1, ln);
		pass3(e->e2, ln);
		pass3(e->e3, ln);
		pass3(e->e4, ln);
		break;
	}
}

static void
countunused(void *u, char *k, void *v)
{
	unsigned *x;
	Var *var;
	USED(k);
	x = u;
	if(v == (void*)Vref)
		return;
	var = v;
	if(var->id[0] == '$')	/* FIXME: system vars should not be unused! */
		return;
	(*x)++;
}

static void
warn1unused(void *u, char *k, void *v)
{
	U *ctx;
	Var *var;
	USED(k);
	ctx = u;
	if(v == (void*)Vref)
		return;
	var = v;
	cprintf(ctx->out, "%s ", var->id);
}

static void
warnunused(U *ctx, Expr *e, Xenv *rib)
{
	unsigned unused;
	unused = 0;
	xenvforeach(rib, countunused, &unused);
	if(unused == 0)
		return;
	if(unused == 1)
		cwarn(ctx, e, "unused variable: ");
	else
		cwarn(ctx, e, "unused variables: ");
	xenvforeach(rib, warn1unused, ctx);
	cprintf(ctx->out, "\n");
}

/* FIXME: rationalize with the uv pass in spec */
static void
pass4(U *ctx, Expr *e, Xenv *lex)
{
	Lambda *l;
	Block *b;
	Xenv *rib;
	char *id;
	Var *v;
	Expr *p;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		l = (Lambda*)e->xp;
		rib = mkxenv(lex);
		bindvars(rib, l->disp, l->ndisp);
		bindvars(rib, l->param, l->nparam);
		pass4(ctx, e->e2, rib);
//		warnunused(ctx, e->e1, rib);
		freexenv(rib);
		break;
	case Eblock:
		b = (Block*)e->xp;
		rib = mkxenv(lex);
		bindvars(rib, b->loc, b->nloc);
		pass4(ctx, e->e2, rib);
		warnunused(ctx, e->e1, rib);
		freexenv(rib);
		break;
	case E_tid:
		id = idsym(e->e1);
		v = xenvlook(lex, id);
		if(v)
			fatal("bug");
		break;
	case Elabel:
	case Egoto:
		break;
	case Eid:
		id = idsym(e);
		v = xenvlook(lex, id);
		if(!v)
			fatal("bug");
		xenvupdate(lex, id, (void*)Vref);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			pass4(ctx, p->e1, lex);
			p = p->e2;
		}
		break;
	default:
		pass4(ctx, e->e1, lex);
		pass4(ctx, e->e2, lex);
		pass4(ctx, e->e3, lex);
		pass4(ctx, e->e4, lex);
		break;
	}
}

static Boxset*
newboxset(void)
{
	Boxset *bxst;
	bxst = emalloc(sizeof(Boxset));
	bxst->max = 8;
	bxst->var = emalloc(bxst->max*sizeof(Var*));
	return bxst;
}

static void
freeboxset(Boxset *bxst)
{
	efree(bxst->var);
	efree(bxst);
}

static int
varequal(Var *a, Var *b)
{
	if(a->where != b->where)
		return 0;
	if(a->where == Vtopl){
		if(a->sym == b->sym)
			return 1;
		else
			return 0;
	}
	if(a->where == Vtopr){
		if(a->kv == b->kv)
			return 1;
		else
			return 0;
	}
	return (a->idx == b->idx && a->box == b->box);
}

static void
boxsetins(Boxset *bxst, Var *v)
{
	if(bxst->n >= bxst->max){
		bxst->var = erealloc(bxst->var, bxst->max*sizeof(Var*),
				     2*bxst->max*sizeof(Var*));
		bxst->max *= 2;
	}
	bxst->var[bxst->n++] = v;
}

static void
boxsetdel(Boxset *bxst, Var *v)
{
	unsigned i;
	for(i = 0; i < bxst->n; i++)
		if(varequal(bxst->var[i], v))
			break;
	if(i >= bxst->n)
		fatal("bug");
	bxst->n--;
	memmove(&bxst->var[i], &bxst->var[i+1], (bxst->n-i)*sizeof(Var*));
}

static int
boxsethas(Boxset *bxst, Var *v)
{
	unsigned i;
	for(i = 0; i < bxst->n; i++)
		if(varequal(bxst->var[i], v))
			return 1;
	return 0;
}

static Boxset*
mkboxset(Exprs *les, Exprs *lp)
{
	Boxset *bxst;
	Block *b;
	unsigned m;
	Var *v;

	if(les == lp)
		return newboxset();
	bxst = mkboxset(les->link, lp);
	if(les->e->kind != Eblock)
		fatal("bug");
	b = (Block*)les->e->xp;
	for(m = 0; m < b->nloc; m++){
		v = &b->loc[m];
		if(v->box && !boxsethas(bxst, v))
			boxsetins(bxst, v);
		else if(!v->box && boxsethas(bxst, v))
			boxsetdel(bxst, v);
	}
	return bxst;
}

static Boxset*
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

	return mkboxset(les, lp);
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

static void
free1ls(void *u, char *k, void *v)
{
	USED(k);
	USED(u);
	freeexprs((Exprs*)v);
}

// pass5: compute variable initialize plan for each goto
static void
pass5(Expr *e, HT *ls, Exprs *les)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;

	switch(e->kind){
	case Elambda:
		les = newexprs(e, 0);
		ls = mkhts();
		pass5(e->e2, ls, les);
		gotos(e->e2, ls, les);
		free1exprs(les);
		hforeach(ls, free1ls, 0);
		freeht(ls);
		break;
	case Eblock:
		les = newexprs(e, les);
		pass5(e->e2, ls, les);
		free1exprs(les);
		break;
	case Elabel:
		id = idsym(e->e1);
		hputs(ls, id, strlen(id), copyexprs(les));
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			pass5(p->e1, ls, les);
			p = p->e2;
		}
		break;
	default:
		pass5(e->e1, ls, les);
		pass5(e->e2, ls, les);
		pass5(e->e3, ls, les);
		pass5(e->e4, ls, les);
		break;
	}
}

Expr*
docompilev(U *ctx, Expr *e)
{
	pass0(e);
	pass0_5(e, 0);
	pass1(e, 0);
	pass2(e, 0, ctx->top->env);
	pass3(e, 0);
	if(0 && ctx)
		pass4(ctx, e, 0);
	pass5(e, 0, 0);
	return e;
}
