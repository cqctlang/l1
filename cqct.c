#include "sys.h"
#include "util.h"
#include "syscqct.h"

char **cqctloadpath;
char cqctflags[256];

#if 0
static void
checkxp(Expr *e)
{
	Expr *p;
	if(e == 0)
		return;
	if(e->xp && e->kind != Ekon)
		fatal("compiler was untidy");
	switch(e->kind){
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			checkxp(p->e1);
			p = p->e2;
		}
		break;
	default:
		checkxp(e->e1);
		checkxp(e->e2);
		checkxp(e->e3);
		checkxp(e->e4);
		break;
	}
}
#endif

Expr*
cqctparse(char *s, Toplevel *top, char *src)
{
	U ctx;

	if(src == 0)
		src = "<stdin>";

	memset(&ctx, 0, sizeof(ctx));
	ctx.out = &top->out;
	return doparse(&ctx, s, src);
}

Val
cqctcompile(VM *vm, char *s, char *src, Toplevel *top, char *argsid)
{
	Expr *e;
	e = cqctparse(s, top, src);
	if(e == 0)
		return 0;
	return compile(vm, e, top, argsid);
}

int
cqcteval(VM *vm, char *s, char *src, Val *rv)
{
	Val v, cl;

	cl = cqctcompile(vm, s, src, vm->top, 0);
	if(cl == 0)
		return -1;
	if(rv == 0)
		rv = &v;
	if(cqctcallfn(vm, cl, 0, 0, rv))
		return -1;
	return 0;
}

void
cqctgcenable(VM *vm)
{
	gcenable();
}

void
cqctgcdisable(VM *vm)
{
	gcdisable();
}

void
cqctinterrupt(VM *vm)
{
	if(vm->flags&VMirq)
		fatal("hard interrupt");
	vm->flags |= VMirq;
}

Toplevel*
cqctinit(char **lp, Xfd *in, Xfd *out, Xfd *err)
{
	Xfd xfd[3];

	if(in == 0){
		in = &xfd[0];
		memset(in, 0, sizeof(Xfd));
		in->read = xfdread;
		in->fd = 0;
	}
	if(out == 0){
		out = &xfd[1];
		memset(out, 0, sizeof(Xfd));
		out->write = xfdwrite;
		out->fd = 1;
	}
	if(err == 0){
		err = &xfd[2];
		memset(err, 0, sizeof(Xfd));
		err->write = xfdwrite;
		err->fd = 2;
	}
	cqctloadpath = copystrv(lp);
	initmem();
	initparse();
	initcid();
	inittype();
	initcg();
	initvm();
	return mktoplevel(in, out, err);
}

void
cqctfini(Toplevel *top)
{
	efree(cqctloadpath);
	freetoplevel(top);
	finivm();
	finicg();
	finitype();
	finicid();
	finiparse();
	finimem();
}
