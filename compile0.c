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
	Vns	= 1<<6,
	Vtypetab= 1<<7,
	Vsymtab	= 1<<8,
	Vtn	= 1<<9,
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
	char *ns;
	char *typetab;
	char *symtab;
	char *tn;
} Varset;

static void compile0(Expr *e, Varset *pvs, Vars *vars, int needval);

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
			vs->tmp = freshvar(vars, "$tmp");
		if(req&Vtype)
			vs->type = freshvar(vars, "$type");
		if(req&Vrange)
			vs->range = freshvar(vars, "$range");
		if(req&Vaddr)
			vs->addr = freshvar(vars, "$addr");
		if(req&Vstr)
			vs->str = freshvar(vars, "$str");
		if(req&Vstr)
			vs->str = freshvar(vars, "$str");
		if(req&Vns)
			vs->ns = freshvar(vars, "$ns");
		if(req&Vtypetab)
			vs->typetab = freshvar(vars, "$typetab");
		if(req&Vsymtab)
			vs->symtab = freshvar(vars, "$symtab");
		if(req&Vtn)
			vs->tn = freshvar(vars, "$tn");
		return vs;
	}

	vs->tmp = outer->tmp ? outer->tmp : freshvar(vars, "$tmp");
	vs->type = outer->type ? outer->type : freshvar(vars, "$type");
	vs->range = outer->range ? outer->range : freshvar(vars, "$range");
	vs->addr = outer->addr ? outer->addr : freshvar(vars, "$addr");
	vs->str = outer->str ? outer->str : freshvar(vars, "$str");
	vs->ns = outer->ns ? outer->ns : freshvar(vars, "$ns");
	vs->typetab = (outer->typetab ? outer->typetab
		       : freshvar(vars, "$typetab"));
	vs->symtab = outer->symtab ? outer->symtab : freshvar(vars, "$symtab");
	vs->tn = outer->tn ? outer->tn : freshvar(vars, "$tn");
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
		if(lvs->ns)
			e = Qcons(doid(lvs->ns), e);
		if(lvs->typetab)
			e = Qcons(doid(lvs->typetab), e);
		if(lvs->symtab)
			e = Qcons(doid(lvs->symtab), e);
		if(lvs->tn)
			e = Qcons(doid(lvs->tn), e);
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
		if(lvs->ns != pvs->ns)
			e = Qcons(doid(lvs->ns), e);
		if(lvs->typetab != pvs->typetab)
			e = Qcons(doid(lvs->typetab), e);
		if(lvs->symtab != pvs->symtab)
			e = Qcons(doid(lvs->symtab), e);
		if(lvs->tn != pvs->tn)
			e = Qcons(doid(lvs->tn), e);
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

static Expr*
gentypename(Type *t, Varset *lvs, Vars *vars)
{
	Expr *e, *se, *te, *id, *off, *tn, *sz;
	Decl *dl;

	e = newexpr(E_tn, 0, 0, 0, 0);
	switch(t->kind){
	case Tbase:
		e->xn = TBITS(t->kind, t->base);
		break;
	case Tstruct:
	case Tunion:
		if(t->field == 0){
			e->xn = TBITS(t->kind, Vnil);
			e->e1 = Qstr(t->tag);
			break;
		}

		if(t->tag == 0)
			fatal("incomplete support for anonymous aggregates");

		se = nullelist();
		dl = t->field;
		while(dl){
			id = Qstr(dl->id);
			if(dl->offs){
				compile0(dl->offs, lvs, vars, 1);
				off = dl->offs; /* steal */
				dl->offs = 0;
			}else
				off = Qnil();
			tn = gentypename(dl->type, lvs, vars);
			se = Qcons(Qcall(doid("vector"), 3, tn, id, off), se);
			dl = dl->link;
		}
		se = Qapply(doid("vector"), se);
		if(t->sz){
			compile0(t->sz, lvs, vars, 1);
			sz = t->sz; /* steal */
			t->sz = 0;
		}else
			sz = Qnil();

		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(t->kind, Vnil);
		tn->e1 = Qstr(t->tag);
		se = Qcall(doid("vector"), 3, tn, se, sz);

		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(t->kind, Vnil);
		tn->e1 = Qstr(t->tag);

		te = nullelist();
		se = Qcall(doid("tabinsert"), 3, doid(lvs->typetab), tn, se);
		te = Qcons(se, te);
		
		e->xn = TBITS(t->kind, Vnil);
		e->e1 = Qstr(t->tag);
		te = Qcons(e, te);
		
		e = newexpr(Eblock, nullelist(), invert(te), 0, 0);
		break;
	case Tenum:
		e->xn = TBITS(t->kind, Vnil);
		e->e1 = Qstr(t->tag);
		fatal("incomplete support for enums");
		break;
	case Tptr:
	case Tarr:
	case Tfun:
		e->xn = TBITS(t->kind, Vnil);
		e->e1 = gentypename(t->link, lvs, vars);
		if(t->kind == Tarr){
			if(t->cnt){
				compile0(t->cnt, lvs, vars, 1);
				e->e2 = t->cnt; /* steal */
				t->cnt = 0;
			}else
				e->e2 = Qnil();
		}
		if(t->kind == Tfun){
			se = nullelist();
			dl = t->param;
			while(dl){
				if(dl->id)
					id = Qstr(dl->id);
				else
					id = Qnil();
				tn = gentypename(dl->type, lvs, vars);
				se = Qcons(Qcall(doid("vector"), 2, tn, id),
					   se);
				dl = dl->link;
			}
			e->e2 = Qapply(doid("vector"), se);
		}
		break;
	case Ttypedef:
		e->xn = TBITS(t->kind, Vnil);
		e->e1 = Qstr(t->tid);
		break;
	default:
		fatal("bug");
	}
	return e;
}

