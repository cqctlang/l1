#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
enum Attr {
	Alval		= 1,
	Aptr		= Alval<<1,
	Anotlval 	= Aptr<<1
} Attr;

static Expr* expanda(U *ctx, Expr *e, unsigned d, unsigned *w);

static Attr
islval(Expr *e, Expr **a)
{
	int x;
	switch(e->kind){
	case Eticke:
		return Alval;
	case Ederef:
		return Alval;
	case Earef:
		/* FIXME: check both operands? eg for e1[dom`x] and e1[x]. */
		x = islval(e->e1, a);
		if(x == Aptr)
			return Alval;
		if(x)
			return x;
		if(*a)
			return 0;
		*a = e;
		return 0;
	case Ecast:
		return islval(e->e2, a);
	case Earrow:
	case Edot:
		x = islval(e->e1, a);
		if(x == Aptr)
			return Alval;
		return x;
	default:
		if(e->attr&Aptr)
			return Aptr;
		if(e->attr&Anotlval){
			*a = 0;
			return 0;
		}
		/* we need to know the run-time *value* of E */
		*a = e;
		return 0;
	}
}

static Expr*
Zap(Expr *e, char *id)
{
	Expr *ne;

	ne = newexpr(0, 0, 0, 0, 0);
	*ne = *e;

	e->kind = Eid;
	setaux(e, mkvalcid(mkcid(id, strlen(id))));
	setskind(e, mkcid0(S[Eid]));
	e->e1 = 0;
	e->e2 = 0;
	e->e3 = 0;
	e->e4 = 0;
	return ne;
}

static void
clearattr(Expr *e)
{
	Expr *p;

	if(e == 0)
		return;
	e->attr = 0;
	switch(e->kind){
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			clearattr(e->e1);
			p = p->e2;
		}
		break;
	default:
		clearattr(e->e1);
		clearattr(e->e2);
		clearattr(e->e3);
		clearattr(e->e4);
		break;
	}
}

/* assignment to c lvalue */
static Expr*
cassign(U *ctx, Expr *e, unsigned d, unsigned *w)
{
	sete1(e->e1, expanda(ctx, e->e1->e1, d, w));
	sete2(e->e1, expanda(ctx, e->e1->e2, d, w));
	sete2(e, expanda(ctx, e->e2, d, w));
	return e;
}

/* disambiguate [] or . expression  */
static Expr*
disambig(U *ctx, Expr *a, Expr *e, unsigned d)
{
	char t[8];
	Expr *p, *te, *ye, *ne, *xe;
	unsigned w;

	w = 0;
	snprint(t, sizeof(t), "$t%d", d);
	p = expanda(ctx, Zap(a, t), d+1, 0);
	a->attr = Aptr;
	xe = copyexpr(e);
	ye = expanda(ctx, xe, d+1, &w);
	if(w)
		ye = Zcall(G("error"), 1,
			   Zstr("cannot apply dot to a cvalue"));
	a->attr = Anotlval;
	xe = copyexpr(e);
	ne = expanda(ctx, xe, d+1, 0);
	if(ne == 0)
		fatal("bug");

	te = Zblock(Zlocals(1, t),
		    Zset(doid(t), p),
		    Zifelse(Zcall(G("iscvalue"), 1,
				  doid(t)),
			    ye, ne),
		    NULL);
	putsrc(te, e->src);
	clearattr(te);
	return te;
}

static Expr*
disambig0(U *ctx, Expr *a, Expr *e, unsigned d)
{
	char t[8];
	Expr *p, *te, *ye, *xe;
	unsigned w;

	w = 0;
	snprint(t, sizeof(t), "$t%d", d);
	p = expanda(ctx, Zap(a, t), d+1, 0);
	a->attr = Aptr;
	xe = copyexpr(e);
	ye = expanda(ctx, xe, d+1, &w);
	if(w)
		return Zcall(G("error"), 1,
			     Zstr("cannot apply dot to a cvalue"));
	te = Zblock(Zlocals(1, t),
		    Zset(doid(t), p),
		    Zifelse(Zcall(G("iscvalue"), 1,
				  doid(t)),
			    ye,
			    Zcall(G("error"), 1,
				  Zstr("attempt to apply & to non-lvalue"))),
		    NULL);
	putsrc(te, e->src);
	clearattr(te);
	return te;
}

