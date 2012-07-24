#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum{
	FmtWidth	= 1,
	FmtLeft		= FmtWidth << 1,
	FmtPrec		= FmtLeft << 1,
	FmtSharp	= FmtPrec << 1,
	FmtSpace	= FmtSharp << 1,
	FmtSign		= FmtSpace << 1,
	FmtZero		= FmtSign << 1,
	FmtFlag		= FmtZero << 1
};

/* distinct, cqct-specific approximation of Fmt in x/lib9 */
typedef struct Fmt Fmt;
struct Fmt {
	char *start, *to, *stop;
	int (*flush)(VM*, Fmt*);
	void *farg;
	int width;
	int prec;
	unsigned int flags;
};

static char* _fmtctype(Ctype *t, char *o);
static char* _fmtdecl(Ctype *t, Cid *is);

static char*
fmtplist(Vec *param)
{
	char **ds, *buf, *bp;
	Imm i, n;
	unsigned long m;
	Ctype *t;
	Vec *pvec;
	Val id;

	n = param->len;
	if(n == 0)
		return xstrdup("");

	ds = emalloc(n*sizeof(char**));
	m = 1;			/* null */
	for(i = 0; i < n; i++){
		pvec = valvec(vecref(param, i));
		t = valctype(vecref(pvec, Typepos));
		id = vecref(pvec, 1);
		if(Vkind(id) == Qcid)
			ds[i] = _fmtdecl(t, valcid(id));
		else
			ds[i] = _fmtctype(t, xstrdup(""));
		m += strlen(ds[i]);
		if(i < n-1)
			m += 2;	/* comma, space */
	}
	buf = emalloc(m);
	bp = buf;
	for(i = 0; i < n; i++){
		strcpy(bp, ds[i]);
		bp += strlen(ds[i]);
		efree(ds[i]);
		if(i < n-1){
			strcpy(bp, ", ");
			bp += 2;
		}
	}
	*bp = 0;
	efree(ds);
	return buf;
}

static char*
_fmtctype(Ctype *t, char *o)
{
	char *buf, *w, *pl;
	unsigned m, leno;
	Cid *id;
	Cbase cb;
	Cval *cv;
	Ctype *sub;
	Ctypearr *ta;
	Ctypefunc *tf;

	leno = strlen(o);
	switch(t->tkind){
	case Tvoid:
		m = 4+1+leno+1;
		buf = emalloc(m);
		if(leno)
			snprint(buf, m, "void %s", o);
		else
			snprint(buf, m, "void");
		efree(o);
		return buf;
	case Tbase:
		cb = typecbase(t);
		m = strlen(cbasename[cb])+1+leno+1;
		buf = emalloc(m);
		if(leno)
			snprint(buf, m, "%s %s", cbasename[cb], o);
		else
			snprint(buf, m, "%s", cbasename[cb]);
		efree(o);
		return buf;
	case Ttypedef:
		id = typetid(t);
		m = id->len+1+leno+1;
		buf = emalloc(m);
		if(leno)
			snprint(buf, m, "%.*s %s",
				(int)id->len, ciddata(id), o);
		else
			snprint(buf, m, "%.*s",
				(int)id->len, ciddata(id));
		efree(o);
		return buf;
	case Tstruct:
	case Tunion:
	case Tenum:
		w = tkindstr[t->tkind];
		id = typetag(t);
		if(id){
			m = strlen(w)+1+id->len+1+leno+1;
			buf = emalloc(m);
			if(leno)
				snprint(buf, m, "%s %.*s %s",
					w, (int)id->len, ciddata(id), o);
			else
				snprint(buf, m, "%s %.*s", w,
					(int)id->len, ciddata(id));
		}else{
			m = strlen(w)+1+leno+1;
			buf = emalloc(m);
			if(leno)
				snprint(buf, m, "%s %s", w, o);
			else
				snprint(buf, m, "%s", w);
		}
		efree(o);
		return buf;
	case Tundef:
		return _fmtctype(subtype(t), o);
	case Tptr:
		m = 2+leno+1+1;
		buf = emalloc(m);
		sub = subtype(t);
		if(sub->tkind == Tfun || sub->tkind == Tarr)
			snprint(buf, m, "(*%s)", o);
		else
			snprint(buf, m, "*%s", o);
		efree(o);
		return _fmtctype(sub, buf);
	case Tarr:
		ta = (Ctypearr*)t;
		m = leno+1+10+1+1;	/* assume max 10 digit size */
		buf = emalloc(m);
		if(Vkind(ta->cnt) == Qnil)
			snprint(buf, m, "%s[]", o);
		else{
			cv = valcval(ta->cnt);
			snprint(buf, m, "%s[%" PRIu64 "]", o, cvalu(cv));
		}
		efree(o);
		return _fmtctype(subtype(t), buf);
	case Tfun:
		tf = (Ctypefunc*)t;
		pl = fmtplist(tf->param);
		m = leno+1+strlen(pl)+1+1;
		buf = emalloc(m);
		snprint(buf, m, "%s(%s)", o, pl);
		efree(o);
		efree(pl);
		return _fmtctype(subtype(t), buf);
	case Tconst:
	case Tbitfield:
		/* this is questionable...maybe we want more accurate printed
		   representation of these objects, especially bitfields */
		return _fmtctype(subtype(t), o);
	}
	return 0;

}

