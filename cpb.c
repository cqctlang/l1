#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
newlocal(Expr *e, Expr *id)
{
	if(e->kind != Eblock)
		fatal("bug");
	e->e1 = newexprsrc(&e->e1->src, Eelist, id, e->e1, 0, 0);
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
		newlocal(scope->e1, e->e1);
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
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Eglobal:
		p = e->e1;
		while(p->kind == Eelist){
			if(!envbinds(env, idcid(p->e1)))
				envdefine(env, idcid(p->e1), Xnil);
			p = p->e2;
		}
		return putsrc(Znop(), &e->src);
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
			xenvbind(xe, idsym(p->e1), v);
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
		id = idsym(e);
		if(xenvlook(lex, id))
			return e;
		se = Ztid(id);
		putsrc(se, &e->src);
		return se;
	case Eg:
		id = idsym(e->e1);
		e->e2 = toplevel(ctx, e->e2, env, lex);
		if(xenvlook(lex, id))
			return e;
		if(!envbinds(env, idcid(e->e1)))
			envdefine(env, idcid(e->e1), Xnil);
		se = Ztg(id, e->e2);
		putsrc(se, &e->src);
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
resolve1(U *ctx, Expr *e, Env *top, Xenv *lex, Expr *scope, Xenv *slex)
{
	Expr *se, *p;
	char *id;
	Xenv *rib;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eg:
		id = idsym(e->e1);
		if(xenvlook(lex, id)){
			e->e2 = resolve1(ctx, e->e2, top, lex, scope, slex);
			return e;
		}else if(!envbinds(top, idcid(e->e1)) && scope){
			/* create binding in innermost lexical scope */
			if(cqctflags['w'])
				cwarnln(ctx,
					e, "assignment to unbound variable: %s",
					id);
			newlocal(scope->e1, e->e1);
			xenvbind(slex, id, e);
			e->e2 = resolve1(ctx, e->e2, top, lex, scope, slex);
			return e;
		}else{
			/* global assignment */
			e->e2 = resolve1(ctx, e->e2, top, lex, scope, slex);
			se = Ztg(id, e->e2);
			putsrc(se, &e->src);
			return se;
		}
		break;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		e->e2 = resolve1(ctx, e->e2, top, rib, scope, slex);
		freexenv(rib);
		return e;
	case Escope:
		e->e1 = resolve1(ctx, e->e1, top, lex, e, slex);
		return e;
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		// slex always points to rib of innermost Escope's block
		// scope is 0 where we've introduced blocks at toplevel
		if(scope && scope->e1 == e)
			slex = rib;
		e->e2 = resolve1(ctx, e->e2, top, rib, scope, slex);
		freexenv(rib);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = resolve1(ctx, p->e1, top, lex, scope, slex);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = resolve1(ctx, e->e1, top, lex, scope, slex);
		e->e2 = resolve1(ctx, e->e2, top, lex, scope, slex);
		e->e3 = resolve1(ctx, e->e3, top, lex, scope, slex);
		e->e4 = resolve1(ctx, e->e4, top, lex, scope, slex);
		return e;
	}
}

static Expr*
resolve2(U *ctx, Expr *e, Env *top, Xenv *lex, Expr *scope, Xenv *slex)
{
	Expr *se, *p;
	char *id;
	Xenv *rib;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eid:
		id = idsym(e);
		if(xenvlook(lex, id))
			return e;
		if(cqctflags['w'] && !envbinds(top, idcid(e)))
			cwarnln(ctx, e,
				"reference to unbound variable: %s", id);
		se = Ztid(id);
		putsrc(se, &e->src);
		return se;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		e->e2 = resolve2(ctx, e->e2, top, rib, scope, slex);
		freexenv(rib);
		return e;
	case Escope:
		e->e1 = resolve2(ctx, e->e1, top, lex, e, slex);
		return e;
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		// slex always points to rib of innermost Escope's block
		// scope is 0 where we've introduced blocks at toplevel
		if(scope && scope->e1 == e)
			slex = rib;
		e->e2 = resolve2(ctx, e->e2, top, rib, scope, slex);
		freexenv(rib);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = resolve2(ctx, p->e1, top, lex, scope, slex);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = resolve2(ctx, e->e1, top, lex, scope, slex);
		e->e2 = resolve2(ctx, e->e2, top, lex, scope, slex);
		e->e3 = resolve2(ctx, e->e3, top, lex, scope, slex);
		e->e4 = resolve2(ctx, e->e4, top, lex, scope, slex);
		return e;
	}
}

static Expr*
resolve(U *ctx, Expr *e, Env *top, Xenv *lex, Expr *scope, Xenv *slex)
{
	e = resolve1(ctx, e, top, lex, scope, slex);
	e = resolve2(ctx, e, top, lex, scope, slex);
	return e;
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
docompileb(U *ctx, Expr *e)
{
	Src s;
	Xenv *lex;

	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */

	e = defloc(ctx, e, 0);
	e = globals(ctx, e, ctx->top->env);
	if(ctx->argsid){
		lex = mkxenv(0);
		xenvbind(lex, ctx->argsid, ctx->argsid);
	}else
		lex = 0;
	e = toplevel(ctx, e, ctx->top->env, lex);
	e = resolve(ctx, e, ctx->top->env, lex, 0, 0);
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
	e = Zlambda(ctx->argsid ? Zvararg(doid(ctx->argsid)) : nullelist(),
		     Zret(Ztg("$$", Zblock(nullelist(), e, NULL))));
	putsrc(e, &s);
	return e;
}