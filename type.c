#include "sys.h"
#include "util.h"
#include "syscqct.h"

static char* cbasector[Vnbase] = {
	[Vchar]               = "%mkctype_char",
	[Vshort]	      = "%mkctype_short",
	[Vint]		      = "%mkctype_int",
	[Vlong]		      = "%mkctype_long",
	[Vvlong]	      = "%mkctype_vlong",
	[Vuchar]	      = "%mkctype_uchar", 
	[Vushort]	      = "%mkctype_ushort",
	[Vuint]		      = "%mkctype_uint", 
	[Vulong]	      = "%mkctype_ulong", 
	[Vuvlong]	      = "%mkctype_uvlong",
	[Vfloat]	      = "%mkctype_float",
	[Vdouble]	      = "%mkctype_double",
	[Vlongdouble]	      = "%mkctype_ldouble",
};

Expr*
gentypename(Type *t, Expr *(recpass)(U*, Expr*), U *ctx)
{
	Expr *e, *se, *te, *id, *off, *tn, *sz, *loc;
	Enum *en;
	Decl *dl;
	char *mk;
	Imm nen;

	switch(t->kind){
	case Tvoid:
		e = Zcall(doid("%mkctype_void"), 0);
		break;	
	case Tbase:
		e = Zcall(doid(cbasector[t->base]), 0);
		break;
	case Ttypedef:
		e = Zcall(doid("%mkctype_typedef"), 1, Zstr(t->tid));
		break;
	case Tstruct:
	case Tunion:
		mk = t->kind == Tstruct ? "%mkctype_struct" : "%mkctype_union";
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
				dl->offs = recpass(ctx, dl->offs);
				dl->type->bit0 =          /* steal */
					newbinop(Emod, dl->offs, Zuint(8));
				off = newbinop(Ediv,
					       copyexpr(dl->offs),
					       Zuint(8));
				dl->offs = 0;
			}else if(dl->offs){
				dl->offs = recpass(ctx, dl->offs);
				off = dl->offs; /* steal */
				dl->offs = 0;
			}else
				off = Znil();
			tn = gentypename(dl->type, recpass, ctx);
			se = Zcons(Zcall(doid("%mkfield"),
					 3, tn, id, off), se);
			dl = dl->link;
		}
		se = Zapply(doid("%vector"), invert(se));
		if(t->sz){
			t->sz = recpass(ctx, t->sz);
			sz = t->sz; /* steal */
			t->sz = 0;
		}else
			sz = Znil();

		loc = Zlocals(1, "$tmp");

		te = nullelist();
		te = Zcons(Zset(doid("$tmp"),
				Zcall(doid(mk), 1, Zstr(t->tag))),
			   te);
		se = Zcall(doid("%tabinsert"), 3,
			   doid("$typetab"), doid("$tmp"),
			   Zcall(doid(mk), 3, Zstr(t->tag), se, sz));
		te = Zcons(se, te);
		se = doid("$tmp");
		te = Zcons(se, te);
		e = newexpr(Eblock, loc, invert(te), 0, 0);
		break;
	case Tenum:
		if(t->en == 0){
			e = Zcall(doid("%mkctype_enum"), 1, Zstr(t->tag));
			break;
		}
		if(t->tag == 0)
			fatal("anonymous enums not implemented");

		loc = Zlocals(2, "$tmp", "$tn");
		te = nullelist();

		/* count enum constants and allocate vector
		   (FIXME: use list?) */
		nen = 0;
		en = t->en;
		while(en){
			nen++;
			en = en->link;
		}
		se = Zset(doid("$tmp"), Zcall(doid("%mkvec"), 1, Zuint(nen)));
		te = Zcons(se, te);

		/* insert enum constants into vector */
		nen = 0;
		en = t->en;
		while(en){
			se = Zcall(doid("%vecset"), 3,
				   doid("$tmp"), Zuint(nen),
				   Zcall(doid("%vector"), 2,
					 Zstr(en->id), en->val)); /* steal */
			te = Zcons(se, te);
			en->val = 0;
			en = en->link;
			nen++;
		}

		/* define enum type */
		se = Zset(doid("$tn"),
			  Zcall(doid("%mkctype_enum"), 1, Zstr(t->tag)));
		te = Zcons(se, te);
		se = Zcall(doid("%tabinsert"), 3,
			   doid("$typetab"),
			   doid("$tn"),
			   Zcall(doid("%mkctype_enum"), 2,
				 Zstr(t->tag), doid("$tmp")));
		te = Zcons(se, te);

		se = doid("$tn");
		te = Zcons(se, te);
		e = newexpr(Eblock, loc, invert(te), 0, 0);
		break;
	case Tptr:
		e = Zcall(doid("%mkctype_ptr"), 1,
			  gentypename(t->link, recpass, ctx));
		break;
	case Tarr:
		if(t->cnt){
			t->cnt = recpass(ctx, t->cnt);
			se = t->cnt; /* steal */
			t->cnt = 0;
			e = Zcall(doid("%mkctype_array"), 2,
				  gentypename(t->link, recpass, ctx), se);
		}else
			e = Zcall(doid("%mkctype_array"), 1,
				  gentypename(t->link, recpass, ctx)); 
		break;
	case Tfun:
		te = nullelist();
		dl = t->param;
		while(dl){
			if(dl->id)
				id = Zstr(dl->id);
			else
				id = Znil();
			tn = gentypename(dl->type, recpass, ctx);
			te = Zcons(Zcall(doid("%vector"), 2, tn, id),
				   te);
			dl = dl->link;
		}
		e = Zcall(doid("%mkctype_fn"), 2,
			  gentypename(t->link, recpass, ctx),
			  Zapply(doid("%vector"), invert(te)));
		break;
	default:
		fatal("bug");
	}

	if(t->bitw){
		if(t->kind != Tbase && t->kind != Ttypedef && t->kind != Tenum)
			// FIXME: E does not have source location information!
			cerror(ctx, e, "invalid bitfield");
		t->bitw = recpass(ctx, t->bitw);
		t->bit0 = recpass(ctx, t->bit0);
		e = Zcall(doid("%mkctype_bitfield"), 3, e, t->bitw, t->bit0);
		t->bitw = 0;	/* steal */
		t->bit0 = 0;	/* steal */
	}

	return e;
}
