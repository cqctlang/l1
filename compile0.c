#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

static jmp_buf esc;

static void cerror(Expr *e, char *fmt, ...) __attribute__((noreturn));
static Expr* compile0(Expr *e);

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
};

Expr*
gentypename(Type *t, Pass *recpass)
{
	Expr *e, *se, *te, *id, *off, *tn, *sz, *loc;
	Enum *en;
	Decl *dl;
	char *mk;

	switch(t->kind){
	case Tvoid:
		e = Zcall(doid("mkctype_void"), 0);
		break;	
	case Tbase:
		e = Zcall(doid(cbasector[t->base]), 0);
		break;
	case Ttypedef:
		e = Zcall(doid("mkctype_typedef"), 1, Zstr(t->tid));
		break;
	case Tstruct:
	case Tunion:
		mk = t->kind == Tstruct ? "mkctype_struct" : "mkctype_union";
		if(t->field == 0 && t->sz == 0){
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
				dl->offs = recpass(dl->offs);
				dl->type->bit0 =          /* steal */
					newbinop(Emod, dl->offs, Zuint(8));
				off = newbinop(Ediv,
					       copyexpr(dl->offs),
					       Zuint(8));
				dl->offs = 0;
			}else if(dl->offs){
				dl->offs = recpass(dl->offs);
				off = dl->offs; /* steal */
				dl->offs = 0;
			}else
				off = Znil();
			tn = gentypename(dl->type, recpass);
			se = Zcons(Zcall(doid("vector"), 3, tn, id, off), se);
			dl = dl->link;
		}
		se = Zapply(doid("vector"), invert(se));
		if(t->sz){
			t->sz = recpass(t->sz);
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
			e = Zcall(doid("mkctype_enum"), 1, Zstr(t->tag));
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
		e = Zcall(doid("mkctype_ptr"), 1,
			  gentypename(t->link, recpass));
		break;
	case Tarr:
		if(t->cnt){
			t->cnt = recpass(t->cnt);
			se = t->cnt; /* steal */
			t->cnt = 0;
			e = Zcall(doid("mkctype_array"), 2,
				  gentypename(t->link, recpass), se);
		}else
			e = Zcall(doid("mkctype_array"), 1,
				  gentypename(t->link, recpass)); 
		break;
	case Tfun:
		te = nullelist();
		dl = t->param;
		while(dl){
			if(dl->id)
				id = Zstr(dl->id);
			else
				id = Znil();
			tn = gentypename(dl->type, recpass);
			te = Zcons(Zcall(doid("vector"), 2, tn, id),
				   te);
			dl = dl->link;
		}
		e = Zcall(doid("mkctype_fn"), 2,
			  gentypename(t->link, recpass),
			  Zapply(doid("vector"), invert(te)));
		break;
	default:
		fatal("bug");
	}

	if(t->bitw){
		if(t->kind != Tbase && t->kind != Ttypedef)
			cerror(se, "invalid bitfield");
		t->bitw = recpass(t->bitw);
		t->bit0 = recpass(t->bit0);
		e = Zcall(doid("mkctype_bitfield"), 3,
			  e, t->bitw, t->bit0);
		t->bitw = 0;	/* steal */
		t->bit0 = 0;	/* steal */
	}

	return e;
}

static void
do1tag(void *u, char *k, void *v)
{
	Expr *se, *te;
	Expr **e;
	Decl *d;

	e = u;
	d = v;

	te = nullelist();
	se = gentypename(d->type, compile0);
	te = Zcons(se, te);

	te = newexpr(Eblock, nullelist(), invert(te), 0, 0);
	*e = Zcons(te, *e);
}

static void
do1sym(void *u, char *k, void *v)
{
	Expr *se, *te, *loc, *offs;
	Expr **e;
	Decl *d;

	e = u;
	d = v;

	loc = Zlocals(2, "$tmp", "$tn");

	te = nullelist();
	se = Zset(doid("$tn"), gentypename(d->type, compile0));
	te = Zcons(se, te);

	if(d->offs){
		d->offs = compile0(d->offs);
		offs = d->offs; /* steal */
		d->offs = 0;
	}else
		offs = Znil();

	se = Zset(doid("$tmp"),
		  Zcall(doid("vector"), 3, doid("$tn"),
			Zstr(d->id), offs));
	te = Zcons(se, te);

	se = Zcall(doid("tabinsert"), 3, doid("$symtab"),
		   Zstr(d->id), doid("$tmp"));
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	*e = Zcons(te, *e);
}