static Expr*
expandaref(U *ctx, Expr *e, unsigned d, unsigned *w)
{
	Expr *a, *te;

	/* recognize:
	   e1[e2]
	   e1[e2] = e3
	   e1[e2] ?= e3
	   (e1[e2])++, (e1[e2])--, ++(e1[e2]), --(e1[e2])
	*/
	switch(e->kind){
	case Earef:
		if(islval(e->e1, &a)){
			/* either lval or ptr; C aref */
			sete1(e, expanda(ctx, e->e1, d, w));
			sete2(e, expanda(ctx, e->e2, d, w));
			return e;
		}else if(a)
			return disambig(ctx, a, e, d);
		/* definitely a container ref */
		te = Zcall(G("cntrget"), 2,
			   expanda(ctx, e->e1, d, w),
			   expanda(ctx, e->e2, d, w));
		putsrc(te, e->src);
		return te;
	case Eg:
		if(e->e1->kind != Earef)
			fatal("bug");
		if(islval(e->e1->e1, &a))
			return cassign(ctx, e, d, w);
		else if(a)
			return disambig(ctx, a, e, d);
		te = Zcall(G("cntrput"), 3,
			   expanda(ctx, e->e1->e1, d, w),
			   expanda(ctx, e->e1->e2, d, w),
			   expanda(ctx, e->e2, d, w));
		putsrc(te, e->src);
		return te;
	case EGOP:
		if(e->e1->kind != Earef)
			fatal("bug");
		if(islval(e->e1->e1, &a))
			return cassign(ctx, e, d, w);
		else if(a)
			return disambig(ctx, a, e, d);
		te = Zblock(Zlocals(2, "$a", "$i"),
			    Zset(doid("$a"),
				 expanda(ctx, e->e1->e1, d, w)),
			    Zset(doid("$i"),
				 expanda(ctx, e->e1->e2, d, w)),
			    Zcall(G("cntrput"), 3,
				  doid("$a"),
				  doid("$i"),
				  Zgbinop(e->kind,
					  Zcall(G("cntrget"), 2,
						doid("$a"), doid("$i")),
					  expanda(ctx, e->e2, d, w))),
			    NULL);
		putsrc(te, e->src);
		return te;
	case Epreinc:
	case Epredec:
		if(e->e1->kind != Earef)
			fatal("bug");
		if(islval(e->e1->e1, &a))
			return cassign(ctx, e, d, w);
		else if(a)
			return disambig(ctx, a, e, d);
		te = Zblock(Zlocals(2, "$a", "$i"),
			    Zset(doid("$a"),
				 expanda(ctx, e->e1->e1, d, w)),
			    Zset(doid("$i"),
				 expanda(ctx, e->e1->e2, d, w)),
			    Zcall(G("cntrput"), 3,
				  doid("$a"),
				  doid("$i"),
				  Zbinop(e->kind == Epreinc ? Eadd : Esub,
					 Zcall(G("cntrget"), 2,
					       doid("$a"), doid("$i")),
					 Zint(1))),
			    NULL);
		putsrc(te, e->src);
		return te;
	case Epostinc:
	case Epostdec:
		if(e->e1->kind != Earef)
			fatal("bug");
		if(islval(e->e1->e1, &a))
			return cassign(ctx, e, d, w);
		else if(a)
			return disambig(ctx, a, e, d);
		te = Zblock(Zlocals(3, "$a", "$i", "$l"),
			    Zset(doid("$a"),
				 expanda(ctx, e->e1->e1, d, w)),
			    Zset(doid("$i"),
				 expanda(ctx, e->e1->e2, d, w)),
			    Zset(doid("$l"),
				 Zcall(G("cntrget"), 2,
				       doid("$a"), doid("$i"))),
			    Zcall(G("cntrput"), 3,
				  doid("$a"),
				  doid("$i"),
				  Zbinop(e->kind == Epostinc ? Eadd : Esub,
					 doid("$l"), Zint(1))),
			    doid("$l"),
			    NULL);
		putsrc(te, e->src);
		return te;
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Expr*
expanddot(U *ctx, Expr *e, unsigned d, unsigned *w)
{
	Expr *a, *o, *se, *te;
	char *id;
	Attr x;

	/* recognize:
	   e1.f
	   e1.f = e3
	   e1.f ?= e3
	   (e1.f)++, (e1.f)--, ++(e1.f), --(e1.f)
	*/
	switch(e->kind){
	case Edot:
		x = islval(e->e1, &a);
		if(x == Alval){
			/* definitely an lval; C dot */
			sete1(e, expanda(ctx, e->e1, d, w));
			return e;
		}
		if(x == Aptr){
			if(w == 0)
				fatal("bug");
			*w = 1;
			return Zcall(G("error"), 1,
				     Zstr("cannot apply dot to a cvalue"));
		}
		if(a)
			return disambig(ctx, a, e, d);

		id = idsym(e->e2);
		o = Zset(doid("$o"), expanda(ctx, e->e1, d, w));

		/* record accessor case */
		se = Zcall(Zcall(G("tablook"), 2,
				 Zcall(G("rdgettab"), 1,
				       Zcall(G("rdof"), 1, doid("$o"))),
				 Zid2sym(e->e2)),
			   1, doid("$o"));

		/* cval/as/ns/dom case */
		if(!strcmp(id, "ns"))
			te = Zcall(G("nsof"), 1, doid("$o"));
		else if(!strcmp(id, "as"))
			te = Zcall(G("asof"), 1, doid("$o"));
		else
			te = Zlambdn(Zvararg(doid("$args")),
				     Zblock(nullelist(),
					    Zret(Zcall(G("callmethod"),
						       3,
						       doid("$o"),
						       Zstr(id),
						       doid("$args"))),
					    NULL),
				     copyexpr(e->e2));
		te = Zblock(Zlocals(1, "$o"),
			    Zifelse(Zcall(G("isrec"), 1, o), se, te),
			    NULL);
		putsrc(te, e->src);
		return te;
	case Eg:
		if(e->e1->kind != Edot)
			fatal("bug");
		x = islval(e->e1->e1, &a);
		if(x == Alval)
			return cassign(ctx, e, d, w);
		if(x == Aptr){
			if(w == 0)
				fatal("bug");
			*w = 1;
			return Zcall(G("error"), 1,
				     Zstr("cannot apply dot to a cvalue"));
		}
		if(a)
			return disambig(ctx, a, e, d);
		te = Zblock(Zlocals(1, "$o"),
			    Zset(doid("$o"), expanda(ctx, e->e1->e1, d, w)),
			    Zcall(Zcall(G("tablook"), 2,
					Zcall(G("rdsettab"), 1,
					      Zcall(G("rdof"), 1,
						    doid("$o"))),
					Zid2sym(e->e1->e2)), 2,
				  doid("$o"), expanda(ctx, e->e2, d, w)),
			    NULL);
		putsrc(te, e->src);
		return te;
	case EGOP:
		if(e->e1->kind != Edot)
			fatal("bug");
		x = islval(e->e1->e1, &a);
		if(x == Alval)
			return cassign(ctx, e, d, w);
		if(x == Aptr){
			if(w == 0)
				fatal("bug");
			*w = 1;
			return Zcall(G("error"), 1,
				     Zstr("cannot apply dot to a cvalue"));
		}
		if(a)
			return disambig(ctx, a, e, d);
		id = idsym(e->e1->e2);
		te = Zblock(Zlocals(2, "$o", "$rd"),
			    Zset(doid("$o"), expanda(ctx, e->e1->e1, d, w)),
			    Zset(doid("$rd"), Zcall(G("rdof"), 1,
						    doid("$o"))),
			    Zcall(Zcall(G("tablook"), 2,
					Zcall(G("rdsettab"),
					      1, doid("$rd")),
					Zid2sym(e->e1->e2)), 2,
				  doid("$o"),
				  Zgbinop(e->kind,
					  Zcall(Zcall(G("tablook"), 2,
						      Zcall(G("rdgettab"),
							    1, doid("$rd")),
						      Zid2sym(e->e1->e2)), 1,
						doid("$o")),
					  expanda(ctx, e->e2, d, w))),
			    NULL);
		putsrc(te, e->src);
		return te;
	case Epreinc:
	case Epredec:
		if(e->e1->kind != Edot)
			fatal("bug");
		x = islval(e->e1->e1, &a);
		if(x == Alval)
			return cassign(ctx, e, d, w);
		if(x == Aptr){
			if(w == 0)
				fatal("bug");
			*w = 1;
			return Zcall(G("error"), 1,
				     Zstr("cannot apply dot to a cvalue"));
		}
		if(a)
			return disambig(ctx, a, e, d);
		te = Zblock(Zlocals(2, "$o", "$rd"),
			    Zset(doid("$o"), expanda(ctx, e->e1->e1, d, w)),
			    Zset(doid("$rd"), Zcall(G("rdof"), 1,
						    doid("$o"))),
			    Zcall(Zcall(G("tablook"), 2,
					Zcall(G("rdsettab"),
					      1, doid("$rd")),
					Zid2sym(e->e1->e2)), 2,
				  doid("$o"),
				  Zbinop(e->kind == Epreinc ? Eadd : Esub,
					 Zcall(Zcall(G("tablook"), 2,
						     Zcall(G("rdgettab"),
							   1, doid("$rd")),
						     Zid2sym(e->e1->e2)), 1,
					       doid("$o")),
					 Zint(1))),
			    NULL);
		putsrc(te, e->src);
		e->e1->e1 = 0;
		return te;
	case Epostinc:
	case Epostdec:
		if(e->e1->kind != Edot)
			fatal("bug");
		x = islval(e->e1->e1, &a);
		if(x == Alval)
			return cassign(ctx, e, d, w);
		if(x == Aptr){
			if(w == 0)
				fatal("bug");
			*w = 1;
			return Zcall(G("error"), 1,
				     Zstr("cannot apply dot to a cvalue"));
		}
		if(a)
			return disambig(ctx, a, e, d);
		te = Zblock(Zlocals(3, "$o", "$rd", "$l"),
			    Zset(doid("$o"), expanda(ctx, e->e1->e1, d, w)),
			    Zset(doid("$rd"), Zcall(G("rdof"), 1,
						    doid("$o"))),
			    Zset(doid("$l"),
				 Zcall(Zcall(G("tablook"), 2,
					     Zcall(G("rdgettab"), 1,
						   doid("$rd")),
					     Zid2sym(e->e1->e2)), 1,
				       doid("$o"))),
			    Zcall(Zcall(G("tablook"), 2,
					Zcall(G("rdsettab"),
					      1, doid("$rd")),
					Zid2sym(e->e1->e2)), 2,
				  doid("$o"),
				  Zbinop(e->kind == Epostinc ? Eadd : Esub,
					 doid("$l"), Zint(1))),
			    doid("$l"),
			    NULL);
		putsrc(te, e->src);
		return te;
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Expr*
expanda(U *ctx, Expr *e, unsigned d, unsigned *w)
{
	Expr *p, *a;

	if(e == 0)
		return e;

	/* recognize:

	   e1[e2]
	   e1.f

	   e1[e2] = e3
	   e1.e2 = e3;

	   e1[e2] ?= e3
	   e1.f ?= e3

	   (e1[e2])++, (e1[e2])--, ++(e1[e2]), --(e1[e2])
	   (e1.f)++, (e1.f)--, ++(e1.f), --(e1.f)
	*/
	switch(e->kind){
	case Eref:
		if(islval(e->e1, &a))
			sete1(e, expanda(ctx, e->e1, d, w));
		else if(a)
			return disambig0(ctx, a, e, d);
		return e;
	case Eg:
	case EGOP:
	case Epostinc:
	case Epostdec:
	case Epreinc:
	case Epredec:
		if(e->e1->kind == Earef)
			return expandaref(ctx, e, d, w);
		if(e->e1->kind == Edot)
			return expanddot(ctx, e, d, w);
		sete1(e, expanda(ctx, e->e1, d, w));
		sete2(e, expanda(ctx, e->e2, d, w));
		return e;
	case Earef:
		return expandaref(ctx, e, d, w);
	case Edot:
		return expanddot(ctx, e, d, w);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, expanda(ctx, p->e1, d, w));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, expanda(ctx, e->e1, d, w));
		sete2(e, expanda(ctx, e->e2, d, w));
		sete3(e, expanda(ctx, e->e3, d, w));
		sete4(e, expanda(ctx, e->e4, d, w));
		return e;
	}
}

Expr*
docompilea(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return 0;
	expanda(ctx, e, 0, 0);
	return e;
}
