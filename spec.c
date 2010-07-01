#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct Svar {
	char *id;
} Svar;

#define UNKNOWN ((void*)~0)
#define LIT     ((void*)~1)
#define NIL     ((void*)~2)
#define USE     ((void*)~3)

static char*
isdomof(Expr *e)
{
	if(e->kind != Ecall)
		return 0;
	if(e->e1->kind != Eid)
		return 0;
	if(strcmp(e->e1->id, "domof") && strcmp(e->e1->id, "%domof"))
		return 0;
	if(e->e2->kind != Eelist)
		return 0;
	e = e->e2->e1;
	if(e->kind != Eid)
		return 0;
	return e->id;
}

static void
bindids(Xenv *xe, Expr *e, void *v)
{
	Expr *p;
	switch(e->kind){
	case Eid:
		xenvbind(xe, e->id, v);
		break;
	case Eellipsis:
		break;
	case Enull:
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			if(p->e1->kind != Eellipsis)
				xenvbind(xe, p->e1->id, v);
			p = p->e2;
		}
		break;
	default:
		fatal("bug");
	}
}

static Val
meet(Val d1, Val d2)
{
	if(d1 == d2)
		return d1;
	if(d1 == LIT)
		return d2;
	if(d2 == LIT)
		return d1;
	return UNKNOWN;
}

static Expr*
specsubst(Expr *e, Xenv *xe, Val *rdom)
{
	char *id;
	Expr *p;
	Xenv *rib, *xt, *xf;
	HT *dt, *df;
	Val v, rt, rf;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Ecall:
		e->e1 = specsubst(e->e1, xe, rdom);
		e->e2 = specsubst(e->e2, xe, rdom);
		id = isdomof(e);
		if(!id)
			return e;
		v = xenvlook(xe, id);
		if(v == UNKNOWN)
			return e;
		p = Zkon(v);
		putsrc(p, &e->src);
		freeexpr(e);
		*rdom = v;
		return p;
	case Eif:
		e->e1 = specsubst(e->e1, xe, rdom);
		xt = xenvcopy(xe);
		e->e2 = specsubst(e->e2, xt, &rt);
		dt = xenvdiff(xe, xt);
		freexenvall(xt);
		if(e->e3){
			xf = xenvcopy(xe);
			e->e3 = specsubst(e->e3, xf, &rf);
			df = xenvdiff(xe, xf);
			freexenvall(xf);
			if(!heqs(dt, df))
				fatal("ifelse merge failed");
			xenvapply(xe, dt);
			freeht(df);
			if(rt == rf)
				*rdom = rt;
			else
				*rdom = UNKNOWN;
		}else{
			if(hnent(dt) > 0)
				fatal("if merge failed");
			if(rt != *rdom)
				*rdom = UNKNOWN;
		}
		freeht(dt);
		break;
	case Econst:
	case Econsts:
		fatal("bug");
	case Ekon:
		v = (Val)e->xp;
		if(Vkind(v) == Qdom)
			*rdom = e->xp;
		else
			*rdom = UNKNOWN;
		break;
	case Eid:
		*rdom = xenvlook(xe, e->id);
		break;
	case Epreinc:
	case Epredec:
	case Epostinc:
	case Epostdec:
		e->e1 = specsubst(e->e1, xe, rdom);
		break;
	case Egop:
	case Ebinop:
		e->e1 = specsubst(e->e1, xe, &rt);
		e->e2 = specsubst(e->e2, xe, &rf);
		*rdom = meet(rt, rf);
		break;
	case Eg:
		e->e1 = specsubst(e->e1, xe, rdom);
		e->e2 = specsubst(e->e2, xe, rdom);
		if(e->e1->kind != Eid)
			fatal("bug");
		/* propagate domain */
		xenvupdate(xe, e->e1->id, *rdom);
		break;
	case Efor:
	case Ewhile:
	case Edo:
		fatal("no loops");
	case Elambda:
	case Eblock:
		rib = mkxenv(xe);
		bindids(rib, e->e1, UNKNOWN);
		e->e2 = specsubst(e->e2, rib, rdom);
		freexenv(rib);
		if(e->kind == Elambda)
			*rdom = NIL;
		break;
	case Excast:
		e->e1 = specsubst(e->e1, xe, &rt);
		e->e2 = specsubst(e->e2, xe, &rf);
		if(rt == NIL)
			*rdom = rf;
		else
			*rdom = rt;
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = specsubst(p->e1, xe, rdom);
			p = p->e2;
		}
		break;
	default:
		e->e1 = specsubst(e->e1, xe, rdom);
		e->e2 = specsubst(e->e2, xe, rdom);
		e->e3 = specsubst(e->e3, xe, rdom);
		e->e4 = specsubst(e->e4, xe, rdom);
		break;
	}
	return e;
}

