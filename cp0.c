#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* compile_rval(U *ctx, Expr *e, unsigned lfree);
static Expr* compile0(U *ctx, Expr* e);

static int
islval(Expr *e)
{
	switch(e->kind){
	case Eticke:
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
	Src src;
	if(lfree)
		e = Zlocals(1, "$val");
	else
		e = Zlocals(4, "$val", "$dom", "$type", "$addr");
	src = body->src;
	te = Zblock(e, body, NULL);
	putsrc(te, src);
	return te;
}

static Expr*
lvalblock(Expr *body)
{
	Expr *te;
	Src src;
	src = body->src;
	te = Zblock(Zlocals(1, "$tmp"), body, NULL);
	putsrc(te, src);
	return te;
}

/* FIXME: rationalize with compile1 */
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
compile_lval(U *ctx, Expr *e, int needaddr)
{
	Expr *se, *te, *dom, *t;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Ecast:
		te = nullelist();

		// compile lvalue reference to expression,
		// using dom, type bindings
		se = compile_lval(ctx, e->e2, needaddr);
		te = Zcons(se, te);

		domandtype(e->e1, &dom, &t);
		if(!dom)
			dom = doid("$dom");
		t = compile0(ctx, t);
		se = Zblock(Zlocals(1, "$tn"),
			    Zset(doid("$tn"), t),
			    Zset(doid("$type"),
				 Zcall(G("looktype"), 2,
				       dom, doid("$tn"))),
			    Zif(Zcall(G("isnil"), 1, doid("$type")),
				Zcall(G("error"), 2,
				      Zstr("undefined type: %t"),
				      doid("$tn"))),
			    NULL);
		te = Zcons(se, te);
		putsrc(te, e->src);
		return lvalblock(invert(te));
	case Eticke:
		te = nullelist();

		// $dom = dom;
		se = Zset(doid("$dom"), e->e1);
		te = Zcons(se, te);

		// $tmp = looksym($dom,sym)
		se = Zset(doid("$tmp"),
			  Zcall(G("looksym"), 2,
				doid("$dom"), Zid2sym(e->e2)));
		te = Zcons(se, te);

		// if(isnil($tmp)) error("undefined symbol: %s", sym);
		se = newexpr(Eif,
			     Zcall(G("isnil"), 1, doid("$tmp")),
			     Zcall(G("error"), 2,
				   Zstr("undefined symbol: %a"),
				   Zid2sym(e->e2)),
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
					   Zstr("symbol lacks address: %a"),
					   Zid2sym(e->e2)),
				     0, 0);
			te = Zcons(se, te);
		}

		putsrc(te, e->src);
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
					  Zxcast(Zcall(G("mkctype_ptr"), 1,
						       Zcall(G("mkctype_void"),
							     0)),
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
		putsrc(te, e->src);
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
				doid("$type"), Zid2sym(e->e2)));
		te = Zcons(se, te);

		// if(isnil($tmp)) error("undefined field: %s", sym);
		se = newexpr(Eif,
			     Zcall(G("isnil"), 1, doid("$tmp")),
			     Zcall(G("error"), 2,
				   Zstr("undefined field: %s"),
				   Zid2sym(e->e2)),
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

		putsrc(te, e->src);
		return lvalblock(invert(te));
	default:
		cerror(ctx, e, "expression is not an lvalue");
	}
	return 0; /* not reached */
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
	case Eticke:
	case Edot:
	case Ederef:
		te = nullelist();
		se = compile_lval(ctx, e, 1);
		te = Zcons(se, te);
		se = Zcval(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		te = invert(te);
		putsrc(te, src);
		return rvalblock(te, lfree);
	case Eref:
		te = nullelist();
		se = compile_lval(ctx, e->e1, 1);
		te = Zcons(se, te);
		se = Zref(doid("$dom"), doid("$type"), doid("$addr"));
		te = Zcons(se, te);
		te = invert(te);
		putsrc(te, src);
		return rvalblock(te, lfree);
	case Eg:
		if(!islval(e->e1)){
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			sete1(e, compile_rval(ctx, e->e1, 0));
			sete2(e, compile_rval(ctx, e->e2, 0));
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
		putsrc(te, src);
		return rvalblock(te, lfree);
	case EGOP:
		if(!islval(e->e1)){
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			sete1(e, compile_rval(ctx, e->e1, 0));
			sete2(e, compile_rval(ctx, e->e2, 0));
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(ctx, e->e1, 1));
		te = Zcons(se, te);

		se = Zset(doid("$val"),
			  Zgbinop(e->kind, doid("$val"),
				  compile_rval(ctx, e->e2, 0)));
		te = Zcons(se, te);

		se = Zcall(doid("$put"), 4,
			   doid("$dom"), doid("$addr"), doid("$type"),
			   doid("$val"));
		te = Zcons(se, te);

		te = invert(te);
		putsrc(te, src);
		return rvalblock(te, lfree);
	case Epostinc:
	case Epostdec:
		if(!islval(e->e1)){
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			sete1(e, compile_rval(ctx, e->e1, 0));
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(ctx, e->e1, 1));
		te = Zcons(se, te);

		se = Zcall(doid("$put"), 4,
			   doid("$dom"), doid("$addr"), doid("$type"),
			   (e->kind == Epostinc
			    ? Zadd(doid("$val"), Zint(1))
			    : Zsub(doid("$val"), Zint(1))));
		te = Zcons(se, te);

		se = doid("$val");
		te = Zcons(se, te);

		te = invert(te);
		putsrc(te, src);
		return rvalblock(te, lfree);
	case Epreinc:
	case Epredec:
		if(!islval(e->e1)){
			if(e->e1->kind != Eid)
				cerror(ctx, e, "invalid assignment");
			sete1(e, compile_rval(ctx, e->e1, 0));
			return e;
		}

		te = nullelist();

		/* reuse lval bindings */
		se = Zset(doid("$val"), compile_rval(ctx, e->e1, 1));
		te = Zcons(se, te);

		if(e->kind == Epreinc)
			se = Zadd(doid("$val"), Zint(1));
		else
			se = Zsub(doid("$val"), Zint(1));
		se = Zset(doid("$val"), se);
		te = Zcons(se, te);

		se = Zcall(doid("$put"), 4,
			   doid("$dom"), doid("$addr"), doid("$type"),
			   doid("$val"));
		te = Zcons(se, te);

		te = invert(te);
		putsrc(te, src);
		return rvalblock(te, lfree);
	case Esizeofe:
		/* special case: &foo => sizeof ptr to type of cvalue foo */
		if(e->e1->kind == Eref && !islval(e->e1->e1)){
			se = Zblock(Zlocals(1, "$v"),
				    Zset(doid("$v"),
					 compile_rval(ctx, e->e1->e1, 0)),
				    Zsizeof(Zcall(G("mkctype_ptr"), 2,
						  Zcall(doid("$typeof"), 1,
							doid("$v")),
						  Zcall(G("nsptr"), 1,
							Zcall(G("domof"), 1,
							      doid("$v"))))),
				    NULL);
			putsrc(se, src);
			return se;
		}

		if(!islval(e->e1)){
			se = Zsizeof(compile_rval(ctx, e->e1, 0));
			putsrc(se, src);
			return se;
		}

		te = nullelist();

		se = compile_lval(ctx, e->e1, 0);
		te = Zcons(se, te);

		se = Zsizeof(Zcall(doid("$typeof"), 1, doid("$type")));
		te = Zcons(se, te);

		te = invert(te);
		putsrc(te, src);
		return rvalblock(te, lfree);
	case Etypeofe:
		/* special case: &foo => ptr to type of cvalue foo */
		if(e->e1->kind == Eref && !islval(e->e1->e1)){
			se = Zblock(Zlocals(1, "$v"),
				    Zset(doid("$v"),
					 compile_rval(ctx, e->e1->e1, 0)),
				    Zcall(G("mkctype_ptr"), 2,
					  Zcall(doid("$typeof"), 1, doid("$v")),
					  Zcall(G("nsptr"), 1,
						Zcall(G("domof"), 1,
						      doid("$v")))),
				    NULL);
			putsrc(se, src);
			return se;
		}

		if(!islval(e->e1)){
			se = Zcall(doid("$typeof"), 1,
				   compile_rval(ctx, e->e1, 0));
			putsrc(se, src);
			return se;
		}

		te = nullelist();

		se = compile_lval(ctx, e->e1, 0);
		te = Zcons(se, te);

		se = Zcall(doid("$typeof"), 1, doid("$type"));
		te = Zcons(se, te);

		te = invert(te);
		putsrc(te, src);
		return rvalblock(te, lfree);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, compile_rval(ctx, p->e1, 0));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, compile_rval(ctx, e->e1, 0));
		sete2(e, compile_rval(ctx, e->e2, 0));
		sete3(e, compile_rval(ctx, e->e3, 0));
		sete4(e, compile_rval(ctx, e->e4, 0));
		return e;
	}
}

