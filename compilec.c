#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

static jmp_buf esc;

static void cerror(Expr *e, char *fmt, ...) __attribute__((noreturn));

static void
cerror(Expr *e, char *fmt, ...)
{
	va_list args;
	fprintf(stderr, "%s:%u: ", e->src.filename, e->src.line);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	longjmp(esc, 1);
}

static Expr*
Q1(unsigned kind, Expr *e1)
{
	return newexpr(kind, e1, 0, 0, 0);
}

static Expr*
Q2(unsigned kind, Expr *e1, Expr *e2)
{
	return newexpr(kind, e1, e2, 0, 0);
}

static Expr*
Qcons(Expr *hd, Expr *tl)
{
	return Q2(Eelist, hd, tl);
}

static Expr*
Qset(Expr *l, Expr *r)
{
	return Q2(Eg, l, r);
}

static Expr*
Qcval(Expr *dom, Expr *type, Expr *val)
{
	return newexpr(E_cval, dom, type, val, 0);
}

static Expr*
Qref(Expr *dom, Expr *type, Expr *val)
{
	return newexpr(E_ref, dom, type, val, 0);
}

static Expr*
Qrange(Expr *addr, Expr *sz)
{
	return newbinop(E_range, addr, sz);
}

static Expr*
Qxcast(Expr *type, Expr *cval)
{
	return newbinop(E_xcast, type, cval);
}

static Expr*
Qsizeof(Expr *e)
{
	return Q1(E_sizeof, e);
}

static Expr*
Qencode(Expr *e)
{
	return Q1(E_encode, e);
}

/* arguments in usual order */
static Expr*
Qcall(Expr *fn, unsigned narg, ...)
{
	Expr *e;
	va_list args;

	va_start(args, narg);
	e = nullelist();
	while(narg-- > 0)
		e = Qcons(va_arg(args, Expr*), e);
	va_end(args);
	return Q2(Ecall, fn, e);
}

/* arguments in usual order */
static Expr*
Qapply(Expr *fn, Expr *args)
{
	return Q2(Ecall, fn, invert(args));
}

static Expr*
Qconsts(char *s)
{
	Expr *e;
	e = newexpr(Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, strlen(s));
	return e;
}

static Expr*
Quint(Imm val)
{
	return mkconst(Vuint, val);
}

static Expr*
Qnil()
{
	Expr *e;
	e = newexpr(Enil, 0, 0, 0, 0);
	return e;
}

static Expr*
Qstr(char *s)
{
	Expr *e;
	e = newexpr(Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, strlen(s));
	return e;
}

static int
islval(Expr *e)
{
	switch(e->kind){
	case Etick:
		return 1;
	case Ederef:
		return 1;
	case Edot:
		return 1;
	default:
		return 0;
	}
}

static Expr*
rvalblock(Expr *body)
{
	Expr *e;

        e = nullelist();
	e = Qcons(doid("$dom"), e);
	e = Qcons(doid("$type"), e);
	e = Qcons(doid("$addr"), e);
	e = Qcons(doid("$val"), e);

	/* local bindings are list of identifier lists */
	e = Qcons(e, nullelist());

	return newexpr(Eblock, e, body, 0, 0);
}

static Expr*
lvalblock(Expr *body)
{
	Expr *e;

        e = nullelist();
	e = Qcons(doid("$tmp"), e);

	/* local bindings are list of identifier lists */
	e = Qcons(e, nullelist());

	return newexpr(Eblock, e, body, 0, 0);
}