static Expr*
compiledecl(unsigned kind, Decl *dl, Varset *pvs, Vars *vars)
{
	Type *t;
	int binds;
	Varset *lvs;
	Expr *e, *offs, *se, *te, *tn;

	binds = Vtmp|Vtn;
	lvs = bindings(vars, pvs, binds);
	pushlevel(vars);

	te = nullelist();
	t = dl->type;
	se = Qset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Qcons(se, te);

	switch(kind){
	case Edecls:
		if(dl->id){
			if(dl->offs){
				compile0(dl->offs, lvs, vars, 1);
				offs = dl->offs; /* steal */
				dl->offs = 0;
			}else
				offs = Qnil();

			se = Qset(doid(lvs->tmp),
				  Qcall(doid("vector"), 3, doid(lvs->tn),
					Qstr(dl->id), offs));
			te = Qcons(se, te);

			se = Qcall(doid("tabinsert"), 3, doid(lvs->symtab),
				   Qstr(dl->id), doid(lvs->tmp));
			te = Qcons(se, te);
		}
		break;
	case Etypedef:
		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(Ttypedef, Vnil);
		tn->e1 = Qstr(dl->id);
		se = Qcall(doid("tabinsert"), 3, doid(lvs->typetab), tn, 
			   doid(lvs->tn));
		te = Qcons(se, te);
		break;
	default:
		fatal("bug");
	}

	e = newexpr(Eblock, locals(lvs, pvs), invert(te), 0, 0);
	poplevel(vars);
	freevarset(lvs);
	return e;
}

static void
compile0(Expr *e, Varset *pvs, Vars *vars, int needval)
{
	Expr *se, *te;
	Expr *ex;
	Decl *dl, *nxt;
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
		// $p = dispatch($looksym, sym)
		// $type = car(p);
		// $str = dispatch($get, range(cdr(p), sizeof(t)));
		// cval($str, $type);
		binds = Vtmp|Vtype|Vrange|Vaddr;
		if(needval)
			binds |= Vstr;

		lvs = bindings(vars, pvs, binds);

		te = nullelist();

		se = Qconsts(e->id);
		free(e->id);
		se = Qcall(doid("dispatch"), 2, doid("$looksym"), se);
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
					doid("$get"),
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
			   doid("$put"),
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
	case Ens:
		binds = Vns|Vtypetab|Vsymtab;
		lvs = bindings(vars, pvs, binds);
		pushlevel(vars);

		te = nullelist();

		se = Qcall(doid("table"), 0);
		se->src = e->src;
		se = Qset(doid(lvs->typetab), se);
		te = Qcons(se, te);
		se = Qset(doid(lvs->symtab), Qcall(doid("table"), 0));
		te = Qcons(se, te);

		/* inherited names expression */
		compile0(e->e1, lvs, vars, 0);
		se = Qset(doid(lvs->ns), e->e1);
		te = Qcons(se, te);

		/* declarations */
		ex = e->e2;
		if(ex->kind != Eelist && ex->kind != Enull)
			fatal("broken");
		while(ex->kind == Eelist){
			dl = ex->e1->xp;
			while(dl){
				se = compiledecl(ex->e1->kind, dl, lvs, vars);
				te = Qcons(se, te);
				nxt = dl->link;
				dl = nxt;
			}
			freedecl(ex->e1->xp, freeexprx);
			ex = ex->e2;
		}

		freeexpr(e->e2, 0);

		/* new name space */
		se = Qcall(doid("mkns"), 1,
			   Qcall(doid("vector"), 3,
				 doid(lvs->ns),
				 doid(lvs->typetab),
				 doid(lvs->symtab)));
		te = Qcons(se, te);

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
