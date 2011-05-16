#include "sys.h"
#include "util.h"
#include "syscqct.h"

/*
  variant declarations

	Ctypearr *ta;
	Ctypebase *tb;
	Ctypeconst *tc;
	Ctypedef *td;
	Ctypeenum *te;
	Ctypefunc *tf;
	Ctypesu *ts;
	Ctypeptr *tp;
	Ctypeundef *tu;
	Ctypebitfield *tw;

 */

Val*
iterctype(Head *hd, Ictx *ictx)
{
	Ctype *t;
	Ctypearr *ta;
	Ctypebase *tb;
	Ctypeconst *tc;
	Ctypedef *td;
	Ctypeenum *te;
	Ctypefunc *tf;
	Ctypesu *ts;
	Ctypeptr *tp;
	Ctypeundef *tu;
	Ctypebitfield *tw;


	t = (Ctype*)hd;
	switch(t->tkind){
	case Tvoid:
	case Tbase:
		return GCiterdone;
	case Tstruct:
	case Tunion:
		ts = (Ctypesu*)t;
		switch(ictx->n++){
		case 0:
			return (Val*)&ts->tag;
		case 1:
			return (Val*)&ts->field;
		case 2:
			return &ts->attr;
		default:
			return GCiterdone;
		}
	case Tenum:
		te = (Ctypeenum*)t;
		switch(ictx->n++){
		case 0:
			return (Val*)&te->tag;
		case 1:
			return (Val*)&te->sub;
		case 2:
			return (Val*)&te->konst;
		default:
			return GCiterdone;
		}
	case Ttypedef:
		td = (Ctypedef*)t;
		switch(ictx->n++){
		case 0:
			return (Val*)&td->sub;
		case 1:
			return (Val*)&td->tid;
		default:
			return GCiterdone;
		}
	case Tundef:
		tu = (Ctypeundef*)t;
		if(ictx->n++ > 0)
			return GCiterdone;
		else
			return (Val*)&tu->sub;
	case Tconst:
		tc = (Ctypeconst*)t;
		if(ictx->n++ > 0)
			return GCiterdone;
		else
			return (Val*)&tc->sub;
	case Tptr:
		tp = (Ctypeptr*)t;
		if(ictx->n++ > 0)
			return GCiterdone;
		else
			return (Val*)&tp->sub;
	case Tarr:
		ta = (Ctypearr*)t;
		switch(ictx->n++){
		case 0:
			return &ta->cnt;
		case 1:
			return (Val*)&ta->sub;
		default:
			return GCiterdone;
		}
	case Tfun:
		tf = (Ctypefunc*)t;
		switch(ictx->n++){
		case 0:
			return (Val*)&tf->sub;
		case 1:
			return (Val*)&tf->param;
		default:
			return GCiterdone;
		}
		break;
	case Tbitfield:
		tw = (Ctypebitfield*)t;
		switch(ictx->n++){
		case 0:
			return &tw->cnt;
		case 1:
			return &tw->bit0;
		case 2:
			return (Val*)&tw->sub;
		default:
			return GCiterdone;
		}
	default:
		bug();
	}
	return 0;
}

u32
hashctype(Ctype *t)
{
	u32 x;
	Ctype *t;
	Ctypearr *ta;
	Ctypearr *tf;

	x = hashu64(t->kind);
	switch(t->tkind){
	case Tvoid:
		return x;
	case Tbase:
		x = hashx(x, hashu64(typecbase(t)));
		x = hashx(x, hashu64(typerep(t)));
		return x;
	case Ttypedef:
		return hashx(x, hashstr(typetid(t)));
	case Tstruct:
	case Tunion:
	case Tenum:
		return hashx(x, hashstr(typetag(t)));
	case Tundef:
		return hashx(x, hashctype(subtype(t)));
	case Tconst:
		return hashx(x, hashctype(subtype(t)));
	case Tptr:
		x = hashx(x, hashu64(typerep(t)));
		x = hashx(x, hashctype(subtype(t)));
		return x;
	case Tarr:
		ta = (Ctypearr*)t;
		if(Vkind(ta->cnt) == Qcval)
			x = hashx(x, hashcval(valcval(ta)));
		x = hashx(x, hashctype(subtype(t)));
		return x;
	case Tfun:
		tf = (Ctypefunc*)t;
		x = hashx(x, hashctype(subtype(t)));
		for(m = 0; m < tf->param->len; m++)
			x = hashx(x, hashctype(paramtype(funcparam(tf, m))));
		return x;
	case Tbitfield:
		tw = (Ctypebitfield*)t;
		x = hashx(x, hashcval(valcval(tw->cnt)));
		x = hashx(x, hashcval(valcval(tw->bit0)));
		x = hashx(x, hashctype(subtype(t)));
		return x;
	default:
		bug();
	}
}

