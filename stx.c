#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
l1_mkstxop(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Kind k, op;
	Cval *cv;
	Expr *e;
	if(argc != 4)
		vmerr(vm, "wrong number of arguments to mkstxop");
	checkarg(vm, "mkstxop", argv, 0, Qcval);
	checkarg(vm, "mkstxop", argv, 1, Qcval);
	checkarg(vm, "mkstxop", argv, 2, Qexpr);
	checkarg(vm, "mkstxop", argv, 3, Qexpr);
	cv = valcval(argv[0]);
	k = cvalu(cv);
	cv = valcval(argv[1]);
	op = cvalu(cv);
	switch(k){
	case Ebinop:
	case Egop:
		e = newexpr(k, valexpr(argv[2]), valexpr(argv[3]), 0, 0);
		e->op = op;
		break;
	default:
		vmerr(vm, "mkstxop on invalid expression type");
	}
	*rv = mkvalexpr(e);
}

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
	case Elabel:
	case Egoto:
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
l1_mkstx(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Kind k;
	Cval *cv;
	unsigned i;
	Expr *earg[4];
	if(argc < 1 || argc > 5)
		vmerr(vm, "wrong number of arguments to mkstx");
	checkarg(vm, "mkexpr", argv, 0, Qcval);
	cv = valcval(argv[0]);
	k = cvalu(cv);
	if(k >= Emax)
		vmerr(vm, "invalid expression kind");
	memset(earg, 0, sizeof(earg));
	argv++;
	argc--;
	for(i = 0; i < argc; i++)
		if(argv[i] != Xnil){
			/* the argv dance ensures correct operand is flagged */
			checkarg(vm, "mkexpr", argv-1, i+1, Qexpr);
			earg[i] = valexpr(argv[i]);
		}
	*rv = mkvalexpr(Z4(k, earg[0], earg[1], earg[2], earg[3]));
}

void
fnstx(Env *env)
{
	FN(mkstx);
	FN(mkstxaux);
	FN(mkstxop);
}
