#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
newlocal(Expr *e, Expr *id)
{
	if(e->kind != Eblock)
		fatal("bug");
	sete1(e, putsrc(Zcons(id, e->e1), e->e1->src));
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
		putsrc(p, e->src);
		return p;
	case Escope:
		sete1(e, defloc(ctx, e->e1, e));
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, defloc(ctx, p->e1, scope));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, defloc(ctx, e->e1, scope));
		sete2(e, defloc(ctx, e->e2, scope));
		sete3(e, defloc(ctx, e->e3, scope));
		sete4(e, defloc(ctx, e->e4, scope));
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
		return putsrc(Znop(), e->src);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, globals(ctx, p->e1, env));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, globals(ctx, e->e1, env));
		sete2(e, globals(ctx, e->e2, env));
		sete3(e, globals(ctx, e->e3, env));
		sete4(e, globals(ctx, e->e4, env));
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
		sete2(e, toplevel(ctx, e->e2, env, rib));
		freexenv(rib);
		return e;
	case Elabel:
	case Egoto:
	case Escope:
	case Elambda:
		return e;
	case E_tid:
		bug();
	case Eid:
		id = idsym(e);
		if(xenvlook(lex, id))
			return e;
		se = Ztid(id);
		putsrc(se, e->src);
		return se;
	case Eg:
		id = idsym(e->e1);
		sete2(e, toplevel(ctx, e->e2, env, lex));
		if(xenvlook(lex, id))
			return e;
		if(!envbinds(env, idcid(e->e1)))
			envdefine(env, idcid(e->e1), Xnil);
		se = Ztg(id, e->e2);
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, toplevel(ctx, p->e1, env, lex));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, toplevel(ctx, e->e1, env, lex));
		sete2(e, toplevel(ctx, e->e2, env, lex));
		sete3(e, toplevel(ctx, e->e3, env, lex));
		sete4(e, toplevel(ctx, e->e4, env, lex));
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
			sete2(e, resolve1(ctx, e->e2, top, lex, scope, slex));
			return e;
		}else if(!envbinds(top, idcid(e->e1)) && scope){
			/* create binding in innermost lexical scope */
			if(cqctflags['w'] && strcmp(id, "_") != 0)
				cwarnln(ctx,
					e, "assignment to unbound variable: %s",
					id);
			newlocal(scope->e1, e->e1);
			xenvbind(slex, id, e);
			sete2(e, resolve1(ctx, e->e2, top, lex, scope, slex));
			return e;
		}else{
			/* global assignment */
			sete2(e, resolve1(ctx, e->e2, top, lex, scope, slex));
			se = Ztg(id, e->e2);
			putsrc(se, e->src);
			return se;
		}
		break;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		sete2(e, resolve1(ctx, e->e2, top, rib, scope, slex));
		freexenv(rib);
		return e;
	case Escope:
		sete1(e, resolve1(ctx, e->e1, top, lex, e, slex));
		return e;
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		// slex always points to rib of innermost Escope's block
		// scope is 0 where we've introduced blocks at toplevel
		if(scope && scope->e1 == e)
			slex = rib;
		sete2(e, resolve1(ctx, e->e2, top, rib, scope, slex));
		freexenv(rib);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, resolve1(ctx, p->e1, top, lex, scope, slex));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, resolve1(ctx, e->e1, top, lex, scope, slex));
		sete2(e, resolve1(ctx, e->e2, top, lex, scope, slex));
		sete3(e, resolve1(ctx, e->e3, top, lex, scope, slex));
		sete4(e, resolve1(ctx, e->e4, top, lex, scope, slex));
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
	case Elabel:
	case Egoto:
	case E_tid:
		return e;
	case Eid:
		id = idsym(e);
		if(xenvlook(lex, id))
			return e;
		if(cqctflags['w']
		   && !envbinds(top, idcid(e))
		   && strcmp(id, "_") != 0)
			cwarnln(ctx, e,
				"reference to unbound variable: %s", id);
		se = Ztid(id);
		putsrc(se, e->src);
		return se;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		sete2(e, resolve2(ctx, e->e2, top, rib, scope, slex));
		freexenv(rib);
		return e;
	case Escope:
		sete1(e, resolve2(ctx, e->e1, top, lex, e, slex));
		return e;
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		// slex always points to rib of innermost Escope's block
		// scope is 0 where we've introduced blocks at toplevel
		if(scope && scope->e1 == e)
			slex = rib;
		sete2(e, resolve2(ctx, e->e2, top, rib, scope, slex));
		freexenv(rib);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, resolve2(ctx, p->e1, top, lex, scope, slex));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, resolve2(ctx, e->e1, top, lex, scope, slex));
		sete2(e, resolve2(ctx, e->e2, top, lex, scope, slex));
		sete3(e, resolve2(ctx, e->e3, top, lex, scope, slex));
		sete4(e, resolve2(ctx, e->e4, top, lex, scope, slex));
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