static int
isemptyblock(Expr *e)
{
	if(e->kind == Escope && e->e1->e2->kind == Enull)
		return 1;
	else if(e->kind == Eblock && e->e2->kind == Enull)
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
		sete1(e, groomc(ctx, e->e1));
		if(isemptyblock(e->e2))
			sete2(e, newexpr(Enil, 0, 0, 0, 0));
		else
			sete2(e, groomc(ctx, e->e2));
		if(e->e3 && isemptyblock(e->e3))
			sete3(e, newexpr(Enil, 0, 0, 0, 0));
		else
			sete3(e, groomc(ctx, e->e3));
		putsrc(e, e->src);
		return e;
	case Ecomma:
		se = Zblock(nullelist(),
			    groomc(ctx, e->e1),
			    groomc(ctx, e->e2),
			    NULL);
		putsrc(se, e->src);
		return se;
	case Econd:
		se = Zifelse(groomc(ctx, e->e1),
			     groomc(ctx, e->e2),
			     groomc(ctx, e->e3));
		putsrc(se, e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, groomc(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, groomc(ctx, e->e1));
		sete2(e, groomc(ctx, e->e2));
		sete3(e, groomc(ctx, e->e3));
		sete4(e, groomc(ctx, e->e4));
		return e;
	}
}

