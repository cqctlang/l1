#include "sys.h"
#include "util.h"
#include "syscqct.h"

char stxsrcfile[] = "(user syntax extension)";
static Src stxsrc = { stxsrcfile, 0, 0 };

static void
l1_mkstxid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Expr *e;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkstxid");
	checkarg(vm, "mkstxid", argv, 0, Qcid);
	e = Zidcid(valcid(argv[0]));
	e->skind = mkcid0("id");
	e->src = stxsrc;
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
	e->src = stxsrc;
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
	e->src = stxsrc;
	*rv = mkvalexpr(e);
}

void
fnstx(Env *env)
{
	FN(mkstx);
	FN(mkstxid);
	FN(mkstxval);
}