static void
do1tid(void *u, char *k, void *v)
{
	Expr *se, *te, *tn, *loc;
	Expr **e;
	Decl *d;

	e = u;
	d = v;

	loc = Zlocals(1, "$tn");

	te = nullelist();
	se = Zset(doid("$tn"), gentypename(d->type, compile0));
	te = Zcons(se, te);

	/* typedef T TID => typetab[typedef(TID)] = typename(T) */
	tn = Zcall(doid("mkctype_typedef"), 1, Zstr(d->id));
	se = Zcall(doid("tabinsert"), 3, doid("$typetab"), tn, 
		   doid("$tn"));
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	*e = Zcons(te, *e);
}

static void
hashdecl(unsigned kind, Decl *d, HT *sym, HT *tag, HT *tid)
{
	Type *t;
	static char *err = "anonymous tagged types not implemented";

	switch(kind){
	case Edecls:
		if(d->id)
			hput(sym, d->id, strlen(d->id), d);
		else{
			t = d->type;
			if((t->kind == Tstruct || t->kind == Tunion)
			   && (t->field != 0 || t->sz != 0)){
				if(t->tag == 0)
					fatal(err);
				hput(tag, t->tag, strlen(t->tag), d);
			}
			if(t->kind == Tenum && t->en != 0){
				if(t->tag == 0)
					fatal(err);
				hput(tag, t->tag, strlen(t->tag), d);
			}
		}
		break;
	case Etypedef:
		hput(tid, d->id, strlen(d->id), d);
		break;	
	default:
		fatal("bug");
	}
}

static Expr*
compilens(Expr *e)
{
	Expr *se, *te;
	Expr *ex;
	Expr *loc;
	Decl *dl;
	HT *sym, *tag, *tid;

	loc = Zlocals(3, "$ns", "$typetab", "$symtab");

	te = nullelist();

	se = Zcall(doid("table"), 0);
	se->src = e->src;
	se = Zset(doid("$typetab"), se);
	te = Zcons(se, te);
	se = Zset(doid("$symtab"), Zcall(doid("table"), 0));
	te = Zcons(se, te);

	/* inherited names expression */
	e->e1 = compile0(e->e1);
	se = Zset(doid("$ns"), e->e1);
	te = Zcons(se, te);

	/* declarations */
	ex = e->e2;
	if(ex->kind != Eelist && ex->kind != Enull)
		fatal("broken");
	sym = mkht();
	tag = mkht();
	tid = mkht();
	while(ex->kind == Eelist){
		dl = ex->e1->xp;
		while(dl){
			hashdecl(ex->e1->kind, dl, sym, tag, tid);
			dl = dl->link;
		}
		ex = ex->e2;
	}
	hforeach(sym, do1sym, &te);
	hforeach(tag, do1tag, &te);
	hforeach(tid, do1tid, &te);
	freeht(sym);
	freeht(tag);
	freeht(tid);
	freeexpr(e->e2);

	/* new name space */
	se = Zcall(doid("mkns"), 1,
		   Zcall(doid("vector"), 3,
			 doid("$ns"),
			 doid("$typetab"),
			 doid("$symtab")));
	te = Zcons(se, te);

	return newexpr(Eblock, loc, invert(te), 0, 0);
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
	se = Zset(doid("$tn"), gentypename(t, compile0));
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
	se = Zset(doid("$tn"), gentypename(t, compile0));
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
	e->e2 = compile0(e->e2);
	se = Zset(doid("$tmp"), e->e2);
	te = Zcons(se, te);

	d = e->e1->xp;
	t = d->type;
	if(t->dom)
		dom = doid(t->dom);
	else
		dom = Zcall(doid("domof"), 1, doid("$tmp"));

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t, compile0));
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

