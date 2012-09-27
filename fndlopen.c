#include "sys.h"
#include "util.h"
#include "syscqct.h"

#include <dlfcn.h>

void
l1_dlopen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char* name;
	void* handle;
	int mode;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to dlopen");
	checkarg(vm, argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	mode = (int)valimm(argv[1]);

	handle = dlopen(name, mode);

	efree(name);
	name = 0;

	*rv = mkvallitcval(Vulong, (Imm)handle);
}

void
l1_dlclose(VM *vm, Imm argc, Val *argv, Val *rv)
{
	void* handle;
	int ret;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to libclose");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qstr);

	handle = (void*)valimm(argv[0]);

	ret = dlclose(handle);

	*rv = mkvallitcval(Vint, ret);
}

void
l1_dlsym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	void* handle;
	void* addr;
	Str *ftnnm;
	char *ftn;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to ftnaddr");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qstr);

	handle = (void*)valimm(argv[0]);

	ftnnm = valstr(argv[1]);
	ftn = str2cstr(ftnnm);

	addr = dlsym(handle, ftn);

	efree(ftn);

	*rv = mkvallitcval(Vulong, (Imm)addr);
}

void
l1_dlerror(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *ret;

	if(argc != 0)
		vmerr(vm, "wrong number of arguments to ftnaddr");

	ret = dlerror();

	*rv = mkvalcval(litdom,
			mkctypeptr(mkctypebase(Vchar,
					       typerep(litdom->ns->base[Vptr])),
				   typerep(litdom->ns->base[Vptr])),
			(uptr)ret);
}

void
fndlopen(Env *env)
{
	FN(dlopen);
	FN(dlsym);
	FN(dlclose);
	FN(dlerror);
}
