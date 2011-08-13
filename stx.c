#include "sys.h"
#include "util.h"
#include "syscqct.h"


static void
l1_mkstxaux(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Kind k;
	Cval *cv;
	Expr *e;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mkstxaux");
	checkarg(vm, "mkstxaux", argv, 0, Qcval);
	cv = valcval(argv[0]);
	k = cvalu(cv);
	switch(k){
	case Eid:
	case E_tid:
	case Ekon:
		e = newexpr(k, 0, 0, 0, 0);
		e->aux = argv[1];
		break;
	default:
		vmerr(vm, "mkstxaux on invalid expression type");
	}
	*rv = mkvalexpr(e);
}

static void
l1_mkstxid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkstxid");
	checkarg(vm, "mkstxaux", argv, 0, Qcid);
	e = Zidcid(valcid(argv[0]));
	e->skind = mkcid0("id");
	*rv = mkvalexpr(e);
}

static void
l1_mkstxval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkstxval");
	e = Zkon(argv[0]);
	e->skind = mkcid0("kon");
	*rv = mkvalexpr(e);
}

static void
l1_mkstx(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Kind k;
	Cid *sk;
	unsigned i;
	Expr *earg[4], *e;
	if(argc < 1 || argc > 5)
		vmerr(vm, "wrong number of arguments to mkstx");
	checkarg(vm, "mkstx", argv, 0, Qcid);
	sk = valcid(argv[0]);
	k = s2kind(ciddata(sk));
	memset(earg, 0, sizeof(earg));
	argv++;
	argc--;
	for(i = 0; i < argc; i++)
		if(argv[i] != Xnil){
			/* the argv dance ensures correct operand is flagged */
			checkarg(vm, "mkexpr", argv-1, i+1, Qexpr);
			earg[i] = valexpr(argv[i]);
		}
	e = Z4(k, earg[0], earg[1], earg[2], earg[3]);
	e->skind = sk;
	*rv = mkvalexpr(e);
}

void
fnstx(Env *env)
{
	FN(mkstx);
	FN(mkstxaux);
	FN(mkstxid);
	FN(mkstxval);
}
