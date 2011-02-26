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

Expr*
cqctcompilex(Expr *e, Toplevel *top, char *argsid)
{
	U ctx;

	memset(&ctx, 0, sizeof(ctx));
	ctx.out = &top->out;
	e = docompileq(&ctx, e);
	if(e == 0)
		return 0;
	if(dotypes(&ctx, e) != 0)
		return 0;
	if(docompilens(&ctx, e) != 0)
		return 0;
	if(docompilea(&ctx, e) != 0)
		return 0;
	if(docompile0(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(docompileg(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(docompilel(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(docompilei(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(docompile1(&ctx, e) != 0)
		return 0;
	return e;
	resetuniqid();
	e = docompileb(&ctx, e, top, argsid);
	if(e == 0)
		return 0;
	checkxp(e);
	e = docompileu(&ctx, e);
	if(e == 0)
		return 0;
	e = docompilex(&ctx, e);
	if(e == 0)
		return 0;
	e = docompilec(&ctx, e);
	if(e == 0)
		return 0;
	e = docompiles(&ctx, e);
	if(e == 0)
		return 0;
	return e;
}

Val
cqctcompile0(Expr *e, Toplevel *top, char *argsid)
{
	U ctx;
	Closure *cl;
	enum { Maxphase = 128 };
	char *phase[Maxphase];
	Imm tv[Maxphase];
	unsigned i, ntv;

	memset(&ctx, 0, sizeof(ctx));
	ctx.out = &top->out;
	if(cqctflags['p']){
		xprintf("input:\n");
		printexpr(e);
		xprintf("\n");
	}
	ntv = 0;
	if(cqctflags['T']){
		phase[ntv] = "init";
		tv[ntv++] = usec();
	}
	e = docompileq(&ctx, e);
	if(e == 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "q";
		tv[ntv++] = usec();
	}
	if(dotypes(&ctx, e) != 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "types";
		tv[ntv++] = usec();
	}
	if(docompilens(&ctx, e) != 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "ns";
		tv[ntv++] = usec();
	}
	if(docompilea(&ctx, e) != 0)
		return 0;
	if(cqctflags['T']){
		phase[ntv] = "a";
		tv[ntv++] = usec();
	}
	if(docompile0(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(cqctflags['T']){
		phase[ntv] = "0";
		tv[ntv++] = usec();
	}
	if(docompileg(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(cqctflags['T']){
		phase[ntv] = "g";
		tv[ntv++] = usec();
	}
	if(docompilel(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(cqctflags['T']){
		phase[ntv] = "l";
		tv[ntv++] = usec();
	}
	if(docompilei(&ctx, e) != 0){
		freeexpr(e);
		return 0;
	}
	if(cqctflags['T']){
		phase[ntv] = "i";
		tv[ntv++] = usec();
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
		tv[ntv++] = usec();
	}

	if(cqctflags['6']){
		resetuniqid();
		if(cqctflags['q']){
			xprintf("*** 1 ***\n");
			printcqct(e);
			xprintf("\n");
		}
		e = docompileb(&ctx, e, top, argsid);
		if(e == 0)
			return 0;
		checkxp(e);
		if(cqctflags['T']){
			phase[ntv] = "b";
			tv[ntv++] = usec();
		}
		if(cqctflags['q']){
			xprintf("*** b ***\n");
			printcqct(e);
			xprintf("\n");
		}
		e = docompileu(&ctx, e);
		if(e == 0)
			return 0;
		if(cqctflags['T']){
			phase[ntv] = "u";
			tv[ntv++] = usec();
		}
		if(cqctflags['q']){
			xprintf("*** u ***\n");
			printcqct(e);
			xprintf("\n");
		}
		e = docompilex(&ctx, e);
		if(e == 0)
			return 0;
		if(cqctflags['T']){
			phase[ntv] = "x";
			tv[ntv++] = usec();
		}
		if(cqctflags['q']){
			xprintf("*** x ***\n");
			printcqct(e);
			xprintf("\n");
		}
		e = docompilec(&ctx, e);
		if(e == 0)
			return 0;
		if(cqctflags['T']){
			phase[ntv] = "c";
			tv[ntv++] = usec();
		}
		if(cqctflags['q']){
			xprintf("*** c ****\n");
			printcqct(e);
			xprintf("\n");
		}
		e = docompiles(&ctx, e);
		if(e == 0)
			return 0;
		if(cqctflags['T']){
			phase[ntv] = "s";
			tv[ntv++] = usec();
		}
		if(cqctflags['q']){
			xprintf("*** s ***\n");
			printcqct(e);
			xprintf("\n");
		}
		return 0;
	}else{
		e = docompile2(&ctx, e, top, argsid);
		if(e == 0)
			return 0;
		checkxp(e);
		if(cqctflags['T']){
			phase[ntv] = "2";
			tv[ntv++] = usec();
		}
		e = docompilev(&ctx, e, top);
		if(e == 0)
			return 0;
		if(cqctflags['T']){
			phase[ntv] = "v";
			tv[ntv++] = usec();
		}
		if(cqctflags['q']){
			xprintf("transformed source:\n");
			printcqct(e);
			xprintf("\n");
		}

		cl = codegen(e);
		if(cqctflags['T']){
			phase[ntv] = "codegen";
			tv[ntv++] = usec();
			for(i = 0; i < ntv-1; i++)
				xprintf("%-40s\t%16" PRIu64 " usec\n",
					phase[i+1], tv[i+1]-tv[i]);
		}
		return mkvalcl(cl);
	}
}

Val
cqctcompile(char *s, char *src, Toplevel *top, char *argsid)
{
	Expr *e;
	e = cqctparse(s, top, src);
	if(e == 0)
		return 0;
	return cqctcompile0(e, top, argsid);
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
	gcenable();
}

void
cqctgcdisable(VM *vm)
{
	gcdisable();
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
cqctinit(int gcthread, u64 heapmax, char **lp,
	 Xfd *in, Xfd *out, Xfd *err)
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
	finimem();
}
