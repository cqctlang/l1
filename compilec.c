#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct Vs
{
	Expr *vs;
} Vs;

static void
vsinsert(Expr *e, Vs *vs)
{
	vs->vs = vinsert(e, vs->vs);
}

static void
vsunion(Vs *from, Vs *to)
{
	Expr *r;
	r = vunion(from->vs, to->vs);
	freeexpr(to->vs);
	to->vs = r;
}

static void
vsdiff(Vs *a, Expr *b)
{
	Expr *r;
	r = vdiff(a->vs, b);
	freeexpr(a->vs);
	a->vs = r;
}

static void
vsinit(Vs *vs)
{
	vs->vs = nullelist();
}

static void
vsfree(Vs *vs)
{
	freeexpr(vs->vs);
	vs->vs = 0;
}

static Expr*
letrec(U *ctx, Expr *e)
{
	Expr *p, *u, *se;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Elambda:
		u = uniqid("f");
		e->e2 = letrec(ctx, e->e2);
		se = Zletrec(Zcons(Zbind(u, e), nullelist()), copyexpr(u));
		putsrc(se, &e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = letrec(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = letrec(ctx, e->e1);
		e->e2 = letrec(ctx, e->e2);
		e->e3 = letrec(ctx, e->e3);
		e->e4 = letrec(ctx, e->e4);
		return e;
	}
}

static Expr*
uncoverfree(U *ctx, Expr *e, Vs *fs)
{
	Expr *p;
	Vs nfs;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eid:
		vsinsert(e, fs);
		return e;
	case Eblock:
		vsinit(&nfs);
		e->e2 = uncoverfree(ctx, e->e2, &nfs);
		vsdiff(&nfs, e->e1);
		vsunion(&nfs, fs);
		vsfree(&nfs);
		return e;
	case Elambda:
		vsinit(&nfs);
		e->e2 = uncoverfree(ctx, e->e2, &nfs);
		vsdiff(&nfs, e->e1);
		e->xp = copyexpr(nfs.vs);
		vsunion(&nfs, fs);
		vsfree(&nfs);

		xprintf("%s:%d @lambda(", e->src.filename, e->src.line);
		printids(e->e1);
		xprintf(") -> ");
		printids(e->xp);
		xprintf("\n");

		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = uncoverfree(ctx, p->e1, fs);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = uncoverfree(ctx, e->e1, fs);
		e->e2 = uncoverfree(ctx, e->e2, fs);
		e->e3 = uncoverfree(ctx, e->e3, fs);
		e->e4 = uncoverfree(ctx, e->e4, fs);
		return e;
	}

}

static Expr*
convert0(U *ctx, Expr *e)
{
	Expr *p, *u, *c, *cp, *se;
	unsigned i, nf;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Elambda:
		u = uniqid("f");
		c = uniqid("c");
		cp = uniqid("cp");
		nf = elistlen(e->xp);
		e->e1 = Zcons(cp, e->e1);
		e->e2 = convert0(ctx, e->e2);
		se = nullelist();
		p = e->xp;
		for(i = 0; i < nf; i++){
			se = Zcons(Zcall(Ztid("%clset"), 2,
					 Zuint(i), copyexpr(p->e1)),
				   se);
			p = p->e2;
		}
		se = Zblock(Zlocals(1, c->id),
			    Zset(copyexpr(c),
				 Zcall(Ztid("%mkcl"),
				       2, copyexpr(u), Zuint(nf))),
			    invert(se),
			    c,
			    NULL);
		se = Zletrec(Zcons(Zbind(u, e), nullelist()), se);
		putsrc(se, &e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = convert0(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = convert0(ctx, e->e1);
		e->e2 = convert0(ctx, e->e2);
		e->e3 = convert0(ctx, e->e3);
		e->e4 = convert0(ctx, e->e4);
		return e;
	}
}

static int
isprim(Expr *e)
{
	return e->kind == E_tid && e->id[0] == '%';
}

static Expr*
convert1(U *ctx, Expr *e)
{
	Expr *p, *se, *t;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Ecall:
		e->e2 = convert1(ctx, e->e2);
		switch(e->e1->kind){
		case Eid:
			e->e1 = Zcall(Ztid("%clcode"), 1, e->e1);
			e->e2 = Zcons(copyexpr(e->e1), e->e2);
			putsrc(e->e1, &e->src);
			putsrc(e->e2, &e->src);
			return e;
		case E_tid:
			if(isprim(e->e1))
				return e;
			e->e1 = Zcall(Ztid("%clcode"), 1, e->e1);
			e->e2 = Zcons(copyexpr(e->e1), e->e2);
			putsrc(e->e1, &e->src);
			putsrc(e->e2, &e->src);
			return e;
		default:
			t = uniqid("t");
			se = Zblock(Zlocals(1, t->id),
				    Zset(copyexpr(t), convert1(ctx, e->e1)),
				    Zapply(Zcall(Ztid("%clcode"),
						 1, copyexpr(t)),
					   Zcons(t, e->e2)),
				    NULL);
			e->e1 = 0;
			e->e2 = 0;
			putsrc(se, &e->src);
			freeexpr(e);
			return se;
		}
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = convert1(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = convert1(ctx, e->e1);
		e->e2 = convert1(ctx, e->e2);
		e->e3 = convert1(ctx, e->e3);
		e->e4 = convert1(ctx, e->e4);
		return e;
	}
}

static int
findex(Expr *id, Expr *fs)
{
	Expr *p;
	int i;
	i = 0; 
	p = fs;
	while(p->kind != Enull){
		if(!strcmp(id->id, p->e1->id))
			return i;
		i++;
		p = p->e2;
	}
	return -1;
}

static Expr*
convert2(U *ctx, Expr *e, Expr *cp, Expr *fs)
{
	Expr *p, *se;
	int i;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Eid:
		if(fs == 0)
			/* letrec label */
			return e;
		i = findex(e, fs);
		if(i < 0)
			return e;
		se = Zcall(Ztid("%clref"), 2, copyexpr(cp), Zuint(i));
		putsrc(se, &e->src);
		freeexpr(e);
		return se;
	case Elambda:
		e->e2 = convert2(ctx, e->e2, e->e1->e1, e->xp);
		freeexpr(e->xp);
		e->xp = 0;
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = convert2(ctx, p->e1, cp, fs);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = convert2(ctx, e->e1, cp, fs);
		e->e2 = convert2(ctx, e->e2, cp, fs);
		e->e3 = convert2(ctx, e->e3, cp, fs);
		e->e4 = convert2(ctx, e->e4, cp, fs);
		return e;
	}
}

Expr*
docompilec(U *ctx, Expr *e)
{
	Vs fs;
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	vsinit(&fs);
	e = uncoverfree(ctx, e, &fs);
	if(fs.vs->kind != Enull)
		fatal("free bug");
	vsfree(&fs);
	e = convert0(ctx, e);
	e = convert1(ctx, e);
	e = convert2(ctx, e, 0, 0);
	return e;
}
