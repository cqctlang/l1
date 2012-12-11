#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
chknil(U *ctx, Expr *e)
{
	char *id;
	Expr *p;
	p = e;
	while(p->kind == Eelist){
		if(p->e1->kind != Eellipsis){
			id = idsym(p->e1);
			if(!strcmp(id, "nil"))
				cerror(ctx, p->e1, "attempt to rebind nil");

		}
		p = p->e2;
	}
}

/* turn any nil Eid reference to Enil.
   error any rebinding or redefinition of nil
   FIXME: update for defstx?  avoid need for such updates!
*/
static Expr*
xnil(U *ctx, Expr *e)
{
	Expr *p;
	char *id;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Estx:
		/* FIXME: this is only for @match stx patterns */
		sete2(e, xnil(ctx, e->e2));
		return e;
	case Eid:
		id = idsym(e);
		if(!strcmp(id, "nil"))
			return putsrc(Znil(), e->src);
		return e;
	case Eg:
		id = idsym(e->e1);
		if(!strcmp(id, "nil"))
			cerror(ctx, e, "attempt to redefine nil");
		sete2(e, xnil(ctx, e->e2));
		return e;
	case Eblock:
	case Elambda:
		chknil(ctx, e->e1);
		sete2(e, xnil(ctx, e->e2));
		return e;
	case Eglobal:
		chknil(ctx, e->e1);
		return e;
	case Edefine:
	case Edefloc:
		id = idsym(e->e1);
		if(!strcmp(id, "nil"))
			cerror(ctx, e, "attempt to redefine nil");
		chknil(ctx, e->e2);
		sete3(e, xnil(ctx, e->e3));
		return e;
	case Egoto:
	case Elabel:
		id = idsym(e->e1);
		if(!strcmp(id, "nil"))
			cerror(ctx, e, "attempt to treat nil as label");
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, xnil(ctx, p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, xnil(ctx, e->e1));
		sete2(e, xnil(ctx, e->e2));
		sete3(e, xnil(ctx, e->e3));
		sete4(e, xnil(ctx, e->e4));
		return e;
	}

}

Expr*
docompilek(U *ctx, Expr *e)
{
	e = xnil(ctx, e);
	return e;
}

void
l1_cpk(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpk");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompilek(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