typedef struct HTlist HTlist;

struct HTlist {
	HT *ht;
	HTlist *link;
};

static void checkgoto(U *ctx, Expr *e, HTlist *ls);
enum {
	Unusedlabel = 1,
	Usedlabel,
};

static HTlist*
mkhtlist(HT *ht, HTlist *link)
{
	HTlist *l;
	l = emalloc(sizeof(HTlist));
	l->ht = ht;
	l->link = link;
	return l;
}

static void
freehtlist(HTlist *l)
{
	efree(l);
}

static void
freeallhtlist(HTlist *l)
{
	if(l == 0)
		return;
	freeallhtlist(l->link);
	freeht(l->ht);
	freehtlist(l);
}

static void
labels(U *ctx, Expr *e, HTlist *ls)
{
	char *id;
	Expr *p;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
	case Edefine:
	case Edefloc:
		return;
	case Elabel:
		id = idsym(e->e1);
		if(hgets(ls->ht, id, strlen(id))){
			freeallhtlist(ls);
			cerror(ctx, e, "duplicate label: %s", id);
		}else{
			e->attr = Unusedlabel;
			hputs(ls->ht, id, strlen(id), e);
		}
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
reccheckgoto(U *ctx, Expr *e, HTlist *ls)
{
	char *id;
	Expr *p;
	void *q;

	if(e == 0)
		return;
	switch(e->kind){
	case Elambda:
		checkgoto(ctx, e->e2, ls);
		break;
	case Edefine:
	case Edefloc:
		checkgoto(ctx, e->e3, ls);
		break;
	case Egoto:
		id = idsym(e->e1);
		q = hgets(ls->ht, id, strlen(id));
		if(q == 0){
			freeallhtlist(ls);
			cerror(ctx, e, "undefined label: %s", id);
		}else{
			p = q;
			p->attr = Usedlabel;
		}
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
	USED(k);
	p = q;
	if(p->attr == Unusedlabel)
		cwarn((U*)u, p, "unused label: %s", idsym(p->e1));
}

static void
checkgoto(U *ctx, Expr *e, HTlist *link)
{
	HTlist *ls;

	ls = mkhtlist(mkhts(), link);
	labels(ctx, e, ls);
	reccheckgoto(ctx, e, ls);
	hforeach(ls->ht, check1label, ctx);
	freeht(ls->ht);
	freehtlist(ls);
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
		checkctl(ctx, e->e2, inloop, 1);
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
	case Edefloc:
		checkctl(ctx, e->e3, 0, 0);
		break;
	case Econtinue:
		if(inloop == 0)
			cerror(ctx, e, "continue not within loop");
		break;
	case Ebreak:
		if(inloop == 0 && inswitch == 0)
			cerror(ctx, e, "break not within loop or switch");
		break;
	case Edefault:
		if(inswitch == 0)
			cerror(ctx, e, "default label not within switch");
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
	checkgoto(ctx, e, 0);
	groomc(ctx, e);
	compile_rval(ctx, e, 0);
	return e;
}

Expr*
docompile0(U *ctx, Expr *e)
{
	compile0(ctx, e);
	return e;
}

void
l1_cp0(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cp0");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompile0(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
