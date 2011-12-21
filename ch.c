#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
l1_toupper(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	int x;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to toupper");
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	x = xtoupper(cvalu(cv));
	*rv = mkvalcval(cv->dom, cv->type, x);
}

static void
l1_tolower(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	int x;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tolower");
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	x = xtolower(cvalu(cv));
	*rv = mkvalcval(cv->dom, cv->type, x);
}

static void
isxctype(VM *vm, Imm argc, Val *argv, Val *rv, int (*fn)(int))
{
	Cval *cv;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", vmfnid(vm));
	checkarg(vm, argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(fn((int)cvalu(cv)))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isalnum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisalnum);
}

static void
l1_isalpha(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisalpha);
}

static void
l1_isascii(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisascii);
}

static void
l1_isblank(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisblank);
}

static void
l1_iscntrl(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xiscntrl);
}

static void
l1_isdigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisdigit);
}

static void
l1_isgraph(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisgraph);
}

static void
l1_islower(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xislower);
}

static void
l1_isodigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisodigit);
}

static void
l1_isprint(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisprint);
}

static void
l1_ispunct(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xispunct);
}

static void
l1_isspace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisspace);
}

static void
l1_isupper(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisupper);
}

static void
l1_isxdigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	isxctype(vm, argc, argv, rv, xisxdigit);
}

void
fnch(Env *env)
{
	FN(isalnum);
	FN(isalpha);
	FN(isascii);
	FN(isblank);
	FN(iscntrl);
	FN(isdigit);
	FN(isgraph);
	FN(islower);
	FN(isodigit);
	FN(isprint);
	FN(ispunct);
	FN(isspace);
	FN(isupper);
	FN(isxdigit);
	FN(tolower);
	FN(toupper);
}