static int
isconst(Expr *e)
{
	Expr *p;
	if(e == 0)
		return 1;
	switch(e->kind){
	case Ekon:
		return 1;
	case Enull:
		return 1;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			if(!isconst(p->e1))
				return 0;
			p = p->e2;
		}
		return 1;
	default:
		return 0;
	}
}

static int
isconstfn(char *id)
{
	static char *fns[] = {
		"%mkctype_void",
		"%mkctype_typedef",
		"%mkctype_ptr",
		"%looksym",
		"%looktype",
		"%symtype",
		"%subtype",
		"%nsptr",
		"%isnil",
		"$typeof",
		0,
	};
	char **p;
	p = fns;
	while(*p){
		if(!strcmp(id, *p))
			return 1;
		p++;
	}
	return 0;
}

static Expr*
cfeval(VM *vm, Expr *e)
{
	Closure *cl;
	Val rv;

	e = Zlambda(nullelist(), Zblock(nullelist(), e, NULL));
	e = docompilev(0, e, vm->top);
	if(cqctflags['q']){
		xprintf("evaluate constant expression: ");
		printcqct(e->e2->e2->e1);
		xprintf("\n");
	}
	cl = codegen(e);
	if(cl == 0)
		fatal("cannot compile constant expression");
	if(cqctcallfn(vm, mkvalcl(cl), 0, 0, &rv))
		fatal("cannot eval constant expression");
	return Zkon(rv);
}

/* constant fold */
static Expr*
cf(VM *vm, Expr *e, Xenv *xe)
{
	Expr *p;
	Xenv *rib;
	Val v;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eid:
		v = xenvlook(xe, e->id);
		if(v == 0){
			if(envbinds(vm->top->env, e->id) == 0)
				fatal("environment bug: %s", e->id);
			return e;
		}
		if(v == UNKNOWN)
			return e;
		p = Zkon(v);
		putsrc(p, &e->src);
		freeexpr(e);
		return p;
	case Eg:
		e->e2 = cf(vm, e->e2, xe);
		if(e->e2->kind == Ekon){
//			xprintf("updating %s to ", e->e1->id, e->e2->xp);
//			printkon(e->e2->xp);
//			xprintf("\n");
			xenvupdate(xe, e->e1->id, e->e2->xp);
		}
		return e;
	case Ecall:
		e->e1 = cf(vm, e->e1, xe);
		e->e2 = cf(vm, e->e2, xe);
		if(e->e1->kind == Eid
		   && isconstfn(e->e1->id)
		   && isconst(e->e2))
			return cfeval(vm, e);
		return e;
	case E_sizeof:
		e->e1 = cf(vm, e->e1, xe);
		if(isconst(e->e1))
			return cfeval(vm, e);
		return e;
	case E_cval:
		e->e1 = cf(vm, e->e1, xe);
		e->e2 = cf(vm, e->e2, xe);
		e->e3 = cf(vm, e->e3, xe);
		if(isconst(e->e1) && isconst(e->e2) && isconst(e->e3))
			return cfeval(vm, e);
		return e;
	case Eif:
		e->e1 = cf(vm, e->e1, xe);
		if(!isconst(e->e1))
			return e;
		v = e->e1->xp;
		if(Vkind(v) == Qcval && iszerocval(valcval(v))){
			p = e->e3;
			e->e3 = 0;
			freeexpr(e);
			if(p){
				p = cf(vm, p, xe);
				return p;
			}else
				return Znil();
		}else{
			p = e->e2;
			e->e2 = 0;
			freeexpr(e);
			p = cf(vm, p, xe);
			return p;
		}
//	case Epreinc:
//	case Epredec:
//	case Epostinc:
//	case Epostdec:
//		return e;
//	case Egop:
//		return e;
	case Ebinop:
		e->e1 = cf(vm, e->e1, xe);
		e->e2 = cf(vm, e->e2, xe);
		if(isconst(e->e1) && isconst(e->e2))
			return cfeval(vm, e);
		return e;
	case Efor:
	case Ewhile:
	case Edo:
		fatal("no loops");
	case Elambda:
	case Eblock:
		rib = mkxenv(xe);
		bindids(rib, e->e1, UNKNOWN);
		e->e2 = cf(vm, e->e2, rib);
		freexenv(rib);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = cf(vm, p->e1, xe);
			p = p->e2;
		}
		break;
	default:
		e->e1 = cf(vm, e->e1, xe);
		e->e2 = cf(vm, e->e2, xe);
		e->e3 = cf(vm, e->e3, xe);
		e->e4 = cf(vm, e->e4, xe);
		break;
	}
	return e;
}

