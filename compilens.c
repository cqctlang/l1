#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Expr* compile(U *ctx, Expr *e);

struct Arg {
	Expr **e;
	U *ctx;
};

static void
do1tag(void *u, char *k, void *v)
{
	Expr *se, *te;
	Expr **e;
	Decl *d;
	struct Arg *up;

	up = u;
	e = up->e;
	d = v;

	te = nullelist();
	se = gentypename(d->type, compile, up->ctx);
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
	struct Arg *up;

	up = u;
	e = up->e;
	d = v;

	loc = Zlocals(2, "$tmp", "$tn");

	te = nullelist();
	se = Zset(doid("$tn"), gentypename(d->type, compile, up->ctx));
	te = Zcons(se, te);

	if(d->offs){
		d->offs = compile(up->ctx, d->offs);
		offs = d->offs; /* steal */
		d->offs = 0;
	}else
		offs = Znil();

	se = Zset(doid("$tmp"),
		  Zcall(doid("%mksym"), 3, doid("$tn"), Zstr(d->id), offs));
	te = Zcons(se, te);

	se = Zcall(doid("%tabinsert"), 3, doid("$symtab"),
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
	struct Arg *up;

	up = u;
	e = up->e;
	d = v;

	loc = Zlocals(1, "$tn");

	te = nullelist();
	se = Zset(doid("$tn"),
		  Zcall(doid("%mkctype_typedef"), 2,
			Zstr(d->id),
			gentypename(d->type, compile, up->ctx)));
	te = Zcons(se, te);

	/* typedef T TID => typetab[typedef(TID)] = typedef(TID,typename(T)) */
	tn = Zcall(doid("%mkctype_typedef"), 1, Zstr(d->id));
	se = Zcall(doid("%tabinsert"), 3, doid("$typetab"), tn, 
		   doid("$tn"));
	te = Zcons(se, te);

	te = newexpr(Eblock, loc, invert(te), 0, 0);
	*e = Zcons(te, *e);
}

static void
hashdecl(unsigned kind, Decl *d, HT *sym, HT *tag, HT *tid)
{
	Type *t;

	switch(kind){
	case Edecls:
		if(d->id)
			hput(sym, d->id, strlen(d->id), d);
		else{
			t = d->type;
			if((t->kind == Tstruct || t->kind == Tunion)
			   && (t->field != 0 || t->sz != 0))
				hput(tag, t->tag, strlen(t->tag), d);
			if(t->kind == Tenum && t->en != 0)
				hput(tag, t->tag, strlen(t->tag), d);
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
rmenid(Expr *e, HT *enid)
{
	Expr *v;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Eid:
		v = hget(enid, e->id, strlen(e->id));
		if(v == 0)
			/* FIXME: undefined identifier in enum const;
			   tell user about it now (instead of when ns
			   is evaluated). */
			return e;
		freeexpr(e);
		return copyexpr(v);
	default:
		e->e1 = rmenid(e->e1, enid);
		e->e2 = rmenid(e->e2, enid);
		e->e3 = rmenid(e->e3, enid);
		e->e4 = rmenid(e->e4, enid);
		return e;
	}
}

static void
rmenids(Type *t, HT *enid)
{
	Decl *d;
	Enum *en;

	switch(t->kind){
	case Tenum:
		en = t->en;
		while(en){
			en->val = rmenid(en->val, enid);
			hput(enid, en->id, strlen(en->id), en->val);
			en = en->link;
		}
		break;
	case Tstruct:
	case Tunion:
		if(t->field == 0)
			return;
		d = t->field;
		while(d){
			rmenids(d->type, enid);
			d = d->link;
		}
		break;
	default:
		break;
	}
}

static Expr*
compilens(U *ctx, Expr *e)
{
	Expr *se, *te;
	Expr *ex;
	Expr *loc;
	Decl *dl;
	HT *sym, *tag, *tid, *enid;
	struct Arg a;

	loc = Zlocals(3, "$ns", "$typetab", "$symtab");

	te = nullelist();

	se = Zcall(doid("%mktab"), 0);
	se->src = e->src;
	se = Zset(doid("$typetab"), se);
	te = Zcons(se, te);
	se = Zset(doid("$symtab"), Zcall(doid("%mktab"), 0));
	te = Zcons(se, te);

	/* inherited names expression */
	se = Zset(doid("$ns"), compile(ctx, e->e1));
	te = Zcons(se, te);

	/* declarations */
	ex = e->e2;
	if(ex->kind != Eelist && ex->kind != Enull)
		fatal("broken");
	sym = mkht();
	tag = mkht();
	tid = mkht();
	enid = mkht();
	while(ex->kind == Eelist){
		dl = ex->e1->xp;
		while(dl){
			rmenids(dl->type, enid);
			hashdecl(ex->e1->kind, dl, sym, tag, tid);
			dl = dl->link;
		}
		ex = ex->e2;
	}
	a.e = &te;
	a.ctx = ctx;
	hforeach(sym, do1sym, &a);
	hforeach(tag, do1tag, &a);
	hforeach(tid, do1tid, &a);
	freeht(sym);
	freeht(tag);
	freeht(tid);
	freeht(enid);

	/* new name space */
	if(e->e3)
		se = Zcall(doid("%mknsraw"), 4,
			   doid("$ns"), doid("$typetab"), doid("$symtab"),
			   compile(ctx, e->e3));
	else
		se = Zcall(doid("%mknsraw"), 3,
			   doid("$ns"), doid("$typetab"), doid("$symtab"));
	te = Zcons(se, te);

	return newexpr(Eblock, loc, invert(te), 0, 0);
}

static Expr*
compile(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return e;

	switch(e->kind){
	case Ens:
		p = compilens(ctx, e);
		e->e1 = 0;
		e->e3 = 0;
		freeexpr(e);
		return p;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			p->e1 = compile(ctx, p->e1);
			p = p->e2;
		}
		return e;
	default:
		e->e1 = compile(ctx, e->e1);
		e->e2 = compile(ctx, e->e2);
		e->e3 = compile(ctx, e->e3);
		e->e4 = compile(ctx, e->e4);
		return e;
	}
}

int
docompilens(U *ctx, Expr *e)
{
 	/* expr lists ensure we do not have to return a new root Expr */
	if(e->kind != Eelist && e->kind != Enull)
		fatal("bug");
	if(setjmp(ctx->jmp) != 0)
		return -1;	/* error */
	compile(ctx, e);
	return 0;
}
