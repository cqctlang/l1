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

Imm repsize[Rnrep] = {
	[Ru08le]=	1,
	[Ru16le]=	2,
	[Ru32le]=	4,
	[Ru64le]=	8,
	[Rs08le]=	1,
	[Rs16le]=	2,
	[Rs32le]=	4,
	[Rs64le]=	8,
	[Ru08be]=	1,
	[Ru16be]=	2,
	[Ru32be]=	4,
	[Ru64be]=	8,
	[Rs08be]=	1,
	[Rs16be]=	2,
	[Rs32be]=	4,
	[Rs64be]=	8,
	[Rf32]=		4,
	[Rf64]=		8,
};


static Vec*	funcparam(Ctypefunc *tf, Imm m);

u32
hashqvctype(Ctype *t)
{
	return hashctype(t);
}

int
eqvctype(Ctype *a, Ctype *b)
{
	return equalctype(a, b);
}

u32
hashctype(Ctype *t)
{
	u32 x;
	Ctypearr *ta;
	Ctypefunc *tf;
	Ctypebitfield *tw;
	Imm m;

	x = hashu64(t->tkind);
	switch(t->tkind){
	case Tvoid:
		return x;
	case Tbase:
		x = hashx(x, hashu64(typecbase(t)));
		x = hashx(x, hashu64(typerep(t)));
		return x;
	case Ttypedef:
		return hashx(x, hashp(typetid(t)));
	case Tstruct:
	case Tunion:
	case Tenum:
		return hashx(x, hashp(typetag(t)));
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
	Ctypefunc *atf, *btf;
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
		return typetid(a) == typetid(b);
	case Tstruct:
	case Tunion:
	case Tenum:
		return typetag(a) == typetag(b);
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
			&& equalval(atw->bit0, btw->bit0)
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
	Ctypeconst *tc;
	Ctypedef *td;
	Ctypeenum *te;
	Ctypefunc *tf;
	Ctypeptr *tp;
	Ctypeundef *tu;
	Ctypebitfield *tw;

	switch(t->tkind){
	case Tenum:
		te = (Ctypeenum*)t;
		return te->sub;
	case Ttypedef:
		td = (Ctypedef*)t;
		return td->sub;
	case Tfun:
		tf = (Ctypefunc*)t;
		return tf->sub;
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
		tu = (Ctypeundef*)t;
		return tu->sub;
	default:
		bug();
	}
}

/* the existence of this function is unfortunate.
   don't make further use of it. */
void
setsubtype(Ctype *t, Ctype *s)
{
	Ctypearr *ta;
	Ctypeconst *tc;
	Ctypedef *td;
	Ctypeenum *te;
	Ctypefunc *tf;
	Ctypeptr *tp;
	Ctypeundef *tu;
	Ctypebitfield *tw;

	gcwb(mkvalctype(t));
	switch(t->tkind){
	case Tenum:
		te = (Ctypeenum*)t;
		te->sub = s;
		break;
	case Ttypedef:
		td = (Ctypedef*)t;
		td->sub = s;
		break;
	case Tfun:
		tf = (Ctypefunc*)t;
		tf->sub = s;
		break;
	case Tptr:
		tp = (Ctypeptr*)t;
		tp->sub = s;
		break;
	case Tarr:
		ta = (Ctypearr*)t;
		ta->sub = s;
		break;
	case Tbitfield:
		tw = (Ctypebitfield*)t;
		tw->sub = s;
		break;
	case Tconst:
		tc = (Ctypeconst*)t;
		tc->sub = s;
		break;
	case Tundef:
		tu = (Ctypeundef*)t;
		tu->sub = s;
		break;
	default:
		bug();
	}
}

Ctype*
chasetype(Ctype *t)
{
	switch(t->tkind){
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
	switch(t->tkind){
	case Ttypedef:
	case Tenum:
		return safechasetype(subtype(t));
	default:
		return t;
	}
}

Cid*
typetid(Ctype *t)
{
	Ctypedef *td;
	switch(t->tkind){
	case Ttypedef:
		td = (Ctypedef*)t;
		return td->tid;
	default:
		bug();
	}
}

Cid*
typetag(Ctype *t)
{
	Ctypeenum *te;
	Ctypesu *ts;

	switch(t->tkind){
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

	switch(t->tkind){
	case Tbase:
		tb = (Ctypebase*)t;
		return tb->cbase;
	case Tptr:
		return Vptr;
	default:
		bug();
	}
}

Rkind
typerep(Ctype *t)
{
	Ctypebase *tb;
	Ctypeptr *tp;

	switch(t->tkind){
	case Tbase:
		tb = (Ctypebase*)t;
		return tb->rep;
	case Tptr:
		tp = (Ctypeptr*)t;
		return tp->rep;
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
	Ctypeptr *tp;

	switch(t->tkind){
	case Tbase:
		tb = (Ctypebase*)t;
		tb->rep = rep;
		break;
	case Tptr:
		tp = (Ctypeptr*)t;
		tp->rep = rep;
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

enum
{
	cbshift = 5, /* sufficient to hold Rnrep values */
};

#define tcidx(cb,rep) ((cb)<<cbshift|(rep))

static Ctype*
_mkctypevoid(void)
{
	return mkctype(Tvoid);
}

static Ctype*
_mkctypebase(Cbase cbase, Rkind rep)
{
	Ctypebase *cb;
	cb = (Ctypebase*)mkctype(Tbase);
	cb->cbase = cbase;
	cb->rep = rep;
	return (Ctype*)cb;
}

static void
inittypecache()
{
	Cbase cb;
	Rkind rep;
	Vec *v;

	v = mkvec(Vnallbase<<cbshift);
	for(cb = Vlo; cb <= Vnbase; cb++)
		for(rep = Rundef; rep < Rnrep; rep++)
			vecset(v,
			       tcidx(cb, rep),
			       mkvalctype(_mkctypebase(cb, rep)));
	vecset(v,
	       tcidx(Vvoid, Rundef),
	       mkvalctype(_mkctypevoid()));
	typecache = mkvalvec(v);
}

static void
finitypecache()
{
	typecache = Xnil;
}

Ctype*
mkctypevoid(void)
{
	return valctype(vecref(valvec(typecache), tcidx(Vvoid, Rundef)));
}

Ctype*
mkctypebase(Cbase cbase, Rkind rep)
{
	return valctype(vecref(valvec(typecache), tcidx(cbase, rep)));
}

Ctype*
mkctypedef(Cid *tid, Ctype *sub)
{
	Ctypedef *td;
	td = (Ctypedef*)mkctype(Ttypedef);
	td->tid = tid;
	td->sub = sub;
	return (Ctype*)td;
}

Ctype*
mkctypesu(Tkind kind, Cid *tag, Vec *field, Val attr)
{
	Ctypesu *ts;
	ts = (Ctypesu*)mkctype(kind);
	ts->tag = tag;
	ts->field = field;
	ts->attr = attr;
	return (Ctype*)ts;
}

Ctype*
mkctypeenum(Cid *tag, Ctype *sub, Vec *konst)
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
	tf = (Ctypefunc*)mkctype(Tfun);
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
	Val v;
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
			vmerr(vm,
			      "attempt to determine size of undefined type");
		return repsize[rep];
	case Ttypedef:
	case Tconst:
	case Tenum:
		return typesize(vm, subtype(t));
	case Tstruct:
	case Tunion:
		ts = (Ctypesu*)t;
		v = attroff(ts->attr);
		if(Vkind(v) == Qnil)
			vmerr(vm, "attempt to determine size of "
			      "aggregate with no size");
		return cvalu(valcval(v));
	case Tarr:
		ta = (Ctypearr*)t;
		if(Vkind(ta->cnt) != Qcval)
			vmerr(vm,
			      "attempt to determine size of unspecified array");
		cv = valcval(ta->cnt);
		return cvalu(cv)*typesize(vm, subtype(t));
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

static Vec*
funcparam(Ctypefunc *tf, Imm m)
{
	return valvec(vecref(tf->param, m));
}

static void
l1_mkctype_void(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_void");
	USED(argv);
	*rv = mkvalctype(mkctypevoid());
}

static void
domkctype_base(Cbase name, Rkind rep, Val *rv)
{
	*rv = mkvalctype(mkctypebase(name, rep));
}

static void
l1_mkctype_base(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Cbase cb;
	Rkind rep;

	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		cb = cvalu(cv);
		rep = Rundef;
	}else if(argc == 2){
		checkarg(vm, argv, 0, Qcval);
		checkarg(vm, argv, 1, Qcval);
		cv = valcval(argv[0]);
		cb = cvalu(cv);
		cv = valcval(argv[1]);
		rep = cvalu(cv);
	}else
		vmerr(vm, "wrong number of arguments to mkctype_base");

	if(cb == Vvoid){
		*rv = mkvalctype(mkctypevoid());
		return;
	}
	if(cb < Vlo || cb > Vptr)
		// FIXME: beware of changes to Cbase
		vmerr(vm, "invalid base type identifier");
	if(rep >= Rnrep)
		vmerr(vm, "invalid representation identifier");
	domkctype_base(cb, rep, rv);
}

static void
l1_mkctype_bool(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_bool");
	domkctype_base(Vbool, rep, rv);
}

static void
l1_mkctype_char(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_char");
	domkctype_base(Vchar, rep, rv);
}

static void
l1_mkctype_short(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_short");
	domkctype_base(Vshort, rep, rv);
}

static void
l1_mkctype_int(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_int");
	domkctype_base(Vint, rep, rv);
}

static void
l1_mkctype_long(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_long");
	domkctype_base(Vlong, rep, rv);
}

static void
l1_mkctype_vlong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_vlong");
	domkctype_base(Vvlong, rep, rv);
}

static void
l1_mkctype_uchar(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uchar");
	domkctype_base(Vuchar, rep, rv);
}

static void
l1_mkctype_ushort(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ushort");
	domkctype_base(Vushort, rep, rv);
}

static void
l1_mkctype_uint(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uint");
	domkctype_base(Vuint, rep, rv);
}

static void
l1_mkctype_ulong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ulong");
	domkctype_base(Vulong, rep, rv);
}

static void
l1_mkctype_uvlong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uvlong");
	domkctype_base(Vuvlong, rep, rv);
}

static void
l1_mkctype_float(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_float");
	domkctype_base(Vfloat, rep, rv);
}

static void
l1_mkctype_double(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_double");
	domkctype_base(Vdouble, rep, rv);
}

static void
l1_mkctype_ldouble(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ldouble");
	domkctype_base(Vlongdouble, rep, rv);
}

static void
l1_mkctype_complex(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_complex");
	domkctype_base(Vcomplex, rep, rv);
}

static void
l1_mkctype_doublex(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_doublex");
	domkctype_base(Vdoublex, rep, rv);
}

static void
l1_mkctype_ldoublex(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rkind rep;
	Cval *cv;
	rep = Rundef;
	if(argc == 1){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		rep = cvalu(cv);
		if(rep >= Rnrep)
			vmerr(vm, "invalid representation identifier");
	}else if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ldoublex");
	domkctype_base(Vlongdoublex, rep, rv);
}

static void
l1_mkctype_ptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Cval *cv;
	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkctype_ptr");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to mkctype_ptr must be a pointer ctype");
	t = valctype(argv[0]);
	if(argc == 1)
		t = mkctypeptr(t, Rundef);
	else{
		if(Vkind(argv[1]) != Qcval)
			vmerr(vm, "invalid pointer representation");
		cv = valcval(argv[1]);
		if(cvalu(cv) <= Rundef || cvalu(cv) >= Rnrep)
			vmerr(vm, "invalid pointer representation");
		t = mkctypeptr(t, cvalu(cv));
	}
	*rv = mkvalctype(t);
}

