#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

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
	char *val;
	char *type;
	char *range;
	char *addr;
} Varset;

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
	char *s;
	Var *v;
	unsigned len;

	if(vars->level > 999)
		fatal("too many fresh variables");
	len = strlen(s)+3+1;	/* 3 digits of level */
	s = xmalloc(len);
	snprintf(s, len, "%s%lu", pref, vars->level);
	v = xmalloc(sizeof(Var));
	v->s = s;
	v->link = vars->var;
	vars->var = v;
	return s;
}

static void
freevars(Vars *vars)
{
	Var *v, *nv;
	nv = vars->var;
	while(v){
		nv = v->link;
		free(v->s);
		free(v);
		v = nv;
	};
}

static Varset*
mkvarset(char *val, char *type, char *range, char *addr)
{
	Varset *vs;
	vs = xmalloc(sizeof(Varset));
	vs->val = val;
	vs->type = type;
	vs->range = range;
	vs->addr = addr;
	return vs;
}

static void
freevarset(Varset *vs)
{
	free(vs);
}

static Varset*
vsshadow(Varset *outer, Varset *inner)
{
	Varset *vs;

	vs = mkvarset(inner->val, inner->type, inner->range, inner->addr);
	if(vs->val == 0)
		vs->val = outer->val;
	if(vs->type == 0)
		vs->type = outer->type;
	if(vs->range == 0)
		vs->range = outer->range;
	if(vs->addr == 0)
		vs->addr = outer->addr;
	return vs;
}

static Expr*
vs2locals(Varset *vs)
{
	Expr *e;
	e = nullelist();
	if(vs->val)
		e = newexpr(Eelist, doid(vs->val), e, 0, 0);
	if(vs->type)
		e = newexpr(Eelist, doid(vs->type), e, 0, 0);
	if(vs->range)
		e = newexpr(Eelist, doid(vs->range), e, 0, 0);
	if(vs->addr)
		e = newexpr(Eelist, doid(vs->addr), e, 0, 0);
	return e;
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
	Varset *lvs, *vs;

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
	Varset *vs;

	vars = mkvars();
	vs = mkvarset(0, 0, 0, 0);
	compile0(e, vs, vars, 1);
	freevarset(vs);
	freevars(vars);
}