static char*
_fmtdecl(Ctype *t, Cid *id)
{
	char *o;
	o = emalloc(id->len+1);
	memcpy(o, ciddata(id), id->len);
	return _fmtctype(t, o);
}

static Str*
fmtdecl(Ctype *t, Cid *id)
{
	char *s;
	Str *str;
	s = _fmtdecl(t, id);
	str = mkstr0(s);
	efree(s);
	return str;
}

char*
fmtctypec(Ctype *t)
{
	return _fmtctype(t, xstrdup(""));
}

Str*
fmtctype(Ctype *t)
{
	char *s;
	Str *str;
	s = fmtctypec(t);
	str = mkstr0(s);
	efree(s);
	return str;
}

static int
cval2int(Cval *cv)
{
	/* FIXME: implement with rerep? */
	Ctype *t;
	t = chasetype(cv->type);
	switch(typerep(t)){
	case Rs08le:
	case Rs08be:
		return (int)(s8)cvalu(cv);
	case Rs16le:
	case Rs16be:
		return (int)(s16)cvalu(cv);
	case Rs32le:
	case Rs32be:
		return (int)(s32)cvalu(cv);
	case Rs64le:
	case Rs64be:
		return (int)(s64)cvalu(cv);
	case Ru08le:
	case Ru08be:
		return (int)(u8)cvalu(cv);
	case Ru16le:
	case Ru16be:
		return (int)(u16)cvalu(cv);
	case Ru32le:
	case Ru32be:
		return (int)(u32)cvalu(cv);
	case Ru64le:
	case Ru64be:
		return (int)(u64)cvalu(cv);
	default:
		bug();
	}
}

static int
fmtputc(VM *vm, Fmt *f, char ch)
{
	if(f->to >= f->stop)
		if(f->flush(vm, f) == -1)
			return -1;
	*f->to++ = ch;
	return 0;
}

static int
fmtpad(VM *vm, Fmt *f, int n)
{
	char c;
	c = (f->flags&FmtZero) ? '0' : ' ';
	while(n-- > 0)
		if(fmtputc(vm, f, c))
			return -1;
	return 0;
}

static int
fmtputs(VM *vm, Fmt *f, char *p, Imm m)
{
	unsigned int fl, l;

	fl = f->flags;
	if((fl&FmtPrec) && m > f->prec)
		m = f->prec;
	l = m;			/* lost precision */
	if((fl&FmtWidth) && !(fl&FmtLeft)){
		if(fmtpad(vm, f, f->width-m))
			return -1;
	}
	while(l-- > 0)
		fmtputc(vm, f, *p++);
	if((fl&FmtWidth) && (fl&FmtLeft)){
		if(fmtpad(vm, f, f->width-m))
			return -1;
	}
	return 0;
}

static int
fmtputB(VM *vm, Fmt *f, char *p, Imm m)
{
	unsigned int fl, l;
	char *q;
	unsigned char c;
	Imm w;

	w = 0;
	fl = f->flags;
	if((fl&FmtPrec) && m > f->prec)
		m = f->prec;
	if(fl&FmtWidth){
		q = p;
		l = m;			/* lost precision */
		while(l-- > 0){
			c = *q++;
			if(xisgraph(c) || xisspace(c))
				w++;
			else
				w += 4;
		}
	}
	l = m;			/* lost precision */
	if((fl&FmtWidth) && !(fl&FmtLeft)){
		if(fmtpad(vm, f, f->width-w))
			return -1;
	}
	while(l-- > 0){
		c = *p++;
		if(c == '\\'){
			fmtputc(vm, f, '\\');
			fmtputc(vm, f, '\\');
		}else if(c == '\"'){
			fmtputc(vm, f, '\\');
			fmtputc(vm, f, '\"');
		}else if(xisgraph(c) || xisspace(c))
			fmtputc(vm, f, c);
		else{
			fmtputc(vm, f, '\\');
			fmtputc(vm, f, '0'+((c>>6)&0x3));
			fmtputc(vm, f, '0'+((c>>3)&0x7));
			fmtputc(vm, f, '0'+(c&0x7));
		}
	}
	if((fl&FmtWidth) && (fl&FmtLeft)){
		if(fmtpad(vm, f, f->width-w))
			return -1;
	}
	return 0;
}


static int
fmtputs0(VM *vm, Fmt *f, char *p)
{
	return fmtputs(vm, f, p, strlen(p));
}

