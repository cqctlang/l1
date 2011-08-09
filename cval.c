#include "sys.h"
#include "util.h"
#include "syscqct.h"

u32
hashqcval(Cval *v)
{
	return hashx(hashu64(v->val), hashctype(v->type));
}

int
eqcval(Cval *a, Cval *b)
{
	if(a->val != b->val)
		return 0;
	return eqvctype(a->type, b->type);
}

u32
hashqvcval(Cval *v)
{
	return hashqcval(v);
}

int
eqvcval(Cval *a, Cval *b)
{
	return eqcval(a, b);
}

u32
hashcval(Cval *v)
{
	return hashu64(v->val);
}

int
equalcval(Cval *a, Cval *b)
{
	return a->val == b->val;
}

static void
l1_equalcval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to equalcval");
	checkarg(vm, "equalcval", argv, 0, Qcval);
	checkarg(vm, "equalcval", argv, 1, Qcval);
	if(equalcval(valcval(argv[0]), valcval(argv[1])))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_hashcval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to hashcval");
	checkarg(vm, "hashcval", argv, 0, Qcval);
	*rv = mkvalcval2(mklitcval(Vuint, hashcval(valcval(argv[0]))));
}

static void
l1_eqvcval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to eqvcval");
	checkarg(vm, "eqvcval", argv, 0, Qcval);
	checkarg(vm, "eqvcval", argv, 1, Qcval);
	if(eqvcval(valcval(argv[0]), valcval(argv[1])))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_hashqvcval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to hashqvcval");
	checkarg(vm, "hashcval", argv, 0, Qcval);
	*rv = mkvalcval2(mklitcval(Vuint, hashqvcval(valcval(argv[0]))));
}

void
fncval(Env *env)
{
	FN(equalcval);
	FN(eqvcval);
	FN(hashcval);
	FN(hashqvcval);
}
