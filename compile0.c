#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* compile_rval(U *ctx, Expr *e, unsigned lfree);
static Expr* compile0(U *ctx, Expr* e);

static int
islval(Expr *e)
{
	switch(e->kind){
	case Etick:
		return 1;
	case Ederef:
		return 1;
	case Ecast:
		return islval(e->e2);
	case Edot:
		return islval(e->e1);
	default:
		return 0;
	}
}

static Expr*
rvalblock(Expr *body, unsigned lfree)
{
	Expr *e;
	if(lfree)
		e = Zlocals(1, "$val");
	else
		e = Zlocals(4, "$val", "$dom", "$type", "$addr");
	return newexpr(Eblock, e, body, 0, 0);
}

static Expr*
lvalblock(Expr *body)
{
	Expr *e;
	e = Zlocals(1, "$tmp");
	return newexpr(Eblock, e, body, 0, 0);
}

static Expr*
compile_lval(U *ctx, Expr *e, int needaddr)
{
	Expr *se, *te, *dom;
	Type *t;
	Decl *d;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Ecast:
		te = nullelist();

		// compile lvalue reference to expression,
		// using dom, type bindings
		se = compile_lval(ctx, e->e2, needaddr);
		te = Zcons(se, te);

		// clobber type with cast operand 
		d = e->e1->xp;
		t = d->type;
		if(t->dom)
			dom = doid(t->dom);
		else
			dom = doid("$dom");

		se = Zblock(Zlocals(1, "$tn"),
			    Zset(doid("$tn"), gentypename(t, compile0, ctx)),
			    Zset(doid("$type"),
				 Zcall(G("looktype"), 2,
				       dom, doid("$tn"))),
			    Zif(Zcall(G("isnil"), 1, doid("$type")),
				Zcall(G("error"), 2,
				      Zconsts("undefined type: %t"),
				      doid("$tn"))),
			    NULL);
		te = Zcons(se, te);

		e->e2 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	case Etick:
		te = nullelist();

		// $dom = dom;
		se = Zset(doid("$dom"), e->e1);
		te = Zcons(se, te);

		// $tmp = nslooksym(domns($dom))($dom,sym)
		se = Zcall(G("nsof"), 1, doid("$dom"));
		se = Zcall(G("nslooksym"), 1, se);
		se = Zcall(se, 2, doid("$dom"), Zconsts(e->e2->id));
		se = Zset(doid("$tmp"), se);
		te = Zcons(se, te);
		
		// if(isnil($tmp)) error("undefined symbol: %s", sym);
		se = newexpr(Eif,
			     Zcall(G("isnil"), 1, doid("$tmp")),
			     Zcall(G("error"), 2,
				   Zconsts("undefined symbol: %s"),
				   Zconsts(e->e2->id)),
			     0, 0);
		te = Zcons(se, te);

		// $type = symtype($tmp);
		se = Zset(doid("$type"),
			  Zcall(G("symtype"), 1, doid("$tmp")));
		te = Zcons(se, te);

		// $addr = symoff($tmp, 2);
		if(needaddr){
			se = Zset(doid("$addr"),
				  Zcall(G("symoff"), 1, doid("$tmp")));
			te = Zcons(se, te);

			// if(isnil($addr)) error("symbol lacks address: %s");
			se = newexpr(Eif,
				     Zcall(G("isnil"), 1, doid("$addr")),
				     Zcall(G("error"), 2,
					   Zconsts("symbol lacks address: %s"),
					   Zconsts(e->e2->id)),
				     0, 0);
			te = Zcons(se, te);
		}
		
		freeexpr(e->e2);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	case Ederef:
		te = nullelist();

		// $tmp = compile_rval(ctx, e->e1);
		if(needaddr || !islval(e->e1)){
			se = Zset(doid("$tmp"), compile_rval(ctx, e->e1, 0));
			te = Zcons(se, te);

			// $type = subtype($typeof($tmp));
			se = Zset(doid("$type"),
				  Zcall(G("subtype"), 1,
					Zcall(doid("$typeof"), 1,
					      doid("$tmp"))));
			te = Zcons(se, te);

			// $dom = domof($tmp);
			se = Zset(doid("$dom"),
				  Zcall(G("domof"), 1, doid("$tmp")));
			te = Zcons(se, te);

			// $addr = {nsptr($dom)}$tmp
			if(needaddr){
				se = Zset(doid("$addr"),
					  Zxcast(Zcall(G("nsptr"), 1,
						       doid("$dom")),
						 doid("$tmp")));
				te = Zcons(se, te);
			}
		}else{
			// compile lvalue reference to pointer,
			// using dom, type bindings
			se = compile_lval(ctx, e->e1, 0);
			te = Zcons(se, te);

			// $type = subtype($type);
			se = Zset(doid("$type"),
				  Zcall(G("subtype"), 1, doid("$type")));
			te = Zcons(se, te);
		}
		e->e1 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	case Edot:
		te = nullelist();

		// compile lvalue reference to containing struct,
		// using dom, type, addr bindings.
		se = compile_lval(ctx, e->e1, needaddr);
		te = Zcons(se, te);
		
		// $tmp = lookfield(type, field);
		se = Zset(doid("$tmp"),
			  Zcall(G("lookfield"), 2,
				doid("$type"), Zconsts(e->e2->id)));
		te = Zcons(se, te);

		// if(isnil($tmp)) error("undefined field: %s", sym);
		se = newexpr(Eif,
			     Zcall(G("isnil"), 1, doid("$tmp")),
			     Zcall(G("error"), 2,
				   Zconsts("undefined field: %s"),
				   Zconsts(e->e2->id)),
			     0, 0);
		te = Zcons(se, te);

		// $type = fieldtype($tmp);
		se = Zset(doid("$type"),
			  Zcall(G("fieldtype"), 1, doid("$tmp")));
		te = Zcons(se, te);
				     
		// $addr = $addr + fieldoff($tmp)
		if(needaddr){
			se = Zset(doid("$addr"),
				  Zadd(doid("$addr"),
				       Zcall(G("fieldoff"), 1,
					     doid("$tmp"))));
			te = Zcons(se, te);
		}
		
		e->e1 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	default:
		cerror(ctx, e, "expression is not an lvalue");
	}
}

