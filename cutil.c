#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

static Expr*
Z1(unsigned kind, Expr *e1)
{
	return newexpr(kind, e1, 0, 0, 0);
}

static Expr*
Z2(unsigned kind, Expr *e1, Expr *e2)
{
	return newexpr(kind, e1, e2, 0, 0);
}

Expr*
Zcons(Expr *hd, Expr *tl)
{
	return Z2(Eelist, hd, tl);
}

Expr*
Zset(Expr *l, Expr *r)
{
	return Z2(Eg, l, r);
}

Expr*
Zsizeof(Expr *e)
{
	return Z1(E_sizeof, e);
}

Expr*
Zxcast(Expr *type, Expr *cval)
{
	return newbinop(Excast, type, cval);
}

Expr*
Zbinop(unsigned op, Expr *x, Expr *y)
{
       return newbinop(op, x, y);
}

Expr*
Zadd(Expr *x, Expr *y)
{
	return Zbinop(Eadd, x, y);
}

Expr*
Zsub(Expr *x, Expr *y)
{
	return Zbinop(Esub, x, y);
}

/* arguments in usual order */
Expr*
Zcall(Expr *fn, unsigned narg, ...)
{
	Expr *e;
	va_list args;

	va_start(args, narg);
	e = nullelist();
	while(narg-- > 0)
		e = Zcons(va_arg(args, Expr*), e);
	va_end(args);
	return Z2(Ecall, fn, e);
}

/* arguments in usual order */
Expr*
Zapply(Expr *fn, Expr *args)
{
	return Z2(Ecall, fn, invert(args));
}

Expr*
Zconsts(char *s)
{
	Expr *e;
	e = newexpr(Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, strlen(s));
	return e;
}

Expr*
Zuint(Imm val)
{
	return mkconst(Vuint, val);
}

Expr*
Znil()
{
	Expr *e;
	e = newexpr(Enil, 0, 0, 0, 0);
	return e;
}

Expr*
Zstr(char *s)
{
	Expr *e;
	e = newexpr(Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, strlen(s));
	return e;
}

Expr*
Zcval(Expr *dom, Expr *type, Expr *val)
{
	return newexpr(E_cval, dom, type, val, 0);
}

Expr*
Zref(Expr *dom, Expr *type, Expr *val)
{
	return newexpr(E_ref, dom, type, val, 0);
}

Expr*
Zrange(Expr *addr, Expr *sz)
{
	return newbinop(E_range, addr, sz);
}

Expr*
Zencode(Expr *e)
{
	return Z1(E_encode, e);
}