typedef
struct VU
{
	Expr *e; /* source expression */
	u32 ref;
	u32 def;
	u32 shadows;
} VU;

static VU*
mkvu(Expr *e)
{
	VU *vu;
	vu = emalloc(sizeof(VU));
	vu->e = e;
	return vu;
}

static void
freevu(VU *v)
{
	efree(v);
}

static void
wbindids(U *ctx, Xenv *xe, Expr *e, char *what)
{
	Expr *p;
	VU *vu;

	p = e;
	while(p->kind == Eelist){
		if(p->e1->kind == Eellipsis){
			p = p->e2;
			continue;
		}
		vu = xenvlook(xe, idsym(p->e1));
		if(vu){
			cwarnln(ctx, vu->e,
				"multiple declarations of %s %s",
				what,
				idsym(p->e1));
			p = p->e2;
			continue;
		}
		xenvbind(xe, idsym(p->e1), mkvu(p->e1));
		p = p->e2;
	}
}

static void
checkbvar(void *u, char *id, void *v)
{
	U *ctx;
	VU *vu;
	ctx = u;
	vu = v;
	if(!strcmp(id, "_"))
		return;
	if(vu->shadows)
		cwarnln(ctx, vu->e, "variable shadows parameter: %s", id);
	if(vu->ref == 0 && vu->def == 0)
		cwarnln(ctx, vu->e, "unused variable: %s", id);
	else if(vu->ref == 0)
		cwarnln(ctx, vu->e, "variable defined but not referenced: %s",
			id);
	else if(vu->def == 0)
		cwarnln(ctx, vu->e, "variable referenced but not defined: %s",
			id);
}

static void
checkfnvar(void *u, char *id, void *v)
{
	U *ctx;
	VU *vu;
	ctx = u;
	vu = v;
//	if(vu->ref == 0 && vu->def == 0)
//		cwarnln(ctx, vu->e, "unused parameter: %s", id);
}

static void
markshadow(void *u, char *id, void *v)
{
	VU *vu;
	Xenv *fn;
	fn = u;
	vu = v;
	if(xenvlook(fn, id))
		vu->shadows++;
}

static void
free1vu(void *u, char *id, void *v)
{
	VU *vu;
	vu = (VU*)v;
	freevu(vu);
}

/* warn on:
   - locals that shadow lambda vars
   - redundant vars in lambda arg list
   - redundant vars in locals list
   - unused parameters
   - unused locals
   - defined but not used
   - used but not defined
*/
static void
check(U *ctx, Expr *e, Xenv *fn, Xenv *lex)
{
	Expr *p;
	Xenv *rib;
	VU *vu;
	if(e == 0)
		return;
	switch(e->kind){
	case Eg:
		check(ctx, e->e2, fn, lex);
		vu = xenvlook(lex, idsym(e->e1));
		if(vu == 0)
			return;
		vu->def++;
		return;
	case Elabel:
	case Egoto:
		return;
	case Eid:
		vu = xenvlook(lex, idsym(e));
		if(vu == 0)
			return;
		vu->ref++;
		return;
	case Escope:
		rib = mkxenv(0);
		wbindids(ctx, rib, e->e1->e1, "variable");
		if(fn)
			xenvforeach(rib, markshadow, fn);
		xenvlink(rib, lex);
		check(ctx, e->e1->e2, fn, rib);
		xenvforeach(rib, checkbvar, ctx);
		xenvforeach(rib, free1vu, 0);
		freexenv(rib);
		return;
	case Elambda:
		rib = mkxenv(0);
		wbindids(ctx, rib, e->e1, "parameter");
		fn = xenvcopy(rib);
		xenvlink(rib, lex);
		check(ctx, e->e2, fn, rib);
		xenvforeach(rib, checkfnvar, ctx);
		xenvforeach(rib, free1vu, 0);
		freexenv(rib);
		freexenv(fn);
		return;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			check(ctx, p->e1, fn, lex);
			p = p->e2;
		}
		return;
	default:
		check(ctx, e->e1, fn, lex);
		check(ctx, e->e2, fn, lex);
		check(ctx, e->e3, fn, lex);
		check(ctx, e->e4, fn, lex);
		return;
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
	check(ctx, e, 0, 0);
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
	putsrc(e, s);
	return e;
}
