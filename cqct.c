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

static void
checksrc(char *pass, Expr *e)
{
	Expr *p;
	if(e == 0)
		return;
	if(e->src.filename == 0 || e->src.line == 0)
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

	if(np > Maxpass)
		fatal("bug");

	if(cqctflags['p']){
		xprintf("\n*** compiler input ***\n");
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

#define CP(id)      { "cp"#id, docompile##id }
#define NPASS(ps)   (sizeof(ps)/sizeof((ps)[0]))

static Pass all[] = {
	CP(q),
	CP(n),
	CP(a),
	CP(0),
	CP(g),
	CP(l),
	CP(i),
	CP(1),
	CP(b),
	CP(v),
};

Val
cqctcompile0(Expr *e, Toplevel *top, char *argsid)
{
	Imm tv[2];
	Closure *cl;

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

Expr*
cqctcompilex(Expr *e, Toplevel *top, char *argsid)
{
	static Pass front[] = {
		CP(q),
		CP(n),
		CP(a),
		CP(0),
		CP(g),
		CP(l),
		CP(i),
		CP(1),
	};
	return dopasses(e, top, argsid, front, NPASS(front));
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