static void
l1_mkctype_typedef(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t, *sub;
	Cid *id;

	t = 0;
	switch((unsigned)argc){
	case 1:
		checkarg(vm, argv, 0, Qcid);
		id = valcid(argv[0]);
		t = mkctypedef(id, 0);
		break;
	case 2:
		checkarg(vm, argv, 0, Qcid);
		checkarg(vm, argv, 1, Qctype);
		id = valcid(argv[0]);
		sub = valctype(argv[1]);
		t = mkctypedef(id, sub);
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_typedef");
	}
	*rv = mkvalctype(t);
}

static void
domkctype_su(VM *vm, Tkind tkind, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Cid *id;
	Vec *f;

	t = 0;
	switch((unsigned)argc){
	case 1:
		/* TAG */
		checkarg(vm, argv, 0, Qcid);
		id = valcid(argv[0]);
		t = mkctypesu(tkind, id, 0, 0);
		break;
	case 3:
		/* TAG FIELDS SIZE */
		checkarg(vm, argv, 0, Qcid);
		checkarg(vm, argv, 1, Qvec);
		if(Vkind(argv[2]) != Qcval
		   && Vkind(argv[2]) != Qtab
		   && Vkind(argv[2]) != Qnil)
			vmerr(vm, "operand 3 to %s must be a cvalue, "
			      "table, or nil", vmfnid(vm));
		id = valcid(argv[0]);
		f = valvec(argv[1]);
		if(!issymvec(f))
			vmerr(vm, "bad field vector");
		t = mkctypesu(tkind, id, f, mkattr(argv[2]));
		break;
	default:
		vmerr(vm, "wrong number of arguments to %s", vmfnid(vm));
	}
	*rv = mkvalctype(t);
}