/* mark used variables */
static Expr*
uv0(Expr *e, Xenv *xe)
{
	Expr *p;
	void *v;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Eid:
		/* record use */
		v = xenvlook(xe, e->id);
		if(v == 0)
			/* it must be toplevel */
			return e;
		xenvupdate(xe, e->id, USE);
		return e;
	case Eg:
		e->e2 = uv0(e->e2, xe);
		return e;
	case Egop:
		if(e->e1->kind != Eid)
			fatal("bug");
		/* technically a use; but let's not record it */
		e->e2 = uv0(e->e2, xe);
		return e;
	case Epreinc:
	case Epredec:
	case Epostinc:
	case Epostdec:
		if(e->e1->kind != Eid)
			fatal("bug");
		/* technically a use; but let's not record it */
		return e;
	case Elambda:
	case Eblock:
		xe = mkxenv(xe);
		e->xp = xe;	/* freed in uv1 */
		bindids(xe, e->e1, NIL);
		e->e2 = uv0(e->e2, xe);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = uv0(p->e1, xe);
			p = p->e2;
		}
		break;
	default:
		e->e1 = uv0(e->e1, xe);
		e->e2 = uv0(e->e2, xe);
		e->e3 = uv0(e->e3, xe);
		e->e4 = uv0(e->e4, xe);
		break;
	}
	return e;
}

static Expr*
uvfilter(Expr *l, Xenv *xe)
{
	void *v;
	Expr *n;
	if(l == 0)
		fatal("bug");
	if(l->kind == Enull)
		return l;
	n = uvfilter(l->e2, xe);
	l->e2 = 0;
	if(l->e1->kind != Eid)
		fatal("bug");
	v = xenvlook(xe, l->e1->id);
	if(v == USE){
		l->e2 = n;
		return l;
	}else{
		l->kind = Epreinc; /* fake freeexpr Eelist loop */
		freeexpr(l);
		return n;
	}
}

/* nuke unused variables and assignments to such */
static Expr*
uv1(Expr *e, Xenv *xe)
{
	Expr *p;
	void *v;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Eg:
	case Egop:
	case Epreinc:
	case Epredec:
	case Epostinc:
	case Epostdec:
		v = xenvlook(xe, e->e1->id);
		if(v == 0){
			/* must be toplevel */
			e->e2 = uv1(e->e2, xe);
			return e;
		}
		if(v == USE){
			e->e2 = uv1(e->e2, xe);
			return e;
		}
		if((e->kind == Eg || e->kind == Egop) && !isconst(e->e2)){
			/* FIXME: look harder for side effects */
			e->e2 = uv1(e->e2, xe);
			return e;
		}
		freeexpr(e);
		return Znil();
	case Elambda:
		xe = e->xp;
		e->e2 = uv1(e->e2, xe);
		e->xp = 0;
		freexenv(xe);
		break;
	case Eblock:
		xe = e->xp;
		e->e2 = uv1(e->e2, xe);
		e->e1 = uvfilter(e->e1, xe);
		e->xp = 0;
		freexenv(xe);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = uv1(p->e1, xe);
			p = p->e2;
		}
		break;
	default:
		e->e1 = uv1(e->e1, xe);
		e->e2 = uv1(e->e2, xe);
		e->e3 = uv1(e->e3, xe);
		e->e4 = uv1(e->e4, xe);
		break;
	}
	return e;
}


/* eliminate unused variables */
static Expr*
uv(Expr *e, Xenv *xe)
{
	e = uv0(e, xe);
	e = uv1(e, xe);
	return e;
}

Expr*
residue(VM *vm, Expr *e, Expr *pat, Val v)
{
	Xenv *xe;
	char *id;
	Val rdom;
	unsigned i;

	id = isdomof(pat);
	if(!id)
		return e;
//	printf("residue for domof(%s)\n", id);

	for(i = 0; i < 1; i++){
		xe = mkxenv(0);
		bindids(xe, e->e1, UNKNOWN);
		xenvupdate(xe, id, v);
		e->e2 = specsubst(e->e2, xe, &rdom);
		freexenv(xe);

		xe = mkxenv(0);
		bindids(xe, e->e1, UNKNOWN);
		e->e2 = cf(vm, e->e2, xe);
		freexenv(xe);

		xe = mkxenv(0);
		bindids(xe, e->e1, NIL);
		e->e2 = uv(e->e2, xe);
		freexenv(xe);
	}

if(0){
	xe = mkxenv(0);
	bindids(xe, e->e1, UNKNOWN);
	xenvupdate(xe, id, v);
	e->e2 = specsubst(e->e2, xe, &rdom);
	freexenv(xe);
}

	return e;
}
