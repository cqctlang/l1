#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

static jmp_buf esc;

static void cerror(Expr *e, char *fmt, ...) __attribute__((noreturn));
static void compile0(Expr *e);

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

static char* cbasector[Vnbase] = {
	[Vchar]               = "mkctype_char",
	[Vshort]	      = "mkctype_short",
	[Vint]		      = "mkctype_int",
	[Vlong]		      = "mkctype_long",
	[Vvlong]	      = "mkctype_vlong",
	[Vuchar]	      = "mkctype_uchar", 
	[Vushort]	      = "mkctype_ushort",
	[Vuint]		      = "mkctype_uint", 
	[Vulong]	      = "mkctype_ulong", 
	[Vuvlong]	      = "mkctype_uvlong",
	[Vfloat]	      = "mkctype_float",
	[Vdouble]	      = "mkctype_double",
	[Vlongdouble]	      = "mkctype_ldouble",
	[Vvoid]		      = "mkctype_void",
};

static Expr*
gentypename(Type *t)
{
	Expr *e, *se, *te, *id, *off, *tn, *sz, *loc;
	Enum *en;
	Decl *dl;
	char *mk;

	switch(t->kind){
	case Tbase:
		e = Zcall(doid(cbasector[t->base]), 0);
		break;
	case Ttypedef:
		e = Zcall(doid("mkctype_typedef"), 1, Zstr(t->tid));
		break;
	case Tstruct:
	case Tunion:
		mk = t->kind == Tstruct ? "mkctype_struct" : "mkctype_union";
		if(t->field == 0){
			e = Zcall(doid(mk), 1, Zstr(t->tag));
			break;
		}

		if(t->tag == 0)
			fatal("anonymous aggregates not implemented");

		se = nullelist();
		dl = t->field;
		while(dl){
			id = Zstr(dl->id);
			if(dl->offs && dl->type->bitw){
				/* bitfield */
				compile0(dl->offs);
				dl->type->bit0 =          /* steal */
					newbinop(Emod, dl->offs, Zuint(8));
				off = newbinop(Ediv,
					       copyexpr(dl->offs),
					       Zuint(8));
				dl->offs = 0;
			}else if(dl->offs){
				compile0(dl->offs);
				off = dl->offs; /* steal */
				dl->offs = 0;
			}else
				off = Znil();
			tn = gentypename(dl->type);
			se = Zcons(Zcall(doid("vector"), 3, tn, id, off), se);
			dl = dl->link;
		}
		se = Zapply(doid("vector"), invert(se));
		if(t->sz){
			compile0(t->sz);
			sz = t->sz; /* steal */
			t->sz = 0;
		}else
			sz = Znil();

		loc = Zlocals(1, "$tmp");

		te = nullelist();
		te = Zcons(Zset(doid("$tmp"),
				Zcall(doid(mk), 1, Zstr(t->tag))),
			   te);
		se = Zcall(doid("tabinsert"), 3,
			   doid("$typetab"), doid("$tmp"),
			   Zcall(doid(mk), 3, Zstr(t->tag), se, sz));
		te = Zcons(se, te);
		se = doid("$tmp");
		te = Zcons(se, te);
		e = newexpr(Eblock, loc, invert(te), 0, 0);
		break;
	case Tenum:
		if(t->en == 0){
			e = Zcall(doid("myctype_enum"), 1, Zstr(t->tag));
			break;
		}
		if(t->tag == 0)
			fatal("anonymous enums not implemented");

		loc = Zlocals(3, "$tmp", "$tn", "$type");
		te = nullelist();

		se = nullelist();
		en = t->en;
		while(en){
			se = Zcons(Zcall(doid("vector"), 2,
					 Zstr(en->id),
					 en->val),      /* steal */
				   se);
			en->val = 0; 
			en = en->link;
		}
		se = Zset(doid("$tmp"), Zapply(doid("vector"), invert(se)));
		te = Zcons(se, te);

		se = Zset(doid("$type"),
			  Zcall(doid("enconsts"), 2,
				doid("$ns"), doid("$tmp")));
		te = Zcons(se, te);

		se = Zset(doid("$tn"),
			  Zcall(doid("mkctype_enum"), 1, Zstr(t->tag)));
		te = Zcons(se, te);

		se = Zcall(doid("tabinsert"), 3,
			   doid("$typetab"),
			   doid("$tn"),
			   Zcall(doid("mkctype_enum"), 3,
				 Zstr(t->tag), doid("$type"), doid("$tmp")));
		te = Zcons(se, te);

		se = Zcall(doid("foreach"), 2,
			   Zlambda(Zargs(1, "$e"),
				   Zblock(Zlocals(3, "$ctn", "$id", "$val"),
					  Zset(doid("$ctn"),
					       Zcall(doid("mkctype_const"), 1,
						     doid("$tn"))),
					  Zset(doid("$id"),
					       Zcall(doid("vecref"), 2,
						     doid("$e"),Zuint(0))),
					  Zset(doid("$val"),
					       Zcall(doid("vecref"), 2,
						     doid("$e"),Zuint(1))),
					  Zcall(doid("tabinsert"), 3,
						doid("$symtab"),
						doid("$id"),
						Zcall(doid("vector"), 3,
						      doid("$ctn"),
						      doid("$id"),
						      doid("$val"))),
					  NULL)),
			   doid("$tmp"));
		te = Zcons(se, te);

		se = doid("$tn");
		te = Zcons(se, te);
		e = newexpr(Eblock, loc, invert(te), 0, 0);
		break;
	case Tptr:
		e = Zcall(doid("mkctype_ptr"), 1, gentypename(t->link));
		break;
	case Tarr:
		if(t->cnt){
			compile0(t->cnt);
			se = t->cnt; /* steal */
			t->cnt = 0;
			e = Zcall(doid("mkctype_array"), 2,
				  gentypename(t->link), se);
		}else
			e = Zcall(doid("mkctype_array"), 1,
				  gentypename(t->link)); 
		break;
	case Tfun:
		te = nullelist();
		dl = t->param;
		while(dl){
			if(dl->id)
				id = Zstr(dl->id);
			else
				id = Znil();
			tn = gentypename(dl->type);
			te = Zcons(Zcall(doid("vector"), 2, tn, id),
				   te);
			dl = dl->link;
		}
		e = Zcall(doid("mkctype_fn"), 2,
			  gentypename(t->link),
			  Zapply(doid("vector"), invert(te)));
		break;
	default:
		fatal("bug");
	}

	if(t->bitw){
		if(t->kind != Tbase && t->kind != Ttypedef)
			cerror(se, "invalid bitfield");
		compile0(t->bitw);
		compile0(t->bit0);
		e = Zcall(doid("mkctype_bitfield"), 3,
			  e, t->bitw, t->bit0);
		t->bitw = 0;	/* steal */
		t->bit0 = 0;	/* steal */
	}

	return e;
}