static void
l1_mkctype_struct(VM *vm, Imm argc, Val *argv, Val *rv)
{
	domkctype_su(vm, Tstruct, argc, argv, rv);
}

static void
l1_mkctype_union(VM *vm, Imm argc, Val *argv, Val *rv)
{
	domkctype_su(vm, Tunion, argc, argv, rv);
}

static void
l1_mkctype_array(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t, *sub;

	t = 0;
	switch((unsigned)argc){
	case 1:
		/* TYPE */
		checkarg(vm, argv, 0, Qctype);
		sub = valctype(argv[0]);
		t = mkctypearr(sub, Xnil); /* Xnil? */
		break;
	case 2:
		/* TYPE CNT */
		checkarg(vm, argv, 0, Qctype);
		checkarg(vm, argv, 1, Qcval);
		sub = valctype(argv[0]);
		t = mkctypearr(sub, argv[1]);
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_array");
	}
	*rv = mkvalctype(t);
}

static void
l1_mkctype_fn(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t, *sub;
	Vec *p;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mkctype_fn");
	checkarg(vm, argv, 0, Qctype);
	checkarg(vm, argv, 1, Qvec);
	sub = valctype(argv[0]);
	p = valvec(argv[1]);
	if(!issymvec(p))
		vmerr(vm, "bad parameter vector");
	t = mkctypefunc(sub, p);
	*rv = mkvalctype(t);
}

