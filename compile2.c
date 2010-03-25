#include "sys.h"
#include "util.h"
#include "syscqct.h"

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
				 copyexpr(e->e1),
				 e->e4));
		e->e1 = 0;
		e->e2 = 0;
		e->e3 = 0;
		e->e4 = 0;
		putsrc(p, &e->src);
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
				Zcall(G("mkrd"), 2,
				      Zconsts(id),
				      Zids2strs(e->e2))),
			   Zset(doid(id), Zcall(G("rdmk"),
						1, doid("$rd"))),
			   Zset(doid(is), Zcall(G("rdis"),
						1, doid("$rd"))),
			   doid("$rd"),
			   NULL);
		efree(is);
		putsrc(p, &e->src);
		freeexpr(e);
		return p;
	case Eglobal:
		p = e->e1;
		while(p->kind == Eelist){
			envgetbind(env, p->e1->id);
			p = p->e2;
		}
		freeexpr(e);
		return newexpr(Enil, 0, 0, 0, 0);
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
	e->e1 = newexprsrc(&e->e1->src, Eelist, doid(id), e->e1, 0, 0);
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
		if(!xenvlook(lex, id) && !envbinds(top, id)){
			if(inner){
				/* create binding in inner-most lexical scope */
				if(cqctflags['w'])
					cwarnln(ctx,
						e, "assignment to unbound variable: %s",
						id);
				newlocal(inner, id);
				bindids(lex, e->e1, e);
			}else
				/* create top-level binding */
				envgetbind(top, id);
		}
		if(e->kind == Eg || e->kind == Egop)
			topresolve(ctx, e->e2, top, lex, inner);
		break;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		/* assume e->e2 is Eblock */
		topresolve(ctx, e->e2, top, rib, inner);
		topresolve(ctx, e->e4, top, rib, inner);    /* FIXME: okay? */
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
	USED(u);
	efree(id);
	freeexpr((Expr*)v);
}

Expr*
docompile2(U *ctx, Expr *el, Toplevel *top, char *argsid)
{
	Expr *te;
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
			     newexpr(Enil, 0, 0, 0, 0),
			     el, 0, 0);

	/* add @global and implicit @global bindings to env */
	el = globals(el, top->env);

	/* resolve top-level bindings */
	topresolve(ctx, el, top->env, 0, 0);

	/*
	 * convert expression into callable lambda.
	 * errors occurring in toplevel tail calls lack
	 * top-level source line information.  prevent
	 * tail optimization by wrapping the body in a
	 * continuation; as a bonus, we get a binding
	 * for the most recent toplevel evaluation.
	 */
	te = newexpr(Elambda, argsid ? doid(argsid) : nullelist(),
		     newexpr(Eret,
			     Zset(doid("$$"),
				  newexpr(Eblock, nullelist(), el, 0, 0)),
			     0, 0, 0),
		     0, 0);
	putsrc(te, &el->src);
	return te;
}