static Expr*
compilecontainer(Expr *e)
{
	Type *t;
	Expr *se, *te, *dom, *loc;
	Decl *d;

	te = nullelist();

	loc = Zlocals(4, "$tn", "$fld", "$ptype", "$type", "$tmp");

	// $tmp = e->e1;
	e->e1 = compile0(e->e1);
	se = Zset(doid("$tmp"), e->e1);
	te = Zcons(se, te);

	// pick domain in which to lookup type
	d = e->e2->xp;
	t = d->type;
	if(t->dom)
		dom = doid(t->dom);
	else
		dom = Zcall(doid("domof"), 1, doid("$tmp"));

	// $tn = gentypename(t);
	se = Zset(doid("$tn"), gentypename(t, compile0));
	te = Zcons(se, te);

	// $type = looktype(dom, $tn);
	se = Zset(doid("$type"),
		  Zcall(doid("looktype"), 2, dom, doid("$tn")));
	te = Zcons(se, te);
	
	// if(isnil($type)) error("undefined type: %t", $tn);
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid("$type")),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined type: %t"),
			   doid("$tn")),
		     0, 0);
	te = Zcons(se, te);

	// $fld = lookfield($type, field);
	se = Zset(doid("$fld"),
		  Zcall(doid("lookfield"), 2,
			doid("$type"), Zconsts(e->e3->id)));
	te = Zcons(se, te);

	// if(isnil($fld)) error("undefined field: %s", sym);
	se = newexpr(Eif,
		     Zcall(doid("isnil"), 1, doid("$fld")),
		     Zcall(doid("error"), 2,
			   Zconsts("undefined field: %s"),
			   Zconsts(e->e3->id)),
		     0, 0);
	te = Zcons(se, te);

	// $ptype = nsptr(domof($tmp));
	se = Zset(doid("$ptype"),
		  Zcall(doid("nsptr"), 1,
			Zcall(doid("domof"), 1, doid("$tmp"))));
	te = Zcons(se, te);

	// FIXME: maybe it be simpler to rewrite at higher level
	//        where type names can be reused:
	//        (typename*)((void*)p - fieldoff(looktype(typename)))
	//        but note that above draws final pointer type definition
	//        from typename domain, whereas here we always draw from
	//        P's domain.
	// {mkctype_ptr($type,$ptype)} ({$ptype}$tmp - fieldoff($fld))
	se = Zxcast(Zcall(doid("mkctype_ptr"), 2,
			  doid("$type"), doid("$ptype")),
		    Zsub(Zxcast(doid("$ptype"), doid("$tmp")),
			 Zcall(doid("fieldoff"), 1, doid("$fld"))));
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
	se = Zset(doid("$tn"), gentypename(t, compile0));
	te = Zcons(se, te);

	// $tmp = nslooktype(domns(dom))($tn)
	se = Zcall(doid("domns"), 1, doid(dom));
	se = Zcall(doid("nslooktype"), 1, se);
	se = Zset(doid("$tmp"), Zcall(se, 1, doid("$tn")));
	te = Zcons(se, te);
	
	/* must we compile TF only after using all references to TF->...->type?
	   (including gentypename and dom?) */
	tf = compile0(tf);
	of = compile0(of);

	// if(isnil($tmp)) <other form> else <type form>
	se = newexpr(Eif, Zcall(doid("isnil"), 1, doid("$tmp")), of, tf, 0);
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	return te;
}

static Expr*
compile0(Expr *e)
{
	Expr *se;

	if(e == NULL)
		return e;

	switch(e->kind){
	case Elambda:
	case Eblock:
		e->e2 = compile0(e->e2);
		return e;
	case Edefine:
		se = Zset(e->e1,
			  Zlambdn(e->e2, compile0(e->e3), copyexpr(e->e1)));
		e->e1 = 0;
		e->e2 = 0;
		e->e3 = 0;
		freeexpr(e);
		return se;
	case Eambig:
		se = compileambig(e);
		e->e1 = 0;
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Esizeoft:
		se = compilesizeof(e->e1->xp);
		freeexpr(e);
		return se;
	case Etypeoft:
		se = compiletypeof(e->e1->xp);
		freeexpr(e);
		return se;
	case Ecast:
		se = compilecast(e);
		// e1 is a decl that needs to be freed
		e->e2 = 0;
		freeexpr(e);
		return se;
	case Econtainer:
		se = compilecontainer(e);
		e->e1 = 0;
		freeexpr(e);
		return se;
	case Elist:
		se = Zapply(doid("list"), compile0(e->e1));
		e->e1 = 0;
		freeexpr(e);
		return se;
	case Ens:
		se = compilens(e);
		return se;
	default:
		e->e1 = compile0(e->e1);
		e->e2 = compile0(e->e2);
		e->e3 = compile0(e->e3);
		e->e4 = compile0(e->e4);
		return e;
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
