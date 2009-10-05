#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* compile1(U *ctx, Expr *e);

struct Arg {
	Expr **e;
	U *ctx;
};

static Expr*
compiletab(U *ctx, Expr *e)
{
	Expr *loc, *te, *se, *ti;
	Src *src;

	src = &e->src;
	loc = Zlocals(1, "$tab");
	te = nullelist();

	se = Zset(doid("$tab"), Zcall(G("mktab"), 0));
//	putsrc(se, src);
	te = Zcons(se, te);

	e->e1 = compile1(ctx, e->e1);
	e = e->e1;
	while(e->kind == Eelist){
		ti = e->e1;
		se = Zcall(G("tabinsert"), 3, doid("$tab"), ti->e1, ti->e2->e1);
		putsrc(se, &ti->src);
		te = Zcons(se, te);
		ti->e1 = 0;
		ti->e2->e1 = 0;
		e = e->e2;
	}
	se = doid("$tab");
	te = Zcons(se, te);
	te = newexpr(Eblock, loc, invert(te), 0, 0);
	putsrc(te, src);
	return te;
}

static Expr*
compilelist(U *ctx, Expr *e)
{
	Expr *loc, *te, *se;
	Src *src;
	Imm i;

	src = &e->src;
	loc = Zlocals(1, "$lst");
	te = nullelist();

	se = Zset(doid("$lst"), Zcall(G("mklist"), 1, Zuint(0)));
//	putsrc(se, src);
	te = Zcons(se, te);

	e->e1 = compile1(ctx, e->e1);
	e = e->e1;
	i = 0;
	while(e->kind == Eelist){
		se = Zcall(G("listins"), 3, doid("$lst"), Zuint(i++), e->e1);
		putsrc(se, &e->e1->src);
		te = Zcons(se, te);
		e->e1 = 0;
		e = e->e2;
	}
	se = doid("$lst");
	te = Zcons(se, te);
	te = newexpr(Eblock, loc, invert(te), 0, 0);
	putsrc(te, src);
	return te;
}

