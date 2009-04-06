#include "sys.h"
#include "util.h"
#include "syscqct.h"

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
expandaref(U *ctx, Expr *e)
{
	Expr *p, *te, *a, *i;

	if(e == 0)
		return e;
	/* FIXME: unify with expanddot */
	switch(e->kind){
	case Eref:
	case Edot:
		if(e->e1->kind == Earef){
			/* E->E1 must be a lvalue, so don't expand it
			   (but do expand its children) */
			e->e1->e1 = expandaref(ctx, e->e1->e1);
			e->e1->e2 = expandaref(ctx, e->e1->e2);
		}else
			e->e1 = expandaref(ctx, e->e1);
		return e;
	case Eg:
		e->e2 = expandaref(ctx, e->e2);
		if(e->e1->kind != Earef)
			return e;
		e->e1->e2 = expandaref(ctx, e->e1->e2);
		if(islval(e->e1->e1))
			return e;
		a = expandaref(ctx, e->e1->e1);
		i = e->e1->e2;
		e->e1->e1 = doid("$a");
		e->e1->e2 = doid("$i");
		te = Zblock(Zlocals(2, "$a", "$i"),
			    Zset(doid("$a"), a),
			    Zset(doid("$i"), i),
			    Zifelse(Zcall(doid("%iscvalue"), 1, doid("$a")),
				    e,
				    Zcall(doid("%cntrput"), 3,
					  doid("$a"),
					  doid("$i"),
					  copyexpr(e->e2))),
			    NULL);
		putsrc(te, &e->src);
		return te;
	case Egop:
		e->e2 = expandaref(ctx, e->e2);
		if(e->e1->kind != Earef)
			return e;
		e->e1->e2 = expandaref(ctx, e->e1->e2);
		if(islval(e->e1->e1))
			return e;
		a = expandaref(ctx, e->e1->e1);
		i = e->e1->e2;
		e->e1->e1 = doid("$a");
		e->e1->e2 = doid("$i");
		te = Zblock(Zlocals(2, "$a", "$i"),
			    Zset(doid("$a"), a),
			    Zset(doid("$i"), i),
			    Zifelse(Zcall(doid("%iscvalue"), 1, doid("$a")),
				    e,
				    Zcall(doid("%cntrput"), 3,
					  doid("$a"),
					  doid("$i"),
					  Zbinop(e->op,
						 Zcall(doid("%cntrget"), 2,
						       doid("$a"), doid("$i")),
						 copyexpr(e->e2)))),
			    NULL);
		putsrc(te, &e->src);
		return te;
	case Epreinc:
	case Epredec:
		if(e->e1->kind != Earef)
			return e;
		e->e1->e2 = expandaref(ctx, e->e1->e2);
		if(islval(e->e1->e1))
			return e;
		a = expandaref(ctx, e->e1->e1);
		i = e->e1->e2;
		e->e1->e1 = doid("$a");
		e->e1->e2 = doid("$i");
		te = Zblock(Zlocals(2, "$a", "$i"),
			    Zset(doid("$a"), a),
			    Zset(doid("$i"), i),
			    Zifelse(Zcall(doid("%iscvalue"), 1, doid("$a")),
				    e,
				    Zcall(doid("%cntrput"), 3,
					  doid("$a"),
					  doid("$i"),
					  Zbinop(e->kind == Epreinc ?
						 Eadd : Esub,
						 Zcall(doid("%cntrget"), 2,
						       doid("$a"), doid("$i")),
						 Zuint(1)))),
			    NULL);
		putsrc(te, &e->src);
		return te;
	case Epostinc:
	case Epostdec:
		if(e->e1->kind != Earef)
			return e;
		e->e1->e2 = expandaref(ctx, e->e1->e2);
		if(islval(e->e1->e1))
			return e;
		a = expandaref(ctx, e->e1->e1);
		i = e->e1->e2;
		e->e1->e1 = doid("$a");
		e->e1->e2 = doid("$i");
		te = Zblock(Zlocals(2, "$a", "$i"),
			    Zset(doid("$a"), a),
			    Zset(doid("$i"), i),
			    Zifelse(Zcall(doid("%iscvalue"), 1, doid("$a")),
				    e,
				    Zblock(Zlocals(1, "$l"),
					   Zset(doid("$l"),
						Zcall(doid("%cntrget"), 2,
						      doid("$a"), doid("$i"))),
					   Zcall(doid("%cntrput"), 3,
						 doid("$a"),
						 doid("$i"),
						 Zbinop(e->kind == Epostinc ?
							Eadd : Esub,
							doid("$l"),
							Zuint(1))),
					   doid("$l"),
					   NULL)),
			    NULL);
		putsrc(te, &e->src);
		return te;
	case Earef:
		e->e2 = expandaref(ctx, e->e2);
		if(islval(e->e1))
			return e;
		a = expandaref(ctx, e->e1);
		i = e->e2;
		e->e1 = doid("$a");
		e->e2 = doid("$i");
		te = Zblock(Zlocals(2, "$a", "$i"),
			    Zset(doid("$a"), a),
			    Zset(doid("$i"), i),
			    Zifelse(Zcall(doid("%iscvalue"), 1, doid("$a")),
				    e,
				    Zcall(doid("%cntrget"), 2,
					  doid("$a"), doid("$i"))),
			    NULL);
		putsrc(te, &e->src);
		return te;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = expandaref(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = expandaref(ctx, e->e1);
		e->e2 = expandaref(ctx, e->e2);
		e->e3 = expandaref(ctx, e->e3);
		e->e4 = expandaref(ctx, e->e4);
		return e;
	}
}


/* expand syntax of various C forms for subsequent stages */
static Expr*
expandc(U *ctx, Expr *e)
{
	Expr *se, *p;

	if(e == 0)
		return e;
	switch(e->kind){
	case Earef: /* for compile_rval */
		/* rewrite: E1[E2] => *(E1+E2) */
		se = newexpr(Ederef, 
			     Zadd(expandc(ctx, e->e1), expandc(ctx, e->e2)),
			     0, 0, 0);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Earrow: /* for compile_rval */
		/* rewrite: E->field => (*E).field */
		se = newexpr(Edot,
			     newexpr(Ederef, expandc(ctx, e->e1), 0, 0, 0),
			     e->e2, 0, 0);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = expandc(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = expandc(ctx, e->e1);
		e->e2 = expandc(ctx, e->e2);
		e->e3 = expandc(ctx, e->e3);
		e->e4 = expandc(ctx, e->e4);
		return e;
	}
}

static Expr*
expanddot(U *ctx, Expr *e)
{
	Expr *te, *se, *o, *p;
	char *id;

	if(e == 0)
		return e;
	switch(e->kind){
	case Eg:
		e->e2 = expanddot(ctx, e->e2);
		if(e->e1->kind != Edot)
			return e;
		if(islval(e->e1->e1))
			return e;
		id = e->e1->e2->id;
		te = Zblock(Zlocals(1, "$o"),
			    Zset(doid("$o"), expanddot(ctx, e->e1->e1)),
			    Zcall(Zcall(doid("%tablook"), 2,
					Zcall(doid("%rdsettab"), 1,
					      Zcall(doid("%recrd"), 1,
						    doid("$o"))),
					Zconsts(id)), 2,
				  doid("$o"), e->e2),
			    NULL);
		e->e1->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return te;
	case Egop:
		e->e2 = expanddot(ctx, e->e2);
		if(e->e1->kind != Edot)
			return e;
		if(islval(e->e1->e1))
			return e;
		id = e->e1->e2->id;
		te = Zblock(Zlocals(2, "$o", "$rd"),
			    Zset(doid("$o"), expanddot(ctx, e->e1->e1)),
			    Zset(doid("$rd"), Zcall(doid("%recrd"), 1,
						    doid("$o"))),
			    Zcall(Zcall(doid("%tablook"), 2,
					Zcall(doid("%rdsettab"),
					      1, doid("$rd")),
					Zconsts(id)), 2,
				  doid("$o"),
				  Zbinop(e->op,
					 Zcall(Zcall(doid("%tablook"), 2,
						     Zcall(doid("%rdgettab"),
							   1, doid("$rd")),
						     Zconsts(id)), 1,
					       doid("$o")),
					 e->e2)),
			    NULL);
		e->e1->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return te;
	case Epreinc:
	case Epredec:
		if(e->e1->kind != Edot)
			return e;
		if(islval(e->e1->e1))
			return e;
		id = e->e1->e2->id;
		te = Zblock(Zlocals(2, "$o", "$rd"),
			    Zset(doid("$o"), expanddot(ctx, e->e1->e1)),
			    Zset(doid("$rd"), Zcall(doid("%recrd"), 1,
						    doid("$o"))),
			    Zcall(Zcall(doid("%tablook"), 2,
					Zcall(doid("%rdsettab"),
					      1, doid("$rd")),
					Zconsts(id)), 2,
				  doid("$o"),
				  Zbinop(e->kind == Epreinc ? Eadd : Esub,
					 Zcall(Zcall(doid("%tablook"), 2,
						     Zcall(doid("%rdgettab"),
							   1, doid("$rd")),
						     Zconsts(id)), 1,
					       doid("$o")),
					 Zuint(1))),
			    NULL);
		e->e1->e1 = 0;
		freeexpr(e);
		return te;
	case Epostinc:
	case Epostdec:
		if(e->e1->kind != Edot)
			return e;
		if(islval(e->e1->e1))
			return e;
		id = e->e1->e2->id;
		te = Zblock(Zlocals(3, "$o", "$rd", "$l"),
			    Zset(doid("$o"), expanddot(ctx, e->e1->e1)),
			    Zset(doid("$rd"), Zcall(doid("%recrd"), 1,
						    doid("$o"))),
			    Zset(doid("$l"),
				 Zcall(Zcall(doid("%tablook"), 2,
					     Zcall(doid("%rdgettab"), 1,
						   doid("$rd")),
					     Zconsts(id)), 1,
				       doid("$o"))),
			    Zcall(Zcall(doid("%tablook"), 2,
					Zcall(doid("%rdsettab"),
					      1, doid("$rd")),
					Zconsts(id)), 2,
				  doid("$o"),
				  Zbinop(e->kind == Epostinc ? Eadd : Esub,
					 doid("$l"), Zuint(1))),
			    doid("$l"),
			    NULL);
		e->e1->e1 = 0;
		freeexpr(e);
		return te;
	case Edot:
		if(islval(e->e1))
			return e;

		id = e->e2->id; 
		o = Zset(doid("$o"), expanddot(ctx, e->e1));

		/* record accessor case */
		se = Zcall(Zcall(doid("%tablook"), 2,
				 Zcall(doid("%rdgettab"), 1,
				       Zcall(doid("%recrd"), 1, doid("$o"))),
				 Zconsts(id)),
			   1, doid("$o"));

		/* cval/as/ns/dom case */
		if(!strcmp(id, "ns"))
			te = Zcall(doid("%nsof"), 1, doid("$o"));
		else if(!strcmp(id, "as"))
			te = Zcall(doid("%asof"), 1, doid("$o"));
		else
			te = Zlambdn(doid("$args"),
				     Zret(Zcall(doid("%callmethod"),
						3,
						doid("$o"),
						Zconsts(id),
						doid("$args"))),
				     copyexpr(e->e2));

		te = Zblock(Zlocals(1, "$o"),
			    Zifelse(Zcall(doid("%isrec"), 1, o), se, te),
			    NULL);
		e->e1 = 0;
		freeexpr(e);
		return te;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = expanddot(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = expanddot(ctx, e->e1);
		e->e2 = expanddot(ctx, e->e2);
		e->e3 = expanddot(ctx, e->e3);
		e->e4 = expanddot(ctx, e->e4);
		return e;
	}
}

static Expr*
compilea(U *ctx, Expr* e)
{
	expandaref(ctx, e);
	expandc(ctx, e);
	expanddot(ctx, e);
	return e;
}

int
docompilea(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return -1;	/* error */
	compilea(ctx, e);
	return 0;
}
