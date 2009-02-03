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
	Toplevel *toplevel;
	char *s;

	toplevel = cqctinit(0, 0, NULL);
	vm = cqctmkvm(toplevel);
	e = cqctparsestr("3+3;", NULL);
	if(e == 0)
		goto out;
	entry = cqctcompile(e, toplevel, NULL);
	if(entry == 0){
		cqctfreeexpr(e);
		goto out;
	}
	/* now storage for E is managed by cqct storage manager */
	if(cqctcallthunk(vm, entry, &v))
		return 0;
	s = cqctsprintval(vm, v);
	printf("%s\n", s);
	efree(s);
out:
	cqctfreevm(vm);
	cqctfini(toplevel);
	return 0;
}
