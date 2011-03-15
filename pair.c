#include "sys.h"
#include "util.h"
#include "syscqct.h"

Pair*
mkpair(Val a, Val d)
{
	Pair *p;
	p = (Pair*)malq(Qpair);
	// setc*r would invoke write barrier
	_setcar(p, a);
	_setcdr(p, d);
	return p;
}

Pair*
mkweakpair(Val a, Val d)
{
	Pair *p;
	p = (Pair*)malweak();
	// setc*r would invoke write barrier
	_setcar(p, a);
	_setcdr(p, d);
	return p;
}

static void
l1_setcar(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Pair *p;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to setcar");
	checkarg(vm, "setcar", argv, 0, Qpair);
	gcwb(argv[0]);
	p = valpair(argv[0]);
	p->car = argv[1];
}

static void
l1_setcdr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Pair *p;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to setcdr");
	checkarg(vm, "setcar", argv, 0, Qpair);
	gcwb(argv[0]);
	p = valpair(argv[0]);
	p->cdr = argv[1];
}

static void
l1_car(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Pair *p;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to car");
	checkarg(vm, "car", argv, 0, Qpair);
	p = valpair(argv[0]);
	*rv = p->car;
}

static void
l1_cdr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Pair *p;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cdr");
	checkarg(vm, "cdr", argv, 0, Qpair);
	p = valpair(argv[0]);
	*rv = p->cdr;
}

static void
l1_cons(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cons");
	*rv = mkvalpair(cons(argv[0], argv[1]));
}

static void
l1_weakcons(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to weakcons");
	*rv = mkvalpair(weakcons(argv[0], argv[1]));
}

static void
l1_isweakpair(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isweakpair");
	if(Vkind(argv[0]) == Qpair && isweak(argv[0]))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

void
fnpair(Env *env)
{
	FN(car);
	FN(cdr);
	FN(cons);
	FN(isweakpair);
	FN(setcar);
	FN(setcdr);
	FN(weakcons);
}