static Expr*
compiledecl(unsigned kind, Decl *dl)
{
	Type *t;
	Expr *e, *offs, *se, *te, *tn, *loc;

	loc = Zlocals(2, "$tmp", "$tn");

	te = nullelist();
	t = dl->type;
	se = Zset(doid("$tn"), gentypename(t));
	te = Zcons(se, te);

	switch(kind){
	case Edecls:
		if(dl->id){
			if(dl->offs){
				compile0(dl->offs);
				offs = dl->offs; /* steal */
				dl->offs = 0;
			}else
				offs = Znil();

			se = Zset(doid("$tmp"),
				  Zcall(doid("vector"), 3, doid("$tn"),
					Zstr(dl->id), offs));
			te = Zcons(se, te);

			se = Zcall(doid("tabinsert"), 3, doid("$symtab"),
				   Zstr(dl->id), doid("$tmp"));
			te = Zcons(se, te);
		}
		break;
	case Etypedef:
		/* typedef T TID => typetab[typedef(TID)] = typename(T) */
		tn = Zcall(doid("mkctype_typedef"), 1, Zstr(dl->id));
		se = Zcall(doid("tabinsert"), 3, doid("$typetab"), tn, 
			   doid("$tn"));
		te = Zcons(se, te);
		break;
	default:
		fatal("bug");
	}

	e = newexpr(Eblock, loc, invert(te), 0, 0);
	return e;
}

static Expr*
compilesizeof(Decl *d)
{
	Type *t;
	Expr *e, *se, *te, *loc;
	char *dom;

	t = d->type;
	if(t->dom)
		dom = t->dom;
	else
		dom = "litdom";

	loc = Zlocals(2, "$tn", "$tmp");

	te = nullelist();

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn);
	se = Zset(doid("$tmp"),
		  Zcall(doid("looktype"), 2, doid(dom), doid("$tn")));
	te = Zcons(se, te);

	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid("$tmp")),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// sizeof($tmp);
	se = Zsizeof(doid("$tmp"));
	te = Zcons(se, te);

	e = newexpr(Eblock, loc, invert(te), 0, 0);
	return e;
}

static Expr*
compiletypeof(Decl *d)
{
	Type *t;
	Expr *e, *se, *te, *loc;
	char *dom;

	t = d->type;
	if(t->dom)
		dom = t->dom;
	else
		dom = "litdom";

	loc = Zlocals(2, "$tn", "$tmp");

	te = nullelist();

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t));
	te = Zcons(se, te);

	// $tmp = looktype(dom, $tn);
	se = Zset(doid("$tmp"),
		  Zcall(doid("looktype"), 2, doid(dom), doid("$tn")));
	te = Zcons(se, te);
	
	// if(isnil($tmp)) error("undefined type: %t", $tmp);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid("$tmp")),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// $tmp;
	se = doid("$tmp");
	te = Zcons(se, te);

	e = newexpr(Eblock, loc, invert(te), 0, 0);
	return e;
}