int
equalctype(Ctype *a, Ctype *b)
{
	Imm m;
	Ctypearr *ata, *bta;
	Ctypebase *atb, *btb;
	Ctypeconst *atc, *btc;
	Ctypedef *ats, *bts;
	Ctypeenum *ate, *bte;
	Ctypefunc *atf, *btf;
	Ctypeptr *atp, *btp;
	Ctypesu *ats, *bts;
	Ctypeundef *atu, *btu;
	Ctypebitfield *atw, *btw;

	if(a->tkind != b->tkind)
		return 0;
	switch(a->tkind){
	case Tvoid:
		return 1;
	case Tbase:
		return (typecbase(a) == typecbase(b)
			&& typerep(a) == typerep(b));
	case Ttypedef:
		atd = (Ctypedef*)a;
		btd = (Ctypedef*)b;
		return equalstr(atd->tid, btd->tid);
	case Tstruct:
	case Tunion:
	case Tenum:
		return equalstr(typetag(a), typetag(b));
	case Tptr:
		return (typerep(a) == typerep(b)
			&& equalctype(subtype(a), subtype(b)));
	case Tarr:
		ata = (Ctypearr*)a;
		bta = (Ctypearr*)b;
		if(Vkind(ata->cnt) != Vkind(bta->cnt))
			return 0;
		if(Vkind(ata->cnt) == Qcval)
			if(!equalcval(valcval(ata->cnt), valcval(bta->cnt)))
				return 0;
		return equalctype(subtype(a), subtype(b));
	case Tfun:
		atf = (Ctypefunc*)a;
		btf = (Ctypefunc*)b;
		if(atf->param->len != btf->param->len)
			return 0;
		if(!equalctype(subtype(a), subtype(b)))
			return 0;
		for(m = 0; m < atf->param->len; m++)
			if(!equalctype(paramtype(funcparam(atf, m)),
				       paramtype(funcparam(btf, m))))
				return 0;
		return 1;
	case Tbitfield:
		atw = (Ctypebitfield*)a;
		btw = (Ctypebitfield*)b;
		return (equalcval(valcval(atw->cnt), valcval(btw->cnt))
			&& equalcval(valcval(atw->bit0), valcval(btw->bit0))
			&& equalctype(subtype(a), subtype(b)));
	case Tconst:
	case Tundef:
		return equalctype(subtype(a), subtype(b));
	}
	bug();
}


Ctype*
subtype(Ctype *t)
{
	Ctypearr *ta;
	Ctypebase *tb;
	Ctypeconst *tc;
	Ctypedef *ts;
	Ctypeenum *te;
	Ctypefunc *tf;
	Ctypesu *ts;
	Ctypeptr *tp;
	Ctypeundef *tu;
	Ctypebitfield *tw;

	switch(t->kind){
	case Tenum:
		te = (Ctypeenum*)t;
		return te->sub;
	case Ttypedef:
		tt = (Ctypedef*)t;
		return tt->sub;
	case Tfunc:
		tf = (Ctypefunc*)t;
		return tt->sub;
	case Tptr:
		tp = (Ctypeptr*)t;
		return tp->sub;
	case Tarr:
		ta = (Ctypearr*)t;
		return ta->sub;
	case Tbitfield:
		tw = (Ctypebitfield*)t;
		return tw->sub;
	case Tconst:
		tc = (Ctypeconst*)t;
		return tc->sub;
	case Tundef:
		tc = (Ctypeundef*)t;
		return tc->undef;
	default:
		bug();
	}
}

