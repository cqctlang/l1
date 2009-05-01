#include "sys.h"
#include "util.h"
#include "syscqct.h"

char **cqctloadpath;
char cqctflags[256];

Closure*
cqctcompile(char *s, char *src, Toplevel *top, char *argsid)
{
	U ctx;
	Expr *e;

	if(src == 0)
		src = "<stdin>";
	memset(&ctx, 0, sizeof(ctx));
	ctx.xfd = &top->xfd;

	e = doparse(&ctx, s, src);
	if(e == 0)
		return 0;
	if(cqctflags['p']){
		xprintf("input:\n");
		printexpr(e);
		xprintf("\n");
	}
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
	Val v;

	cl = cqctcompile(s, src, vm->top, 0);
	if(cl == 0)
		return -1;
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

static uint64_t
xfdwrite(Xfd *xfd, char *buf, uint64_t len)
{
	return xwrite(1, buf, len);
}

Toplevel*
cqctinit(int gcthread, u64 heapmax, char **lp, Xfd *xfd)
{
	Xfd def;
	if(xfd == 0){
		memset(&def, 0, sizeof(def));
		def.write = xfdwrite;
		xfd = &def;
	}
	cqctloadpath = copyargv(lp);
	initparse();
	initcg();
	initvm(gcthread, heapmax);
	return mktoplevel(xfd);
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

void
cvprintf(Xfd *xfd, char *fmt, va_list args)
{
	int len;
	char *p;

	if(!xfd->write)
		return;
	/* hold your nose */
	len = vasprintf(&p, fmt, args);
	if(p == 0 || len < 0)
		xabort();
	xfd->write(xfd, p, len);
	free(p);
}

void
cprintf(Xfd *xfd, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	cvprintf(xfd, fmt, args);
	va_end(args);
}

