#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* expanda(Expr *e, u32 d);

static char Sbaddot[] = "attempt to apply dot to a cvalue";

static char Acntr[] = "cntr";
static char Alval[] = "lval";
static char Aptr[] = "ptr";
static char Aunknown[] = "unknown";

typedef
enum Mode
{
	Mlval,
	Mptr,
	Mcntr,
	Munknown,
} Mode;

static int
attreq(Expr *e, char *id)
{
	if(e->kind != E_attr)
		bug();
	if(e->e2->kind != Eval)
		bug();
	if(Vkind(e->e2->aux) != Qcid)
		bug();
	return !strcmp(ciddata(valcid(e->e2->aux)), id);
}

static Expr*
Zattr(Expr *e, char *id)
{
	if(e == 0)
		bug();
	return Z2(E_attr, e, Zval(mkvalcid(mkcid0(id))));
}

static Expr*
Zaref(Expr *e1, Expr *e2)
{
	return Z2(Earef, e1, e2);
}

static Expr*
Zdot(Expr *e1, Expr *e2)
{
	return Z2(Edot, e1, e2);
}

static Expr*
Zderef(Expr *e1)
{
	return Z1(Ederef, e1);
}

/* FIXME: deconflict with cutil Zref */
static Expr*
Zref0(Expr *e1)
{
	return Z1(Eref, e1);
}

static Expr*
Zptr(Expr *e)
{
	return Zattr(e, Aptr);
}

static Expr*
Zcntr(Expr *e)
{
	return Zattr(e, Acntr);
}

static Expr*
Ztabget(Expr *r, char *f)
{
	if(r->kind != Eid)
		bug();
	return Zcall(G("tablook"), 2, r, Zcid(f));
}

static Expr*
Ztabset(Expr *r, char *f, Expr *v)
{
	if(r->kind != Eid)
		bug();
	return Zcall(G("tabinsert"), 3, r, Zcid(f), v);
}

enum { Tmpidsz = 8 };

static void
mktmpid(char *t, u32 d)
{
	snprint(t, Tmpidsz, "$t%d", d);
}

typedef
enum Aform
{
	Anone,
	Aaref,
	Aarefref,
	Aarefg,
	Aarefgop,
	Aarefinc,
	Adot,
	Adotref,
	Adotg,
	Adotgop,
	Adotinc,
} Aform;

static Aform
aform(Expr *e)
{
	if(e == 0)
		return Anone;
	switch(e->kind){
	case Earef:
		return Aaref;
	case Edot:
		return Adot;
	case Eref:
		switch(e->e1->kind){
		case Earef:
			return Aarefref;
		case Edot:
			return Adotref;
		default:
			return Anone;
		}
	case Eg:
		switch(e->e1->kind){
		case Earef:
			return Aarefg;
		case Edot:
			return Adotg;
		default:
			return Anone;
		}
	case EGOP:
		switch(e->e1->kind){
		case Earef:
			return Aarefgop;
		case Edot:
			return Adotgop;
		default:
			return Anone;
		}
	case Epreinc:
	case Epredec:
	case Epostinc:
	case Epostdec:
		switch(e->e1->kind){
		case Earef:
			return Aarefinc;
		case Edot:
			return Adotinc;
		default:
			return Anone;
		}
	default:
		return Anone;
	}
}

static Expr*
clear(Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case E_attr:
		return clear(e->e1);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, clear(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, clear(e->e1));
		sete2(e, clear(e->e2));
		sete3(e, clear(e->e3));
		sete4(e, clear(e->e4));
		return e;
	}
}

static Expr*	markup(Expr *e);

static Expr*
rec(Expr *e)
{
	if(e == 0)
		return 0;

	switch(e->kind){
	case Eticke:
		return Zattr(e, Alval);
	case Ecast:
		return putsrc(Z2(Ecast, markup(e->e1), rec(e->e2)),
			      e->src);
	case Earef:
		return putsrc(Zaref(rec(e->e1), markup(e->e2)), e->src);
	case Edot:
		return putsrc(Zdot(rec(e->e1), markup(e->e2)), e->src);
	case Ederef:
		return putsrc(Zderef(rec(e->e1)), e->src);
	default:
		return Zattr(markup(e), Aunknown);
	}
}

static Expr*
markup(Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Earef:
		return putsrc(Zaref(rec(e->e1), markup(e->e2)), e->src);
	case Edot:
		return putsrc(Zdot(rec(e->e1), markup(e->e2)), e->src);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, markup(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, markup(e->e1));
		sete2(e, markup(e->e2));
		sete3(e, markup(e->e3));
		sete4(e, markup(e->e4));
		return e;
	}
}

static int
iscntrkey(Expr *e)
{
	Val v;
	if(e->kind == Eval){
		v = e->aux;
		return Vkind(v) != Qcval;
	}
	return 0;
}

