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
		|| e->kind == Econsts
		|| e->kind == Ekon
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
		pass0(e->e4);
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
		pass0_5(e->e4, rib);
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
		fv(e->e4, lex, rib, free);
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
		fv(e->e4, lex, loc, free);
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
		pass1(e->e4, loc);
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
		pass2(e->e4, rib, top);
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
		l->ntmp = max(tmppass(e->e2), tmppass(e->e4));
		l->nloc = l->isvarg+max(locpass(e->e2), locpass(e->e4));
		pass3(e->e2, l->isvarg);
		pass3(e->e4, l->isvarg);
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

Expr*
docompilev(Expr *e, Toplevel *top)
{
	pass0(e);
	pass0_5(e, 0);
	pass1(e, 0);
	pass2(e, 0, top->env);
	pass3(e, 0);
	return e;
}
