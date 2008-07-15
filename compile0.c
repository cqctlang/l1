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

static jmp_buf esc;

static void cerror(Expr *e, char *fmt, ...) __attribute__((noreturn));
static void compile0(Expr *e, Varset *pvs, Vars *vars);

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
			e = Zcons(doid(lvs->tmp), e);
		if(lvs->type)
			e = Zcons(doid(lvs->type), e);
		if(lvs->range)
			e = Zcons(doid(lvs->range), e);
		if(lvs->addr)
			e = Zcons(doid(lvs->addr), e);
		if(lvs->str)
			e = Zcons(doid(lvs->str), e);
		if(lvs->as)
			e = Zcons(doid(lvs->as), e);
		if(lvs->ns)
			e = Zcons(doid(lvs->ns), e);
		if(lvs->dom)
			e = Zcons(doid(lvs->dom), e);
		if(lvs->typetab)
			e = Zcons(doid(lvs->typetab), e);
		if(lvs->symtab)
			e = Zcons(doid(lvs->symtab), e);
		if(lvs->tn)
			e = Zcons(doid(lvs->tn), e);
	}else{
		if(lvs->tmp != pvs->tmp)
			e = Zcons(doid(lvs->tmp), e);
		if(lvs->type != pvs->type)
			e = Zcons(doid(lvs->type), e);
		if(lvs->range != pvs->range)
			e = Zcons(doid(lvs->range), e);
		if(lvs->addr != pvs->addr)
			e = Zcons(doid(lvs->addr), e);
		if(lvs->str != pvs->str)
			e = Zcons(doid(lvs->str), e);
		if(lvs->as != pvs->as)
			e = Zcons(doid(lvs->as), e);
		if(lvs->ns != pvs->ns)
			e = Zcons(doid(lvs->ns), e);
		if(lvs->dom != pvs->dom)
			e = Zcons(doid(lvs->dom), e);
		if(lvs->typetab != pvs->typetab)
			e = Zcons(doid(lvs->typetab), e);
		if(lvs->symtab != pvs->symtab)
			e = Zcons(doid(lvs->symtab), e);
		if(lvs->tn != pvs->tn)
			e = Zcons(doid(lvs->tn), e);
	}

	/* local bindings are list of identifier lists */
	return Zcons(e, nullelist());
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
	case Ttypedef:
		e->xn = TBITS(t->kind, Vundef);
		e->e1 = Zstr(t->tid);
		break;
	case Tstruct:
	case Tunion:
		if(t->field == 0){
			e->xn = TBITS(t->kind, Vundef);
			e->e1 = Zstr(t->tag);
			break;
		}

		if(t->tag == 0)
			fatal("incomplete support for anonymous aggregates");

		se = nullelist();
		dl = t->field;
		while(dl){
			id = Zstr(dl->id);
			if(dl->offs && dl->type->bitw){
				/* bitfield */
				compile0(dl->offs, lvs, vars);
				dl->type->bit0 =          /* steal */
					newbinop(Emod, dl->offs, Zuint(8));
				off = newbinop(Ediv,
					       copyexpr(dl->offs),
					       Zuint(8));
				dl->offs = 0;
			}else if(dl->offs){
				compile0(dl->offs, lvs, vars);
				off = dl->offs; /* steal */
				dl->offs = 0;
			}else
				off = Znil();
			tn = gentypename(dl->type, lvs, vars);
			se = Zcons(Zcall(doid("vector"), 3, tn, id, off), se);
			dl = dl->link;
		}
		se = Zapply(doid("vector"), invert(se));
		if(t->sz){
			compile0(t->sz, lvs, vars);
			sz = t->sz; /* steal */
			t->sz = 0;
		}else
			sz = Znil();

		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(t->kind, Vundef);
		tn->e1 = Zstr(t->tag);
		se = Zcall(doid("vector"), 3, tn, se, sz);

		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(t->kind, Vundef);
		tn->e1 = Zstr(t->tag);

		te = nullelist();
		se = Zcall(doid("tabinsert"), 3, doid(lvs->typetab), tn, se);
		te = Zcons(se, te);
		
		e->xn = TBITS(t->kind, Vundef);
		e->e1 = Zstr(t->tag);
		te = Zcons(e, te);
		
		e = newexpr(Eblock, nullelist(), invert(te), 0, 0);
		break;
	case Tenum:
		e->xn = TBITS(t->kind, Vundef);
		e->e1 = Zstr(t->tag);
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
				e->e2 = Znil();
		}
		if(t->kind == Tfun){
			se = nullelist();
			dl = t->param;
			while(dl){
				if(dl->id)
					id = Zstr(dl->id);
				else
					id = Znil();
				tn = gentypename(dl->type, lvs, vars);
				se = Zcons(Zcall(doid("vector"), 2, tn, id),
					   se);
				dl = dl->link;
			}
			e->e2 = Zapply(doid("vector"), invert(se));
		}
		break;
	default:
		fatal("bug");
	}

	if(t->bitw){
		se = e;
		if(TBITSTYPE(se->xn) != Tbase && TBITSTYPE(se->xn) != Ttypedef)
			cerror(se, "invalid bitfield");
		e = newexpr(E_tn, 0, 0, 0, 0);
		e->xn = TBITS(Tbitfield, Vundef);
		e->e1 = se;
		e->e2 = t->bitw; /* steal */
		t->bitw = 0;
		e->e3 = t->bit0; /* steal */
		printf("bit0: ");
		printexpr(e->e3);
		printf("\n");
		t->bit0 = 0;
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
	se = Zset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Zcons(se, te);

	switch(kind){
	case Edecls:
		if(dl->id){
			if(dl->offs){
				compile0(dl->offs, lvs, vars);
				offs = dl->offs; /* steal */
				dl->offs = 0;
			}else
				offs = Znil();

			se = Zset(doid(lvs->tmp),
				  Zcall(doid("vector"), 3, doid(lvs->tn),
					Zstr(dl->id), offs));
			te = Zcons(se, te);

			se = Zcall(doid("tabinsert"), 3, doid(lvs->symtab),
				   Zstr(dl->id), doid(lvs->tmp));
			te = Zcons(se, te);
		}
		break;
	case Etypedef:
		/* typedef T TID => typetab[typedef(TID)] = typename(T) */
		tn = newexpr(E_tn, 0, 0, 0, 0);
		tn->xn = TBITS(Ttypedef, Vundef);
		tn->e1 = Zstr(dl->id);
		se = Zcall(doid("tabinsert"), 3, doid(lvs->typetab), tn, 
			   doid(lvs->tn));
		te = Zcons(se, te);
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
	se = Zset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn);
	se = Zset(doid(lvs->tmp),
		  Zcall(doid("looktype"), 2, doid(dom), doid(lvs->tn)));
	te = Zcons(se, te);

	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid(lvs->tmp)),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid(lvs->tn)),
		     0, 0);
	te = Zcons(se, te);

	// sizeof($tmp);
	se = Zsizeof(doid(lvs->tmp));
	te = Zcons(se, te);

	e = newexpr(Eblock, locals(lvs, pvs), invert(te), 0, 0);
	poplevel(vars);
	freevarset(lvs);
	return e;
}

