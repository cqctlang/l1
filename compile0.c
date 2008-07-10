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
	Vas	= 1<<6,
	Vns	= 1<<7,
	Vdom	= 1<<8,	
	Vtypetab= 1<<9,	
	Vsymtab	= 1<<10,
	Vtn	= 1<<11,
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
	char *dom;
	char *as;
	char *ns;
	char *typetab;
	char *symtab;
	char *tn;
} Varset;

static void compile0(Expr *e, Varset *pvs, Vars *vars);

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
Qsizeof(Expr *e)
{
	return Q1(E_sizeof, e);
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
		if(req&Vas)
			vs->as = freshvar(vars, "$as");
		if(req&Vns)
			vs->ns = freshvar(vars, "$ns");
		if(req&Vdom)
			vs->dom = freshvar(vars, "$dom");
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
	vs->as = outer->as ? outer->as : freshvar(vars, "$as");
	vs->ns = outer->ns ? outer->ns : freshvar(vars, "$ns");
	vs->dom = outer->dom ? outer->dom : freshvar(vars, "$dom");
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
		if(lvs->as)
			e = Qcons(doid(lvs->as), e);
		if(lvs->ns)
			e = Qcons(doid(lvs->ns), e);
		if(lvs->dom)
			e = Qcons(doid(lvs->dom), e);
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
		if(lvs->as != pvs->as)
			e = Qcons(doid(lvs->as), e);
		if(lvs->ns != pvs->ns)
			e = Qcons(doid(lvs->ns), e);
		if(lvs->dom != pvs->dom)
			e = Qcons(doid(lvs->dom), e);
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
			e->xn = TBITS(t->kind, Vundef);
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
				compile0(dl->offs, lvs, vars);
				off = dl->offs; /* steal */
				dl->offs = 0;
			}else
				off = Qnil();
			tn = gentypename(dl->type, lvs, vars);
			se = Qcons(Qcall(doid("vector"), 3, tn, id, off), se);
			dl = dl->link;
		}
		se = Qapply(doid("vector"), invert(se));
		if(t->sz){
			compile0(t->sz, lvs, vars);
			sz = t->sz; /* steal */
			t->sz = 0;
		}else
			sz = Qnil();

		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(t->kind, Vundef);
		tn->e1 = Qstr(t->tag);
		se = Qcall(doid("vector"), 3, tn, se, sz);

		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(t->kind, Vundef);
		tn->e1 = Qstr(t->tag);

		te = nullelist();
		se = Qcall(doid("tabinsert"), 3, doid(lvs->typetab), tn, se);
		te = Qcons(se, te);
		
		e->xn = TBITS(t->kind, Vundef);
		e->e1 = Qstr(t->tag);
		te = Qcons(e, te);
		
		e = newexpr(Eblock, nullelist(), invert(te), 0, 0);
		break;
	case Tenum:
		e->xn = TBITS(t->kind, Vundef);
		e->e1 = Qstr(t->tag);
		fatal("incomplete support for enums");
		break;
	case Tptr:
	case Tarr:
	case Tfun:
		e->xn = TBITS(t->kind, Vundef);
		e->e1 = gentypename(t->link, lvs, vars);
		if(t->kind == Tarr){
			if(t->cnt){
				compile0(t->cnt, lvs, vars);
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
			e->e2 = Qapply(doid("vector"), invert(se));
		}
		break;
	case Ttypedef:
		e->xn = TBITS(t->kind, Vundef);
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
				compile0(dl->offs, lvs, vars);
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
		/* typedef T TID => typetab[typedef(TID)] = typename(T) */
		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(Ttypedef, Vundef);
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

static Expr*
compilesizeof(Decl *d, Varset *pvs, Vars *vars)
{
	Type *t;
	int binds;
	Varset *lvs;
	Expr *e, *se, *te;
	char *dom;

	t = d->type;
	if(t->dom)
		dom = t->dom;
	else
		dom = "litdom";

	binds = Vtmp|Vtn;
	lvs = bindings(vars, pvs, binds);
	pushlevel(vars);

	te = nullelist();

	// $tn = gentypename(t);
	se = Qset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Qcons(se, te);

	// $tmp = nslooktype(domns(dom))(type)
	se = Qcall(doid("domns"), 1, doid(dom));
	se = Qcall(doid("nslooktype"), 1, se);
	se = Qset(doid(lvs->tmp), Qcall(se, 1, doid(lvs->tn)));
	te = Qcons(se, te);
	
	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	se = newexpr(Eif,
		     Qcall(doid("isnil"), 1, doid(lvs->tmp)),
		     Qcall(doid("error"), 2,
			   Qconsts("undefined type: %t"),
			   doid(lvs->tn)),
		     0, 0);
	te = Qcons(se, te);

	// sizeof($tmp);
	se = Qsizeof(doid(lvs->tmp));
	te = Qcons(se, te);

	e = newexpr(Eblock, locals(lvs, pvs), invert(te), 0, 0);
	poplevel(vars);
	freevarset(lvs);
	return e;
}

static int
istypeform(Expr *e)
{
	switch(e->kind){
	case Ecast:
	case Esizeoft:
	case Etypeoft:
		return 1;
	default:
		return 0;
	}
}

static Expr*
compileambig(Expr *e, Varset *pvs, Vars *vars)
{
	Expr *tf, *of;
	Expr *se, *te;
	char *dom;
	Decl *d;
	Type *t;
	int binds;
	Varset *lvs;
		
	/* exactly one of the two parses is a form that operates on a
	   type name */
	tf = of = 0;
	if(istypeform(e->e1)){
		tf = e->e1;
		of = e->e2;
	}
	if(istypeform(e->e2)){
		if(tf)
			fatal("bug");
		tf = e->e2;
		of = e->e1;
	}
	if(tf == 0)
		fatal("bug");

	d = tf->e1->xp;
	t = d->type;

	if(t->dom)
		dom = t->dom;
	else
		dom = "litdom";
	
	binds = Vtmp|Vtn;
	lvs = bindings(vars, pvs, binds);
	pushlevel(vars);

	te = nullelist();

	// $tn = gentypename(t);
	se = Qset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Qcons(se, te);

	// $tmp = nslooktype(domns(dom))($tn)
	se = Qcall(doid("domns"), 1, doid(dom));
	se = Qcall(doid("nslooktype"), 1, se);
	se = Qset(doid(lvs->tmp), Qcall(se, 1, doid(lvs->tn)));
	te = Qcons(se, te);
	
	/* must we compile TF only after using all references to TF->...->type?
	   (including gentypename and dom?) */
	compile0(tf, lvs, vars);
	compile0(of, lvs, vars);

	// if(isnil($tmp)) <other form> else <type form>
	se = newexpr(Eif, Qcall(doid("isnil"), 1, doid(lvs->tmp)), of, tf, 0);
	te = Qcons(se, te);

	te = newexpr(Eblock, locals(lvs, pvs), invert(te), 0, 0);
	poplevel(vars);
	freevarset(lvs);
	return te;
}

static void
compile0(Expr *e, Varset *pvs, Vars *vars)
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
		compile0(e->e2, pvs, vars);
		break;
	case Edefine:
		se = newexpr(Elambda, e->e2, e->e3, copyexpr(e->e1), 0);
		compile0(se, pvs, vars);
		e->kind = Eg;
		e->e2 = se;
		e->e3 = 0;
		break;
	case Eambig:
		se = compileambig(e, pvs, vars);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
		break;
	case Esizeoft:
		se = compilesizeof(e->e1->xp, pvs, vars);
		freeexpr(e->e1);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
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
		compile0(e->e1, lvs, vars);
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
			ex = ex->e2;
		}

		freeexpr(e->e2);

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
		compile0(e->e1, pvs, vars);
		compile0(e->e2, pvs, vars);
		compile0(e->e3, pvs, vars);
		compile0(e->e4, pvs, vars);
		break;
	}
}

void
docompile0(Expr *e)
{
	Vars *vars;
	vars = mkvars();
	compile0(e, 0, vars);
	freevars(vars);
}
