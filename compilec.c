#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

static jmp_buf esc;

static void cerror(Expr *e, char *fmt, ...) __attribute__((noreturn));
static Expr* compile_rval(Expr *e, unsigned lfree);

static void
cerror(Expr *e, char *fmt, ...)
{
	va_list args;
	fprintf(stderr, "%s:%u: ", e->src.filename, e->src.line);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	longjmp(esc, 1);
}

static int
islval(Expr *e)
{
	switch(e->kind){
	case Etick:
		return 1;
	case Ederef:
		return 1;
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
compile_lval(Expr *e, int needaddr)
{
	Expr *se, *te;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Etick:
		te = nullelist();

		// $dom = dom;
		se = Zset(doid("$dom"), e->e1);
		te = Zcons(se, te);

		// $tmp = nslooksym(domns($dom))(sym)
		se = Zcall(doid("domns"), 1, doid("$dom"));
		se = Zcall(doid("nslooksym"), 1, se);
		se = Zcall(se, 1, Zconsts(e->e2->id));
		se = Zset(doid("$tmp"), se);
		te = Zcons(se, te);
		
		// if(isnil($tmp)) error("undefined symbol: %s", sym);
		se = newexpr(Eif,
			     Zcall(doid("isnil"), 1, doid("$tmp")),
			     Zcall(doid("error"), 2,
				   Zconsts("undefined symbol: %s"),
				   Zconsts(e->e2->id)),
			     0, 0);
		te = Zcons(se, te);

		// $type = symtype($tmp);
		se = Zset(doid("$type"),
			  Zcall(doid("symtype"), 1, doid("$tmp")));
		te = Zcons(se, te);

		// $addr = symval($tmp, 2);
		if(needaddr){
			se = Zset(doid("$addr"),
				  Zcall(doid("symval"), 1, doid("$tmp")));
			te = Zcons(se, te);

			// if(isnil($addr)) error("symbol lacks address: %s");
			se = newexpr(Eif,
				     Zcall(doid("isnil"), 1, doid("$addr")),
				     Zcall(doid("error"), 2,
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

		// $tmp = compile_rval(e->e1);
		if(needaddr || !islval(e->e1)){
			se = Zset(doid("$tmp"), compile_rval(e->e1, 0));
			te = Zcons(se, te);

			// $type = subtype($typeof($tmp));
			se = Zset(doid("$type"),
				  Zcall(doid("subtype"), 1,
					Zcall(doid("$typeof"), 1,
					      doid("$tmp"))));
			te = Zcons(se, te);

			// $dom = domof($tmp);
			se = Zset(doid("$dom"),
				  Zcall(doid("domof"), 1, doid("$tmp")));
			te = Zcons(se, te);

			// $addr = {nsptr($dom)}$tmp
			if(needaddr){
				se = Zset(doid("$addr"),
					  Zxcast(Zcall(doid("nsptr"), 1,
						       doid("$dom")),
							doid("$tmp")));
				te = Zcons(se, te);
			}
		}else{
			// compile lvalue reference to pointer,
			// using dom, type bindings
			se = compile_lval(e->e1, 0);
			te = Zcons(se, te);

			// $type = subtype($type);
			se = Zset(doid("$type"),
				  Zcall(doid("subtype"), 1, doid("$type")));
			te = Zcons(se, te);
		}
		e->e1 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	case Edot:
		te = nullelist();

		// compile lvalue reference to containing struct,
		// using dom, type, addr bindings.
		se = compile_lval(e->e1, needaddr);
		te = Zcons(se, te);
		
		// $tmp = lookfield(type, field);
		se = Zset(doid("$tmp"),
			  Zcall(doid("lookfield"), 2,
				doid("$type"), Zconsts(e->e2->id)));
		te = Zcons(se, te);

		// if(isnil($tmp)) error("undefined field: %s", sym);
		se = newexpr(Eif,
			     Zcall(doid("isnil"), 1, doid("$tmp")),
			     Zcall(doid("error"), 2,
				   Zconsts("undefined field: %s"),
				   Zconsts(e->e2->id)),
			     0, 0);
		te = Zcons(se, te);

		// $type = fieldtype($tmp);
		se = Zset(doid("$type"),
			  Zcall(doid("fieldtype"), 1, doid("$tmp")));
		te = Zcons(se, te);
				     
		// $addr = $addr + fieldoff($tmp)
		if(needaddr){
			se = Zset(doid("$addr"),
				  Zadd(doid("$addr"),
				       Zcall(doid("fieldoff"), 1,
					     doid("$tmp"))));
			te = Zcons(se, te);
		}
		
		e->e1 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	default:
		cerror(e, "expression is not an lvalue");
	}
}

static Expr*
compile_rval(Expr *e, unsigned lfree)
{
	Expr *se, *te, *p;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Etick:
	case Edot:
	case Ederef:
		te = nullelist();
		se = compile_lval(e, 1);
		te = Zcons(se, te);
		se = Zcval(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		return rvalblock(invert(te), lfree);
	case Eref:
		te = nullelist();
		se = compile_lval(e->e1, 1);
		te = Zcons(se, te);
		se = Zref(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Eg:
		if(!islval(e->e1)){
			if(e->e1->kind != Eid)
				cerror(e, "invalid assignment");
			e->e1 = compile_rval(e->e1, 0);
			e->e2 = compile_rval(e->e2, 0);
			return e;
		}

		te = nullelist();

		se = compile_lval(e->e1, 1);
		te = Zcons(se, te);

		se = Zset(doid("$val"), compile_rval(e->e2, 0));
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
				cerror(e, "invalid assignment");
			e->e1 = compile_rval(e->e1, 0);
			e->e2 = compile_rval(e->e2, 0);
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(e->e1, 1));
		te = Zcons(se, te);

		se = Zset(doid("$val"),
			  Zbinop(e->op, doid("$val"), compile_rval(e->e2, 0)));
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
				cerror(e, "invalid assignment");
			e->e1 = compile_rval(e->e1, 0);
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(e->e1, 1));
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
				cerror(e, "invalid assignment");
			e->e1 = compile_rval(e->e1, 0);
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(e->e1, 1));
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
			se = Zsizeof(compile_rval(e->e1, 0));
			e->e1 = 0;
			freeexpr(e);
			return se;
		}

		te = nullelist();

		se = compile_lval(e->e1, 0);
		te = Zcons(se, te);

		se = Zsizeof(Zcall(doid("$typeof"), 1, doid("$type")));
		te = Zcons(se, te);
		
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Etypeofe:
		if(!islval(e->e1)){
			se = Zcall(doid("$typeof"), 1, compile_rval(e->e1, 0));
			e->e1 = 0;
			freeexpr(e);
			return se;
		}

		te = nullelist();

		se = compile_lval(e->e1, 0);
		te = Zcons(se, te);

		se = Zcall(doid("$typeof"), 1, doid("$type"));
		te = Zcons(se, te);
		
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te), lfree);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = compile_rval(p->e1, 0);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = compile_rval(e->e1, 0);
		e->e2 = compile_rval(e->e2, 0);
		e->e3 = compile_rval(e->e3, 0);
		e->e4 = compile_rval(e->e4, 0);
		return e;
	}
}

