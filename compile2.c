#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void freedecl(Decl *d);
static void freelambda(Lambda *l);
static void freeblock(Block *b);
static u64 szblock(Block *b);
static u64 szlambda(Lambda *l);
static u64 szdecl(Decl *d);

int
issimple(Expr *e)
{
	return (e->kind == Eid
		|| e->kind == Econst
		|| e->kind == Enil);
}

static unsigned
elistlen(Expr *l)
{
	unsigned n;
	n = 0;
	while(l->kind != Enull){
		l = l->e2;
		n++;
	}
	return n;
}

/* determine # of locals for lambda body E */
static unsigned
locpass(Expr *e)
{
	unsigned m;
	Expr *p;

	if(e == NULL)
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

	if(e == NULL)
		return 0;

	switch(e->kind){
	case Elambda:
		return 0;
	case Eblock:
		return tmppass(e->e2);
	case Ebinop:
		if(issimple(e->e1) && issimple(e->e2))
			return 0;
		else if(issimple(e->e1))
			return tmppass(e->e2);
		else if(issimple(e->e2))
			return tmppass(e->e1);
		else
			return 1+max(tmppass(e->e1),
				     tmppass(e->e2));
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

static void
freetype(Type *t)
{
	if(t == 0)
		return;

	freetype(t->link);
	efree(t->dom);
	efree(t->tid);
	efree(t->tag);
	freedecl(t->field);
	freeenum(t->en);
	freedecl(t->param);
	freeexpr(t->bitw);
	freeexpr(t->sz);
	freeexpr(t->cnt);
	efree(t);
}

static u64
sztype(Type *t)
{
	u64 m;

	if(t == 0)
		return 0;
	m = 0;
	m += sztype(t->link);
	m += esize(t->dom);
	m += esize(t->tid);
	m += esize(t->tag);
	m += szdecl(t->field);
	m += szenum(t->en);
	m += szdecl(t->param);
	m += szexpr(t->bitw);
	m += szexpr(t->sz);
	m += szexpr(t->cnt);
	m += esize(t);

	return m;
}

static void
freedecl(Decl *d)
{
	Decl *nxt;

	nxt = d;
	while(nxt){
		d = nxt;
		nxt = d->link;
		freetype(d->type);
		freeexpr(d->attr);
		efree(d->id);
		efree(d);
	}
}

static u64
szdecl(Decl *d)
{
	u64 m;
	m = 0;
	while(d){
		m += sztype(d->type);
		m += szexpr(d->attr);
		m += esize(d->id);
		m += esize(d);
		d = d->link;
	}
	return m;
}

void
freeexprx(Expr *e)
{
	Var *v;

	switch(e->kind){
	case Eid:
		v = e->xp;
		if(v->where == Vtop)
			/* others types point to lambda/block vars */
			efree(v);
		break;
	case Elambda:
		freelambda(e->xp);
		break;
	case Eblock:
		freeblock(e->xp);
		break;
	case Etypedef:
	case Edecl:
	case Edecls:
		freedecl((Decl*)e->xp);
		break;
	default:
		break;
	}
}

u64
szexprx(Expr *e)
{
	Var *v;
	u64 m;

	m = 0;
	switch(e->kind){
	case Eid:
		v = e->xp;
		if(v->where == Vtop)
			/* others types point to lambda/block vars */
			m += esize(v);
		break;
	case Elambda:
		m += szlambda(e->xp);
		break;
	case Eblock:
		m += szblock(e->xp);
		break;
	case Etypedef:
	case Edecl:
	case Edecls:
		m += szdecl(e->xp);
		break;
	default:
		break;
	}
	return m;
}

static int
lexbinds(Expr *e, char *id)
{
	Expr *p;

	if(e == 0)
		return 0;
	
	/* special case: vararg Lambda */
	if(e->kind == Elambda && e->e1->kind == Eid){
		if(!strcmp(e->e1->id, id))
			return 1;
		return lexbinds((Expr*)e->xp, id);
	}

	/* Eblock or Elambda with variable list */
	p = e->e1;
	while(p->kind != Enull){
		if(!strcmp(p->e1->id, id))
			return 1;
		p = p->e2;
	}
	return lexbinds((Expr*)e->xp, id);
}

static Expr*
globals(Expr *e, Env *env)
{
	Expr *p;
	char *id, *is;
	unsigned len;

	if(e == 0)
		return e;

	switch(e->kind){
	case Edefine:
		p = Zset(e->e1,
			 Zlambdn(e->e2,
				 globals(e->e3, env),
				 copyexpr(e->e1)));
		if(e->kind == Edefine)
			envgetbind(env, e->e1->id);
		e->e1 = 0;
		e->e2 = 0;
		e->e3 = 0;
		p->src = e->src;
		freeexpr(e);
		return p;
	case Edefrec:
		id = e->e1->id;
		len = 2+strlen(id)+1;
		is = emalloc(len);
		snprint(is, len, "is%s", id);
		envgetbind(env, id);
		envgetbind(env, is);
		p = Zblock(Zlocals(1, "$rd"),
			   Zset(doid("$rd"),
				Zcall(doid("%mkrd"), 2,
				      Zconsts(id),
				      Zids2strs(e->e2))),
			   Zset(doid(id), Zcall(doid("%rdmk"),
						1, doid("$rd"))),
			   Zset(doid(is), Zcall(doid("%rdis"),
						1, doid("$rd"))),
			   doid("$rd"),
			   NULL);
		efree(is);
		p->src = e->src;
		freeexpr(e);
		return p;
	case Eglobal:
		p = e->e1;
		while(p->kind == Eelist){
			envgetbind(env, p->e1->id);
			p = p->e2;
		}
		freeexpr(e);
		return newexpr(Enop, 0, 0, 0, 0);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = globals(p->e1, env);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = globals(e->e1, env);
		e->e2 = globals(e->e2, env);
		e->e3 = globals(e->e3, env);
		e->e4 = globals(e->e4, env);
		return e;
	}
}

static void
newlocal(Expr *e, char *id)
{
	if(e->kind != Eblock)
		fatal("bug");
	e->e1 = newexpr(Eelist, doid(id), e->e1, 0, 0);
}

static void
bindids(Xenv *xe, Expr *e, void *v)
{
	Expr *p;
	switch(e->kind){
	case Eid:
		xenvbind(xe, e->id, v);
		break;
	case Enull:
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			xenvbind(xe, p->e1->id, v);
			p = p->e2;
		}
		break;
	default:
		fatal("bug");
	}
}

static void
topresolve(U *ctx, Expr *e, Env *top, Xenv *lex, Expr *inner)
{
	Expr *p;
	char *id;
	Xenv *rib;

	if(e == 0)
		return;

	switch(e->kind){
	case Epreinc:
	case Epostinc:
	case Epredec:
	case Epostdec:
	case Eg:
	case Egop:
		if(e->e1->kind != Eid)
			fatal("bug");
		id = e->e1->id;
		if(xenvlook(lex, id) || envbinds(top, id))
			; /* no binding required */
		else if(inner){
			/* create binding in inner-most lexical scope */
			if(cqctflags['w'])
				cwarn(ctx,
				      e, "assignment to unbound variable: %s",
				      id);
			newlocal(inner, id);
			bindids(lex, e->e1, e);
		}else
			/* create top-level binding */
			envgetbind(top, id);
		if(e->kind == Eg || e->kind == Egop)
			topresolve(ctx, e->e2, top, lex, inner);
		break;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		/* assume e->e2 is Eblock */
		topresolve(ctx, e->e2, top, rib, inner);
		freexenv(rib);
		break;
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		topresolve(ctx, e->e2, top, rib, e);
		freexenv(rib);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			topresolve(ctx, p->e1, top, lex, inner);
			p = p->e2;
		}
		break;
	default:
		topresolve(ctx, e->e1, top, lex, inner);
		topresolve(ctx, e->e2, top, lex, inner);
		topresolve(ctx, e->e3, top, lex, inner);
		topresolve(ctx, e->e4, top, lex, inner);
		break;
	}
}

