#include "sys.h"
#include "util.h"
#include "syscqct.h"

static u32 cnt;

static void
reset()
{
	cnt = 0;
}

/* we don't actually need to compute the name
   unless we want to see it. */
static char*
uniqid(char *id)
{
	unsigned long len;
	char *p;
	len = strlen(p)+1+10+1;
	p = emalloc(len);
	snprintf(p, len, "%s.%u", cnt);
	if(cnt+1 < cnt)
		fatal("too many identifiers");
	cnt++;
	return p;
}

static Expr*
alpha(U *ctx, Expr *e, Xenv *lex)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Elambda:
	case Eblock:
	case Eid;
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
			p->e1 = alpha(ctx, p->e1, lex);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = alpha(ctx, e->e1, lex);
		e->e2 = alpha(ctx, e->e2, lex);
		e->e3 = alpha(ctx, e->e3, lex);
		e->e4 = alpha(ctx, e->e4, lex);
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
		if(envbinds(top, id)){
			se = Ztid(id);
			putsrc(se, &e->src);
			freeexpr(e);
			return se;
		}
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
			newlocal(scope, id);
			bindids(slex, e->e1, e);
			e->e2 = resolve(ctx, e->e2, top, lex, scope, slex);
			return e;
		}else{
			/* new top-level binding */
			envgetbind(top, id);
			e->e2 = resolve(ctx, e->e2, top, lex, scope, slex);
			se = Ztg(id, e->e2);
			e->e2 = 0;
			putsrc(se, &e->src);
			freeexpr(e);
			return se;
		}
		break;
	case Elambda:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		e = resolve(ctx, e->e2, top, rib, scope, slex);
		freexenv(rib);
		return e;
	case Escope:
		return resolve(ctx, e->e1, top, lex, e->e1, slex);
	case Eblock:
		rib = mkxenv(lex);
		bindids(rib, e->e1, e);
		// slex always points to rib of innermost Escope's block
		if(scope->e1 == e)
			slex = rib;
		e = resolve(ctx, e->e2, top, rib, scope, slex);
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
	Src *s;

	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	
	e = globals(ctx, e, top->env);
	e = resolve(ctx, e, top->env, 0, 0, 0);
	e = rmscope(ctx, e);

	/*
	 * convert expression to function.  wrap the
	 * body in an assignment to avoid tail call
	 * optimization, to preserve presentation of
	 * top-level source line info in errors.
	 */
	s = &e->src;
	e = Zlambda(argsid ? doid(argsid) : nullelist(),
		     Zret(Zset(doid("$$"), Zblock(nullelist(), e, NULL))));
	putsrc(e, s);
	return e;
}
