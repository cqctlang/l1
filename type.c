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
gentypename(Type *t, Expr *(recpass)(U*, Expr*), U *ctx, unsigned effect)
{
	Expr *e, *se, *te, *id, *tn, *sz;
	Enum *en;
	Decl *dl;
	char *mk;

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
		mk = t->kind == Tstruct ? "mkctype_struct" : "mkctype_union";
		if(t->field == 0 && t->sz == 0){
			e = Zcall(G(mk), 1, Zstr(t->tag));
			break;
		}

		if(t->tag == 0)
			/* we missed a form of creating anonymous aggregates */
			fatal("bug");

		se = nullelist();
		dl = t->field;
		while(dl){
			tn = gentypename(dl->type, recpass, ctx, 0);
			if(dl->id)
				id = Zstr(dl->id);
			else
				id = Znil();
			if(dl->attr && dl->type->bitw){
				/* bitfield */
				te = Zblock(Zlocals(1, "$o"),
					    Zset(doid("$o"),
						 recpass(ctx, dl->attr)),
					    Zcall(G("mkfield"), 3,
						  Zcall(G("mkctype_bitfield"),
							3,
							tn,
							dl->type->bitw,
							Zbinop(Emod,
							       doid("$o"),
							       Zuint(8))),
						  id,
						  Zcall(G("mkattr"), 1,
							Zbinop(Ediv, doid("$o"),
							       Zuint(8)))),
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

		if(effect){
			e = Zcall(G("tabinsert"), 3,
				  doid("$typetab"),
				  Zcall(G(mk), 1, Zstr(t->tag)),
				  Zcall(G(mk), 3, Zstr(t->tag), se, sz));
			break;
		}
		e = Zblock(Zlocals(1, "$tmp"),
			   Zset(doid("$tmp"), Zcall(G(mk), 1, Zstr(t->tag))),
			   Zcall(G("tabinsert"), 3,
				 doid("$typetab"), doid("$tmp"),
				 Zcall(G(mk), 3, Zstr(t->tag), se, sz)),
			   doid("$tmp"),
			   NULL);
		break;
	case Tenum:
		if(t->en == 0){
			e = Zcall(G("mkctype_enum"), 1, Zstr(t->tag));
			break;
		}
		if(t->tag == 0)
			/* we missed a form of creating anonymous enums */
			fatal("bug");

		se = nullelist();
		en = t->en;
		while(en && en != (Enum*)EmptyDecl){
			te = Zcall(G("vector"), 2, Zstr(en->id), en->val);
			en->val = 0; /* steal */
			se = Zcons(te, se);
			en = en->link;
		}
		se = Zapply(G("vector"), invert(se));

		if(effect){
			e = Zcall(G("tabinsert"), 3,
				  doid("$typetab"),
				  Zcall(G("mkctype_enum"), 1, Zstr(t->tag)),
				  Zcall(G("mkctype_enum"), 2,
					Zstr(t->tag), se));
			break;
		}
		e = Zblock(Zlocals(1, "$tmp"),
			   Zset(doid("$tmp"),
				Zcall(G("mkctype_enum"), 1, Zstr(t->tag))),
			   Zcall(G("tabinsert"), 3,
				 doid("$typetab"),
				 doid("$tmp"),
				 Zcall(G("mkctype_enum"), 2,
				       Zstr(t->tag), se)),
			   doid("$tmp"),
			   NULL);
		break;
	case Tptr:
		e = Zcall(G("mkctype_ptr"), 1,
			  gentypename(t->link, recpass, ctx, 0));
		break;
	case Tarr:
		if(t->cnt){
			t->cnt = recpass(ctx, t->cnt);
			se = t->cnt; /* steal */
			t->cnt = 0;
			e = Zcall(G("mkctype_array"), 2,
				  gentypename(t->link, recpass, ctx, 0), se);
		}else
			e = Zcall(G("mkctype_array"), 1,
				  gentypename(t->link, recpass, ctx, 0)); 
		break;
	case Tfun:
		te = nullelist();
		dl = t->param;
		while(dl){
			if(dl->id)
				id = Zstr(dl->id);
			else
				id = Znil();
			tn = gentypename(dl->type, recpass, ctx, 0);
			te = Zcons(Zcall(G("vector"), 2, tn, id),
				   te);
			dl = dl->link;
		}
		e = Zcall(G("mkctype_fn"), 2,
			  gentypename(t->link, recpass, ctx, 0),
			  Zapply(G("vector"), invert(te)));
		break;
	default:
		fatal("bug");
	}
	return e;
}