static Expr*
compile_rval(U *ctx, Expr *e, unsigned lfree)
{
	Expr *se, *te, *p;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Etick:
	case Edot:
	case Ederef:
		te = nullelist();
		se = compile_lval(ctx, e, 1);
		te = Zcons(se, te);
		se = Zcval(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		return rvalblock(invert(te), lfree);
	case Eref:
		te = nullelist();
		se = compile_lval(ctx, e->e1, 1);
		te = Zcons(se, te);
		se = Zref(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Eg:
		if(!islval(e->e1)){
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			e->e1 = compile_rval(ctx, e->e1, 0);
			e->e2 = compile_rval(ctx, e->e2, 0);
			return e;
		}

		te = nullelist();

		se = compile_lval(ctx, e->e1, 1);
		te = Zcons(se, te);

		se = Zset(doid("$val"), compile_rval(ctx, e->e2, 0));
		te = Zcons(se, te);

		se = Zcall(doid("$put"), 4,
			   doid("$dom"), doid("$addr"), doid("$type"),
			   doid("$val"));
		te = Zcons(se, te);

		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Egop:
		if(!islval(e->e1)){
			/* FIXME: if we translate ordinary cval Egop here
			   into equivalent source, do we generate same
			   or similar code as compile.c on Egop? */
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			e->e1 = compile_rval(ctx, e->e1, 0);
			e->e2 = compile_rval(ctx, e->e2, 0);
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(ctx, e->e1, 1));
		te = Zcons(se, te);

		se = Zset(doid("$val"),
			  Zbinop(e->op, doid("$val"),
				 compile_rval(ctx, e->e2, 0)));
		te = Zcons(se, te);

		se = Zcall(doid("$put"), 4,
			   doid("$dom"), doid("$addr"), doid("$type"), 
			   doid("$val"));
		te = Zcons(se, te);

		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Epostinc:
	case Epostdec:
		if(!islval(e->e1)){
			/* FIXME: if we translate ordinary cval ++ here
			   into equivalent source, do we generate same
			   or similar code as compile.c on ++? */
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			e->e1 = compile_rval(ctx, e->e1, 0);
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(ctx, e->e1, 1));
		te = Zcons(se, te);

		se = Zcall(doid("$put"), 4,
			   doid("$dom"), doid("$addr"), doid("$type"),
			   (e->kind == Epostinc
			    ? Zadd(doid("$val"), Zuint(1))
			    : Zsub(doid("$val"), Zuint(1))));
		te = Zcons(se, te);

		se = doid("$val");
		te = Zcons(se, te);

		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Epreinc:
	case Epredec:
		if(!islval(e->e1)){
			/* FIXME: if we translate ordinary cval ++ here
			   into equivalent source, do we generate same
			   or similar code as compile.c on ++? */
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			e->e1 = compile_rval(ctx, e->e1, 0);
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(ctx, e->e1, 1));
		te = Zcons(se, te);

		if(e->kind == Epreinc)
			se = Zadd(doid("$val"), Zuint(1));
		else
			se = Zsub(doid("$val"), Zuint(1));
		se = Zset(doid("$val"), se);
		te = Zcons(se, te);

		se = Zcall(doid("$put"), 4,
			   doid("$dom"), doid("$addr"), doid("$type"),
			   doid("$val"));
		te = Zcons(se, te);

		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Esizeofe:
		if(!islval(e->e1)){
			se = Zsizeof(compile_rval(ctx, e->e1, 0));
			e->e1 = 0;
			freeexpr(e);
			return se;
		}

		te = nullelist();

		se = compile_lval(ctx, e->e1, 0);
		te = Zcons(se, te);

		se = Zsizeof(Zcall(doid("$typeof"), 1, doid("$type")));
		te = Zcons(se, te);
		
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Etypeofe:
		if(!islval(e->e1)){
			se = Zcall(doid("$typeof"), 1,
				   compile_rval(ctx, e->e1, 0));
			e->e1 = 0;
			freeexpr(e);
			return se;
		}

		te = nullelist();

		se = compile_lval(ctx, e->e1, 0);
		te = Zcons(se, te);

		se = Zcall(doid("$typeof"), 1, doid("$type"));
		te = Zcons(se, te);
		
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = compile_rval(ctx, p->e1, 0);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = compile_rval(ctx, e->e1, 0);
		e->e2 = compile_rval(ctx, e->e2, 0);
		e->e3 = compile_rval(ctx, e->e3, 0);
		e->e4 = compile_rval(ctx, e->e4, 0);
		return e;
	}
}

static int
isemptyblock(Expr *e)
{
	if(e->kind == Eblock && e->e2->kind == Enull)
		return 1;
	else
		return 0;
}

static Expr*
groomc(U *ctx, Expr *e)
{
	Expr *p, *se;

	if(e == 0)
		return e;

	switch(e->kind){
	case Eif:
		e->e1 = groomc(ctx, e->e1);
		if(isemptyblock(e->e2)){
			freeexpr(e->e2);
			e->e2 = newexpr(Enop, 0, 0, 0, 0);
		}else
			e->e2 = groomc(ctx, e->e2);
		if(e->e3 && isemptyblock(e->e3)){
			freeexpr(e->e3);
			e->e3 = newexpr(Enop, 0, 0, 0, 0);
		}else
			e->e3 = groomc(ctx, e->e3);
		return e;
	case Eswitch: /* for cg */
		/*
		   switch(E){       { @local $tmp;
		   case V1:     =>    switch($tmp = E){
		      ...             case $tmp==V1:
		   }                      ...
                                      }
                                    }
		*/ 
		se = Zblock(Zlocals(1, "$tmp"),
			   newexpr(Eswitch,
				   Zset(doid("$tmp"), groomc(ctx, e->e1)),
				   groomc(ctx, e->e2), 0, 0),
			   NULL);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ecase:
		se = newexpr(Ecase,
			     Zbinop(Eeq, doid("$tmp"), groomc(ctx, e->e1)),
			     groomc(ctx, e->e2), 0, 0);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ecomma:
		se = Zblock(nullelist(),
			    groomc(ctx, e->e1),
			    groomc(ctx, e->e2),
			    NULL);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Econd:
		se = Zifelse(groomc(ctx, e->e1),
			     groomc(ctx, e->e2),
			     groomc(ctx, e->e3));
		e->e1 = 0;
		e->e2 = 0;
		e->e3 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = groomc(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = groomc(ctx, e->e1);
		e->e2 = groomc(ctx, e->e2);
		e->e3 = groomc(ctx, e->e3);
		e->e4 = groomc(ctx, e->e4);
		return e;
	}
}

static Expr*
compile0(U *ctx, Expr* e)
{
	groomc(ctx, e);
	compile_rval(ctx, e, 0);
	return e;
}

int
docompile0(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return -1;	/* error */
	compile0(ctx, e);
	return 0;
}
