#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
Ztabget(Expr *r, char *f)
{
	if(r->kind != Eid)
		bug();
	return Zcall(G("tablook"), 2, r, Zcid(f));
}

/* expand (Ecall (Edot (Eid O) (Eid I)) ( ARG ...)) */
static Expr*
expandm(Expr *e)
{
	char *mid;
	Expr *te;
	Expr *o, *m, *a, *p;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Ecall:
		if(e->e1->kind != Edot
		   || e->e1->e1->kind != Eid
		   || e->e1->e2->kind != Eid){
			sete1(e, expandm(e->e1));
			sete2(e, expandm(e->e2));
			sete3(e, expandm(e->e3));
			sete4(e, expandm(e->e4));
			return e;
		}
		mid = idsym(e->e1->e2);
		if(!strcmp(mid, "ns") || !strcmp(mid, "as"))
			/* handled by cpa */
			return e;
		o = e->e1->e1;
		m = e->e1->e2;
		a = expandm(e->e2);
		te = Zifelse(Zcall(G("istable"), 1, o),
			     Zapply(Ztabget(copyexpr(o), idsym(m)), copyexpr(a)),
			     Zifelse(Zcall(G("isrec"), 1, copyexpr(o)),
				     Zapply(Zcall(Zcall(G("tablook"), 2,
							Zcall(G("rdgettab"), 1,
							      Zcall(G("rdof"), 1, copyexpr(o))),
							Zid2sym(m)),
						  1, copyexpr(o)),
					    copyexpr(a)),
				     Zifelse(Zcall(G("iscallable"), 1, copyexpr(o)),
					     Zapply(G("callmethodx"),
						    Zcons(o, Zcons(Zstr(idsym(m)), a))),
					     Zerror("invalid left operand to ."))));
		putsrc(te, e->src);
		return te;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			sete1(p, expandm(p->e1));
			p = p->e2;
		}
		return e;
	default:
		sete1(e, expandm(e->e1));
		sete2(e, expandm(e->e2));
		sete3(e, expandm(e->e3));
		sete4(e, expandm(e->e4));
		return e;
	}
}

Expr*
docompilem(U *ctx, Expr *e)
{
	return expandm(e);
}

void
l1_cpm(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cpm");
	checkarg(vm, argv, 0, Qexpr);
	initctx(&ctx, vm);
	e = docompilem(&ctx, valexpr(argv[0]));
	if(e)
		*rv = mkvalexpr(e);
}
