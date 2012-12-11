#include "sys.h"
#include "util.h"
#include "syscqct.h"

void
boot(VM *vm)
{
	char **lp, *p, *src;
	char bootfile[] = "boot.cqct";
	U ctx;
	Expr *e;
	Val fn;
	u32 len;

	lp = cqctloadpath;
	while(*lp){
		len = strlen(*lp)+1+strlen(bootfile)+1;
		p = emalloc(len);
		snprint(p, len, "%s/%s", *lp, bootfile);
		src = readfile(p);
		if(src)
			break;
		efree(p);
		lp++;
	}
	if(src == 0)
		fatal("boot: cannot read bootfile");
	initctxboot(&ctx, vm->top);
	if(setjmp(ctx.jmp) != 0)
		fatal("boot: parse error");
	e = doparse(&ctx, src, p, 1);
	efree(src);
	efree(p);
	if(e == 0)
		fatal("boot: parse error");
	fn = bootcompile(vm->top, e);
	if(fn == 0)
		fatal("boot: compile error");
	ccall(vm, valcl(fn), 0, 0);
	/* will throw error if it fails */
}
