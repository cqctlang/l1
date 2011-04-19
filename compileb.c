#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
newlocal(Expr *e, char *id)
{
	if(e->kind != Eblock)
		fatal("bug");
	e->e1 = newexprsrc(&e->e1->src, Eelist, doid(id), e->e1, 0, 0);
}

static Expr*
defloc(U *ctx, Expr *e, Expr *scope)
{
	Expr *p;
	if(e == 0)
		return e;
	switch(e->kind){
	case Edefloc:
		if(scope == 0)
			cerror(ctx, e, "@defloc without scope");
		p = Zset(e->e1,
			 Zlambdn(e->e2,
				 defloc(ctx, e->e3, scope),
				 copyexpr(e->e1)));
		newlocal(scope->e1, e->e1->id);
		e->e1 = 0;
		e->e2 = 0;
		e->e3 = 0;
		e->e4 = 0;
		putsrc(p, &e->src);
		freeexpr(e);
		return p;
	case Escope:
		e->e1 = defloc(ctx, e->e1, e);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = defloc(ctx, p->e1, scope);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = defloc(ctx, e->e1, scope);
		e->e2 = defloc(ctx, e->e2, scope);
		e->e3 = defloc(ctx, e->e3, scope);
		e->e4 = defloc(ctx, e->e4, scope);
		return e;
	}
}

static Expr*
globals(U *ctx, Expr *e, Env *env)
{
	Expr *p, *se;

	if(e == 0)
		return e;

	switch(e->kind){
	case Eglobal:
		p = e->e1;
		se = nullelist();
		while(p->kind == Eelist){
			envgetbind(env, p->e1->id);
			se = Zcons(Zset(p->e1, Znil()), se);
			p->e1 = 0; /* steal */
			p = p->e2;
		}
		freeexpr(e);
		return invert(se);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = globals(ctx, p->e1, env);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = globals(ctx, e->e1, env);
		e->e2 = globals(ctx, e->e2, env);
		e->e3 = globals(ctx, e->e3, env);
		e->e4 = globals(ctx, e->e4, env);
		return e;
	}
}

static void
bindids(Xenv *xe, Expr *e, void *v)
{
	Expr *p;
	p = e;
	while(p->kind == Eelist){
		if(p->e1->kind != Eellipsis)
			xenvbind(xe, p->e1->id, v);
		p = p->e2;
	}
}

/* FIXME: the side-effect of binding ids into toplevel may be
   undesirable when they occur erroneously */
static Expr*
toplevel(U *ctx, Expr *e, Env *env, Xenv *lex)
{
	Expr *p, *se;
	Xenv *rib;
	char *id;

	if(e == 0)
		return e;

	switch(e->kind){
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		e->e2 = toplevel(ctx, e->e2, env, rib);
		freexenv(rib);
		return e;
	case Escope:
	case Elambda:
		return e;
	case Eid:
		id = e->id;
		if(xenvlook(lex, id))
			return e;
		if(!envbinds(env, id))
			envgetbind(env, id);
		se = Ztid(id);
		putsrc(se, &e->src);
		freeexpr(e);
		return se;
	case Eg:
		id = e->e1->id;
		e->e2 = toplevel(ctx, e->e2, env, lex);
		if(xenvlook(lex, id))
			return e;
		if(!envbinds(env, id))
			envgetbind(env, id);
		se = Ztg(id, e->e2);
		putsrc(se, &e->src);
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = toplevel(ctx, p->e1, env, lex);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = toplevel(ctx, e->e1, env, lex);
		e->e2 = toplevel(ctx, e->e2, env, lex);
		e->e3 = toplevel(ctx, e->e3, env, lex);
		e->e4 = toplevel(ctx, e->e4, env, lex);
		return e;
	}
}

static Expr*
resolve(U *ctx, Expr *e, Env *top, Xenv *lex, Expr *scope, Xenv *slex)
{
	Expr *se, *p;
	char *id;
	Xenv *rib;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eid:
		id = e->id;
		if(xenvlook(lex, id))
			return e;
		else if(envbinds(top, id)){
			se = Ztid(id);
			putsrc(se, &e->src);
			freeexpr(e);
			return se;
		}else if(scope){
			/* bind to innermost lexical scope */
			newlocal(scope->e1, id);
			xenvbind(slex, id, e);
			e->e2 = resolve(ctx, e->e2, top, lex, scope, slex);
			return e;
		}else
			fatal("bug");
		return e;
	case Eg:
		id = e->e1->id;
		if(xenvlook(lex, id)){
			e->e2 = resolve(ctx, e->e2, top, lex, scope, slex);
			return e;
		}else if(envbinds(top, id)){
			e->e2 = resolve(ctx, e->e2, top, lex, scope, slex);
			se = Ztg(id, e->e2);
			e->e2 = 0;
			putsrc(se, &e->src);
			freeexpr(e);
			return se;
		}else if(scope){
			/* bind to innermost lexical scope */
			if(cqctflags['w'])
				cwarnln(ctx,
					e, "assignment to unbound variable: %s",
					id);
			newlocal(scope->e1, id);
			xenvbind(slex, id, e);
			e->e2 = resolve(ctx, e->e2, top, lex, scope, slex);
			return e;
		}else
			fatal("bug");
		break;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		e->e2 = resolve(ctx, e->e2, top, rib, scope, slex);
		freexenv(rib);
		return e;
	case Escope:
		e->e1 = resolve(ctx, e->e1, top, lex, e, slex);
		return e;
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		// slex always points to rib of innermost Escope's block
		// scope is 0 where we've introduced blocks at toplevel
		if(scope && scope->e1 == e)
			slex = rib;
		e->e2 = resolve(ctx, e->e2, top, rib, scope, slex);
		freexenv(rib);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = resolve(ctx, p->e1, top, lex, scope, slex);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = resolve(ctx, e->e1, top, lex, scope, slex);
		e->e2 = resolve(ctx, e->e2, top, lex, scope, slex);
		e->e3 = resolve(ctx, e->e3, top, lex, scope, slex);
		e->e4 = resolve(ctx, e->e4, top, lex, scope, slex);
		return e;
	}
}

static Expr*
rmscope(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Escope:
		p = rmscope(ctx, e->e1);
		e->e1 = 0;
		freeexpr(e);
		return p;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = rmscope(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = rmscope(ctx, e->e1);
		e->e2 = rmscope(ctx, e->e2);
		e->e3 = rmscope(ctx, e->e3);
		e->e4 = rmscope(ctx, e->e4);
		return e;
	}
}

Expr*
docompileb(U *ctx, Expr *e, Toplevel *top, char *argsid)
{
	Src s;
	Xenv *lex;

	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */

	e = defloc(ctx, e, 0);
	e = globals(ctx, e, top->env);
	if(argsid){
		lex = mkxenv(0);
		xenvbind(lex, argsid, argsid);
	}else
		lex = 0;
	e = toplevel(ctx, e, top->env, lex);
	e = resolve(ctx, e, top->env, lex, 0, 0);
	if(lex)
		freexenv(lex);
	e = rmscope(ctx, e);

	/*
	 * convert expression to function.  wrap the
	 * body in an assignment to avoid tail call
	 * optimization, to preserve presentation of
	 * top-level source line info in errors.
	 */
	s = e->src;
	envgetbind(top->env, "$$");
	e = Zlambda(argsid ? Zvararg(doid(argsid)) : nullelist(),
		     Zret(Ztg("$$", Zblock(nullelist(), e, NULL))));
	putsrc(e, &s);
	return e;
}
