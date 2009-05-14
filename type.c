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
	Expr *e, *se, *te, *id, *tn, *sz, *loc;
	Enum *en;
	Decl *dl;
	char *mk;
	Imm nen;

	switch(t->kind){
	case Tvoid:
		e = Zcall(G("mkctype_void"), 0);
		break;	
	case Tbase:
		e = Zcall(doid(cbasector[t->base]), 0);
		break;
	case Ttypedef:
		e = Zcall(G("mkctype_typedef"), 1, Zstr(t->tid));
		break;
	case Tstruct:
	case Tunion:
		mk = t->kind == Tstruct ? "%mkctype_struct" : "%mkctype_union";
		if(t->field == 0 && t->sz == 0){
			e = Zcall(doid(mk), 1, Zstr(t->tag));
			break;
		}

		if(t->tag == 0)
			/* we missed a form of creating anonymous aggregates */
			fatal("bug");

		se = nullelist();
		dl = t->field;
		while(dl){
			tn = gentypename(dl->type, recpass, ctx);
			if(dl->id)
				id = Zstr(dl->id);
			else
				id = Znil();
			if(dl->attr && dl->type->bitw){
				/* bitfield */
				te = Zblock(Zlocals(2, "$a", "$o"),
					    Zset(doid("$a"),
						 Zcall(G("mkattr"), 1,
						       recpass(ctx, dl->attr))),
					    Zset(doid("$o"),
						 Zcall(G("attroff"),
						       1, doid("$a"))),
					    Zcall(G("setattroff"),
						  2, doid("$a"), 
						  Zbinop(Ediv, doid("$o"),
							 Zuint(8))),
					    Zcall(G("mkfield"), 3,
						  Zcall(G("mkctype_bitfield"),
							3,
							tn,
							dl->type->bitw,
							Zbinop(Emod,
							       doid("$o"),
							       Zuint(8))),
						  id,
						  doid("$a")),
					    NULL);
				dl->attr = 0;
				dl->type->bitw = 0;
			}else if(dl->attr){
				te = Zcall(G("mkfield"), 3,
					   tn, 
					   id,
					   Zcall(G("mkattr"), 1,
						 recpass(ctx, dl->attr)));
				dl->attr = 0;
			}else
				te = Zcall(G("mkfield"), 3,
					   tn, id, Znil()); 
			se = Zcons(te, se);
			dl = dl->link;
		}
		se = Zapply(G("vector"), invert(se));
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
		se = Zcall(G("tabinsert"), 3,
			   doid("$typetab"), doid("$tmp"),
			   Zcall(doid(mk), 3, Zstr(t->tag), se, sz));
		te = Zcons(se, te);
		se = doid("$tmp");
		te = Zcons(se, te);
		e = newexpr(Eblock, loc, invert(te), 0, 0);
		break;
	case Tenum:
		if(t->en == 0){
			e = Zcall(G("mkctype_enum"), 1, Zstr(t->tag));
			break;
		}
		if(t->tag == 0)
			/* we missed a form of creating anonymous enums */
			fatal("bug");

		loc = Zlocals(2, "$tmp", "$tn");
		te = nullelist();

		/* count enum constants and allocate vector
		   (FIXME: use list?) */
		nen = 0;
		en = t->en;
		while(en && en != (Enum*)EmptyDecl){
			nen++;
			en = en->link;
		}
		se = Zset(doid("$tmp"), Zcall(G("mkvec"), 1, Zuint(nen)));
		te = Zcons(se, te);

		/* insert enum constants into vector */
		nen = 0;
		en = t->en;
		while(en && en != (Enum*)EmptyDecl){
			se = Zcall(G("vecset"), 3,
				   doid("$tmp"), Zuint(nen),
				   Zcall(G("vector"), 2,
					 Zstr(en->id), en->val)); /* steal */
			te = Zcons(se, te);
			en->val = 0;
			en = en->link;
			nen++;
		}

		/* define enum type */
		se = Zset(doid("$tn"),
			  Zcall(G("mkctype_enum"), 1, Zstr(t->tag)));
		te = Zcons(se, te);
		se = Zcall(G("tabinsert"), 3,
			   doid("$typetab"),
			   doid("$tn"),
			   Zcall(G("mkctype_enum"), 2,
				 Zstr(t->tag), doid("$tmp")));
		te = Zcons(se, te);

		se = doid("$tn");
		te = Zcons(se, te);
		e = newexpr(Eblock, loc, invert(te), 0, 0);
		break;
	case Tptr:
		e = Zcall(G("mkctype_ptr"), 1,
			  gentypename(t->link, recpass, ctx));
		break;
	case Tarr:
		if(t->cnt){
			t->cnt = recpass(ctx, t->cnt);
			se = t->cnt; /* steal */
			t->cnt = 0;
			e = Zcall(G("mkctype_array"), 2,
				  gentypename(t->link, recpass, ctx), se);
		}else
			e = Zcall(G("mkctype_array"), 1,
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
			te = Zcons(Zcall(G("vector"), 2, tn, id),
				   te);
			dl = dl->link;
		}
		e = Zcall(G("mkctype_fn"), 2,
			  gentypename(t->link, recpass, ctx),
			  Zapply(G("vector"), invert(te)));
		break;
	default:
		fatal("bug");
	}
	return e;
}