static Expr*
compiletypeof(Decl *d, Varset *pvs, Vars *vars)
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
	se = Zset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn);
	se = Zset(doid(lvs->tmp),
		  Zcall(doid("looktype"), 2, doid(dom), doid(lvs->tn)));
	te = Zcons(se, te);
	
	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid(lvs->tmp)),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid(lvs->tn)),
		     0, 0);
	te = Zcons(se, te);

	// $tmp;
	se = doid(lvs->tmp);
	te = Zcons(se, te);

	e = newexpr(Eblock, locals(lvs, pvs), invert(te), 0, 0);
	poplevel(vars);
	freevarset(lvs);
	return e;
}

static Expr*
compilecast(Expr *e, Varset *pvs, Vars *vars)
{
	Type *t;
	int binds;
	Varset *lvs;
	Expr *se, *te, *dom;
	Decl *d;

	binds = Vtmp|Vtn|Vtype;
	lvs = bindings(vars, pvs, binds);
	pushlevel(vars);

	te = nullelist();
	
	// $tmp = e->e2;
	compile0(e->e2, lvs, vars);
	se = Zset(doid(lvs->tmp), e->e2);
	te = Zcons(se, te);

	d = e->e1->xp;
	t = d->type;
	if(t->dom)
		dom = doid(t->dom);
	else
		dom = Zcall(doid("domof"), 1, doid(lvs->tmp));

	// $tn = gentypename(t);
	se = Zset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Zcons(se, te);

	// $type = looktype(dom, $tn);
	se = Zset(doid(lvs->type),
		  Zcall(doid("looktype"), 2, dom, doid(lvs->tn)));
	te = Zcons(se, te);
	
	// if(isnil($type)) error("undefined type: %t", $tn);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid(lvs->type)),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid(lvs->tn)),
		     0, 0);
	te = Zcons(se, te);

	// cast($type, $tmp);
	se = Zxcast(doid(lvs->type), doid(lvs->tmp));
	te = Zcons(se, te);

	te = newexpr(Eblock, locals(lvs, pvs), invert(te), 0, 0);
	poplevel(vars);
	freevarset(lvs);
	return te;
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
	se = Zset(doid(lvs->tn), gentypename(t, lvs, vars));
	te = Zcons(se, te);

	// $tmp = nslooktype(domns(dom))($tn)
	se = Zcall(doid("domns"), 1, doid(dom));
	se = Zcall(doid("nslooktype"), 1, se);
	se = Zset(doid(lvs->tmp), Zcall(se, 1, doid(lvs->tn)));
	te = Zcons(se, te);
	
	/* must we compile TF only after using all references to TF->...->type?
	   (including gentypename and dom?) */
	compile0(tf, lvs, vars);
	compile0(of, lvs, vars);

	// if(isnil($tmp)) <other form> else <type form>
	se = newexpr(Eif, Zcall(doid("isnil"), 1, doid(lvs->tmp)), of, tf, 0);
	te = Zcons(se, te);

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
	case Etypeoft:
		se = compiletypeof(e->e1->xp, pvs, vars);
		freeexpr(e->e1);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
		break;
	case Ecast:
		se = compilecast(e, pvs, vars);
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

		se = Zcall(doid("table"), 0);
		se->src = e->src;
		se = Zset(doid(lvs->typetab), se);
		te = Zcons(se, te);
		se = Zset(doid(lvs->symtab), Zcall(doid("table"), 0));
		te = Zcons(se, te);

		/* inherited names expression */
		compile0(e->e1, lvs, vars);
		se = Zset(doid(lvs->ns), e->e1);
		te = Zcons(se, te);

		/* declarations */
		ex = e->e2;
		if(ex->kind != Eelist && ex->kind != Enull)
			fatal("broken");
		while(ex->kind == Eelist){
			dl = ex->e1->xp;
			while(dl){
				se = compiledecl(ex->e1->kind, dl, lvs, vars);
				te = Zcons(se, te);
				nxt = dl->link;
				dl = nxt;
			}
			ex = ex->e2;
		}

		freeexpr(e->e2);

		/* new name space */
		se = Zcall(doid("mkns"), 1,
			   Zcall(doid("vector"), 3,
				 doid(lvs->ns),
				 doid(lvs->typetab),
				 doid(lvs->symtab)));
		te = Zcons(se, te);

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

int
docompile0(Expr *e)
{
	Vars *vars;
	vars = mkvars();
	if(setjmp(esc) != 0){
		freevars(vars);
		return -1;	/* error */
	}
	compile0(e, 0, vars);
	freevars(vars);
	return 0;		/* success */
}
