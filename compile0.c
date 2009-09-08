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
	Expr *e, *te;
	if(lfree)
		e = Zlocals(1, "$val");
	else
		e = Zlocals(4, "$val", "$dom", "$type", "$addr");
	te = newexpr(Eblock, e, body, 0, 0);
	putsrc(te, &body->src);
	return te;
}

static Expr*
lvalblock(Expr *body)
{
	Expr *e, *te;
	e = Zlocals(1, "$tmp");
	te = newexpr(Eblock, e, body, 0, 0);
	putsrc(te, &body->src);
	return te;
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
			    Zset(doid("$tn"), gentypename(t, compile0, ctx, 0)),
			    Zset(doid("$type"),
				 Zcall(G("looktype"), 2,
				       dom, doid("$tn"))),
			    Zif(Zcall(G("isnil"), 1, doid("$type")),
				Zcall(G("error"), 2,
				      Zconsts("undefined type: %t"),
				      doid("$tn"))),
			    NULL);
		te = Zcons(se, te);
		putsrc(te, &e->src);
		e->e2 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	case Etick:
		te = nullelist();

		// $dom = dom;
		se = Zset(doid("$dom"), e->e1);
		te = Zcons(se, te);

		// $tmp = looksym($dom,sym)
		se = Zset(doid("$tmp"),
			  Zcall(G("looksym"), 2,
				doid("$dom"), Zconsts(e->e2->id)));
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

		putsrc(te, &e->src);
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
		putsrc(te, &e->src);
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
		
		putsrc(te, &e->src);
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
	Src src;

	if(e == 0)
		return 0;

	src = e->src;
	switch(e->kind){
	case Etick:
	case Edot:
	case Ederef:
		te = nullelist();
		se = compile_lval(ctx, e, 1);
		te = Zcons(se, te);
		se = Zcval(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		te = invert(te);
		putsrc(te, &src);
		return rvalblock(te, lfree);
	case Eref:
		te = nullelist();
		se = compile_lval(ctx, e->e1, 1);
		te = Zcons(se, te);
		se = Zref(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		te = invert(te);
		putsrc(te, &src);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(te, lfree);
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

		te = invert(te);
		putsrc(te, &src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return rvalblock(te, lfree);
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

		te = invert(te);
		putsrc(te, &src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return rvalblock(te, lfree);
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

		te = invert(te);
		putsrc(te, &src);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(te, lfree);
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

		te = invert(te);
		putsrc(te, &src);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(te, lfree);
	case Esizeofe:
		if(!islval(e->e1)){
			se = Zsizeof(compile_rval(ctx, e->e1, 0));
			putsrc(se, &src);
			e->e1 = 0;
			freeexpr(e);
			return se;
		}

		te = nullelist();

		se = compile_lval(ctx, e->e1, 0);
		te = Zcons(se, te);

		se = Zsizeof(Zcall(doid("$typeof"), 1, doid("$type")));
		te = Zcons(se, te);
		
		te = invert(te);
		putsrc(te, &src);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(te, lfree);
	case Etypeofe:
		if(!islval(e->e1)){
			se = Zcall(doid("$typeof"), 1,
				   compile_rval(ctx, e->e1, 0));
			putsrc(se, &src);
			e->e1 = 0;
			freeexpr(e);
			return se;
		}

		te = nullelist();

		se = compile_lval(ctx, e->e1, 0);
		te = Zcons(se, te);

		se = Zcall(doid("$typeof"), 1, doid("$type"));
		te = Zcons(se, te);
		
		te = invert(te);
		putsrc(te, &src);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(te, lfree);
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
			e->e2 = newexpr(Enil, 0, 0, 0, 0);
		}else
			e->e2 = groomc(ctx, e->e2);
		if(e->e3 && isemptyblock(e->e3)){
			freeexpr(e->e3);
			e->e3 = newexpr(Enil, 0, 0, 0, 0);
		}else
			e->e3 = groomc(ctx, e->e3);
		putsrc(e, &e->src);
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
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ecase:
		se = newexpr(Ecase,
			     Zbinop(Eeq, doid("$tmp"), groomc(ctx, e->e1)),
			     groomc(ctx, e->e2), 0, 0);
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ecomma:
		se = Zblock(nullelist(),
			    groomc(ctx, e->e1),
			    groomc(ctx, e->e2),
			    NULL);
		putsrc(se, &e->src);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Econd:
		se = Zifelse(groomc(ctx, e->e1),
			     groomc(ctx, e->e2),
			     groomc(ctx, e->e3));
		putsrc(se, &e->src);
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

static void checkgoto(U *ctx, Expr *e);
enum {
	Unusedlabel = 1,
	Usedlabel,
};

static void
labels(U *ctx, Expr *e, HT *ls)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
	case Edefine:
		return;
	case Elabel:
		id = e->e1->id;
		if(hget(ls, id, strlen(id)))
			cposterror(ctx, e, "duplicate label: %s", id);
		else{
			e->e1->attr = Unusedlabel;
			hput(ls, id, strlen(id), e->e1);
		}
		labels(ctx, e->e2, ls);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			labels(ctx, p->e1, ls);
			p = p->e2;
		}
		break;
	default:
		labels(ctx, e->e1, ls);
		labels(ctx, e->e2, ls);
		labels(ctx, e->e3, ls);
		labels(ctx, e->e4, ls);
		break;
	}
}

static void
reccheckgoto(U *ctx, Expr *e, HT *ls)
{
	char *id;
	Expr *p;
	void *q;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
		checkgoto(ctx, e->e2);
		break;
	case Edefine:
		checkgoto(ctx, e->e3);
		break;
	case Egoto:
		id = e->e1->id;
		q = hget(ls, id, strlen(id));
		if(q == 0)
			cposterror(ctx, e, "undefined label: %s", id);
		else{
			p = q;
			p->attr = Usedlabel;
		}
		reccheckgoto(ctx, e->e2, ls);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			reccheckgoto(ctx, p->e1, ls);
			p = p->e2;
		}
		break;
	default:
		reccheckgoto(ctx, e->e1, ls);
		reccheckgoto(ctx, e->e2, ls);
		reccheckgoto(ctx, e->e3, ls);
		reccheckgoto(ctx, e->e4, ls);
		break;
	}
}