static Expr*
compilecast(Expr *e)
{
	Type *t;
	Expr *se, *te, *dom, *loc;
	Decl *d;

	loc = Zlocals(3, "$tmp", "$tn", "$type");

	te = nullelist();
	
	// $tmp = e->e2;
	compile0(e->e2);
	se = Zset(doid("$tmp"), e->e2);
	te = Zcons(se, te);

	d = e->e1->xp;
	t = d->type;
	if(t->dom)
		dom = doid(t->dom);
	else
		dom = Zcall(doid("domof"), 1, doid("$tmp"));

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t));
	te = Zcons(se, te);

	// $type = looktype(dom, $tn);
	se = Zset(doid("$type"),
		  Zcall(doid("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);
	
	// if(isnil($type)) error("undefined type: %t", $tn);
	// FIXME: this is a redundant test under Eambig
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid("$type")),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// cast($type, $tmp);
	se = Zxcast(doid("$type"), doid("$tmp"));
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
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
compileambig(Expr *e)
{
	Expr *tf, *of;
	Expr *se, *te, *loc;
	char *dom;
	Decl *d;
	Type *t;
		
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

	loc = Zlocals(2, "$tmp", "$tn");

	te = nullelist();

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t));
	te = Zcons(se, te);

	// $tmp = nslooktype(domns(dom))($tn)
	se = Zcall(doid("domns"), 1, doid(dom));
	se = Zcall(doid("nslooktype"), 1, se);
	se = Zset(doid("$tmp"), Zcall(se, 1, doid("$tn")));
	te = Zcons(se, te);
	
	/* must we compile TF only after using all references to TF->...->type?
	   (including gentypename and dom?) */
	compile0(tf);
	compile0(of);

	// if(isnil($tmp)) <other form> else <type form>
	se = newexpr(Eif, Zcall(doid("isnil"), 1, doid("$tmp")), of, tf, 0);
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	return te;
}

static void
compile0(Expr *e)
{
	Expr *se, *te;
	Expr *ex;
	Expr *loc;
	Decl *dl, *nxt;

	if(e == NULL)
		return;

	switch(e->kind){
	case Elambda:
	case Eblock:
		compile0(e->e2);
		break;
	case Edefine:
		se = newexpr(Elambda, e->e2, e->e3, copyexpr(e->e1), 0);
		compile0(se);
		e->kind = Eg;
		e->e2 = se;
		e->e3 = 0;
		break;
	case Eambig:
		se = compileambig(e);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
		break;
	case Esizeoft:
		se = compilesizeof(e->e1->xp);
		freeexpr(e->e1);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
		break;
	case Etypeoft:
		se = compiletypeof(e->e1->xp);
		freeexpr(e->e1);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
		break;
	case Ecast:
		se = compilecast(e);
		freeexpr(e->e1);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
		break;
	case Elist:
		compile0(e->e2);
		se = Zapply(doid("list"), e->e1);
		e->kind = Eblock;
		e->e1 = nullelist();
		e->e2 = se;
		break;
	case Ens:
		loc = Zlocals(3, "$ns", "$typetab", "$symtab");

		te = nullelist();

		se = Zcall(doid("table"), 0);
		se->src = e->src;
		se = Zset(doid("$typetab"), se);
		te = Zcons(se, te);
		se = Zset(doid("$symtab"), Zcall(doid("table"), 0));
		te = Zcons(se, te);

		/* inherited names expression */
		compile0(e->e1);
		se = Zset(doid("$ns"), e->e1);
		te = Zcons(se, te);

		/* declarations */
		ex = e->e2;
		if(ex->kind != Eelist && ex->kind != Enull)
			fatal("broken");
		while(ex->kind == Eelist){
			dl = ex->e1->xp;
			while(dl){
				se = compiledecl(ex->e1->kind, dl);
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
				 doid("$ns"),
				 doid("$typetab"),
				 doid("$symtab")));
		te = Zcons(se, te);

		e->kind = Eblock;
		e->e1 = loc;
		e->e2 = invert(te);

		break;
	default:
		compile0(e->e1);
		compile0(e->e2);
		compile0(e->e3);
		compile0(e->e4);
		break;
	}
}

int
docompile0(Expr *e)
{
	if(setjmp(esc) != 0)
		return -1;	/* error */
	compile0(e);
	return 0;		/* success */
}