Ctype*
chasetype(Ctype *t)
{
	switch(t->kind){
	case Ttypedef:
	case Tenum:
		return chasetype(subtype(t));
	default:
		return t;
	}
}

Ctype*
safechasetype(Ctype *t)
{
	if(t == 0)
		return 0;
	switch(t->kind){
	case Ttypedef:
	case Tenum:
		return safechasetype(subtype(t));
	default:
		return t;
	}
}

Str*
typetid(Ctype *t)
{
	Ctypetypedef *td;
	switch(t->kind){
	case Ttypedef:
		td = (Ctypedef*)t;
		return t->tid;
	default:
		bug();
	}
}

Str*
typetag(Ctype *t)
{
	Ctypeenum *te;
	Ctypesu *ts;

	switch(t->kind){
	case Tstruct:
	case Tunion:
		ts = (Ctypesu*)t;
		return ts->tag;
	case Tenum:
		te = (Ctypeenum*)t;
		return te->tag;
	default:
		bug();
	}

}

Cbase
typecbase(Ctype *t)
{
	Ctypebase *tb;

	switch(t->kind){
	case Tbase:
		tb = (Ctypebase*)t;
		return tb->cbase;
	default:
		bug();
	}
}

Vec*
funcparam(Ctypefunc *tf, Imm i)
{
	return valvec(vecref(tf->param, i));
}

Rkind
typerep(Ctype *t)
{
	Ctypebase *tb;
	Ctypeenum *te;
	Ctypeptr *tp;

	switch(t->kind){
	case Tbase:
		tb = (Ctypebase*)t;
		return tb->rep;
	case Tptr:
		tp = (Ctypeptr*)t;
		return tp->rep;
	case Tenum:
		te = (Ctypeenum*)t;
		return te->rep;
	default:
		bug();
	}
}

/* the existence of this function is unfortunate.
   don't make further use of it. */
void
typesetrep(Ctype *t, Rkind rep)
{
	Ctypebase *tb;
	Ctypeenum *te;
	Ctypeptr *tp;

	switch(t->kind){
	case Tbase:
		tb = (Ctypebase*)t;
		tb->rep = rep;
		break;
	case Tptr:
		tp = (Ctypeptr*)t;
		tp->rep = rep;
		break;
	case Tenum:
		te = (Ctypeenum*)t;
		te->rep = rep;
		break;
	default:
		bug();
	}
}

static Ctype*
mkctype(Tkind kind)
{
	Ctype *t;
	switch(kind){
	case Tvoid:
		t = (Ctype*)malv(Qctype, sizeof(Ctype));
		break;
	case Tbase:
		t = (Ctype*)malv(Qctype, sizeof(Ctypebase));
		break;
	case Tstruct:
	case Tunion:
		t = (Ctype*)malv(Qctype, sizeof(Ctypesu));
		break;
	case Tenum:
		t = (Ctype*)malv(Qctype, sizeof(Ctypeenum));
		break;
	case Ttypedef:
		t = (Ctype*)malv(Qctype, sizeof(Ctypedef));
		break;
	case Tptr:
		t = (Ctype*)malv(Qctype, sizeof(Ctypeptr));
		break;
	case Tarr:
		t = (Ctype*)malv(Qctype, sizeof(Ctypearr));
		break;
	case Tfun:
		t = (Ctype*)malv(Qctype, sizeof(Ctypefunc));
		break;
	case Tundef:
		t = (Ctype*)malv(Qctype, sizeof(Ctypeundef));
		break;
	case Tbitfield:
		t = (Ctype*)malv(Qctype, sizeof(Ctypebitfield));
		break;
	case Tconst:
		t = (Ctype*)malv(Qctype, sizeof(Ctypeconst));
		break;
	default:
		bug();
	}
	t->tkind = kind;
	return t;
}

Ctype*
mkctypevoid(void)
{
	return mkctype(Tvoid);
}