static void
check1label(void *u, char *k, void *q)
{
	Expr *p;
	p = q;
	if(p->attr == Unusedlabel)
		cposterror((U*)u, p, "unused label: %s", p->id);
}

static void
checkgoto(U *ctx, Expr *e)
{
	HT *ls;

	ls = mkht();
	labels(ctx, e, ls);
	reccheckgoto(ctx, e, ls);
	hforeach(ls, check1label, ctx);
	freeht(ls);
}

// checkctl: check that break and continue statements occur only within
// control structures
static void
checkctl(U *ctx, Expr *e, unsigned inloop, unsigned inswitch)
{
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Eswitch:
		checkctl(ctx, e->e1, inloop, inswitch);
		checkctl(ctx, e->e3, inloop, 1);
		break;
	case Efor:
		checkctl(ctx, e->e1, inloop, inswitch);
		checkctl(ctx, e->e2, inloop, inswitch);
		checkctl(ctx, e->e3, inloop, inswitch);
		checkctl(ctx, e->e4, 1, inswitch);
		break;
	case Ewhile:
		checkctl(ctx, e->e1, inloop, inswitch);
		checkctl(ctx, e->e2, 1, inswitch);
		break;
	case Edo:
		checkctl(ctx, e->e1, 1, inswitch);
		checkctl(ctx, e->e2, inloop, inswitch);
		break;
	case Elambda:
		checkctl(ctx, e->e2, 0, 0);
		break;
	case Edefine:
		checkctl(ctx, e->e3, 0, 0);
		checkctl(ctx, e->e4, 0, 0);
		break;
	case Econtinue:
		if(inloop == 0)
			cerror(ctx, e, "continue not within loop");
		break;
	case Ebreak:
		if(inloop == 0 && inswitch == 0)
			cerror(ctx, e, "break not within loop or switch");
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			checkctl(ctx, p->e1, inloop, inswitch);
			p = p->e2;
		}
		break;
	default:
		checkctl(ctx, e->e1, inloop, inswitch);
		checkctl(ctx, e->e2, inloop, inswitch);
		checkctl(ctx, e->e3, inloop, inswitch);
		checkctl(ctx, e->e4, inloop, inswitch);
		break;
	}
}

static Expr*
compile0(U *ctx, Expr* e)
{
	checkctl(ctx, e, 0, 0);
	checkgoto(ctx, e);
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
	return ctx->errors;
}
