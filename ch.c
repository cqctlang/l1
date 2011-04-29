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
	checkarg(vm, "toupper", argv, 0, Qcval);
	cv = valcval(argv[0]);
	x = xtoupper(cv->val);
	*rv = mkvalcval(cv->dom, cv->type, x);
}

static void
l1_tolower(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	int x;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tolower");
	checkarg(vm, "tolower", argv, 0, Qcval);
	cv = valcval(argv[0]);
	x = xtolower(cv->val);
	*rv = mkvalcval(cv->dom, cv->type, x);
}

static void
l1_isxctype(VM *vm, Imm argc, Val *argv, Val *rv, char *name, int (*fn)(int))
{
	Cval *cv;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", name);
	checkarg(vm, name, argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(fn((int)cv->val))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isalnum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isalnum", xisalnum);
}

static void
l1_isalpha(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isalpha", xisalpha);
}

static void
l1_isascii(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isascii", xisascii);
}

static void
l1_isblank(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isblank", xisblank);
}

static void
l1_iscntrl(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "iscntrl", xiscntrl);
}

static void
l1_isdigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isdigit", xisdigit);
}

static void
l1_isgraph(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isgraph", xisgraph);
}

static void
l1_islower(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "islower", xislower);
}

static void
l1_isodigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isodigit", xisodigit);
}

static void
l1_isprint(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isprint", xisprint);
}

static void
l1_ispunct(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "ispunct", xispunct);
}

static void
l1_isspace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isspace", xisspace);
}

static void
l1_isupper(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isupper", xisupper);
}

static void
l1_isxdigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isxdigit", xisxdigit);
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