static Expr*
compilesizeof(U *ctx, Decl *d, Src *src)
{
	Type *t;
	Expr *se, *te, *loc;
	char *dom;

	t = d->type;
	if(t->dom)
		dom = t->dom;
	else
		dom = "litdom";

	loc = Zlocals(2, "$tn", "$tmp");

	te = nullelist();

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t, compile1, ctx, 0));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn);
	se = Zset(doid("$tmp"),
		  Zcall(G("looktype"), 2, doid(dom), doid("$tn")));
	te = Zcons(se, te);

	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$tmp")),
		     Zcall(G("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// sizeof($tmp);
	se = Zsizeof(doid("$tmp"));
	te = Zcons(se, te);
	te = newexpr(Eblock, loc, invert(te), 0, 0); 
	putsrc(te, src);
	return te;
}

static Expr*
compiletypeof(U *ctx, Decl *d, Src *src)
{
	Type *t;
	Expr *se, *te, *loc;
	char *dom;

	t = d->type;
	if(t->dom)
		dom = t->dom;
	else
		dom = "litdom";

	loc = Zlocals(2, "$tn", "$tmp");

	te = nullelist();

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t, compile1, ctx, 0));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn);
	se = Zset(doid("$tmp"),
		  Zcall(G("looktype"), 2, doid(dom), doid("$tn")));
	te = Zcons(se, te);
	
	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$tmp")),
		     Zcall(G("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// $tmp;
	se = doid("$tmp");
	te = Zcons(se, te);
	te = newexpr(Eblock, loc, invert(te), 0, 0);
	putsrc(te, src);
	return te;
}

static Expr*
compilecast(U *ctx, Expr *e)
{
	Type *t;
	Expr *se, *te, *dom, *loc;
	Decl *d;

	loc = Zlocals(3, "$tmp", "$tn", "$type");

	te = nullelist();
	
	// $tmp = e->e2;
	e->e2 = compile1(ctx, e->e2);
	se = Zset(doid("$tmp"), e->e2);
	te = Zcons(se, te);

	d = e->e1->xp;
	t = d->type;
	if(t->dom)
		dom = doid(t->dom);
	else
		dom = Zcall(G("domof"), 1, doid("$tmp"));

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t, compile1, ctx, 0));
	te = Zcons(se, te);

	// $type = looktype(dom, $tn);
	se = Zset(doid("$type"),
		  Zcall(G("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);
	
	// if(isnil($type)) error("undefined type: %t", $tn);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$type")),
		     Zcall(G("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// cast($type, $tmp);
	se = Zxcast(doid("$type"), doid("$tmp"));
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	putsrc(te, &e->src);
	return te;
}

static Expr*
compilecontainer(U *ctx, Expr *e)
{
	Type *t;
	Expr *se, *te, *dom, *loc;
	Decl *d;

	te = nullelist();

	loc = Zlocals(5, "$tn", "$fld", "$ptype", "$type", "$tmp");

	// $tmp = e->e1;
	e->e1 = compile1(ctx, e->e1);
	se = Zset(doid("$tmp"), e->e1);
	te = Zcons(se, te);

	// pick domain in which to lookup type
	d = e->e2->xp;
	t = d->type;
	if(t->dom)
		dom = doid(t->dom);
	else
		dom = Zcall(G("domof"), 1, doid("$tmp"));

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t, compile1, ctx, 0));
	te = Zcons(se, te);

	// $type = looktype(dom, $tn);
	se = Zset(doid("$type"),
		  Zcall(G("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);
	
	// if(isnil($type)) error("undefined type: %t", $tn);
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$type")),
		     Zcall(G("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// $fld = lookfield($type, field);
	se = Zset(doid("$fld"),
		  Zcall(G("lookfield"), 2,
			doid("$type"), Zconsts(e->e3->id)));
	te = Zcons(se, te);

	// if(isnil($fld)) error("undefined field: %s", sym);
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$fld")),
		     Zcall(G("error"), 2,
			   Zconsts("undefined field: %s"),
			   Zconsts(e->e3->id)),
		     0, 0);
	te = Zcons(se, te);

	// $ptype = nsptr(domof($tmp));
	se = Zset(doid("$ptype"),
		  Zcall(G("nsptr"), 1,
			Zcall(G("domof"), 1, doid("$tmp"))));
	te = Zcons(se, te);

	// FIXME: maybe it be simpler to rewrite at higher level
	//        where type names can be reused:
	//        (typename*)((void*)p - fieldoff(looktype(typename)))
	//        but note that above draws final pointer type definition
	//        from typename domain, whereas here we always draw from
	//        P's domain.
	// {mkctype_ptr($type,$ptype)} ({$ptype}$tmp - fieldoff($fld))
	se = Zxcast(Zcall(G("mkctype_ptr"), 2,
			  doid("$type"), doid("$ptype")),
		    Zsub(Zxcast(doid("$ptype"), doid("$tmp")),
			 Zcall(G("fieldoff"), 1, doid("$fld"))));
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	putsrc(te, &e->src);
	return te;
}

static int
istypeform(Expr *e)
{
	switch(e->kind){
	case Ecast:
	case Esizeoft:
	case Etypeoft:
		return 1;
	default:
		return 0;
	}
}

static Expr*
compileambig(U *ctx, Expr *e)
{
	Expr *tf, *of;
	Expr *se, *te, *loc;
	char *dom;
	Decl *d;
	Type *t;
		
	/* exactly one of the two parses is a form that operates on a
	   type name */
	tf = of = 0;
	if(istypeform(e->e1)){
		tf = e->e1;
		of = e->e2;
	}
	if(istypeform(e->e2)){
		if(tf)
			fatal("bug");
		tf = e->e2;
		of = e->e1;
	}
	if(tf == 0)
		fatal("bug");

	d = tf->e1->xp;
	t = d->type;

	if(t->dom)
		dom = t->dom;
	else
		dom = "litdom";

	loc = Zlocals(2, "$tmp", "$tn");

	te = nullelist();

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t, compile1, ctx, 0));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn)
	se = Zset(doid("$tmp"),
		  Zcall(G("looktype"), 2, doid(dom), doid("$tn")));
	te = Zcons(se, te);

	/* must we compile TF only after using all references to TF->...->type?
	   (including gentypename and dom?) */
	tf = compile1(ctx, tf);
	of = compile1(ctx, of);

	// if(isnil($tmp)) <other form> else <type form>
	se = newexpr(Eif, Zcall(G("isnil"), 1, doid("$tmp")), of, tf, 0);
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	putsrc(te, &e->src);
	return te;
}

static Expr*
compile1(U *ctx, Expr *e)
{
	Expr *se, *q;

	if(e == NULL)
		return e;

	switch(e->kind){
	case Eglobal:
		return e;
	case Elambda:
		e->e4 = compile1(ctx, e->e4);
		/* fall through */
	case Eblock:
		e->e2 = compile1(ctx, e->e2);
		return e;
	case Eambig:
		se = compileambig(ctx, e);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Esizeoft:
		se = compilesizeof(ctx, e->e1->xp, &e->src);
		freeexpr(e);
		return se;
	case Etypeoft:
		se = compiletypeof(ctx, e->e1->xp, &e->src);
		freeexpr(e);
		return se;
	case Ecast:
		se = compilecast(ctx, e);
		// e1 is a decl that needs to be freed
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Econtainer:
		se = compilecontainer(ctx, e);
		e->e1 = 0;
		freeexpr(e);
		return se;
	case Etab:
		se = compiletab(ctx, e);
		freeexpr(e);
		return se;
	case Elist:
		se = compilelist(ctx, e);
		freeexpr(e);
		return se;
	case Elapply:
		q = e->e2;
		while(q->kind == Eelist){
			q->e1 = Zlambda(nullelist(),
					Zblock(nullelist(),
					       compile1(ctx, q->e1),
					       NULL));
			q = q->e2;
		}
		se = Zapply(compile1(ctx, e->e1), e->e2);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
#if 0
	case Ens:
		se = compilens(ctx, e);
		e->e1 = 0;
		e->e3 = 0;
		freeexpr(e);
		return se;
#endif
	default:
		e->e1 = compile1(ctx, e->e1);
		e->e2 = compile1(ctx, e->e2);
		e->e3 = compile1(ctx, e->e3);
		e->e4 = compile1(ctx, e->e4);
		return e;
	}
}

int
docompile1(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return -1;	/* error */
	compile1(ctx, e);
	return 0;
}
