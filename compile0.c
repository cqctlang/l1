#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

enum {
	Vtmp	= 1<<1,
	Vtype	= 1<<2,
	Vrange	= 1<<3,
	Vaddr	= 1<<4,
	Vstr	= 1<<5,
};

typedef struct Var Var;
struct Var
{
	char *s;
	Var *link;
};

typedef
struct Vars
{
	unsigned long level;
	Var* var;
} Vars;

typedef
struct Varset
{
	char *tmp;
	char *type;
	char *range;
	char *addr;
	char *str;
} Varset;

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
Qcval(Expr *str, Expr *type)
{
	return newbinop(E_cval, str, type);
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
Qcar(Expr *e)
{
	return Q1(E_car, e);
}

static Expr*
Qcdr(Expr *e)
{
	return Q1(E_cdr, e);
}

static Expr*
Qencode(Expr *e)
{
	return Q1(E_encode, e);
}

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

static Vars*
mkvars()
{
	Vars *vars;
	vars = xmalloc(sizeof(Vars));
	return vars;
}

static char*
freshvar(Vars *vars, char *pref)
{
	Var *v;
	char *s;
	unsigned len;

	if(vars->level > 999)
		fatal("too many fresh variables");
	len = strlen(pref)+3+1;	/* 3 digits of level */
	s = xmalloc(len);
	snprintf(s, len, "%s%lu", pref, vars->level);
	v = xmalloc(sizeof(Var));
	v->s = s;
	v->link = vars->var;
	vars->var = v;
	return s;
}

static void
pushlevel(Vars *vars)
{
	vars->level++;
}

static void
poplevel(Vars *vars)
{
	vars->level--;
}

static void
freevars(Vars *vars)
{
	Var *v, *nv;
	v = vars->var;
	while(v){
		nv = v->link;
		free(v->s);
		free(v);
		v = nv;
	};
	free(vars);
}

static Varset*
mkvarset()
{
	Varset *vs;
	vs = xmalloc(sizeof(Varset));
	return vs;
}

static void
freevarset(Varset *vs)
{
	free(vs);
}

static Varset*
bindings(Vars *vars, Varset *outer, int req)
{
	Varset *vs;

	vs = mkvarset();
	if(outer == 0){
		if(req&Vtmp)
			vs->tmp = freshvar(vars, "!tmp");
		if(req&Vtype)
			vs->type = freshvar(vars, "!type");
		if(req&Vrange)
			vs->range = freshvar(vars, "!range");
		if(req&Vaddr)
			vs->addr = freshvar(vars, "!addr");
		if(req&Vstr)
			vs->str = freshvar(vars, "!str");
		return vs;
	}

	vs->tmp = outer->tmp ? outer->tmp : freshvar(vars, "!tmp");
	vs->type = outer->type ? outer->type : freshvar(vars, "!type");
	vs->range = outer->range ? outer->range : freshvar(vars, "!range");
	vs->addr = outer->addr ? outer->addr : freshvar(vars, "!addr");
	vs->str = outer->str ? outer->str : freshvar(vars, "!str");
	return vs;
}

static Expr*
locals(Varset *lvs, Varset *pvs)
{
	Expr *e;

	e = nullelist();

	if(pvs == 0){
		if(lvs->tmp)
			e = Qcons(doid(lvs->tmp), e);
		if(lvs->type)
			e = Qcons(doid(lvs->type), e);
		if(lvs->range)
			e = Qcons(doid(lvs->range), e);
		if(lvs->addr)
			e = Qcons(doid(lvs->addr), e);
		if(lvs->str)
			e = Qcons(doid(lvs->str), e);
	}else{
		if(lvs->tmp != pvs->tmp)
			e = Qcons(doid(lvs->tmp), e);
		if(lvs->type != pvs->type)
			e = Qcons(doid(lvs->type), e);
		if(lvs->range != pvs->range)
			e = Qcons(doid(lvs->range), e);
		if(lvs->addr != pvs->addr)
			e = Qcons(doid(lvs->addr), e);
		if(lvs->str != pvs->str)
			e = Qcons(doid(lvs->str), e);
	}

	/* local bindings are list of identifier lists */
	return Qcons(e, nullelist());
}

static int
isloc(Expr *e)
{
	switch(e->kind){
	case Etick:
		return 1;
	case Eptr:
		return 1;
	case Edot:
		return 1;
	default:
		return 0;
	}
}

static void
xcompile0(Expr *e, Varset *pvs, Vars *vars)
{
	Expr *se, *te;

	if(e == NULL)
		return;

	switch(e->kind){
	case Elambda:
	case Eblock:
		xcompile0(e->e2, pvs, vars);
		break;
	case Etick:
		e->kind = Eblock;

		/* local bindings are list of identifier lists */
		te = nullelist();
		te = newexpr(Eelist, doid("p"), te, 0, 0);
		te = newexpr(Eelist, doid("t"), te, 0, 0);
		te = newexpr(Eelist, doid("b"), te, 0, 0);
		e->e1 = newexpr(Eelist, te, nullelist(), 0, 0);

		te = nullelist();

		// p = dispatch(!looksym, <sym>);
		se = newexpr(Econsts, 0, 0, 0, 0);
		se->lits = e->lits;
		e->lits = 0;
		se = newexpr(Eelist, se, nullelist(), 0, 0);
		se = newexpr(Eelist, doid("!looksym"), se, 0, 0);
		se = newexpr(Ecall, doid("dispatch"), invert(se), 0, 0);
		se = newexpr(Eg, doid("p"), se, 0, 0);
		te = newexpr(Eelist, se, te, 0, 0);

		// t = car(p);
		se = newexpr(E_car, doid("p"), 0, 0, 0);
		se = newexpr(Eg, doid("t"), se, 0, 0);
		te = newexpr(Eelist, se, te, 0, 0);

		// b = dispatch(!get, range(cdr(p), sizeof(t)));
		se = newbinop(E_range,
			      newexpr(E_cdr, doid("p"), 0, 0, 0),
			      newexpr(E_sizeof, doid("t"), 0, 0, 0));
		se = newexpr(Eelist, se, nullelist(), 0, 0);
		se = newexpr(Eelist, doid("!get"), se, 0, 0);
		se = newexpr(Ecall, doid("dispatch"), invert(se), 0, 0);
		se = newexpr(Eg, doid("b"), se, 0, 0);
		te = newexpr(Eelist, se, te, 0, 0);
		
		// cval(b, t);
		se = newbinop(E_cval, doid("b"), doid("t"));
		te = newexpr(Eelist, se, te, 0, 0);

		e->e2 = invert(te);
		break;
	default:
		xcompile0(e->e1, pvs, vars);
		xcompile0(e->e2, pvs, vars);
		xcompile0(e->e3, pvs, vars);
		xcompile0(e->e4, pvs, vars);
		break;
	}
}

static void
compile0(Expr *e, Varset *pvs, Vars *vars, int needval)
{
	Expr *se, *te;
	Varset *lvs;
	int binds;

	if(e == NULL)
		return;

	switch(e->kind){
	case Elambda:
	case Eblock:
		compile0(e->e2, pvs, vars, needval);
		break;
	case Etick:
		// !p = dispatch(!looksym, sym)
		// !type = car(p);
		// !str = dispatch(!get, range(cdr(p), sizeof(t)));
		// cval(!str, !type);
		binds = Vtmp|Vtype|Vrange|Vaddr;
		if(needval)
			binds |= Vstr;

		lvs = bindings(vars, pvs, binds);

		te = nullelist();

		se = newexpr(Econsts, 0, 0, 0, 0);
		se->lits = e->lits;
		e->lits = 0;
		se = Qcall(doid("dispatch"), 2, doid("!looksym"), se);
		se = Qset(doid(lvs->tmp), se);
		te = Qcons(se, te);

		se = Qset(doid(lvs->type), Qcar(doid(lvs->tmp)));
		te = Qcons(se, te);

		se = Qset(doid(lvs->addr), Qcdr(doid(lvs->tmp)));
		te = Qcons(se, te);

		se = Qset(doid(lvs->range),
			  Qrange(doid(lvs->addr), Qsizeof(doid(lvs->type))));
		te = Qcons(se, te);

		if(needval){
			se = Qset(doid(lvs->str),
				  Qcall(doid("dispatch"), 2,
					doid("!get"),
					doid(lvs->range)));
			te = Qcons(se, te);
			se = Qcval(doid(lvs->str), doid(lvs->type));
			te = Qcons(se, te);
		}
		
		e->kind = Eblock;
		e->e1 = locals(lvs, pvs);
		e->e2 = invert(te);
		freevarset(lvs);
		break;
	case Eg:
		if(!isloc(e->e1)){
			compile0(e->e2, 0, vars, 1);
			break;
		}
		binds = Vtmp|Vtype|Vrange|Vaddr|Vstr;
		lvs = bindings(vars, pvs, binds);
		pushlevel(vars);

		te = nullelist();

		compile0(e->e1, lvs, vars, 0);
		se = e->e1;
		te = Qcons(se, te);

		compile0(e->e2, 0, vars, 1);
		se = Qset(doid(lvs->tmp), e->e2);
		te = Qcons(se, te);

		se = Qset(doid(lvs->tmp),
			  Qxcast(doid(lvs->type), doid(lvs->tmp)));
		te = Qcons(se, te);

		se = Qset(doid(lvs->str), Qencode(doid(lvs->tmp)));
		te = Qcons(se, te);

		se = Qcall(doid("dispatch"), 3,
			   doid("!put"),
			   doid(lvs->range),
			   doid(lvs->str));
		te = Qcons(se, te);

		if(needval){
			se = doid(lvs->tmp);
			te = Qcons(se, te);
		}

		e->kind = Eblock;
		e->e1 = locals(lvs, pvs);
		e->e2 = invert(te);
		freevarset(lvs);
		poplevel(vars);
		break;
	default:
		compile0(e->e1, pvs, vars, needval);
		compile0(e->e2, pvs, vars, needval);
		compile0(e->e3, pvs, vars, needval);
		compile0(e->e4, pvs, vars, needval);
		break;
	}
}

void
docompile0(Expr *e)
{
	Vars *vars;
	vars = mkvars();
	compile0(e, 0, vars, 1);
	freevars(vars);
}