static Expr*
compile_lval(Expr *e)
{
	Expr *se, *te;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Etick:
		te = nullelist();

		// $dom = dom;
		se = Qset(doid("$dom"), e->e1);
		te = Qcons(se, te);

		// $tmp = nslooksym(domns($dom))(sym)
		se = Qcall(doid("domns"), 1, doid("$dom"));
		se = Qcall(doid("nslooksym"), 1, se);
		se = Qcall(se, 1, Qconsts(e->e2->id));
		se = Qset(doid("$tmp"), se);
		te = Qcons(se, te);
		
		// if(isnil($tmp)) error("undefined symbol %s", sym);
		se = newexpr(Eif,
			     Qcall(doid("isnil"), 1, doid("$tmp")),
			     Qcall(doid("error"), 2,
				   Qconsts("undefined symbol %s"),
				   Qconsts(e->e2->id)),
			     0, 0);
		te = Qcons(se, te);

		// $type = vecref($tmp, 0);
		se = Qset(doid("$type"),
			  Qcall(doid("vecref"), 2, doid("$tmp"), Quint(0)));
		te = Qcons(se, te);

		// $addr = vecref($tmp, 2);
		se = Qset(doid("$addr"),
			  Qcall(doid("vecref"), 2, doid("$tmp"), Quint(2)));
		te = Qcons(se, te);
		
		freeexpr(e->e2);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return lvalblock(invert(te));
	case Ederef:
		fatal("unimplemented");
	case Edot:
		fatal("unimplemented");
	default:
		cerror(e, "expression is not an lvalue");
	}
}

static Expr*
compile_rval(Expr *e)
{
	Expr *se, *te;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Etick:
		te = nullelist();
		se = compile_lval(e);
		te = Qcons(se, te);
		se = Qcval(doid("$dom"), doid("$type"), doid("$addr"));
		te = Qcons(se, te);
		return rvalblock(invert(te));
	case Eref:
		te = nullelist();
		se = compile_lval(e->e1);
		te = Qcons(se, te);
		se = Qref(doid("$dom"), doid("$type"), doid("$addr"));
		te = Qcons(se, te);
		e->e1 = 0;
		freeexpr(e);
		return rvalblock(invert(te));
	case Edot:
		fatal("unimplemented");
	case Eg:
		if(!islval(e->e1)){
			e->e1 = compile_rval(e->e1);
			e->e2 = compile_rval(e->e2);
			return e;
		}

		te = nullelist();

		se = compile_lval(e->e1);
		te = Qcons(se, te);

		se = Qset(doid("$val"), compile_rval(e->e2));
		te = Qcons(se, te);

		se = Qcall(doid("domas"), 1, doid("$dom"));
		se = Qcall(doid("asdispatch"), 1, se);
		se = Qcall(se, 3,
			   doid("$put"),
			   Qrange(doid("$addr"), Qsizeof(doid("$type"))),
			   Qencode(Qxcast(doid("$type"), doid("$val"))));
		te = Qcons(se, te);

		se = doid("$val");
		te = Qcons(se, te);

		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return rvalblock(invert(te));
	/* handle other cases: x++, x += y, etc for lvalues x */
	default:
		e->e1 = compile_rval(e->e1);
		e->e2 = compile_rval(e->e2);
		e->e3 = compile_rval(e->e3);
		e->e4 = compile_rval(e->e4);
		return e;
	}
}

static void
rewriteptr(Expr *e)
{
	if(e == 0)
		return;
	switch(e->kind){
	case Earef:
		/* rewrite: E1[E2] => *(E1+E2) */
		rewriteptr(e->e1);
		rewriteptr(e->e2);
		e->kind = Ederef;
		e->e1 = newexpr(Eadd, e->e1, e->e2, 0, 0);
		e->e2 = 0;
		break;
	case Earrow:
		/* rewrite: E->field => (*E).field */
		rewriteptr(e->e1);
		e->kind = Edot;
		e->e1 = newexpr(Ederef, e->e1, 0, 0, 0);
		break;
	default:
		rewriteptr(e->e1);
		rewriteptr(e->e2);
		rewriteptr(e->e3);
		rewriteptr(e->e4);
	}
}

Expr*
docompilec(Expr *e)
{
	Expr *rv;
	if(setjmp(esc) != 0)
		return 0;	/* error */
	rewriteptr(e);
	rv = compile_rval(e);
	return rv;
}
