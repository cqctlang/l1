#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
l1_getpid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	*rv = mkvallitcval(vm, Vulong, getpid());
}

static void
l1_gettimeofday(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm tod;
	struct timeval tv;

	gettimeofday(&tv, 0);
	tod = tv.tv_sec;
	tod *= 1000000;
	tod += tv.tv_usec;
	*rv = mkvallitcval(vm, Vulong, tod);
}

static void
l1_randseed(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to randseed");
	arg0 = argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	srand((unsigned int)cv->val);
}

static void
l1_rand(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val arg0;
	Imm r;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rand");
	arg0 = argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	if(cv->val > RAND_MAX)
		vmerr(vm, "operand to rand exceeds RAND_MAX (%d)", RAND_MAX);
	if(cv->val == 0)
		vmerr(vm, "operand to rand must be positive");
	
	r = rand();
	r %= cv->val;
	*rv = mkvallitcval(vm, Vulong, r);
}

void
fnsys(Env *env)
{
	FN(getpid);
	FN(gettimeofday);
	FN(rand);
	FN(randseed);
}
