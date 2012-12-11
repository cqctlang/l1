#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
checksrc(char *pass, Expr *e)
{
	Expr *p;
	if(e == 0)
		return;
	if(e->src == 0)
		fatal("source information missing after pass %s", pass);
	switch(e->kind){
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			checksrc(pass, p->e1);
			p = p->e2;
		}
		break;
	default:
		checksrc(pass, e->e1);
		checksrc(pass, e->e2);
		checksrc(pass, e->e3);
		checksrc(pass, e->e4);
		break;
	}
}

typedef
struct Pass
{
	char *id;
	Expr* (*fn)(U *ctx, Expr *e);
} Pass;

enum
{
	Maxpass = 128
};

static void
printT(char *id, Imm t)
{
	xprintf("%-40s\t%16" PRIu64 " usec\n", id, t);
}

static Expr*
dopasses(U *ctx, Expr *e, Pass *ps, unsigned np)
{
	Pass *p;
	Imm tv[1+Maxpass];
	unsigned i;

	if(np >= Maxpass)
		fatal("bug");

	if(cqctflags['p']){
		xprintf("\n*** post-expand ***\n");
		printexpr(e);
		xprintf("\n");
	}
	if(cqctflags['K'])
		checksrc("parse", e);
	for(i = 0, p = ps; i < np; i++, p++){
		if(cqctflags['T'])
			tv[i] = usec();
		e = p->fn(ctx, e);
		if(e == 0)
			return 0;
		if(cqctflags['K'])
			checksrc(p->id, e);
		if(cqctflags['p']){
			xprintf("\n*** after %s ***\n", p->id);
			printexpr(e);
			xprintf("\n");
		}
	}
	if(cqctflags['T']){
		tv[np] = usec();
		for(i = 0, p = ps; i < np; i++, p++)
			printT(p->id, tv[i+1]-tv[i]);
	}
	return e;
}

#if 0
static Expr*
doexpand(VM *vm, Expr *e)
{
	U ctx;
	Imm bt, et;
	Val argv[1], rv, v;

	/* we need to fix Src handling to pass tests */
	v = cqctenvlook(vm->top, "expand");
	if(v && Vkind(v) == Qcl){
		argv[0] = mkvalexpr(e);
		if(0 > cqctcallfn(vm, v, 1, argv, &rv))
			return 0;
		if(Vkind(rv) != Qexpr)
			/* should call vmerr, but there is
			   no error context, like that
			   set by cqctcallfn */
			bug();
		return valexpr(rv);
	}

	memset(&ctx, 0, sizeof(ctx));
	ctx.out = &l1stderr;
	/* the other ctx fields are not used */

	if(cqctflags['p']){
		xprintf("\n*** post-expand ***\n");
		printexpr(e);
		xprintf("\n");
	}
	bt = et = 0;
	if(cqctflags['T'])
		bt = usec();
	e = docompilee(&ctx, e);
	if(cqctflags['T']){
		et = usec();
		printT("expand", et-bt);
	}
	return e;
}
#endif

#define CP(id)      { "cp"#id, docompile##id }
#define NPASS(ps)   (sizeof(ps)/sizeof((ps)[0]))
#define MKCP(id)    (mkvalcl(cqctmkcfn(#id, l1_##id)))

static Pass all[] = {
	CP(q),
	CP(x),
	CP(n),
	CP(m),
	CP(w),
	CP(a),
	CP(c),
	CP(0),
	CP(g),
	CP(k),
	CP(l),
	CP(i),
	CP(r),
	CP(1),
	CP(b),
};

void
l1_bootcompilerpasses(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to bootcompilerpasses");
	l = mklist();
	_listappend(l, MKCP(cpq));
	_listappend(l, MKCP(cpx));
	_listappend(l, MKCP(cpn));
	_listappend(l, MKCP(cpm));
	_listappend(l, MKCP(cpw));
	_listappend(l, MKCP(cpa));
	_listappend(l, MKCP(cpc));
	_listappend(l, MKCP(cp0));
	_listappend(l, MKCP(cpg));
	_listappend(l, MKCP(cpk));
	_listappend(l, MKCP(cpl));
	_listappend(l, MKCP(cpi));
	_listappend(l, MKCP(cpr));
	_listappend(l, MKCP(cp1));
	_listappend(l, MKCP(cpb));
	_listappend(l, MKCP(codegen));
	*rv = mkvallist(l);
}

void
fncompile(Env *env)
{
	FN(bootcompilerpasses);
}

void
initctxboot(U *ctx, Env *top)
{
	memset(ctx, 0, sizeof(U));
	ctx->top = top;
	ctx->out = &l1stderr;
}

void
initctx(U *ctx, VM *vm)
{
	memset(ctx, 0, sizeof(U));
	ctx->top = vm->top;
	ctx->vm = vm;
}

Val
bootcompile(Env *top, Expr *e)
{
	Closure *cl;
	U ctx;

	initctxboot(&ctx, top);
	if(setjmp(ctx.jmp) != 0)
		return 0;
	e = dopasses(&ctx, e, all, NPASS(all));
	if(e == 0)
		return 0;
	cl = codegen(&ctx, e);
	return mkvalcl(cl);
}