static int
fmtval(VM *vm, Fmt *f, Val val)
{
	char buf[256];
	Cval *cv;
	Closure *cl;
	List *l;
	Pair *p;
	Vec *v;
	Range *r;
	Rd *rd;
	Cid *id;
	Rec *rec;
	Str *str;
	Val bv, rv;
	u32 m, n;
	Head *hd;
	Dom *d;
	As *as;
	Ns *ns;
	Ctype *t;
	float fv;
	double dv;
	Cbase cb;
	Expr *e;

	switch(Vkind(val)){
	case Qcval:
		cv = valcval(val);
		t = chasetype(cv->type);
		switch(t->tkind){
		case Tbase:
			cb = typecbase(t);
			if(isfloat[cb]){
				switch(cb){
				case Vfloat:
					fv = cvalf(cv);
					snprintf(buf, sizeof(buf), "%f",
						 (double)fv);
					break;
				case Vdouble:
					dv = cvald(cv);
					snprintf(buf, sizeof(buf), "%f", dv);
					break;
				case Vlongdouble:
				case Vcomplex:
				case Vdoublex:
				case Vlongdoublex:
					vmerr(vm, "formatter does not support %s",
					      cbasename[cb]);
				default:
					fatal("bug");
				}
			}else if(isunsigned[cb])
				snprint(buf, sizeof(buf), "%" PRIu64,
					cvalu(cv));
			else
				snprint(buf, sizeof(buf), "%" PRId64,
					(int64_t)cvalu(cv));
			break;
		case Tptr:
			snprint(buf, sizeof(buf), "%" PRIx64, cvalu(cv));
			break;
		default:
			fatal("bug");
		}
		return fmtputs0(vm, f, buf);
	case Qcl:
		cl = valcl(val);
		if(cl->fp){
			snprint(buf, sizeof(buf), "<continuation>", cl);
			return fmtputs0(vm, f, buf);
		}else if(cl->dlen > 0){
			if(fmtputs0(vm, f, "<closure "))
				return -1;
		}else{
			if(fmtputs0(vm, f, "<procedure "))
				return -1;
		}
		if(fmtputB(vm, f, ciddata(cl->id), cl->id->len-1))
			return -1;
		return fmtputs0(vm, f, ">");
	case Qundef:
		return 0;
	case Qnil:
		return fmtputs0(vm, f, "<nil>");
	case Qbox:
		if(fmtputs0(vm, f, "<box >"))
			return -1;
		bv = valboxed(val);
		if(fmtval(vm, f, bv))
			return -1;
		return fmtputs0(vm, f, ">");
	case Qns:
		ns = valns(val);
		if(ns->name)
			snprint(buf, sizeof(buf), "<ns %.*s>",
				(int)ns->name->len, strdata(ns->name));
		else
			snprint(buf, sizeof(buf), "<ns>");
		return fmtputs0(vm, f, buf);
	case Qas:
		as = valas(val);
		if(as->name)
			snprint(buf, sizeof(buf), "<as %.*s>",
				(int)as->name->len, strdata(as->name));
		else
			snprint(buf, sizeof(buf), "<as>");
		return fmtputs0(vm, f, buf);
	case Qdom:
		d = valdom(val);
		if(d->name)
			snprint(buf, sizeof(buf), "<domain %.*s>",
				(int)d->name->len, strdata(d->name));
		else
			snprint(buf, sizeof(buf), "<domain>");
		return fmtputs0(vm, f, buf);
	case Qfd:
	case Qtab:
		hd = valhead(val);
		snprint(buf, sizeof(buf), "<%s>", qname[Vkind(hd)]);
		return fmtputs0(vm, f, buf);
	case Qctype:
		str = fmtctype(valctype(val));
		return fmtputB(vm, f, strdata(str), str->len);
	case Qvec:
		v = valvec(val);
		if(fmtputs0(vm, f, "vector("))
			return -1;
		for(m = 0; m < v->len; m++){
			if(m > 0){
				if(fmtputs0(vm, f, ", "))
					return -1;
			}else{
				if(fmtputs0(vm, f, " "))
					return -1;
			}
			if(fmtval(vm, f, vecdata(v)[m]))
				return -1;
		}
		return fmtputs0(vm, f, " )");
	case Qlist:
		l = vallist(val);
		if(fmtputs0(vm, f, "["))
			return -1;
		for(m = 0; m < listlen(l); m++){
			if(m > 0){
				if(fmtputs0(vm, f, ", "))
					return -1;
			}else{
				if(fmtputs0(vm, f, " "))
					return -1;
			}
			if(fmtval(vm, f, listref(l, m)))
				return -1;
		}
		return fmtputs0(vm, f, " ]");
	case Qpair:
		p = valpair(val);
		if (isweak(&p->hd)) {
			if(fmtputs0(vm, f, "<W|"))
				return -1;
		}
		else {
			if(fmtputs0(vm, f, "<|"))
				return -1;
		}
		if(fmtval(vm, f, p->car))
			return -1;
		if(fmtputs0(vm, f, ", "))
			return -1;
		if(fmtval(vm, f, p->cdr))
			return -1;
		return fmtputs0(vm, f, " |>");
	case Qrange:
		r = valrange(val);
 		snprint(buf, sizeof(buf),
			 "<range 0x%" PRIx64 " 0x%" PRIx64 ">",
			cvalu(r->beg), cvalu(r->len));
		return fmtputs0(vm, f, buf);
	case Qcid:
		id = valcid(val);
		return fmtputB(vm, f, ciddata(id), id->len-1);
	case Qstr:
		str = valstr(val);
		if(fmtputs0(vm, f, "\""))
			return -1;
		if(fmtputB(vm, f, strdata(str), str->len))
			return -1;
		return fmtputs0(vm, f, "\"");
	case Qrd:
		rd = valrd(val);
		if(fmtputs0(vm, f, "<rd "))
			return -1;
		if(fmtputs(vm, f, strdata(rd->name), rd->name->len))
			return -1;
		return fmtputs0(vm, f, ">");
	case Qrec:
		rec = valrec(val);
		rv = safedovm(vm, rec->rd->fmt, 1, &val);
		if(Vkind(rv) != Qstr)
			vmerr(vm, "formatter for record type %.*s must "
			      "return a string",
			      (int)rec->rd->name->len, strdata(rec->rd->name));
		str = valstr(rv);
		return fmtputs(vm, f, strdata(str), str->len);
	case Qexpr:
		e = valexpr(val);
		switch(e->kind){
		case Eid:
			if(fmtputs0(vm, f, "#id("))
				return -1;
			if(fmtputs0(vm, f, ciddata(valcid(e->aux))))
				return -1;
			if(fmtputs0(vm, f, ")"))
				return -1;
			break;
		case Eval:
			if(fmtputs0(vm, f, "#val("))
				return -1;
			if(fmtval(vm, f, e->aux))
				return -1;
			if(fmtputs0(vm, f, ")"))
				return -1;
			break;
		case Enull:
			return fmtputs0(vm, f, "#[]");
		case Eelist:
			/* FIXME: don't assume the elist is well formed */
			if(fmtputs0(vm, f, "#["))
				return -1;
			n = elistlen(e);
			for(m = 0; m < n; m++, e = e->e2){
				if(m > 0){
					if(fmtputs0(vm, f, ", "))
						return -1;
				}else{
					if(fmtputs0(vm, f, " "))
						return -1;
				}
				if(fmtval(vm, f, e->e1 ? mkvalexpr(e->e1) : Xnil))
					return -1;
			}
			return fmtputs0(vm, f, " ]");
		default:
			if(fmtputs0(vm, f, "#"))
				return -1;
			if(fmtputs0(vm, f, ciddata(e->skind)))
				return -1;
			if(fmtputs0(vm, f, "("))
				return -1;

			if(e->e1 == 0)
				goto endexpr;
			if(fmtval(vm, f, mkvalexpr(e->e1)))
				return -1;

			if(e->e2 == 0)
				goto endexpr;
			if(fmtputs0(vm, f, ", "))
				return -1;
			if(fmtval(vm, f, mkvalexpr(e->e2)))
				return -1;

			if(e->e3 == 0)
				goto endexpr;
			if(fmtputs0(vm, f, ", "))
				return -1;
			if(fmtval(vm, f, mkvalexpr(e->e3)))
				return -1;

			if(e->e4 == 0)
				goto endexpr;
			if(fmtputs0(vm, f, ", "))
				return -1;
			if(fmtval(vm, f, mkvalexpr(e->e4)))
				return -1;
		endexpr:
			if(fmtputs0(vm, f, ")"))
				return -1;
			break;
		}
		return 0;
	default:
		snprint(buf, sizeof(buf), "<unhandled type %d>", Vkind(val));
		return fmtputs0(vm, f, buf);
	}
}

