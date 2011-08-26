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
dopasses(Expr *e, Toplevel *top, char *argsid, Pass *ps, unsigned np)
{
	U ctx;
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
	memset(&ctx, 0, sizeof(ctx));
	ctx.top = top;
	ctx.argsid = argsid;
	ctx.out = &top->out;
	for(i = 0, p = ps; i < np; i++, p++){
		if(cqctflags['T'])
			tv[i] = usec();
		e = p->fn(&ctx, e);
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
	ctx.out = &vm->top->out;
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


#define CP(id)      { "cp"#id, docompile##id }
#define NPASS(ps)   (sizeof(ps)/sizeof((ps)[0]))

static Pass all[] = {
	CP(q),
	CP(x),
	CP(n),
	CP(m),
	CP(a),
	CP(0),
	CP(g),
	CP(k),
	CP(l),
	CP(i),
	CP(1),
	CP(b),
	CP(v),
};

Val
compile(VM *vm, Expr *e, Toplevel *top, char *argsid)
{
	Imm tv[2];
	Closure *cl;

	e = doexpand(vm, e);
	if(e == 0)
		return 0;
	e = dopasses(e, top, argsid, all, NPASS(all));
	if(e == 0)
		return 0;
	if(cqctflags['T'])
		tv[0] = usec();
	if(cqctflags['q']){
		xprintf("*** input to code generator ***\n");
		printcqct(e);
		xprintf("\n");
	}
	cl = codegen(e);
	if(cqctflags['T']){
		tv[1] = usec();
		printT("cg", tv[1]-tv[0]);
	}
	return mkvalcl(cl);
}
