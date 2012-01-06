#include "sys.h"
#include "util.h"
#include "syscqct.h"

u32
hashqvstx(Expr *e)
{
	u32 m;
	m = Qexpr;
	m = hashx(m, hashqvval(mkvalcid(e->skind)));
	if(e->kind == Eid || e->kind == Eval){
		m = hashx(m, hashqvval(e->aux));
		return m;
	}
	if(e->e1)
		m = hashx(m, hashqvstx(e->e1));
	if(e->e2)
		m = hashx(m, hashqvstx(e->e2));
	if(e->e3)
		m = hashx(m, hashqvstx(e->e3));
	if(e->e4)
		m = hashx(m, hashqvstx(e->e4));
	return m;
}

int
eqvstx(Expr *a, Expr *b)
{
	/* ignore src */

	if(!eqvval(mkvalcid(a->skind), mkvalcid(b->skind)))
		return 0;

	if(a->kind == Eid || a->kind == Eval)
		return eqvval(a->aux, b->aux);

	if((a->e1 && !b->e1) || (!a->e1 && b->e1))
		return 0;
	if((a->e2 && !b->e2) || (!a->e2 && b->e2))
		return 0;
	if((a->e3 && !b->e3) || (!a->e3 && b->e3))
		return 0;
	if((a->e4 && !b->e4) || (!a->e4 && b->e4))
		return 0;
	if(a->e1 && !eqvstx(a->e1, b->e1))
		return 0;
	if(a->e2 && !eqvstx(a->e2, b->e2))
		return 0;
	if(a->e3 && !eqvstx(a->e3, b->e3))
		return 0;
	if(a->e4 && !eqvstx(a->e4, b->e4))
		return 0;
	return 1;
}

u32
hashstx(Expr *e)
{
	return hashqvstx(e);
}

int
equalstx(Expr *a, Expr *b)
{
	return eqvstx(a, b);
}

static void
l1_mkstxid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkstxid");
	checkarg(vm, argv, 0, Qcid);
	e = Zidcid(valcid(argv[0]));
	e->skind = mkcid0("id");
	e->src = mksrcfake("(user syntax)");
	*rv = mkvalexpr(e);
}

static void
l1_mkstxval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkstxval");
	e = Zval(argv[0]);
	e->skind = mkcid0("val");
	e->src = mksrcfake("(user syntax)");
	*rv = mkvalexpr(e);
}

static Expr*
Zappend(Expr *h, Expr *t)
{
	if(h->kind == Enull)
		return t;
	return putsrc(Zcons(h->e1, Zappend(h->e2, t)),
		      h->src);
}

static void
l1_stxsplice(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *h, *t;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to stxsplice");
	checkarg(vm, argv, 0, Qexpr);
	checkarg(vm, argv, 1, Qexpr);
	h = valexpr(argv[0]);
	t = valexpr(argv[1]);
	if(h->kind != Eelist && h->kind != Enull)
		vmerr(vm, "attempt to splice non-Eelist form");
	if(t->kind != Eelist && t->kind != Enull)
		vmerr(vm, "attempt to splice into non-Eelist form");
	*rv = mkvalexpr(Zappend(h, t));
}

static void
l1_stxliftval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stxliftval");
	if(Vkind(argv[0]) == Qexpr)
		*rv = argv[0];
	else
		l1_mkstxval(vm, argc, argv, rv);
}

static void
l1_mkstx(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Kind k;
	Cid *sk;
	unsigned i;
	Src src;
	Expr *earg[4], *e;
	if(argc < 1 || argc > 6)
		vmerr(vm, "wrong number of arguments to mkstx");
	checkarg(vm, argv, 0, Qcid);
	sk = valcid(argv[0]);
	k = s2kind(ciddata(sk));

	if(k == Eid || k == Eval)
		vmerr(vm, "attempt to create #%s with mkstx", S[k]);

	memset(earg, 0, sizeof(earg));
	argv++;
	argc--;
	if(Vkind(argv[argc-1]) == Qvec){
		src = valvec(argv[argc-1]);
		argc--;
	}else
		src = mksrcfake("(user syntax)");
	for(i = 0; i < argc; i++)
		if(argv[i] != Xnil){
			/* argv dance correctly flags the incorrect operand */
			checkarg(vm, argv-1, i+1, Qexpr);
			earg[i] = valexpr(argv[i]);
		}
	e = Z4(k, earg[0], earg[1], earg[2], earg[3]);
	e->skind = sk;
	putsrc(e, src);
	*rv = mkvalexpr(e);
}

static void
l1_stxkind(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stxkind");
	checkarg(vm, argv, 0, Qexpr);
	e = valexpr(argv[0]);
	if(e->skind == 0)
		bug();
	if(Vkind(mkvalcid(e->skind)) != Qcid)
		bug();
	*rv = mkvalcid(e->skind);
}

static void
l1_stxid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stxid");
	checkarg(vm, argv, 0, Qexpr);
	e = valexpr(argv[0]);
	if(e->kind != Eid)
		vmerr(vm, "stxid on non-identifier syntax");
	*rv = e->aux;
}

static void
l1_stxval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stxval");
	checkarg(vm, argv, 0, Qexpr);
	e = valexpr(argv[0]);
	if(e->kind != Eval)
		vmerr(vm, "stxval on non-literal syntax");
	*rv = e->aux;
}

void
l1_stxref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Imm u;
	Expr *e, *s;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to stxref");
	checkarg(vm, argv, 0, Qexpr);
	checkarg(vm, argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to stxref must be "
		      "a non-negative integer");
	u = cvalu(cv);
	if(u >= 4)
		vmerr(vm, "stxref out of bounds");
	e = valexpr(argv[0]);
	switch(u){
	case 0:
		s = e->e1;
		break;
	case 1:
		s = e->e2;
		break;
	case 2:
		s = e->e3;
		break;
	case 3:
		s = e->e4;
		break;
	}
	if(s)
		*rv = mkvalexpr(s);
	/* else, nil */
}

static void
l1_stxsrc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stxsrc");
	checkarg(vm, argv, 0, Qexpr);
	e = valexpr(argv[0]);
	*rv = mkvalvec(e->src);
}

void
fnstx(Env *env)
{
	FN(mkstx);
	FN(mkstxid);
	FN(mkstxval);
	FN(stxliftval);
	FN(stxid);
	FN(stxkind);
	FN(stxref);
	FN(stxsplice);
	FN(stxsrc);
	FN(stxval);
}