static int
fmticval(VM *vm, Fmt *f, unsigned char conv, Cval *cv)
{
	static char buf[Maxprintint];
	static char fmt[1+3+1+Maxprintint+5];
	static char mod[4], *mp;
	unsigned int fl;
	char *fp;
	float fv;
	double dv;
	Rkind rep;

	static char* fmttab[Rnrep][256] = {
	[Ru08le]['d'] = PRId8,	[Ru08le]['i'] = PRIi8,
	[Ru08be]['d'] = PRId8,	[Ru08be]['i'] = PRIi8,
	[Rs08le]['d'] = PRId8,	[Rs08le]['i'] = PRIi8,
	[Rs08be]['d'] = PRId8,	[Rs08be]['i'] = PRIi8,
	[Ru16le]['d'] = PRId16,	[Ru16le]['i'] = PRIi16,
	[Ru16be]['d'] = PRId16,	[Ru16be]['i'] = PRIi16,
	[Rs16le]['d'] = PRId16,	[Rs16le]['i'] = PRIi16,
	[Rs16be]['d'] = PRId16,	[Rs16be]['i'] = PRIi16,
	[Ru32le]['d'] = PRId32,	[Ru32le]['i'] = PRIi32,
	[Ru32be]['d'] = PRId32,	[Ru32be]['i'] = PRIi32,
	[Rs32le]['d'] = PRId32,	[Rs32le]['i'] = PRIi32,
	[Rs32be]['d'] = PRId32,	[Rs32be]['i'] = PRIi32,
	[Ru64le]['d'] = PRId64,	[Ru64le]['i'] = PRIi64,
	[Ru64be]['d'] = PRId64,	[Ru64be]['i'] = PRIi64,
	[Rs64le]['d'] = PRId64,	[Rs64le]['i'] = PRIi64,
	[Rs64be]['d'] = PRId64,	[Rs64be]['i'] = PRIi64,

	[Ru08le]['o'] = PRIo8,	[Ru08le]['u'] = PRIu8,
	[Ru08be]['o'] = PRIo8,	[Ru08be]['u'] = PRIu8,
	[Rs08le]['o'] = PRIo8,	[Rs08le]['u'] = PRIu8,
	[Rs08be]['o'] = PRIo8,	[Rs08be]['u'] = PRIu8,
	[Ru16le]['o'] = PRIo16,	[Ru16le]['u'] = PRIu16,
	[Ru16be]['o'] = PRIo16,	[Ru16be]['u'] = PRIu16,
	[Rs16le]['o'] = PRIo16,	[Rs16le]['u'] = PRIu16,
	[Rs16be]['o'] = PRIo16,	[Rs16be]['u'] = PRIu16,
	[Ru32le]['o'] = PRIo32,	[Ru32le]['u'] = PRIu32,
	[Ru32be]['o'] = PRIo32,	[Ru32be]['u'] = PRIu32,
	[Rs32le]['o'] = PRIo32,	[Rs32le]['u'] = PRIu32,
	[Rs32be]['o'] = PRIo32,	[Rs32be]['u'] = PRIu32,
	[Ru64le]['o'] = PRIo64,	[Ru64le]['u'] = PRIu64,
	[Ru64be]['o'] = PRIo64,	[Ru64be]['u'] = PRIu64,
	[Rs64le]['o'] = PRIo64,	[Rs64le]['u'] = PRIu64,
	[Rs64be]['o'] = PRIo64,	[Rs64be]['u'] = PRIu64,

	[Ru08le]['x'] = PRIx8,	[Ru08le]['X'] = PRIX8,
	[Ru08be]['x'] = PRIx8,	[Ru08be]['X'] = PRIX8,
	[Rs08le]['x'] = PRIx8,	[Rs08le]['X'] = PRIX8,
	[Rs08be]['x'] = PRIx8,	[Rs08be]['X'] = PRIX8,
	[Ru16le]['x'] = PRIx16,	[Ru16le]['X'] = PRIX16,
	[Ru16be]['x'] = PRIx16,	[Ru16be]['X'] = PRIX16,
	[Rs16le]['x'] = PRIx16,	[Rs16le]['X'] = PRIX16,
	[Rs16be]['x'] = PRIx16,	[Rs16be]['X'] = PRIX16,
	[Ru32le]['x'] = PRIx32,	[Ru32le]['X'] = PRIX32,
	[Ru32be]['x'] = PRIx32,	[Ru32be]['X'] = PRIX32,
	[Rs32le]['x'] = PRIx32,	[Rs32le]['X'] = PRIX32,
	[Rs32be]['x'] = PRIx32,	[Rs32be]['X'] = PRIX32,
	[Ru64le]['x'] = PRIx64,	[Ru64le]['X'] = PRIX64,
	[Ru64be]['x'] = PRIx64,	[Ru64be]['X'] = PRIX64,
	[Rs64le]['x'] = PRIx64,	[Rs64le]['X'] = PRIX64,
	[Rs64be]['x'] = PRIx64,	[Rs64be]['X'] = PRIX64,

	[Rf32]['f'] = "f",
	[Rf64]['f'] = "f",
	[Rf32]['g'] = "g",
	[Rf64]['g'] = "g",
	};

	rep = typerep(chasetype(cv->type));
	fp = fmttab[rep][conv];
	fl = f->flags;
	f->flags &= ~(FmtWidth|FmtPrec|FmtSpace|FmtSign|FmtSharp);

	mp = mod;
	if(fl&FmtSharp)
		*mp++ = '#';
	if(fl&FmtSign)
		*mp++ = '+';	/* trumps FmtSpace */
	else if(fl&FmtSpace)
		*mp++ = ' ';
	if(fl&FmtLeft)
		*mp++ = '-';	/* trumps FmtZero */
	else if(fl&FmtZero)
		*mp++ = '0';
	*mp = 0;
	if((fl&FmtWidth) && (fl&FmtPrec))
		snprint(fmt, sizeof(fmt), "%%%s%d.%d%s", mod, f->width, f->prec, fp);
	else if(fl&FmtPrec)
		snprint(fmt, sizeof(fmt), "%%%s.%d%s", mod, f->prec, fp);
	else if(fl&FmtWidth)
		snprint(fmt, sizeof(fmt), "%%%s%d%s", mod, f->width, fp);
	else
		snprint(fmt, sizeof(fmt), "%%%s%s", mod, fp);

	switch(rep){
	case Ru08le:
	case Ru08be:
		snprint(buf, sizeof(buf), fmt, (u8)cvalu(cv));
		break;
	case Rs08le:
	case Rs08be:
		snprint(buf, sizeof(buf), fmt, (s8)cvalu(cv));
		break;
	case Ru16le:
	case Ru16be:
		snprint(buf, sizeof(buf), fmt, (u16)cvalu(cv));
		break;
	case Rs16le:
	case Rs16be:
		snprint(buf, sizeof(buf), fmt, (s16)cvalu(cv));
		break;
	case Ru32le:
	case Ru32be:
		snprint(buf, sizeof(buf), fmt, (u32)cvalu(cv));
		break;
	case Rs32le:
	case Rs32be:
		snprint(buf, sizeof(buf), fmt, (s32)cvalu(cv));
		break;
	case Ru64le:
	case Ru64be:
		snprint(buf, sizeof(buf), fmt, (u64)cvalu(cv));
		break;
	case Rs64le:
	case Rs64be:
		snprint(buf, sizeof(buf), fmt, (s64)cvalu(cv));
		break;
	case Rf32:
		fv = cvalf(cv);
		snprint(buf, sizeof(buf), fmt, fv);
		break;
	case Rf64:
		dv = cvald(cv);
		snprint(buf, sizeof(buf), fmt, dv);
		break;
	default:
		fatal("bug");
	}
	return fmtputs(vm, f, buf, strlen(buf));
}

