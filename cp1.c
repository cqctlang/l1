#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* compile1(U *ctx, Expr *e);

static Expr*
compiledef(U *ctx, Expr *e)
{
	Expr *p;

	p = Zset(e->e1,
		 Zlambdn(e->e2,
			 compile1(ctx, e->e3),
			 copyexpr(e->e1)));
	putsrc(p, e->src);
	return p;
}

static Expr*
compiletab(U *ctx, Expr *e)
{
	Expr *loc, *te, *se, *ti;
	Src src;

	src = e->src;
	loc = Zlocals(1, "$tab");
	te = nullelist();

	se = Zset(doid("$tab"), Zcall(G("mktab"), 0));
//	putsrc(se, src);
	te = Zcons(se, te);

	sete1(e, compile1(ctx, e->e1));
	e = e->e1;
	while(e->kind == Eelist){
		ti = e->e1;
		se = Zcall(G("tabinsert"), 3, doid("$tab"), ti->e1, ti->e2->e1);
		putsrc(se, ti->src);
		te = Zcons(se, te);
		e = e->e2;
	}
	se = doid("$tab");
	te = Zcons(se, te);
	te = Zblock(loc, invert(te), NULL);
	putsrc(te, src);
	return te;
}

static Expr*
compilelist(U *ctx, Expr *e)
{
	Expr *loc, *te, *se;
	Src src;
	Imm i;

	src = e->src;
	loc = Zlocals(1, "$lst");
	te = nullelist();

	se = Zset(doid("$lst"), Zcall(G("mklist"), 1, Zuint(0)));
//	putsrc(se, src);
	te = Zcons(se, te);

	sete1(e, compile1(ctx, e->e1));
	e = e->e1;
	i = 0;
	while(e->kind == Eelist){
		se = Zcall(G("listins"), 3, doid("$lst"), Zuint(i++), e->e1);
		putsrc(se, e->e1->src);
		te = Zcons(se, te);
		e = e->e2;
	}
	se = doid("$lst");
	te = Zcons(se, te);
	te = Zblock(loc, invert(te), NULL);
	putsrc(te, src);
	return te;
}

static Expr*
mkstxlist(Expr *e)
{
	if(e->kind == Enull)
		return putsrc(Zcall(G("mkstx"), 1, Zcid("null")),
			      e->src);
	else
		return putsrc(Zcall(G("mkstx"), 3,
				    Zcid("elist"),
				    e->e1,
				    mkstxlist(e->e2)),
			      e->src);
}

static Expr*
compilestxlist(U *ctx, Expr *e)
{
	return mkstxlist(compile1(ctx, e->e1));
}

static Expr*
compilepair(U *ctx, Expr *e)
{
	Expr *loc, *se;
	Src src;

	src = e->src;
	loc = Zlocals(1, "$pr");
	sete1(e, compile1(ctx, e->e1));
	sete2(e, compile1(ctx, e->e2));
	se = Zset(doid("$pr"), Zcall(G("cons"), 2, e->e1, e->e2));
	se = Zcons(se, doid("$lst"));
	se = Zblock(loc, se, NULL);
	putsrc(se, src);
	return se;
}

static void
domandtype(Expr *e, Expr **dom, Expr **t)
{
	if(e->kind == Etickt){
		if(e->e1)
			*dom = e->e1;
		else
			*dom = 0;
		*t = e->e2;
	}else{
		*dom = 0;
		*t = e;
	}
}