/* stupid optimizer:
     upgrade
     	(unknown(e1))[e2]
     to
        (cntr(e1))[e2]
   if e2 is a key that can only be used with a
   container
*/
static Expr*
stupid(Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Earef:
		if(e->e1->kind == E_attr
		   && attreq(e->e1, Aunknown)
		   && iscntrkey(e->e2))
			return putsrc(Zaref(Zcntr(e->e1), e->e2), e->src);
		else{
			sete1(e, stupid(e->e1));
			sete2(e, stupid(e->e2));
			return e;
		} 
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, stupid(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, stupid(e->e1));
		sete2(e, stupid(e->e2));
		sete3(e, stupid(e->e3));
		sete4(e, stupid(e->e4));
		return e;
	}
}

static Mode
_mode(Expr *e, Expr **u)
{
	switch(e->kind){
	case E_attr:
		if(attreq(e, Alval))
			return Mlval;
		else if(attreq(e, Acntr))
			return Mcntr;
		else if(attreq(e, Aptr))
			return Mptr;
		else if(attreq(e, Aunknown)){
			if(*u)
				bug();
			*u = e;
			return Munknown;
		}else
			bug();
	case Ecast:
		return _mode(e->e2, u);
	case Ederef:
		return Mlval;
	case Edot:
		switch(_mode(e->e1, u)){
		case Munknown:
			return Munknown;
		case Mcntr:
			if(*u == 0)
				*u = e;
			return Munknown;
		case Mlval:
			return Mlval;
		case Mptr:
			/* this case is always an error */
			return Mptr;
		default:
			bug();
		}
	case Earef:
		switch(_mode(e->e1, u)){
		case Munknown:
			return Munknown;
		case Mcntr:
			if(*u == 0)
				*u = e;
			return Munknown;
		case Mlval:
			return Mlval;
		case Mptr:
			return Mlval;
		default:
			bug();
		}
	default:
		bug();
	}
}

static Mode
mode(Expr *e, Expr **u)
{
	Mode m;
	*u = 0;
	m = _mode(e, u);
	if(m == Munknown && *u == 0)
		bug();
	return m;
}

/* like copyexpr */
static Expr*
subst(Expr *e, Expr *o, Expr *n)
{
	Expr *ne;

	if(e == 0)
		return 0;

	if(e == o)
		return n;

	ne = mkexpr();
	ne->kind = e->kind;
	ne->skind = e->skind;
	ne->attr = e->attr;
	ne->src = e->src;
	switch(e->kind){
	case Eid:
	case Eval:
		ne->aux = e->aux;
		break;
	default:
		break;
	}

	ne->e1 = subst(e->e1, o, n);
	ne->e2 = subst(e->e2, o, n);
	ne->e3 = subst(e->e3, o, n);
	ne->e4 = subst(e->e4, o, n);
	return ne;
}

static Expr*
either(Expr *e, Expr *ea, u32 d)
{
	Expr *el, *ec;
	char t[Tmpidsz];

//	printf("container/lval ambiguity at %s:%lu\n",
//	       srcfile(e->src), srcline(e->src));
	mktmpid(t, d);
	el = subst(e, ea, putsrc(Zptr(doid(t)), e->src));
	ec = subst(e, ea, putsrc(Zcntr(doid(t)), e->src));
	return putsrc(Zblock(Zlocals(1, t),
			     Zset(doid(t), expanda(ea, d+1)),
			     Zifelse(Zcall(G("iscvalue"), 1, doid(t)),
				     expanda(el, d+1),
				     expanda(ec, d+1)),
			     NULL),
		      e->src);
}

