#include "sys.h"
#include "util.h"
#include "syscqct.h"

u32
hashvec(Vec *v)
{
	u32 i, len, m;
	m = Qvec;
	len = v->len;
	for(i = 0; i < len; i++)
		m = hashx(m, hashval(vecdata(v)[i]));
	return m;
}

int
equalvec(Vec *a, Vec *b)
{
	u32 len, m;
	len = a->len;
	if(len != b->len)
		return 0;
	for(m = 0; m < len; m++)
		if(!equalval(vecdata(a)[m], vecdata(b)[m]))
			return 0;
	return 1;
}

Imm
veclen(Vec *v)
{
	return v->len;
}

Vec*
veccopy(Vec *old)
{
	Vec *new;
	new = mkvec(old->len);
	memcpy(vecdata(new), vecdata(old), new->len*sizeof(Val));
	return new;
}

Vec*
mkvec(Imm len)
{
	Vec *vec;
	vec = (Vec*)malv(Qvec, vecsize(len));
	vec->len = len;
	return vec;
}

Vec*
mkvecinit(Imm len, Val v)
{
	Vec *vec;
	Imm i;

	vec = mkvec(len);
	for(i = 0; i < len; i++)
		vecdata(vec)[i] = v;
	return vec;
}

Val
vecref(Vec *vec, Imm idx)
{
	return vecdata(vec)[idx];
}

void
_vecset(Vec *vec, Imm idx, Val v)
{
	vecdata(vec)[idx] = v;
}

void
vecset(Vec *vec, Imm idx, Val v)
{
	gcwb(mkvalvec(vec));
	_vecset(vec, idx, v);
}

static void
l1_mkvec(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val v;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkvec");
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(argc == 2)
		v = argv[1];
	else
		v = Xnil;
	if(!isnatcval(cv))
		vmerr(vm, "operand 1 to mkvec must be a non-negative integer");
	*rv = mkvalvec(mkvecinit(cvalu(cv), v));
}

static void
l1_vector(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Imm i;

	USED(vm);
	v = mkvec(argc);
	for(i = 0; i < argc; i++)
		_vecset(v, i, argv[i]);
	*rv = mkvalvec(v);
}

void
l1_vecref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Cval *cv;
	Val vp;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to vecref");
	checkarg(vm, argv, 0, Qvec);
	checkarg(vm, argv, 1, Qcval);
	v = valvec(argv[0]);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to vecref must be "
		      "a non-negative integer");
	if(cvalu(cv) >= v->len)
		vmerr(vm, "vecref out of bounds");
	vp = vecref(v, cvalu(cv));
	*rv = vp;
}

void
l1_vecset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Cval *cv;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to vecset");
	checkarg(vm, argv, 0, Qvec);
	checkarg(vm, argv, 1, Qcval);
	v = valvec(argv[0]);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to vecset must be "
		      "a non-negative integer");
	if(cvalu(cv) >= v->len)
		vmerr(vm, "vecset out of bounds");
	vecset(v, cvalu(cv), argv[2]);
	USED(rv);
}


void
fnvec(Env *env)
{
	FN(mkvec);
	FN(vecref);
	FN(vecset);
	FN(vector);
}