static void
l1_mkctype_bitfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t, *sub;
	/* TYPE WIDTH POS */
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to mkctype_bitfield");
	checkarg(vm, argv, 0, Qctype);
	checkarg(vm, argv, 1, Qcval);
	if(Vkind(argv[2]) != Qcval && Vkind(argv[2]) != Qnil)
		vmerr(vm, "operand 3 to mkctype_bitfield must be a "
		      "cvalue or nil");
	sub = valctype(argv[0]);
	t = mkctypebitfield(sub, argv[1], argv[2]);
	*rv = mkvalctype(t);
}

static void
l1_mkctype_enum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t, *sub;
	Cid *id;
	Vec *c;

	t = 0;
	switch((unsigned)argc){
	case 1:
		/* TAG */
		checkarg(vm, argv, 0, Qcid);
		id = valcid(argv[0]);
		t = mkctypeenum(id, 0, 0);
		break;
	case 2:
		/* TAG CONSTS (FIXME: is this a good form?) */
		checkarg(vm, argv, 0, Qcid);
		checkarg(vm, argv, 1, Qvec);
		id = valcid(argv[0]);
		c = valvec(argv[1]);
		t = mkctypeenum(id, 0, c);
		break;
	case 3:
		/* TAG CONSTS TYPE */
		checkarg(vm, argv, 0, Qcid);
		checkarg(vm, argv, 1, Qvec);
		checkarg(vm, argv, 2, Qctype);
		id = valcid(argv[0]);
		c = valvec(argv[1]);
		sub = valctype(argv[2]);
		t = mkctypeenum(id, sub, c);
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_enum");
	}
	*rv = mkvalctype(t);
}

static void
l1_mkctype_const(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t, *sub;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkctype_const");
	checkarg(vm, argv, 0, Qctype);
	sub = valctype(argv[0]);
	t = mkctypeconst(sub);
	*rv = mkvalctype(t);
}

static void
l1_mkctype_undef(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkctype_undef");
	checkarg(vm, argv, 0, Qctype);
	*rv = mkvalctype(mkctypeundef(valctype(argv[0])));
}

static void
dotypepredicate(VM *vm, Imm argc, Val *argv, Val *rv, char *id, unsigned kind)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", id);
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to %s must be a ctype", id);
	t = valctype(argv[0]);
	if(t->tkind == kind)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isvoid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isvoid", Tvoid);
}