/* expand syntax of various C forms for subsequent stages */
static Expr*
expandc(Expr *e)
{
	Expr *se, *p;

	if(e == 0)
		return e;
	switch(e->kind){
	case Earef: /* for compile_rval */
		/* rewrite: E1[E2] => *(E1+E2) */
		se = newexpr(Ederef, 
			     Zadd(expandc(e->e1), expandc(e->e2)),
			     0, 0, 0);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Earrow: /* for compile_rval */
		/* rewrite: E->field => (*E).field */
		se = newexpr(Edot,
			     newexpr(Ederef, expandc(e->e1), 0, 0, 0),
			     e->e2, 0, 0);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eswitch: /* for cg */
		/*
		   switch(E){       { @local $tmp;
		   case V1:     =>    switch($tmp == E){
		      ...             case $tmp==V1:
		   }                      ...
                                      }
                                    }
		*/ 
		se = Zblock(Zlocals(1, "$tmp"),
			   newexpr(Eswitch,
				   Zset(doid("$tmp"), expandc(e->e1)),
				   expandc(e->e2), 0, 0),
			   NULL);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ecase:
		se = newexpr(Ecase,
			     Zbinop(Eeq, doid("$tmp"), expandc(e->e1)),
			     expandc(e->e2), 0, 0);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Ecomma:
		se = Zblock(nullelist(),
			    expandc(e->e1),
			    expandc(e->e2),
			    NULL);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = expandc(p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = expandc(e->e1);
		e->e2 = expandc(e->e2);
		e->e3 = expandc(e->e3);
		e->e4 = expandc(e->e4);
		return e;
	}
}

static Expr*
expanddot(Expr *e)
{
	Expr *te, *p;
	char *id;

	if(e == 0)
		return e;
	switch(e->kind){
	case Edot:
		if(islval(e->e1))
			return e;
		id = e->e2->id;
		if(!strcmp(id, "dispatch"))
			te = Zcall(doid("asdispatch"), 1, expanddot(e->e1));
		else if(!strcmp(id, "names"))
			te = Zcall(doid("domns"), 1, expanddot(e->e1));
		else if(!strcmp(id, "as"))
			te = Zcall(doid("domas"), 1, expanddot(e->e1));
		else
			te = Zblock(Zlocals(1, "$disp"),
				    Zset(doid("$disp"),
					 Zcall(doid("asdispatch"), 1,
					       expanddot(e->e1))),
				    Zlambdn(doid("$args"),
					    Zblock(nullelist(),
						   Zret(Zcall(doid("apply"), 3,
							      doid("$disp"),
							      Zconsts(id),
							      doid("$args"))),
						   NULL),
					    copyexpr(e->e2)),
				    NULL);
		e->e1 = 0;
		freeexpr(e);
		return te;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = expanddot(p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = expanddot(e->e1);
		e->e2 = expanddot(e->e2);
		e->e3 = expanddot(e->e3);
		e->e4 = expanddot(e->e4);
		return e;
	}
}

Expr*
docompilec(Expr *e)
{
	Expr *rv;
	if(setjmp(esc) != 0)
		return 0;	/* error */
	e = expandc(e);
	e = expanddot(e);
	rv = compile_rval(e, 0);
	return rv;
}
