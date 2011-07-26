#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr*
uncover(U *ctx, Expr *e)
{
	Expr *vs, *r, *s, *p;

	if(e == 0)
		fatal("bug");

	switch(e->kind){
	case Elambda:
	case Eblock:
		vs = uncover(ctx, e->e2);
		e->xp = vintersect(vs, e->e1);
		r = vdiff(vs, e->e1);
		return r;
	case Eg:
		r = uncover(ctx, e->e2);
		s = Zcons(copyexpr(e->e1), nullelist());
		vs = vunion(r, s);
		return vs;
	case Eelist:
		p = e;
		vs = nullelist();
		while(p->kind == Eelist){
			r = uncover(ctx, p->e1);
			s = vs;
			vs = vunion(r, s);
			p = p->e2;
		}
		return vs;
	default:
		if(e->e1)
			vs = uncover(ctx, e->e1);
		else
			vs = nullelist();
		if(e->e2){
			r = uncover(ctx, e->e2);
			vs = vunion(vs, r);
		}
		if(e->e3){
			r = uncover(ctx, e->e3);
			vs = vunion(vs, r);
		}
		if(e->e4){
			r = uncover(ctx, e->e4);
			vs = vunion(vs, r);
		}
		return vs;
	}
}

static Expr*
convert(U *ctx, Expr *e, Expr *vs)
{
	Expr *nvs, *se, *p, *nas, *nl, *t;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Elambda:
		if(!visempty(e->xp)){
			nvs = vunion(vs, e->xp);
			nas = nullelist();
			nl = nullelist();
			se = nullelist();
			p = e->e1;
			while(p->kind == Eelist){
				if(p->e1->kind != Eid
				   || !vmember(p->e1, e->xp)){
					nas = Zcons(copyexpr(p->e1), nas);
					p = p->e2;
					continue;
				}
				t = uniqid("t");
				nas = Zcons(t, nas);
				nl = Zcons(copyexpr(p->e1), nl);
				se = Zcons(Zset(copyexpr(p->e1),
						Zcall(Ztid("%box"),
						      1, copyexpr(t))),
					   se);
				p = p->e2;
			}
			e->e1 = invert(nas);
			e->e2 = Zblock(nl,
				       invert(se),
				       convert(ctx, e->e2, nvs),
				       NULL);
			putsrc(e->e2, &e->src);
//			e->e2 = flatten(e->e2);
		}else
			e->e2 = convert(ctx, e->e2, vs);
		e->xp = 0;
		return e;
	case Eblock:
		if(!visempty(e->xp)){
			nvs = vunion(vs, e->xp);
			se = nullelist();
			p = e->xp;
			while(p->kind == Eelist){
				se = Zcons(Zset(copyexpr(p->e1),
						Zcall(Ztid("%box"), 1, Znil())),
					   se);
				p = p->e2;
			}
			e->e2 = Zblock(nullelist(),
				       invert(se),
				       convert(ctx, e->e2, nvs),
				       NULL);
			putsrc(e->e2, &e->src);
//			e->e2 = flatten(e->e2);
		}else
			e->e2 = convert(ctx, e->e2, vs);
		e->xp = 0;
		return e;
	case Eid:
		if(vmember(e, vs)){
			se = Zcall(Ztid("%unbox"), 1, e);
			putsrc(se, &e->src);
			return se;
		}
		return e;
	case Eg:
		se = Zcall(Ztid("%setbox"), 2, e->e1, convert(ctx, e->e2, vs));
		putsrc(se, &e->src);
		return se;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = convert(ctx, p->e1, vs);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = convert(ctx, e->e1, vs);
		e->e2 = convert(ctx, e->e2, vs);
		e->e3 = convert(ctx, e->e3, vs);
		e->e4 = convert(ctx, e->e4, vs);
		return e;
	}
}

Expr*
docompilex(U *ctx, Expr *e)
{
	Expr *vs;
	if(setjmp(ctx->jmp) != 0)
		return 0;	/* error */
	vs = uncover(ctx, e);
	vs = nullelist();
	e = convert(ctx, e, vs);
	return e;
}