static Expr*
compiletypeof(U *ctx, Expr *e, Src src)
{
	Expr *se, *te, *loc, *t, *dom;

	domandtype(e, &dom, &t);
	if(!dom)
		dom = doid("litdom");
	loc = Zlocals(2, "$tn", "$tmp");

	te = nullelist();

	// $tn = t;
	se = Zset(doid("$tn"), compile1(ctx, t));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn);
	se = Zset(doid("$tmp"),
		  Zcall(G("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);

	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$tmp")),
		     Zcall(G("error"), 2,
			   Zstr("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// $tmp;
	se = doid("$tmp");
	te = Zcons(se, te);
	te = Zblock(loc, invert(te), NULL);
	putsrc(te, src);
	return te;
}

static Expr*
compilesizeof(U *ctx, Expr *e, Src src)
{
	return putsrc(Zsizeof(compiletypeof(ctx, e, src)), src);
}

static Expr*
compilemkctype(U *ctx, Expr *e, Src src)
{
	Expr *t, *dom;

	domandtype(e, &dom, &t);
	if(dom)
		cerror(ctx, e, "attempt to qualify type in @typename");
	return compile1(ctx, t);
}


static Expr*
compilecast(U *ctx, Expr *e)
{
	Expr *se, *te, *dom, *loc, *t;

	loc = Zlocals(3, "$tmp", "$tn", "$type");

	te = nullelist();

	// $tmp = e->e2;
	sete2(e, compile1(ctx, e->e2));
	se = Zset(doid("$tmp"), e->e2);
	te = Zcons(se, te);

	domandtype(e->e1, &dom, &t);
	if(!dom)
		dom = Zcall(G("domof"), 1, doid("$tmp"));

	// $tn = t;
	se = Zset(doid("$tn"), compile1(ctx, t));
	te = Zcons(se, te);

	// $type = looktype(dom, $tn);
	se = Zset(doid("$type"), Zcall(G("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);

	// if(isnil($type)) error("undefined type: %t", $tn);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$type")),
		     Zcall(G("error"), 2,
			   Zstr("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// cast($type, $tmp);
	se = Zxcast(doid("$type"), doid("$tmp"));
	te = Zcons(se, te);

	te = Zblock(loc, invert(te), NULL);
	putsrc(te, e->src);
	return te;
}

static Expr*
compilecontainer(U *ctx, Expr *e)
{
	Expr *se, *te, *dom, *loc, *t;

	te = nullelist();

	loc = Zlocals(5, "$tn", "$fld", "$prep", "$type", "$tmp");

	// $tmp = e->e1;
	sete1(e, compile1(ctx, e->e1));
	se = Zset(doid("$tmp"), e->e1);
	te = Zcons(se, te);

	// pick domain in which to lookup type
	domandtype(e->e2, &dom, &t);
	if(!dom)
		dom = Zcall(G("domof"), 1, doid("$tmp"));

	// $tn = t;
	se = Zset(doid("$tn"), compile1(ctx, t));
	te = Zcons(se, te);

	// $type = looktype(dom, $tn);
	se = Zset(doid("$type"), Zcall(G("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);

	// if(isnil($type)) error("undefined type: %t", $tn);
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$type")),
		     Zcall(G("error"), 2,
			   Zstr("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// $fld = lookfield($type, field);
	se = Zset(doid("$fld"),
		  Zcall(G("lookfield"), 2, doid("$type"), Zid2sym(e->e3)));
	te = Zcons(se, te);

	// if(isnil($fld)) error("undefined field: %s", sym);
	se = newexpr(Eif,
		     Zcall(G("isnil"), 1, doid("$fld")),
		     Zcall(G("error"), 2,
			   Zstr("undefined field: %s"),
			   Zid2sym(e->e3)),
		     0, 0);
	te = Zcons(se, te);

	// $prep = nsptr(domof($tmp));
	se = Zset(doid("$prep"),
		  Zcall(G("nsptr"), 1,
			Zcall(G("domof"), 1, doid("$tmp"))));
	te = Zcons(se, te);

	// FIXME: maybe it be simpler to rewrite at higher level
	//        where type names can be reused:
	//        (typename*)((void*)p - fieldoff(looktype(typename)))
	//        but note that above draws final pointer type definition
	//        from typename domain, whereas here we always draw from
	//        Ps domain.
	// {mkctype_ptr($type,$prep)} ({mkctype_ptr(void,$prep)}$tmp - fieldoff($fld))
	se = Zxcast(Zcall(G("mkctype_ptr"), 2, doid("$type"), doid("$prep")),
		    Zsub(Zxcast(Zcall(G("mkctype_ptr"), 2,
				      Zcall(G("mkctype_void"), 0),
				      doid("$prep")),
				doid("$tmp")),
			 Zcall(G("fieldoff"), 1, doid("$fld"))));
	te = Zcons(se, te);

	te = Zblock(loc, invert(te), NULL);
	putsrc(te, e->src);
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
	Expr *se, *te, *loc, *dom, *t;

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

	domandtype(tf->e1, &dom, &t);
	if(!dom)
		dom = doid("litdom");

	loc = Zlocals(2, "$tmp", "$tn");

	te = nullelist();

	// $tn = t;
	se = Zset(doid("$tn"), compile1(ctx, copyexpr(t)));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn)
	se = Zset(doid("$tmp"), Zcall(G("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);

	of = compile1(ctx, of);
	tf = compile1(ctx, tf);

	// if(isnil($tmp)) <other form> else <type form>
	se = newexpr(Eif, Zcall(G("isnil"), 1, doid("$tmp")), of, tf, 0);
	te = Zcons(se, te);

	te = Zblock(loc, invert(te), NULL);
	putsrc(te, e->src);
	return te;
}

/* expand various independent syntactic forms */
static Expr*
compile1(U *ctx, Expr *e)
{
	Expr *se, *q;

	if(e == 0)
		return e;

	switch(e->kind){
	case Edefine:
		se = compiledef(ctx, e);
		return se;
	case Elambda:
	case Eblock:
		sete2(e, compile1(ctx, e->e2));
		return e;
	case Eambig:
		se = compileambig(ctx, e);
		return se;
	case Esizeoft:
		se = compilesizeof(ctx, e->e1, e->src);
		return se;
	case Etypeoft:
		se = compiletypeof(ctx, e->e1, e->src);
		return se;
	case Emkctype:
		se = compilemkctype(ctx, e->e1, e->src);
		return se;
	case Ecast:
		se = compilecast(ctx, e);
		// e1 is a decl that needs to be freed
		return se;
	case Econtainer:
		se = compilecontainer(ctx, e);
		return se;
	case Etab:
		se = compiletab(ctx, e);
		return se;
	case Elist:
		se = compilelist(ctx, e);
		return se;
	case Estxlist:
		se = compilestxlist(ctx, e);
		return se;
	case Epair:
		se = compilepair(ctx, e);
		return se;
	case Elapply:
		q = e->e2;
		while(q->kind == Eelist){
			sete1(q, putsrc(Zlambda(nullelist(),
						Zblock(nullelist(),
						       compile1(ctx, q->e1),
						       NULL)),
					q->e1->src));
			q = q->e2;
		}
		se = putsrc(Zapply(compile1(ctx, e->e1), e->e2), e->src);
		return se;
	case Eelist:
		q = e;
		while(q->kind == Eelist){
			sete1(q, compile1(ctx, q->e1));
			q = q->e2;
		}
		return e;
	default:
		sete1(e, compile1(ctx, e->e1));
		sete2(e, compile1(ctx, e->e2));
		sete3(e, compile1(ctx, e->e3));
		sete4(e, compile1(ctx, e->e4));
		return e;
	}
}

Expr*
docompile1(U *ctx, Expr *e)
{
	compile1(ctx, e);
	return e;
}

void
l1_cp1(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cp1");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompile1(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