Ctype*
mkctypebase(Cbase cbase, Rkind rep)
{
	Ctypebase *cb;
	cb = (Ctypebase*)mkctype(Tbase);
	cb->cbase = cbase;
	cb->rep = rep;
	return (Ctype*)cb;
}


Ctype*
mkctypedef(Str *tid, Ctype *sub)
{
	Ctypedef *td;
	td = (Ctypedef*)mkctype(Ttypedef);
	td->tid = tid;
	td->sub = sub;
	return (Ctype*)td;
}

Ctype*
mkctypesu(Tkind kind, Str *tag, Vec *field, Val attr)
{
	Ctypesu *ts;
	ts = (Ctypesu*)mkctype(kind);
	ts->tag = tag;
	ts->field = field;
	ts->attr = attr;
	return (Ctype*)ts;
}

Ctype*
mkctypeenum(Str *tag, Ctype *sub, Vec *konst)
{
	Ctypeenum *te;
	te = (Ctypeenum*)mkctype(Tenum);
	te->tag = tag;
	te->sub = sub;
	te->konst = konst;
	return (Ctype*)te;
}

Ctype*
mkctypeptr(Ctype *sub, Rkind rep)
{
	Ctypeptr *tp;
	tp = (Ctypeptr*)mkctype(Tptr);
	tp->sub = sub;
	tp->rep = rep;
	return (Ctype*)tp;
}

Ctype*
mkctypefunc(Ctype *sub, Vec *param)
{
	Ctypefunc *tf;
	tf = (Ctypefunc*)mkctype(Tfunc);
	tf->sub = sub;
	tf->param = param;
	return (Ctype*)tf;
}

Ctype*
mkctypearr(Ctype *sub, Val cnt)
{
	Ctypearr *ta;
	ta = (Ctypearr*)mkctype(Tarr);
	ta->sub = sub;
	ta->cnt = cnt;
	return (Ctype*)ta;
}

Ctype*
mkctypebitfield(Ctype *sub, Val cnt, Val bit0)
{
	Ctypebitfield *tw;
	tw = (Ctypebitfield*)mkctype(Tbitfield);
	tw->sub = sub;
	tw->cnt = cnt;
	tw->bit0 = bit0;
	return (Ctype*)tw;
}

Ctype*
mkctypeundef(Ctype *sub)
{
	Ctypeundef *tu;
	tu = (Ctypeundef*)mkctype(Tundef);
	tu->sub = sub;
	return (Ctype*)tu;
}

Ctype*
mkctypeconst(Ctype *sub)
{
	Ctypeconst *tc;
	tc = (Ctypeconst*)mkctype(Tconst);
	tc->sub = sub;
	return (Ctype*)tc;
}

Imm
typesize(VM *vm, Ctype *t)
{
	Cval *cv;
	Str *es;
	Rkind rep;
	Ctypearr *ta;
	Ctypesu *ts;

	if(t == 0)
		vmerr(vm, "attempt to determine size of undefined type");
	switch(t->tkind){
	case Tvoid:
		vmerr(vm, "attempt to determine size of void type");
	case Tbase:
	case Tptr:
		rep = typerep(t);
		if(rep == Rundef)
			vmerr(vm, "attempt to determine size of undefined type")
		return repsize[rep];
	case Ttypedef:
	case Tconst:
	case Tenum:
		return typesize(vm, subtype(t));
	case Tstruct:
	case Tunion:
		ts = (Ctypesu*)t;
		cv = valcval(attroff(ts->attr));
		return cv->val;
	case Tarr:
		ta = (Ctypearr*)t;
		if(Vkind(ta->cnt) != Qcval)
			vmerr(vm,
			      "attempt to determine size of unspecified array");
		cv = valcval(xtn->cnt);
		return cv->val*typesize(vm, subtype(t));
	case Tfun:
		vmerr(vm, "attempt to determine size of function type");
	case Tbitfield:
		vmerr(vm, "attempt to determine size of bitfield");
	case Tundef:
		es = fmtctype(subtype(t));
		vmerr(vm, "attempt to determine size of undefined type: %.*s",
		      (int)es->len, strdata(es));
	}
	fatal("bug");
}