static Expr*
expandaform(Aform af, Expr *e, u32 d)
{
	Expr *e1, *e2, *e3, *f;
	Expr *p, *b;
	Expr *ea;
	char *id;

	switch(af){
	case Aaref:
		/* e1[e2] */
		e1 = e->e1;
		e2 = e->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
		case Mlval:
			return putsrc(Zaref(expanda(e1, d),
					    expanda(e2, d)),
				      e->src);
		case Mcntr:
			return putsrc(Zcall(G("cntrget"), 2,
					    expanda(e1, d),
					    expanda(e2, d)),
				      e->src);
		}
		bug();
	case Aarefref:
		/* &e1[e2] */
		e1 = e->e1->e1;
		e2 = e->e1->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
		case Mlval:
			return putsrc(Zref0(Zaref(expanda(e1, d),
						  expanda(e2, d))),
				      e->src);
		case Mcntr:
			return putsrc(Zcall(G("error"), 1,
					    Zstr("attempt to apply & to non-lvalue")),
				      e->src);
		}
		bug();
	case Aarefg:
		/* e1[e2] = e3 */
		e1 = e->e1->e1;
		e2 = e->e1->e2;
		e3 = e->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
		case Mlval:
			return putsrc(Zset(Zaref(expanda(e1, d),
						 expanda(e2, d)),
					   expanda(e3, d)),
				      e->src);
		case Mcntr:
			return putsrc(Zcall(G("cntrput"), 3,
					    expanda(e1, d),
					    expanda(e2, d),
					    expanda(e3, d)),
				      e->src);
		}
		bug();
	case Aarefgop:
		/* e1[e2] += e3, etc. */
		e1 = e->e1->e1;
		e2 = e->e1->e2;
		e3 = e->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
		case Mlval:
			return putsrc(Z2(e->kind,
					 Zaref(expanda(e1, d),
					       expanda(e2, d)),
					 expanda(e3, d)),
				      e->src);
		case Mcntr:
			return putsrc(Zblock(Zlocals(2, "$o", "$i"),
					     Zset(doid("$o"), expanda(e1, d)),
					     Zset(doid("$i"), expanda(e2, d)),
					     Zcall(G("cntrput"), 3,
						   doid("$o"),
						   doid("$i"),
						   Zgbinop(e->kind,
							   Zcall(G("cntrget"), 2,
								 doid("$o"), doid("$i")),
							   expanda(e3, d))),
					     NULL),
				      e->src);
		}
		bug();
	case Aarefinc:
		/* e1[e2]++, etc. */
		e1 = e->e1->e1;
		e2 = e->e1->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
		case Mlval:
			return putsrc(Z1(e->kind, Zaref(expanda(e1, d), expanda(e2, d))),
				      e->src);
		case Mcntr:
			switch(e->kind){
			case Epreinc:
			case Epredec:
				return putsrc(Zblock(Zlocals(2, "$o", "$i"),
						     Zset(doid("$o"), expanda(e1, d)),
						     Zset(doid("$i"), expanda(e2, d)),
						     Zcall(G("cntrput"), 3,
							   doid("$o"),
							   doid("$i"),
							   Zgbinop(e->kind,
								   Zcall(G("cntrget"), 2,
									 doid("$o"),
									 doid("$i")),
								   Zint(1))),
						     NULL),
					      e->src);
			case Epostinc:
			case Epostdec:
				return putsrc(Zblock(Zlocals(3, "$o", "$i", "$p"),
						     Zset(doid("$o"), expanda(e1, d)),
						     Zset(doid("$i"), expanda(e2, d)),
						     Zset(doid("$p"), Zcall(G("cntrget"), 2,
									    doid("$o"),
									    doid("$i"))),
						     Zcall(G("cntrput"), 3,
							   doid("$o"),
							   doid("$i"),
							   Zgbinop(e->kind, doid("$p"), Zint(1))),
						     doid("$p"),
						     NULL),
					      e->src);
			default:
				bug();
			}
		}
		bug();
	case Adot:
		/* e1.f */
		e1 = e->e1;
		f = e->e2;
		switch(mode(e1, &ea)){
		case Munknown: {
			return either(e, ea, d);
/*
			id = idsym(f);
			if(!strcmp(id, "ns"))
				p = Zcall(G("nsof"), 1, doid("$o"));
			else if(!strcmp(id, "as"))
				p = Zcall(G("asof"), 1, doid("$o"));
			else
				p = Zlambdn(Zvararg(doid("$args")),
					    Zblock(nullelist(),
						   Zret(Zcall(doid("$callmethod"), 3,
							      doid("$o"),
							      Zstr(id),
							      doid("$args"))),
						   NULL),
					    f);
			b = Zblock(Zlocals(1, "$o"),
				   Zset(doid("$o"), expanda(e1, d)),
				   Zifelse(Zcall(G("isprimitivetable"), 1, doid("$o")),
					   Ztabget(doid("$o"), id),
					   Zifelse(Zcall(G("iscallable"), 1, doid("$o")),
						   p,
						   Zifelse(Zcall(G("iscvalue"), 1, doid("$o")),
							Zerror(Sbaddot),
						   	Zerror("invalid left operand to .")))),
				   NULL);
			return putsrc(b, e->src);
*/
		}
		case Mptr:
			return putsrc(Zerror(Sbaddot), e->src);
		case Mlval:
			return putsrc(Zdot(expanda(e1, d), f), e->src);
		case Mcntr:
			/* in non-assignment position,
			   when e1 is not a cvalue,
			   e1.f has four cases
			     - record access (if e1 is a record)
			     - .ns (if e1 is not a record and f == "ns")
			     - .as (if e1 is not a record and f == "as")
			     - method reference (otherwise)
			*/
			id = idsym(f);
			if(!strcmp(id, "ns"))
				p = Zcall(G("nsof"), 1, doid("$o"));
			else if(!strcmp(id, "as"))
				p = Zcall(G("asof"), 1, doid("$o"));
			else
				p = Zlambdn(Zvararg(doid("$args")),
					    Zblock(nullelist(),
						   Zret(Zcall(doid("$callmethod"), 3,
							      doid("$o"),
							      Zstr(id),
							      doid("$args"))),
						   NULL),
					    f);
			b = Zblock(Zlocals(1, "$o"),
				   Zset(doid("$o"), expanda(e1, d)),
				   Zifelse(Zcall(G("isprimitivetable"), 1, doid("$o")),
					   Ztabget(doid("$o"), id),
					   Zifelse(Zcall(G("iscallable"), 1, doid("$o")),
						   p,
						   Zerror("invalid left operand to ."))),
				   NULL);
			return putsrc(b, e->src);
		}
		bug();
	case Adotref:
		/* &e1.f */
		e1 = e->e1->e1;
		f = e->e1->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
			return putsrc(Zerror(Sbaddot), e->src);
		case Mlval:
			return putsrc(Zref0(Zdot(expanda(e1, d), f)), e->src);
		case Mcntr:
			return putsrc(Zerror("attempt to apply & to non-lvalue"), e->src);
		}
		bug();
	case Adotg:
		/* e1.f = e3 */
		e1 = e->e1->e1;
		f = e->e1->e2;
		e3 = e->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
			return putsrc(Zerror(Sbaddot), e->src);
		case Mlval:
			return putsrc(Zset(Zdot(expanda(e1, d), f), expanda(e3, d)),
				      e->src);
		case Mcntr:
			return putsrc(Zblock(Zlocals(2, "$o", "$v"),
					     Zset(doid("$o"), expanda(e1, d)),
					     Zset(doid("$v"), expanda(e3, d)),
					     Ztabset(doid("$o"), idsym(f), doid("$v")),
					     NULL),
				      e->src);
		}
		bug();
	case Adotgop:
		/* e1.f += e3, etc. */
		e1 = e->e1->e1;
		f = e->e1->e2;
		e3 = e->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
			return putsrc(Zerror(Sbaddot), e->src);
		case Mlval:
			return putsrc(Z2(e->kind, Zdot(expanda(e1, d), f), expanda(e3, d)),
				      e->src);
		case Mcntr:
			id = idsym(f);
			return putsrc(Zblock(Zlocals(2, "$o", "$v"),
					     Zset(doid("$o"), expanda(e1, d)),
					     Zset(doid("$v"), expanda(e3, d)),
					     Ztabset(doid("$o"), id,
						     Zgbinop(e->kind,
							     Ztabget(doid("$o"), id),
							     doid("$v"))),
					     NULL),
				      e->src);
		}
		bug();
	case Adotinc:
		/* e1.f++, etc. */
		e1 = e->e1->e1;
		f = e->e1->e2;
		switch(mode(e1, &ea)){
		case Munknown:
			return either(e, ea, d);
		case Mptr:
			return putsrc(Zerror(Sbaddot), e->src);
		case Mlval:
			return putsrc(Z1(e->kind, Zdot(expanda(e1, d), f)), e->src);
		case Mcntr:
			id = idsym(f);
			switch(e->kind){
			case Epreinc:
			case Epredec:
				return putsrc(Zblock(Zlocals(1, "$o"),
						     Zset(doid("$o"), expanda(e1, d)),
						     Ztabset(doid("$o"), id,
							     Zgbinop(e->kind,
								     Ztabget(doid("$o"), id),
								     Zint(1))),
						     NULL),
					      e->src);
			case Epostinc:
			case Epostdec:
				return putsrc(Zblock(Zlocals(2, "$o", "$p"),
						     Zset(doid("$o"), expanda(e1, d)),
						     Zblock(nullelist(),
							    Zset(doid("$p"),
								 Ztabget(doid("$o"), id)),
							    Ztabset(doid("$o"), id,
								    Zgbinop(e->kind,
									    doid("$p"),
									    Zint(1))),
							    NULL),
						     doid("$p"),
						     NULL),
					      e->src);
			default:
				bug();
			}
		}
		bug();
	default:
		bug();
	}
}

static Expr*
expanda(Expr *e, u32 d)
{
	Expr *p;
	Aform af;

	if(e == 0)
		return e;

	af = aform(e);
	if(af != Anone)
		return expandaform(af, e, d);

	switch(e->kind){
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, expanda(p->e1, d));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, expanda(e->e1, d));
		sete2(e, expanda(e->e2, d));
		sete3(e, expanda(e->e3, d));
		sete4(e, expanda(e->e4, d));
		return e;
	}
}

Expr*
docompilea(U *ctx, Expr *e)
{
	e = markup(e);
	e = stupid(e);
	expanda(e, 0);
	e = clear(e);
	return e;
}

void
l1_cpa(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpa");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompilea(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