static void
l1_isundeftype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isundeftype", Tundef);
}

static void
l1_isbase(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isbase", Tbase);
}

static void
l1_isstruct(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isstruct", Tstruct);
}

static void
l1_isunion(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isunion", Tunion);
}

static void
l1_issu(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to issu");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to issu must be a ctype");
	t = valctype(argv[0]);
	if(t->tkind == Tstruct || t->tkind == Tunion)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isenum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isenum", Tenum);
}

static void
l1_isenumconst(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isenumconst", Tconst);
}

static void
l1_isbitfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isbitfield", Tbitfield);
}

static void
l1_isptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isptr", Tptr);
}

static void
l1_isarray(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isarray", Tarr);
}

static void
l1_isfunc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isfunc", Tfun);
}

static void
l1_istypedef(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "istypedef", Ttypedef);
}

static void
l1_baseid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to baseid");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tbase)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	*rv = mkvalstr(mkstr0(cbasename[typecbase(t)]));
}

static void
l1_basebase(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to basebase");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to basebase must be a base ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tbase && t->tkind != Tptr)
		vmerr(vm, "operand 1 to basebase must be a base ctype");
	*rv = mkvallitcval(Vuchar, typecbase(t));
}

static void
l1_typekind(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to typekind");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to typekind must be a ctype");
	t = valctype(argv[0]);
	*rv = mkvallitcval(Vuchar, t->tkind);
}


static void
l1_baserep(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to baserep");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to baserep must be a base ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tbase && t->tkind != Tptr)
		vmerr(vm, "operand 1 to baserep must be a base ctype");
	*rv = mkvallitcval(Vuchar, typerep(t));
}

static void
l1_subtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	static char *err =
		"operand 1 to subtype must be an "
		"array, pointer, enum, or enum constant";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to subtype");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, err);

	t = valctype(argv[0]);
	if(t->tkind == Ttypedef)
		t = chasetype(t);
	switch(t->tkind){
	case Tptr:
	case Tarr:
	case Tenum:
	case Tconst:
	case Tundef:
		break;
	default:
		vmerr(vm, err);
	}
	t = subtype(t);
	if(t)
		*rv = mkvalctype(t);
}

static void
l1_rettype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rettype");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to rettype must be a function ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tfun)
		vmerr(vm, "operand 1 to rettype must be a function ctype");
	*rv = mkvalctype(subtype(t));
}

static void
l1_suekind(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suekind");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to suekind must be a tagged ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tstruct && t->tkind != Tunion
	   && t->tkind != Tenum)
		vmerr(vm, "operand 1 to suekind must be a tagged ctype");
	*rv = mkvalstr(mkstr0(tkindstr[t->tkind]));
}

static void
l1_suetag(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suetag");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to suetag must be a tagged ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tstruct && t->tkind != Tunion
	   && t->tkind != Tenum)
		vmerr(vm, "operand 1 to sutag must be a tagged ctype");
	*rv = mkvalcid(typetag(t));
}

static void
l1_susize(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypesu *ts;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to susize");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm,
		      "operand 1 to susize must be a struct or union ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tstruct && t->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to susize must be a struct or union ctype");
	ts = (Ctypesu*)t;
	*rv = attroff(ts->attr);
}

static void
l1_suattr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypesu *ts;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suattr");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm,
		      "operand 1 to suattr must be a struct or union ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tstruct && t->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to suattr must be a struct or union ctype");
	ts = (Ctypesu*)t;
	*rv = ts->attr;
}

static void
l1_bitfieldwidth(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypebitfield *tw;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldwidth");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to bitfieldwidth "
		      "must be a bitfield ctype");

	t = valctype(argv[0]);
	if(t->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldwidth "
		      "must be a bitfield ctype");
	tw = (Ctypebitfield*)t;
	*rv = tw->cnt;
}

static void
l1_bitfieldcontainer(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldcontainer");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to bitfieldcontainer "
		      "must be a bitfield ctype");

	t = valctype(argv[0]);
	if(t->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldcontainer "
		      "must be a bitfield ctype");
	*rv = mkvalctype(subtype(t));
}


static void
l1_bitfieldpos(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypebitfield *tw;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldpos");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to bitfieldpos must be a bitfield ctype");

	t = valctype(argv[0]);
	if(t->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldpos must be a bitfield ctype");
	tw = (Ctypebitfield*)t;
	*rv = tw->bit0;
}