static int
fmtenconst(VM *vm, Fmt *f, Cval *cv)
{
	Ctype *t;
	Ctypeenum *te;
	Vec *v;
	Cval *k;
	Cid *id;
	u32 m;

	t = cv->type;
	while(t->tkind == Ttypedef)
		t = subtype(t);
	if(t->tkind != Tenum)
		return fmticval(vm, f, 'd', cv);
	te = (Ctypeenum*)t;
	for(m = 0; m < te->konst->len; m++){
		v = valvec(vecref(te->konst, m));
		k = valcval(vecref(v, 1));
		/* direct comparison is sane because
		 * enum consts all have the same
		 * type */
		if(cvalu(cv) == cvalu(k)){
			id = valcid(vecref(v, 0));
			return fmtputs(vm, f, ciddata(id), id->len-1);
		}
	}
	return fmticval(vm, f, 'd', cv);
}

/* dofmt: dispatch format conversions.
   based on code and ideas from plan 9 libc */
static void
dofmt(VM *vm, Fmt *f, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	static char buf[3+Maxprintint];
	Val *vpp, vp, vq;
	Cval *cv, *cv1;
	Str *as, *ys;
	Cid *xs;
	char *sfmt, *efmt, *p;
	char ch;
	unsigned char c;
	Ctype *t;
	Vec *vec;
	int i;
	Imm len;
	Val xargv[2];

	vpp = &argv[0];
	ch = 0;
	efmt = fmt+fmtlen;
	while(1){
		ch = 0;
		while(fmt < efmt && (ch = *fmt++) != '%')
			if(fmtputc(vm, f, ch))
				return;
		sfmt = fmt-1;
		if(ch == '%' && fmt >= efmt)
			goto fail;
		if(fmt >= efmt)
			return;
		if(*fmt == '%'){
			if(fmtputc(vm, f, ch))
				return;
			fmt++;
			continue;
		}
		if(argc == 0)
			vmerr(vm, "format string needs more arguments");
		vp = *vpp++;
		argc--;
		f->flags = 0;
		f->width = 0;
		f->prec = 0;
	morespec:
		if(fmt >= efmt)
			goto fail;
		ch = *fmt++;
		switch(ch){
		case '.':
			f->flags |= FmtWidth|FmtPrec;
			goto morespec;
		case '0':
			if(!(f->flags&FmtWidth)){
				f->flags |= FmtZero;
				goto morespec;
			}
			/* fall through */
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			i = 0;
			while(ch >= '0' && ch <= '9'){
				i = i*10+ch-'0';
				if(fmt >= efmt)
					return;
				ch = *fmt++;
			}
			fmt--;
		numflag:
			if(f->flags&FmtWidth){
				f->flags |= FmtPrec;
				f->prec = i;
			}else{
				f->flags |= FmtWidth;
				f->width = i;
			}
			goto morespec;
		case '*':
			if(Vkind(vp) != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(argc == 0)
				vmerr(vm,
				      "format string needs more arguments");
			vp = *vpp++;
			argc--;
			i = cval2int(cv);
			if(i < 0){
				/* negative precision =>
				   ignore the precision */
				if(f->flags&FmtPrec){
					f->flags &= ~FmtPrec;
					f->prec = 0;
					goto morespec;
				}
				i = -i;
				f->flags |= FmtLeft;
			}
			goto numflag;
		case '-':
			f->flags |= FmtLeft;
			goto morespec;
		case '+':
			f->flags |= FmtSign;
			goto morespec;
		case ' ':
			f->flags |= FmtSpace;
			goto morespec;
		case '#':
			f->flags |= FmtSharp;
			goto morespec;
		case 'l':
		case 'L':
		case 'h':
			/* ignore length modifier */
			goto morespec;
		case 'a':
			if(fmtval(vm, f, vp))
				return;
			break;
		case 'c':
			if(Vkind(vp) != Qcval)
				goto badarg;
			c = cvalu(valcval(vp));
			if(xisgraph(c) || xisspace(c))
				snprint(buf, sizeof(buf), "%c", c);
			else
				snprint(buf, sizeof(buf), "\\%.3o", c);
			if(fmtputs(vm, f, buf, strlen(buf)))
				return;
			break;
		case 'f':
		case 'g':
			if(Vkind(vp) != Qcval)
				goto badarg;
			cv = valcval(vp);
			t = chasetype(cv->type);
			if(!isfloat[typecbase(t)])
				vmerr(vm, "bad operand to %%%c", ch);
			if(fmticval(vm, f, ch, cv))
				return;
			break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
			if(Vkind(vp) != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmticval(vm, f, ch, cv))
				return;
			break;
		case 'e':
			if(Vkind(vp) != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmtenconst(vm, f, cv))
				return;
			break;
		case 'p':
			if(Vkind(vp) != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmticval(vm, f, 'x', cv))
				return;
			break;
		case 's':
		case 'b':
		case 'B':
			if(Vkind(vp) == Qstr){
				as = valstr(vp);
				p = strdata(as);
				if(ch == 's')
					len = xstrnlen(p, as->len);
				else
					len = as->len;
			}
			else if(Vkind(vp) == Qcval){
				cv = valcval(vp);
				if(!isstrcval(cv))
					goto badarg;
				if(cvalu(cv) == 0 && ch == 's'
				   && !ismapped(vm, cv->dom->as, cvalu(cv), 1)){
					p = "(null)";
					len = 6;
				}else{
					as = stringof(vm, cv);
					p = strdata(as);
					len = as->len;
				}
			}else if(Vkind(vp) == Qnil){
				as = mkstr0("(nil)");
				p = "(nil)";
				len = 5;
			}else if(Vkind(vp) == Qcid){
				xs = valcid(vp);
				p = ciddata(xs);
				len = xs->len-1;
			}else
				goto badarg;
			if(ch == 'B'){
				if(fmtputB(vm, f, p, len))
					return;
			}else{
				if(fmtputs(vm, f, p, len))
					return;
			}
			break;
		case 't':
			if(Vkind(vp) == Qctype)
				as = fmtctype(valctype(vp));
			else if(Vkind(vp) == Qvec){
				vec = valvec(vp);
				if(vec->len < 2)
					goto badarg;
				vq = vecref(vec, Typepos);
				if(Vkind(vq) != Qctype)
					goto badarg;
				t = valctype(vq);
				vq = vecref(vec, Idpos);
				if(Vkind(vq) == Qnil)
					as = fmtctype(t);
				else if(Vkind(vq) == Qcid)
					as = fmtdecl(t, valcid(vq));
				else
					goto badarg;
			}else if(Vkind(vp) == Qcval){
				cv = valcval(vp);
				as = fmtctype(cv->type);
			}else
				vmerr(vm, "bad operand to %%t");
			if(fmtputs(vm, f, strdata(as), as->len))
				return;
			break;
		case 'y':
			if(Vkind(vp) != Qcval)
				vmerr(vm, "bad operand to %%y");
			cv = valcval(vp);
			xargv[0] = mkvalns(cv->dom->ns);
			xargv[1] = vp;
			if(cv->dom->ns->lookaddr == 0)
				vmerr(vm, "name space does not"
				      " define lookaddr");
			vq = safedovm(vm, cv->dom->ns->lookaddr, 2, xargv);
			cv = typecast(vm, cv->dom->ns->base[Vptr], cv);
			if(Vkind(vq) == Qnil){
				snprint(buf, sizeof(buf),
					"0x%" PRIx64, cvalu(cv));
				if(fmtputs(vm, f, buf, strlen(buf)))
					return;
				break;
			}else if(Vkind(vq) != Qvec)
			bady:
				vmerr(vm, "invalid response from lookaddr");
			vec = valvec(vq);
			if(vec->len < 3)
				goto bady;
			vq = vecref(vec, Idpos);
			if(Vkind(vq) != Qcid)
				goto bady;
			xs = valcid(vq);
			vq = attroff(vecref(vec, Attrpos));
			if(Vkind(vq) != Qcval)
				goto bady;
			/* FIXME: too complicated */
			cv1 = typecast(vm,
				       cv->dom->ns->base[Vptr],
				       valcval(vq));
			cv = xcvalalu(vm, Isub, cv, cv1);
			if(cvalu(cv) != 0){
				snprint(buf, sizeof(buf),
					"+0x%" PRIx64, cvalu(cv));
				ys = mkstrn(xs->len-1+strlen(buf));
				memcpy(strdata(ys), ciddata(xs), xs->len-1);
				memcpy(strdata(ys)+xs->len-1, buf, strlen(buf));
				if(fmtputs(vm, f, strdata(ys), ys->len))
					return;
			}else
				if(fmtputs(vm, f, ciddata(xs), xs->len-1))
					return;
			break;
		default:
		fail:
			vmerr(vm, "invalid format conversion: \'%.*s\'",
			      fmt-sfmt, sfmt);
		}
	}
	/* not reached */
badarg:
	vmerr(vm, "wrong type to %%%c conversion", ch);
}

static int
fmtstrflush(VM *vm, Fmt *f)
{
	Str *s, *t;
	Imm m;
	s = f->farg;
	t = mkstrn(s->len*2);
	f->farg = t;
	f->start = strdata(t);
	m = f->to-strdata(s);
	memcpy(f->start, strdata(s), m);
	f->to = f->start+m;
	f->stop = f->start+t->len;
	return 0;
}

/* FIXME: we assume gc does not move f->farg during dofmt */
static Str*
dovsprinta(VM *vm, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	Fmt f;
	Str *s;
	static u32 initlen = 128;
	memset(&f, 0, sizeof(f));
	s = mkstrn(initlen);
	f.farg = s;
	f.start = strdata(s);
	f.to = f.start;
	f.stop = f.start+s->len;
	f.flush = fmtstrflush;
	dofmt(vm, &f, fmt, fmtlen, argc, argv);
	/* FIXME: it would nice not to have to copy ... string slice? */
	return mkstr(f.start, f.to-f.start);
}

static int
fmtfdflush(VM *vm, Fmt *f)
{
	Imm rv;
	Fd *fd;
	Str *s;
	Val argv[1], r;

	fd = (Fd*)f->farg;
	if(fd->flags&Fclosed)
		return -1;
	if((fd->flags&Fwrite) == 0)
		return -1;
	if(fd->flags&Ffn){
		if(!fd->u.fn.write)
			return 0;
		rv = fd->u.fn.write(&fd->u.fn, f->start, f->to-f->start);
		if(rv == -1)
			return -1;
	}else{
		s = mkstrk(f->start, f->to-f->start, Sperm);
		argv[0] = mkvalstr(s);
		r = safedovm(vm, fd->u.cl.write, 1, argv);
		if(Vkind(r) != Qnil)
			return -1;
	}
	f->to = f->start;
	return 0;
}

static void
dofdprint(VM *vm, Fd *fd, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	Fmt f;
	char buf[256];

	memset(&f, 0, sizeof(f));
	f.farg = fd;
	f.start = buf;
	f.to = buf;
	f.stop = buf+sizeof(buf);
	f.flush = fmtfdflush;
	dofmt(vm, &f, fmt, fmtlen, argc, argv);
	fmtfdflush(vm, &f);
}

static void
l1_printf(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *fmts;
	if(argc < 1)
		vmerr(vm, "wrong number of arguments to printf");
	if(Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to printf must be a format string");
	fmts = valstr(argv[0]);
	dofdprint(vm, vmstdout(vm), strdata(fmts), fmts->len, argc-1, argv+1);
}

static void
l1_print(VM *vm, Imm argc, Val *argv, Val *rv)
{
	static char *fmt = "%a\n";
	dofdprint(vm, vmstdout(vm), fmt, strlen(fmt), argc, argv);
}

static void
l1_fprintf(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *fmts;
	if(argc < 2)
		vmerr(vm, "wrong number of arguments to fprintf");
	if(Vkind(argv[0]) != Qfd)
		vmerr(vm, "operand 1 to fprintf must be a file descriptor");
	if(Vkind(argv[1]) != Qstr)
		vmerr(vm, "operand 2 to fprintf must be a format string");
	fd = valfd(argv[0]);
	fmts = valstr(argv[1]);
	dofdprint(vm, fd, strdata(fmts), fmts->len, argc-2, argv+2);
}

void
l1_sprintfa(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *fmts, *rs;
	if(argc < 1)
		vmerr(vm, "wrong number of arguments to sprintfa");
	if(Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to sprintfa must be a format string");
	fmts = valstr(argv[0]);
	rs = dovsprinta(vm, strdata(fmts), fmts->len, argc-1, argv+1);
	*rv = mkvalstr(rs);
}

void
fnfmt(Env *env)
{
	FN(fprintf);
	FN(print);		/* FIXME: remove: held for test suite */
	FN(printf);
	FN(sprintfa);
}