void
freeconst(void *u, char *id, void *v)
{
	efree(id);
	freeexpr((Expr*)v);
}

static Expr*
expandconst(Expr *e, Env *top, Xenv *lex, Xenv *con)
{
	Expr *p;
	Xenv *lexrib, *conrib;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Edefconst:
		e->e2 = expandconst(e->e2, top, lex, con);
		xenvbind(con, xstrdup(e->e1->id), e->e2);
		e->e2 = 0;
		freeexpr(e);
		return newexpr(Enop, 0, 0, 0, 0);
	case Eid:
		if(xenvlook(lex, e->id) || envbinds(top, e->id))
			return e;
		p = xenvlook(con, e->id);
		if(p){
			freeexpr(e);
			return copyexpr(p);
		}
		return e;
	case Elambda:
		lexrib = mkxenv(lex);
		bindids(lexrib, e->e1, e);
		e->e2 = expandconst(e->e2, top, lexrib, con);
		freexenv(lexrib);
		return e;
	case Eblock:
		conrib = mkxenv(con);
		lexrib = mkxenv(lex);
		bindids(lexrib, e->e1, e);
		e->e2 = expandconst(e->e2, top, lexrib, conrib);
		freexenv(lexrib);
		xenvforeach(conrib, freeconst, 0);
		freexenv(conrib);
		return e;
	case Epreinc:
	case Epostinc:
	case Epredec:
	case Epostdec:
		/* don't expand assigned identifiers */
		return e;
	case Eg:
	case Egop:
		/* don't expand assigned identifiers */
		e->e2 = expandconst(e->e2, top, lex, con);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = expandconst(p->e1, top, lex, con);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = expandconst(e->e1, top, lex, con);
		e->e2 = expandconst(e->e2, top, lex, con);
		e->e3 = expandconst(e->e3, top, lex, con);
		e->e4 = expandconst(e->e4, top, lex, con);
		return e;
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

static u64
szlambda(Lambda *l)
{
	u64 m;
	m = 0;
	m += esize(l->cap);
	m += esize(l->disp);
	m += esize(l->param);
	m += esize(l);
	return m;
}

static void
freeblock(Block *b)
{
	efree(b->loc);
	efree(b);
}

static u64
szblock(Block *b)
{
	u64 m;
	m = 0;
	m += esize(b->loc);
	m += esize(b);
	return m;
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
			l->id = e->e3->id;
		p = e->e1;
		if(p == 0)
			fatal("bug");
		if(p->kind == Eid){
			l->isvarg = 1;
			l->nparam = 1;
			v = l->param = emalloc(sizeof(Var));
			v->id = p->id;
			v->where = Vlocal;
			v->idx = 0;
		}else{
			l->nparam = elistlen(p);
			v = l->param = emalloc(l->nparam*sizeof(Var));
			p = e->e1;
			m = 0;
			while(p->kind == Eelist){
				v->id = p->e1->id;
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
		m = 0;
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
			v->id = p->e1->id;
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

/* pass 1.5: determine variables that must be boxed;
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
	case Egop:
	case Epreinc:
	case Epredec:
	case Epostinc:
	case Epostdec:
		/* Box all variables that are assigned */
		v = xenvlook(lex, e->e1->id);
		if(v)
			v->box = 1;
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
	case Eid:
		v = xenvlook(lex, e->id);
		if(v && !xenvlook(loc, e->id))
			xenvbind(free, e->id, v);
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
			w = l->disp = emalloc(l->ncap*sizeof(Var));
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
topvar(Env *top, char *id)
{
	Var *v;
	v = emalloc(sizeof(Var));
	v->id = id;
	v->where = Vtop;
	v->val = envgetbind(top, id);
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
	case Eid:
		id = e->id;
		v = xenvlook(lex, id);
		if(!v)
			v = topvar(top, id);
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
		l->nloc = locpass(e->e2)+l->isvarg;
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
newpasses(Expr *e, Env *top)
{
	pass0(e);
	pass0_5(e, 0);
	pass1(e, 0);
	pass2(e, 0, top);
	pass3(e, 0);
}

Expr*
docompile2(U *ctx, Expr *el, Toplevel *top, char *argsid)
{
	/* 
	 * enclose expression in block to reduce
	 * top-level pollution.
	 * disabled: breaks too many existing programs
	 */
	// el = Zblock(nullelist(), el, 0);

	/* FIXME: test/null1.l1 fails on multiple labels;
	   this is a workaround. */
	if(el->kind == Enull)
		el = newexpr(Eelist,
			     newexpr(Enop, 0, 0, 0, 0),
			     el, 0, 0);

	/* add @global and implicit @global bindings to env */
	el = globals(el, top->env);

	/* resolve top-level bindings */
	topresolve(ctx, el, top->env, 0, 0);

	/* expand @const references */
	el = expandconst(el, top->env, 0, top->env->con);

	/* 
	 * convert expression into callable lambda.
	 * errors occurring in toplevel tail calls lack
	 * top-level source line information.  prevent
	 * tail optimization by wrapping the body in a
	 * continuation; as a bonus, we get a binding
	 * for the most recent toplevel evaluation.
	 */
	el = newexpr(Elambda, argsid ? doid(argsid) : nullelist(),
		     newexpr(Eret,
			     Zset(doid("$$"),
				  newexpr(Eblock, nullelist(), el, 0, 0)),
			     0, 0, 0),
		     0, 0);

	newpasses(el, top->env);

	return el;
}
