#include "sys.h"
#include "util.h"
#include "syscqct.h"

char **cqctloadpath;
char cqctflags[256];

static void
checkxp(Expr *e)
{
	Expr *p;
	if(e == 0)
		return;
	if(e->xp)
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

// return A-B in usec
static uint64_t
toddiff(struct timeval *a, struct timeval *b)
{
	uint64_t usec;
	usec = 1000000*(a->tv_sec-b->tv_sec);
	usec += a->tv_usec-b->tv_usec;
	return usec;
}

Val
cqctcompile(char *s, char *src, Toplevel *top, char *argsid)
{
	U ctx;
	Expr *e;
	Closure *cl;
	enum { Maxphase = 12 };
	char *phase[Maxphase];
	struct timeval tv[Maxphase];
	unsigned i, ntv;

	if(src == 0)
		src = "<stdin>";
	memset(&ctx, 0, sizeof(ctx));
	ctx.out = &top->out;

	ntv = 0;
	if(cqctflags['T']){
		phase[ntv] = "init";
		gettimeofday(&tv[ntv++], 0);
	}
	e = doparse(&ctx, s, src);
	if(e == 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "parse";
		gettimeofday(&tv[ntv++], 0);
	}
	if(cqctflags['p']){
		xprintf("input:\n");
		printexpr(e);
		xprintf("\n");
	}
	if(dotypes(&ctx, e) != 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "types";
		gettimeofday(&tv[ntv++], 0);
	}
	if(docompilens(&ctx, e) != 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "ns";
		gettimeofday(&tv[ntv++], 0);
	}
	if(docompilea(&ctx, e) != 0)
		return 0;
	if(cqctflags['a']){
		xprintf("compilea:\n");
		printcqct(e);
		xprintf("\n");
	}
	if(cqctflags['T']){
		phase[ntv] = "a";
		gettimeofday(&tv[ntv++], 0);
	}
	if(docompile0(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(cqctflags['T']){
		phase[ntv] = "0";
		gettimeofday(&tv[ntv++], 0);
	}
	if(docompile1(&ctx, e) != 0)
		return 0;
	if(cqctflags['p']){
		xprintf("compile1:\n");
		printexpr(e);
		xprintf("\n");
	}
	if(cqctflags['T']){
		phase[ntv] = "1";
		gettimeofday(&tv[ntv++], 0);
	}
	e = docompile2(&ctx, e, top, argsid);
	if(e == 0)
		return 0;
	checkxp(e);
	if(cqctflags['T']){
		phase[ntv] = "2";
		gettimeofday(&tv[ntv++], 0);
	}
	e = docompilev(&ctx, e, top);
	if(e == 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "v";
		gettimeofday(&tv[ntv++], 0);
	}
	if(cqctflags['q']){
		xprintf("transformed source:\n");
		printcqct(e);
		xprintf("\n");
	}

	cl = codegen(e);
	if(cqctflags['T']){
		phase[ntv] = "codegen";
		gettimeofday(&tv[ntv++], 0);
		for(i = 0; i < ntv-1; i++)
			printf("%-40s\t%16" PRIu64 " usec\n",
			       phase[i+1], toddiff(&tv[i+1], &tv[i]));
	}
	return mkvalcl(cl);
}

int
cqcteval(VM *vm, char *s, char *src, Val *rv)
{
	Val v, cl;

	cl = cqctcompile(s, src, vm->top, 0);
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
	gcenable(vm);
}

void
cqctgcdisable(VM *vm)
{
	gcdisable(vm);
}

void
cqctfreeexpr(Expr *e)
{
	freeexpr(e);
}

void
cqctinterrupt(VM *vm)
{
	if(vm->flags&VMirq)
		fatal("hard interrupt");
	vm->flags |= VMirq;
}

Toplevel*
cqctinit(int gcthread, u64 heapmax, char **lp, Xfd *in, Xfd *out, Xfd *err)
	 
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
	initparse();
	initcg();
	initvm(gcthread, heapmax);
	return mktoplevel(in, out, err);
}

void
cqctfini(Toplevel *top)
{
	efree(cqctloadpath);
	freetoplevel(top);
	finivm();
	finicg();
	finiparse();
}

