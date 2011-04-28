#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
struct Seen
{
	HT *sym;
	HT *tag;
	HT *tid;
} Seen;

static void
initseen(Seen *s)
{
	s->sym = mkhts();
	s->tag = mkhts();
	s->tid = mkhts();
}

static void
finiseen(Seen *s)
{
	freeht(s->sym);
	freeht(s->tag);
	freeht(s->tid);
}

static Expr*
inc(Expr *e)
{
	return Zadd(e, Zuint(1));
}

/* substitute enum references with expressions inside @names */
static Expr*
enumsub(U *ctx, HT *tab, Expr *e)
{
	Expr *v, *p, *en;
	HT *nt;
	char *s;

	if(e == 0)
		return e;
	switch(e->kind){
	case Enames:
		e->e1 = enumsub(ctx, tab, e->e1);
		nt = mkhts();
		e->e2 = enumsub(ctx, nt, e->e2);
		freeht(nt);
		return e;
	case Eid:
		/* matching any identifier may be too generous.
		   instead maybe restrict to constant
		   expressions in type definitions. */
		if(tab == 0)
			/* not in @names expression */
			return e;
		v = hgets(tab, idsym(e), strlen(idsym(e)));
		if(v)
			return v;
		else
			return e;
	case Eenum:
		if(e->e2 == 0 || isnull(e->e2))
			return e;
		p = e->e2;
		while(!isnull(p)){
			en = p->e1;
			p = p->e2;
			en->e2 = enumsub(ctx, tab, en->e2);
			s = idsym(en->e1);
			hputs(tab, s, strlen(s), en->e2);
		}
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = enumsub(ctx, tab, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = enumsub(ctx, tab, e->e1);
		e->e2 = enumsub(ctx, tab, e->e2);
		e->e3 = enumsub(ctx, tab, e->e3);
		e->e4 = enumsub(ctx, tab, e->e4);
		return e;
	}
}

/* expand implicit enum constant definitions */
static Expr*
enumincs(U *ctx, Expr *e)
{
	Expr *p, *c, *en;

	if(e == 0)
		return e;
	switch(e->kind){
	case Eenum:
		if(e->e2 == 0 || isnull(e->e2))
			return e;
		c = Zuint(0);
		p = e->e2;
		while(!isnull(p)){
			en = p->e1;
			p = p->e2;
			if(en->e2){
				en->e2 = enumincs(ctx, en->e2);
				c = inc(en->e2);
			}else{
				en->e2 = c;
				c = inc(en->e2);
			}
		}
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = enumincs(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = enumincs(ctx, e->e1);
		e->e2 = enumincs(ctx, e->e2);
		e->e3 = enumincs(ctx, e->e3);
		e->e4 = enumincs(ctx, e->e4);
		return e;
	}
}

static Expr* lift(U *ctx, Expr *e);
static Expr* liftspec(U *ctx, Seen *s, Expr *e, Expr **te);

static Expr*
liftdtors(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *p, *a;
	if(e == 0)
		return e;
	switch(e->kind){
	case Eid:
		return e;
	case Earr:
		e->e1 = liftdtors(ctx, s, e->e1, te);
		if(e->e2)
			e->e2 = lift(ctx, e->e2);
		return e;
	case Eptr:
		e->e1 = liftdtors(ctx, s, e->e1, te);
		return e;
	case Efun:
		e->e1 = liftdtors(ctx, s, e->e1, te);
		/* parameters are list of declarations */
		p = e->e2;
		while(!isnull(p)){
			a = p->e1;
			p = p->e2;
			a->e1 = liftspec(ctx, s, a->e1, te);
			a->e2 = liftdtors(ctx, s, a->e2, te);
			a->e3 = lift(ctx, a->e3); /* optional attribute */
		}
		return e;
	case Eelist:
		p = e;
		while(isnull(p)){
			p->e1 = liftdtors(ctx, s, p->e1, te);
			p = p->e2;
		}
		return e;
	default:
		fatal("bug");
	}
}

static void
liftspecenum(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *p, *en;
	p = e->e2;
	while(!isnull(p)){
		en = p->e1;
		p = p->e2;
		if(en->e2)
			en->e2 = lift(ctx, en->e2);
	}
}

static void
liftspecsu(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *p, *f;

	/* fields */
	p = e->e2;
	while(!isnull(p)){
		f = p->e1;
		p = p->e2;
		f->e1 = liftspec(ctx, s, f->e1, te);
		f->e2 = liftdtors(ctx, s, f->e2, te);
		f->e3 = lift(ctx, f->e3); /* optional field attribute */
		f->e4 = lift(ctx, f->e4); /* width (bitfields only) */
	}

	/* optional su attribute */
	if(e->e3 == 0 && isnull(e->e2))
		/* ascribe size 0 to definitions of form struct tag { } */
		e->e3 = Zint(0);
	else
		e->e3 = lift(ctx, e->e3);
}

static Expr*
liftspec(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *q;
	char *id;

	switch(e->kind){
	case Ebase:
	case Eid:
		return e;
	case Estruct:
	case Eunion:
	case Eenum:
		if(e->e2 == 0)
			return e; /* not a definition */
		id = idsym(e->e1);
		q = Z1(e->kind, e->e1);
		putsrc(q, &e->src);
		if(hgets(s->tag, id, strlen(id)))
			return q; /* already defined */
		hputs(s->tag, id, strlen(id), id);
		if(e->kind == Eenum)
			liftspecenum(ctx, s, e, te);
		else
			liftspecsu(ctx, s, e, te);
		*te = Zcons(e, *te);
		return q;
	default:
		fatal("bug");
	}
}

static void
lift1name(U *ctx, Seen *s, Expr *e, Expr **te)
{
	switch(e->kind){
	case Etypedef:
	case Edecls:
		e->e1 = liftspec(ctx, s, e->e1, te);
		if(e->e2 && !isnull(e->e2)){
			/* translate declarators and emit declarations */
			e->e2 = liftdtors(ctx, s, e->e2, te);
			*te = Zcons(e, *te);
		}
		break;
	default:
		fatal("bug");
	}
}

static Expr*
liftnames(U *ctx, Expr *e)
{
	Seen s;
	Expr *p, *te;

	if(isnull(e))
		return e;
	initseen(&s);
	te = Znull();
	p = e;
	while(!isnull(p)){
		lift1name(ctx, &s, p->e1, &te);
		p = p->e2;
	}
	finiseen(&s);
	return invert(te);
}

static Expr*
lift(U *ctx, Expr *e)
{
	Expr *p;
	if(e == 0)
		return e;
	switch(e->kind){
	case Enames:
		e->e1 = lift(ctx, e->e1);
		e->e2 = liftnames(ctx, e->e2);
		return e;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = lift(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = lift(ctx, e->e1);
		e->e2 = lift(ctx, e->e2);
		e->e3 = lift(ctx, e->e3);
		e->e4 = lift(ctx, e->e4);
		return e;
	}
}

static Expr* tie(U *ctx, Expr *e);
static void dotie(U *ctx, Expr *t, Expr *dtor, Expr **type, Expr **id);

static Expr*
tieparams(U *ctx, Expr *e)
{
	Expr *d, *p;
	p = e;
	while(!isnull(p)){
		d = p->e1; /* Edecl */
		p = p->e2;
		d->e3 = tie(ctx, d->e3);
		dotie(ctx, d->e1, d->e2, &d->e1, &d->e2);
	}
	return e;
}

static void
rdotie(U *ctx, Expr *t, Expr *dtor, Expr **type, Expr **id)
{
	if(dtor == 0){
		*id = 0;
		*type = Z1(Etypename, t);
		return;
	}
	switch(dtor->kind){
	case Eid:
		*id = dtor;
		*type = Z1(Etypename, t);
		break;
	case Earr:
		rdotie(ctx,
		       Z2(Earr, t, tie(ctx, dtor->e2)),
		       dtor->e1,
		       type, id);
		break;
	case Eptr:
		rdotie(ctx,
		       Z1(Eptr, t),
		       dtor->e1,
		       type, id);
		break;
	case Efun:
		rdotie(ctx,
		       Z2(Efun, t, tieparams(ctx, dtor->e2)),
		       dtor->e1,
		       type, id);
		break;
	default:
		fatal("bug");
	}
}

/* t is specifier, dtor is declarator; upon return
   *type is the combined type and *id is the identifier
   or (for abstract declarators) 0 */
static void
dotie(U *ctx, Expr *t, Expr *dtor, Expr **type, Expr **id)
{
	if(t->kind == Etickt){
		rdotie(ctx, t->e2, dtor, type, id);
		*type = Z2(Etickt, t->e1, *type);
	}else
		rdotie(ctx, t, dtor, type, id);
}

static void
tie1sufield(U *ctx, Expr *e, Expr **fs)
{
	Expr *p, *t, *id, *a;

	switch(e->kind){
	case Ebitfield:
		/* only one declarator */
		dotie(ctx, e->e1, e->e2, &e->e1, &e->e2);
		e->e3 = tie(ctx, e->e3); /* attribute */
		e->e4 = tie(ctx, e->e4); /* width */
		*fs = Zcons(e, *fs);
		break;
	case Efields:
		/* list of declarators */
		p = e->e2;
		a = tie(ctx, e->e3); /* associated with the first decl */
		while(!isnull(p)){
			dotie(ctx, e->e1, p->e1, &t, &id);
			*fs = Zcons(Z3(Edecl, t, id, a), *fs);
			a = 0;
			p = p->e2;
		}
		break;
	default:
		fatal("bug");
	}
}

static void
tie1name(U *ctx, Expr *e, Expr **te)
{
	Expr *p, *fs, *en, *t, *id, *a;

	switch(e->kind){
	case Estruct:
	case Eunion:
		fs = Znull();
		p = e->e2;
		while(!isnull(p)){
			tie1sufield(ctx, p->e1, &fs);
			p = p->e2;
		}
		e->e2 = invert(fs);
		e->e3 = tie(ctx, e->e3);
		*te = Zcons(e, *te);
		break;
	case Eenum:
		*te = Zcons(e, *te);
		p = e->e2;
		while(!isnull(p)){
			en = p->e1;
			en->e2 = tie(ctx, en->e2);
			p = p->e2;
		}
		break;
	case Etypedef:
		p = e->e2;
		while(!isnull(p)){
			dotie(ctx, e->e1, p->e1, &t, &id);
			*te = Zcons(Z2(Etypedef, t, id), *te);
			p = p->e2;
		}
		break;
	case Edecls:
		p = e->e2;
		a = tie(ctx, e->e3);
		while(!isnull(p)){
			dotie(ctx, e->e1, p->e1, &t, &id);
			*te = Zcons(Z3(Edecl, t, id, a), *te);
			a = 0;
			p = p->e2;
		}
		break;
	default:
		fatal("bug");
	}
}

static Expr*
tienames(U *ctx, Expr *e)
{
	Expr *p, *te;

	if(isnull(e))
		return e;
	te = Znull();
	p = e;
	while(!isnull(p)){
		tie1name(ctx, p->e1, &te);
		p = p->e2;
	}
	return invert(te);
}

static Expr*
tietypename(U *ctx, Expr *e)
{
	Expr *t, *id;
	dotie(ctx, e->e1, e->e2, &t, &id);
	if(id != 0)
		fatal("bug");
	return t;
}

static Expr*
tie(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;
	switch(e->kind){
	case Enames:
		e->e1 = tie(ctx, e->e1);
		e->e2 = tienames(ctx, e->e2);
		return e;
	case Etypename:
		return tietypename(ctx, e);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = tie(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = tie(ctx, e->e1);
		e->e2 = tie(ctx, e->e2);
		e->e3 = tie(ctx, e->e3);
		e->e4 = tie(ctx, e->e4);
		return e;
	}
}

static Expr* names(U *ctx, Expr *e);

/* FIXME: this is here only to get ids for symbols */
static Expr*
id2sym(Expr *e)
{
	if(e->kind != Eid)
		fatal("bug");
	return Zstr(e->id);
}

static void
do1name(U *ctx, Seen *s, Expr *e, Expr **te)
{
	Expr *q;
	char *id;

	switch(e->kind){
	case Estruct:
	case Eunion:
	case Eenum:
		q = Zcall(G("tabinsert"), 3,
			  doid("$typetab"),
			  Z1(Etypename, Z1(e->kind, e->e1)),
			  Z1(Etypespec, names(ctx, e)));
		*te = Zcons(q, *te);
		break;
	case Etypedef:
		id = idsym(e->e2);
		if(hgets(s->tid, id, strlen(id)))
			return;
		hputs(s->tid, id, strlen(id), id);
		q = Zcall(G("tabinsert"), 3,
			  doid("$typetab"),
			  Z1(Etypename, Z1(Etypedef, e->e2)),
			  Z1(Etypespec, names(ctx, e)));
		*te = Zcons(q, *te);
		break;
	case Edecl:
		e->e3 = names(ctx, e->e3);
		q = Zcall(G("tabinsert"), 3,
			  doid("$symtab"),
			  id2sym(e->e2), names(ctx, e));
		*te = Zcons(q, *te);
		break;
	default:
		fatal("bug");
	}
}

static Expr*
donames(U *ctx, Expr *e)
{
	Expr *l, *te, *p;
	Seen seen;

	l = Zlocals(2, "$typetab", "$symtab");
	te = Znull();
	te = Zcons(Zset(doid("$typetab"), Zcall(G("mktab"), 0)), te);
	te = Zcons(Zset(doid("$symtab"), Zcall(G("mktab"), 0)), te);

	initseen(&seen);
	p = e->e2;
	while(!isnull(p)){
		do1name(ctx, &seen, p->e1, &te);
		p = p->e2;
	}
	finiseen(&seen);

	te = Zcons(Zcall(G("mknsraw"), 3,
			 names(ctx, e->e1),
			 doid("$typetab"),
			 doid("$symtab")),
		   te);
	te = Zscope(Zblock(l, invert(te), NULL));
	putsrc(te, &e->src);
	return te;
}

static Expr*
names(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return 0;
	switch(e->kind){
	case Enames:
		return donames(ctx, e);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = names(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = names(ctx, e->e1);
		e->e2 = names(ctx, e->e2);
		e->e3 = names(ctx, e->e3);
		e->e4 = names(ctx, e->e4);
		return e;
	}
	return 0;
}

static Expr* mkctype(U *ctx, Expr *e);

static unsigned basemod[Vnbase][Enbase] = {
	[Vchar][Eunsigned]     = Vuchar,

	[Vchar][Esigned]       = Vchar,

	[Vshort][Eunsigned]    = Vushort,
	[Vshort][Esigned]      = Vshort,
	[Vshort][Eint]         = Vshort,
	[Vushort][Eint]        = Vushort,

	[Vint][Eunsigned]      = Vuint,
	[Vint][Esigned]        = Vint,
	[Vint][Elong]          = Vlong,
	[Vuint][Elong]         = Vulong,
	[Vint][Eshort]         = Vshort,
	[Vuint][Eshort]        = Vushort,

	[Vlong][Eunsigned]     = Vulong,
	[Vlong][Esigned]       = Vlong,
	[Vlong][Elong]         = Vvlong,
	[Vlong][Edouble]       = Vlongdouble,
	[Vulong][Elong]        = Vuvlong,

	[Vvlong][Eunsigned]    = Vuvlong,
	[Vvlong][Esigned]      = Vvlong,

	[Vdouble][Elong]       = Vlongdouble,

	[Vundef][Ebool]	       = Vbool,
	[Vundef][Echar]        = Vchar,
	[Vundef][Edouble]      = Vdouble,
	[Vundef][Efloat]       = Vfloat,
	[Vundef][Eint]         = Vint,
	[Vundef][Elong]        = Vlong,
	[Vundef][Eshort]       = Vshort,
	[Vundef][Esigned]      = Vint,
	[Vundef][Eunsigned]    = Vuint,
	[Vundef][Evoid]        = Vvoid,
	/* the rest are Vundef, which we assume to be 0 */
};

static char* cbasector[Vnallbase] = {
	[Vbool]               = "mkctype_bool",
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
mkctypebase(U *ctx, Expr *e)
{
	Expr *p;
	Cbase b;
	b = Vundef;
	p = e->e1;
	while(!isnull(p)){
		switch(p->e1->kind){
		case Ebool:
		case Echar:
		case Edouble:
		case Efloat:
		case Eint:
		case Elong:
		case Eshort:
		case Esigned:
		case Eunsigned:
		case Evoid:
			b = basemod[b][p->e1->kind];
			if(b == Vundef)
				cerror(ctx, e, "bad type specifier");
			break;
		default:
			fatal("bug");
		}
		p = p->e2;
	}
	return Zcall(G(cbasector[b]), 0);
}

static Expr*
mkfieldspec(U *ctx, Expr *f)
{
	Expr *tn, *id, *a;

	tn = mkctype(ctx, f->e1);
	id = id2sym(f->e2);
	a = mkctype(ctx, f->e3);
	if(f->kind == Ebitfield)
		return Zblock(Zlocals(1, "$o"),
			      Zset(doid("$o"), a),
			      Zcall(G("mkfield"), 3,
				    Zcall(G("mkctype_bitfield"), 3,
					  tn,
					  mkctype(ctx, f->e4),
					  Zbinop(Emod, doid("$o"), Zuint(32))),
				    id,
				    Zcall(G("mkattr"), 1,
					  Zbinop(Emul,
						 Zbinop(Ediv, doid("$o"),
							Zuint(32)),
						 Zuint(4)))),
			      NULL);
	else
		return Zcall(G("mkfield"), 3, tn, id,
			     a ? Zcall(G("mkattr"), 1, a) : Znil());
}

static Expr*
mkctypespec(U *ctx, Expr *e)
{
	Expr *se, *p, *f, *en;

	if(e == 0)
		fatal("bug");
	switch(e->kind){
	case Estruct:
	case Eunion:
		se = Znull();
		p = e->e2;
		while(!isnull(p)){
			f = p->e1;
			p = p->e2;
			se = Zcons(mkfieldspec(ctx, f), se);
		}
		return Zcall(e->kind == Estruct ?
			     G("mkctype_struct") : G("mkctype_union"),
			     3,
			     id2sym(e->e1),
			     Zapply(G("vector"), invert(se)),
			     e->e3 ? mkctype(ctx, e->e3) : Znil());
	case Eenum:
		se = Znull();
		p = e->e2;
		while(!isnull(p)){
			en = p->e1;
			p = p->e2;
			se = Zcons(Zcall(G("vector"), 2,
					 id2sym(en->e1), mkctype(ctx, en->e2)),
				   se);
		}
		se = Zapply(G("vector"), invert(se));
		return Zcall(G("mkctype_enum"), 2, id2sym(e->e1), se);
	case Etypedef:
		return Zcall(G("mkctype_typedef"), 2,
			     id2sym(e->e2),
			     mkctype(ctx, e->e1));
	default:
		fatal("bug");
	}
}

static Expr*
mkctypename(U *ctx, Expr *e)
{
	Expr *se, *p, *a;
	if(e == 0)
		fatal("bug");
	switch(e->kind){
	case Estruct:
		return Zcall(G("mkctype_struct"), 1, id2sym(e->e1));
	case Eunion:
		return Zcall(G("mkctype_union"), 1, id2sym(e->e1));
	case Eenum:
		return Zcall(G("mkctype_enum"), 1, id2sym(e->e1));
	case Etypedef:
		return Zcall(G("mkctype_typedef"), 1, id2sym(e->e1));
	case Ebase:
		return mkctypebase(ctx, e);
	case Eptr:
		return Zcall(G("mkctype_ptr"), 1, mkctypename(ctx, e->e1));
	case Earr:
		return Zcall(G("mkctype_array"), 2,
			     mkctypename(ctx, e->e1), mkctype(ctx, e->e2));
	case Efun:
		se = Znull();
		p = e->e2;
		while(!isnull(p)){
			a = p->e1;
			p = p->e2;
			se = Zcons(Zcall(G("vector"), 2,
					 mkctype(ctx, a->e1),
					 a->e2 ? id2sym(a->e2) : Znil()),
				   se);
		}
		return Zcall(G("mkctype_fn"), 2,
			     mkctypename(ctx, e->e1),
			     Zapply(G("vector"), invert(se)));
	default:
		fatal("bug");
	}
}

static Expr*
mkctypesym(U *ctx, Expr *e)
{
	return Zcall(G("mksym"), 3,
		     mkctype(ctx, e->e1),
		     id2sym(e->e2),
		     mkctype(ctx, e->e3));
}

static Expr*
mkctype(U *ctx, Expr *e)
{
	Expr *p;
	if(e == 0)
		return 0;
	switch(e->kind){
	case Etypespec:
		return mkctypespec(ctx, e->e1);
	case Etypename:
		return mkctypename(ctx, e->e1);
	case Edecl:
		return mkctypesym(ctx, e);
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = mkctype(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = mkctype(ctx, e->e1);
		e->e2 = mkctype(ctx, e->e2);
		e->e3 = mkctype(ctx, e->e3);
		e->e4 = mkctype(ctx, e->e4);
		return e;
	}
	return 0;
}

Expr*
docompilen(U *ctx, Expr *e)
{
	if(setjmp(ctx->jmp) != 0)
		return 0;
	e = enumincs(ctx, e);
	e = enumsub(ctx, 0, e);
	e = lift(ctx, e);    /* lift sues from interior of names decls */
	e = tie(ctx, e);     /* tie specifiers to declarators */
	e = names(ctx, e);   /* load names tables */
	e = mkctype(ctx, e); /* translate type names and specs to types */
	return e;
}
