#include "sys.h"
#include "util.h"
#include "cqct.h"

int
main(int argc, char *argv[])
{
	Closure *entry;
	Val v;
	Expr *e;
	VM *vm;
	Env *env;
	char *s;

	env = cqctinit();
	vm = cqctmkvm(env, 0);		/* gc in same thread */
	e = cqctparsestr(0, "3+3;");
	if(e == 0)
		goto out;
	entry = cqctcompile(e, env);
	if(entry == 0){
		cqctfreeexpr(e);
		goto out;
	}
	/* now storage for E is managed by cqct storage manager */
	if(cqctcallthunk(vm, entry, &v))
		return 0;
	s = cqctsprintval(vm, v);
	printf("%s\n", s);
	free(s);
out:
	cqctfreevm(vm);
	cqctfini(env);
	return 0;
}
