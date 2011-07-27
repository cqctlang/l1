#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
l1_mkkon(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkkon");
	*rv = mkvalexpr(Zkon(argv[0]));
}

static void
l1_mkexpr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Kind k;
	Cval *cv;
	if(argc < 1 || argc > 5)
		vmerr(vm, "wrong number of arguments to mkexpr");
	checkarg(vm, "mkexpr", argv, 0, Qcval);
	cv = valcval(argv[0]);
	k = cv->val;
	if(k >= Emax)
		vmerr(vm, "invalid expression kind");
	switch(argc-1){
	case 0:
		*rv = mkvalexpr(Z0(k));
		break;
	case 1:
		checkarg(vm, "mkexpr", argv, 1, Qexpr);
		*rv = mkvalexpr(Z1(k, valexpr(argv[1])));
		break;
	case 2:
		checkarg(vm, "mkexpr", argv, 1, Qexpr);
		checkarg(vm, "mkexpr", argv, 2, Qexpr);
		*rv = mkvalexpr(Z2(k, valexpr(argv[1]), valexpr(argv[2])));
		break;
	case 3:
		checkarg(vm, "mkexpr", argv, 1, Qexpr);
		checkarg(vm, "mkexpr", argv, 2, Qexpr);
		checkarg(vm, "mkexpr", argv, 3, Qexpr);
		*rv = mkvalexpr(Z3(k, valexpr(argv[1]), valexpr(argv[2]),
				   valexpr(argv[3])));
		break;
	case 4:
		checkarg(vm, "mkexpr", argv, 1, Qexpr);
		checkarg(vm, "mkexpr", argv, 2, Qexpr);
		checkarg(vm, "mkexpr", argv, 3, Qexpr);
		checkarg(vm, "mkexpr", argv, 4, Qexpr);
		*rv = mkvalexpr(Z4(k, valexpr(argv[1]), valexpr(argv[2]),
				   valexpr(argv[3]), valexpr(argv[4])));
		break;
	default:
		bug();
	}
}

void
fnstx(Env *env)
{
	FN(mkexpr);
	FN(mkkon);
}