static void
l1_arraynelm(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypearr *ta;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to arraynelm");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to arraynelm must be an array ctype");

	t = valctype(argv[0]);
	if(t->tkind != Tarr)
		vmerr(vm, "operand 1 to arraynelm must be an array ctype");
	ta = (Ctypearr*)t;
	*rv = ta->cnt;
}

static void
l1_typedefid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to typedefid");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to typedefid must be a typedef ctype");
	t = valctype(argv[0]);
	if(t->tkind != Ttypedef)
		vmerr(vm, "operand 1 to typedefid must be a typedef ctype");
	*rv = mkvalcid(typetid(t));
}

static void
l1_typedeftype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to typedeftype");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	t = valctype(argv[0]);
	if(t->tkind != Ttypedef)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	t = subtype(t);
	if(t)
		*rv = mkvalctype(t);
}

/* this sleazy operation provides a way to terminate
   type resolution cycles */
static void
l1_settypedeftype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to settypedeftype");
	checkarg(vm, argv, 0, Qctype);
	checkarg(vm, argv, 1, Qctype);
	
	t = valctype(argv[0]);
	if(t->tkind != Ttypedef)
		vmerr(vm, "attempt to settypedeftype on non-typedef");
	setsubtype(t, valctype(argv[1]));
}

static void
l1_params(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypefunc *tf;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to params");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, "operand 1 to params must be a function ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tfun)
		vmerr(vm, "operand 1 to params must be a function ctype");
	tf = (Ctypefunc*)t;
	*rv = mkvalvec(tf->param);
}

static void
l1_fields(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypesu *ts;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fields");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	t = valctype(argv[0]);
	if(t->tkind != Tstruct && t->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	ts = (Ctypesu*)t;
	if(ts->field == 0)
		return;		/* nil */
	*rv = mkvalvec(ts->field);
}

static Val
rlookfield(VM *vm, Ctype *xsu, Val tag)
{
	Ctype *t;
	Ctypesu *su;
	Val vp, rp, id, o, fa, ra;
	Vec *f, *r;
	Imm i;

	su = (Ctypesu*)xsu;
	for(i = 0; i < su->field->len; i++){
		vp = vecref(su->field, i);
		f = valvec(vp);
		id = vecref(f, Idpos);
		if(Vkind(id) == Qcid){
			if(tag == id)
				return vp;
			else
				continue;
		}
		/* assume id is nil */
		t = chasetype(valctype(vecref(f, Typepos)));
		if(t->tkind != Tstruct && t->tkind != Tunion)
			continue;
		/* recursively search embedded anonymous field */
		rp = rlookfield(vm, t, tag);
		if(rp == 0)
			continue;
		r = valvec(rp);
		fa = attroff(vecref(f, Attrpos));
		ra = attroff(vecref(r, Attrpos));
		if(Vkind(fa) == Qnil || Vkind(ra) == Qnil)
			/* FIXME: are there cases when
			   this is sensible? */
			vmerr(vm,
			      "attempt to access field of undefined aggregate");
		o = mkvalcval2(xcvalalu(vm, Iadd, valcval(fa), valcval(ra)));
		r = veccopy(r);
		_vecset(r, Attrpos, copyattr(vecref(r, Attrpos), o));
		return mkvalvec(r);
	}
	return 0;
}

static void
l1_lookfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Val vp;
	static char *err1
		= "operand 1 to lookfield must be a struct or union ctype";
	static char *err2
		= "operand 2 to lookfield must be an identifier";

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to lookfield");
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, err1);
	t = valctype(argv[0]);
	t = chasetype(t);
	if(t->tkind != Tstruct && t->tkind != Tunion)
		vmerr(vm, err1);
	if(Vkind(argv[1]) != Qcid)
		vmerr(vm, err2);
	vp = rlookfield(vm, t, argv[1]);
	if(vp)
		*rv = vp;
}

static void
l1_enumconsts(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ctype *t;
	Ctypeenum *te;
	static char *err
		= "operand 1 to enumconsts must be a defined enum ctype";
	if(argc != 1)
		vmerr(vm, err);
	if(Vkind(argv[0]) != Qctype)
		vmerr(vm, err);
	t = valctype(argv[0]);
	if(t->tkind != Tenum)
		vmerr(vm, err);
	te = (Ctypeenum*)t;
	if(te->konst == 0)
		vmerr(vm, err);
	*rv = mkvalvec(te->konst);
}

