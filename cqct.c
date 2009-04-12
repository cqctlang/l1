#include "sys.h"
#include "util.h"
#include "syscqct.h"

char **cqctloadpath;
char cqctflags[256];

Expr*
cqctparsefile(char *filename)
{
	Expr *e;
	char *str;
	str = readfile(filename);
	if(str == 0)
		return 0;
	e = doparse(str, filename);
	efree(str);
	return e;
}

Expr*
cqctparsestr(char *str, char *whence)
{
	return doparse(str, whence);
}

Closure*
cqctcompile(Expr *e, Toplevel *top, char *argsid)
{
	U ctx;

	if(cqctflags['p']){
		xprintf("input:\n");
		printexpr(e);
		xprintf("\n");
	}
	memset(&ctx, 0, sizeof(ctx));
	dotypes(&ctx, e);
	if(docompilens(&ctx, e) != 0)
		return 0;
	if(docompilea(&ctx, e) != 0)
		return 0;
	if(cqctflags['a']){
		xprintf("compilea:\n");
		printcqct(e);
		xprintf("\n");
	}
	if(docompile0(&ctx, e) != 0)
		return 0;
	if(docompile1(&ctx, e) != 0)
		return 0;
	if(cqctflags['p']){
		xprintf("compile1:\n");
		printexpr(e);
		xprintf("\n");
	}
	e = docompile2(&ctx, e, top, argsid);
	if(e == 0)
		return 0;
	if(cqctflags['q']){
		xprintf("transformed source:\n");
		printcqct(e);
		xprintf("\n");
	}

	return codegen(e);
}

int
cqcteval(VM *vm, char *s, char *src, Val *rv)
{
	Closure *cl;
	Expr *e;
	Val v;

	e = cqctparsestr(s, src);
	if(e == 0)
		return -1;
	cl = cqctcompile(e, vm->top, 0);
	if(cl == 0){
		cqctfreeexpr(e);
		return -1;
	}
	if(rv == 0)
		rv = &v;
	if(cqctcallfn(vm, cl, 0, 0, rv))
		return -1;
	return 0;
}

static char**
copyargv(char **lp)
{
	unsigned n, i, nlp;
	char **p, **rv, *s;

	n = 0;
	nlp = 0;

	if(lp != 0){
		p = lp;
		while(*p){
			n += strlen(*p)+1;
			nlp++;
			p++;
		}
	}
	rv = emalloc((nlp+1)*sizeof(char*)+n);
	p = rv;
	s = (char*)(rv+nlp+1);
	for(i = 0; i < nlp; i++){
		p[i] = s;
		strcpy(s, lp[i]);
		s += strlen(lp[i])+1;
	}
	p[nlp] = 0;

	return rv;
}

void
cqctfreeexpr(Expr *e)
{
	freeexpr(e);
}

Toplevel*
cqctinit(int gcthread, u64 heapmax, char **lp)
{
	cqctloadpath = copyargv(lp);
	initparse();
	initcg();
	initvm(gcthread, heapmax);
	return mktoplevel();
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