Ctype*
typename(Ctype *td)
{
	Ctypearr *ta;
	Ctypefunc *tf;
	Ctypebitfield *tw;
	Vec *o, *p, *pv;
	Imm i;

	switch(td->tkind){
	case Tvoid:
		return td;
	case Tbase:
		return mkctypebase(typecbase(td), Rundef);
	case Tptr:
		return mkctypeptr(typename(subtype(td)), Rundef);
	case Tconst:
		return mkctypeconst(typename(subtype(td)));
	case Ttypedef:
		return mkctypedef(typetid(td), 0);
	case Tstruct:
	case Tunion:
	case Tenum:
		return mkctypesu(td->tkind, typetag(td), 0, 0);
	case Tarr:
		ta = (Ctypearr*)td;
		return mkctypearr(typename(subtype(td)), ta->cnt);
	case Tfun:
		tf = (Ctypefunc*)td;
		pv = mkvec(tf->param->len);
		for(i = 0; i < tf->param->len; i++){
			o = valvec(vecref(tf->param, i));
			p = mkvec(3);
			_vecset(p, Typepos,
				mkvalctype(typename(fieldtype(o))));
			_vecset(p, Idpos, vecref(o, Idpos));
			_vecset(p, Attrpos, vecref(o, Attrpos));
			_vecset(pv, i, mkvalvec(p));
		}
		return mkctypefunc(typename(subtype(td)), pv);
	case Tbitfield:
		tw = (Ctypebitfield*)td;
		return mkctypebitfield(typename(subtype(td)),
				       tw->cnt, tw->bit0);
	case Tundef:
		/* FIXME: this is made-up */
		return td;
	}
	bug();
}

static void
l1_typename(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to typename");
	checkarg(vm, argv, 0, Qctype);
	*rv = mkvalctype(typename(valctype(argv[0])));
}

static void
l1_equalctype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to equalctype");
	checkarg(vm, argv, 0, Qctype);
	checkarg(vm, argv, 1, Qctype);
	if(equalctype(valctype(argv[0]), valctype(argv[1])))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_hashctype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to hashctype");
	checkarg(vm, argv, 0, Qctype);
	*rv = mkvalcval2(mklitcval(Vuint, hashctype(valctype(argv[0]))));
}

void
fnctype(Env *env)
{
	FN(arraynelm);
	FN(baseid);
	FN(basebase);
	FN(baserep);
	FN(bitfieldcontainer);
	FN(bitfieldpos);
	FN(bitfieldwidth);
	FN(enumconsts);
	FN(equalctype);
	FN(fields);
	FN(hashctype);
	FN(isarray);
	FN(isbase);
	FN(isbitfield);
	FN(isenum);
	FN(isenumconst);
	FN(isfunc);
	FN(isptr);
	FN(isstruct);
	FN(issu);
	FN(istypedef);
	FN(isundeftype);
	FN(isunion);
	FN(isvoid);
	FN(lookfield);
	FN(mkctype_array);
	FN(mkctype_base);
	FN(mkctype_bitfield);
	FN(mkctype_bool);
	FN(mkctype_char);
	FN(mkctype_const);
	FN(mkctype_complex);
	FN(mkctype_double);
	FN(mkctype_doublex);
	FN(mkctype_enum);
	FN(mkctype_float);
	FN(mkctype_fn);
	FN(mkctype_int);
	FN(mkctype_ldouble);
	FN(mkctype_ldoublex);
	FN(mkctype_long);
	FN(mkctype_ptr);
	FN(mkctype_short);
	FN(mkctype_struct);
	FN(mkctype_typedef);
	FN(mkctype_uchar);
	FN(mkctype_uint);
	FN(mkctype_ulong);
	FN(mkctype_undef);
	FN(mkctype_union);
	FN(mkctype_ushort);
	FN(mkctype_uvlong);
	FN(mkctype_vlong);
	FN(mkctype_void);
	FN(params);
	FN(rettype);
	FN(settypedeftype);
	FN(subtype);
	FN(suekind);
	FN(suetag);
	FN(suattr);
	FN(susize);
	FN(typedefid);
	FN(typedeftype);
	FN(typekind);
	FN(typename);
}

void
inittype()
{
	inittypecache();
}

void
finitype()
{
	finitypecache();
}
