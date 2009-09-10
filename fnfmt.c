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

static char* _fmtxtn(Xtypename *xtn, char *o);
static char* _fmtdecl(Xtypename *xtn, Str *is);

static char*
fmtplist(Vec *param)
{
	char **ds, *buf, *bp;
	Imm i, n;
	unsigned long m;
	Xtypename *xtn;
	Vec *pvec;
	Val id;
	
	n = param->len;
	if(n == 0)
		return xstrdup("");

	ds = emalloc(n*sizeof(char**));
	m = 1;			/* null */
	for(i = 0; i < n; i++){
		pvec = valvec(vecref(param, i));
		xtn = valxtn(vecref(pvec, Typepos));
		id = vecref(pvec, 1);
		if(id->qkind == Qstr)
			ds[i] = _fmtdecl(xtn, valstr(id));
		else
			ds[i] = _fmtxtn(xtn, xstrdup(""));
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
_fmtxtn(Xtypename *xtn, char *o)
{
	char *buf, *w, *pl;
	unsigned m, leno;
	Str *s;
	Cval *cv;

	leno = strlen(o);
	switch(xtn->tkind){
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
		m = strlen(cbasename[xtn->basename])+1+leno+1;
		buf = emalloc(m);
		if(leno)
			snprint(buf, m, "%s %s", cbasename[xtn->basename], o);
		else
			snprint(buf, m, "%s", cbasename[xtn->basename]);
		efree(o);
		return buf;
	case Ttypedef:
		s = xtn->tid;
		m = s->len+1+leno+1;
		buf = emalloc(m);
		if(leno)
			snprint(buf, m, "%.*s %s", (int)s->len, s->s, o);
		else
			snprint(buf, m, "%.*s", (int)s->len, s->s);
		efree(o);
		return buf;
	case Tstruct:
	case Tunion:
	case Tenum:
		w = tkindstr[xtn->tkind];
		if(xtn->tag){
			s = xtn->tag;
			m = strlen(w)+1+s->len+1+leno+1;
			buf = emalloc(m);
			if(leno)
				snprint(buf, m, "%s %.*s %s",
					 w, (int)s->len, s->s, o);
			else
				snprint(buf, m, "%s %.*s", w,
					 (int)s->len, s->s);
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
		m = leno+1+strlen("/*UNDEFINED*/")+1;
		buf = emalloc(m);
		snprint(buf, m, "%s /*UNDEFINED*/", o);
		return _fmtxtn(xtn->link, buf);
	case Tptr:
		m = 2+leno+1+1;
		buf = emalloc(m);
		if(xtn->link->tkind == Tfun || xtn->link->tkind == Tarr)
			snprint(buf, m, "(*%s)", o);
		else
			snprint(buf, m, "*%s", o);
		efree(o);
		return _fmtxtn(xtn->link, buf);
	case Tarr:
		m = leno+1+10+1+1;	/* assume max 10 digit size */
		buf = emalloc(m);
		if(xtn->cnt->qkind == Qnil)
			snprint(buf, m, "%s[]", o);
		else{
			cv = valcval(xtn->cnt);
			snprint(buf, m, "%s[%" PRIu64 "]", o, cv->val);
		}
		efree(o);
		return _fmtxtn(xtn->link, buf);
	case Tfun:
		pl = fmtplist(xtn->param);
		m = leno+1+strlen(pl)+1+1;
		buf = emalloc(m);
		snprint(buf, m, "%s(%s)", o, pl);
		efree(o);
		efree(pl);
		return _fmtxtn(xtn->link, buf);
	case Tconst:
	case Tbitfield:
	case Txaccess:
		/* this is questionable...maybe we want more accurate printed
		   representation of these objects, especially bitfields */
		return _fmtxtn(xtn->link, o);
	}
	return NULL;

}

static char*
_fmtdecl(Xtypename *xtn, Str *id)
{
	char *o;
	o = emalloc(id->len+1);
	memcpy(o, id->s, id->len);
	return _fmtxtn(xtn, o);
}

static Str*
fmtdecl(Xtypename *xtn, Str *id)
{
	char *s;
	Str *str;
	s = _fmtdecl(xtn, id);
	str = mkstr0(s);
	efree(s);
	return str;
}

char*
fmtxtnc(Xtypename *xtn)
{
	return _fmtxtn(xtn, xstrdup(""));
}

Str*
fmtxtn(Xtypename *xtn)
{
	char *s;
	Str *str;
	s = fmtxtnc(xtn);
	str = mkstr0(s);
	efree(s);
	return str;
}

static int
cval2int(Cval *cv)
{
	/* FIXME: implement with rerep? */
	Xtypename *t;
	t = chasetype(cv->type);
	switch(t->rep){
	case Rs08le:
	case Rs08be:
		return (int)(s8)cv->val;
	case Rs16le:
	case Rs16be:
		return (int)(s16)cv->val;
	case Rs32le:
	case Rs32be:
		return (int)(s32)cv->val;
	case Rs64le:
	case Rs64be:
		return (int)(s64)cv->val;
	case Ru08le:
	case Ru08be:
		return (int)(u8)cv->val;
	case Ru16le:
	case Ru16be:
		return (int)(u16)cv->val;
	case Ru32le:
	case Ru32be:
		return (int)(u32)cv->val;
	case Ru64le:
	case Ru64be:
		return (int)(u64)cv->val;
	default:
		fatal("bug");
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

	fl = f->flags;
	if((fl&FmtPrec) && m > f->prec)
		m = f->prec;
	if(fl&FmtWidth){
		q = p;
		l = m;			/* lost precision */
		w = 0;
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
		if(xisgraph(c) || xisspace(c))
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
	Listx *lx;
	Vec *v;
	Range *r;
	Rd *rd;
	Rec *rec;
	Str *str;
	Val bv, rv;
	u32 m;
	Head *hd;
	Dom *d;
	As *as;
	Ns *ns;
	Xtypename *t;

	switch(val->qkind){
	case Qcval:
		cv = valcval(val);
		t = chasetype(cv->type);
		switch(t->tkind){
		case Tbase:
			if(isunsigned[t->basename])
				snprint(buf, sizeof(buf), "%" PRIu64,
					cv->val);
			else
				snprint(buf, sizeof(buf), "%" PRId64,
					(int64_t)cv->val);
			break;
		case Tptr:
			snprint(buf, sizeof(buf), "%" PRIx64, cv->val);
			break;
		default:
			fatal("bug");
		}
		return fmtputs0(vm, f, buf);
	case Qcl:
		cl = valcl(val);
		if(cl->fp){
			snprint(buf, sizeof(buf), "<continuation %p>", cl);
			return fmtputs0(vm, f, buf);
		}else if(cl->dlen > 0){
			if(fmtputs0(vm, f, "<closure "))
				return -1;
		}else{
			if(fmtputs0(vm, f, "<procedure "))
				return -1;
		}
		if(fmtputs0(vm, f, cl->id))
			return -1;
		return fmtputs0(vm, f, ">");
	case Qundef:
		return 0;
	case Qnil:
		return fmtputs0(vm, f, "<nil>");
	case Qnull:
		return fmtputs0(vm, f, "<null>");
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
				 (int)ns->name->len, ns->name->s);
		else
			snprint(buf, sizeof(buf), "<ns %p>", ns);
		return fmtputs0(vm, f, buf);
	case Qas:
		as = valas(val);
		if(as->name)
			snprint(buf, sizeof(buf), "<as %.*s>",
				 (int)as->name->len, as->name->s);
		else
			snprint(buf, sizeof(buf), "<as %p>", as);
		return fmtputs0(vm, f, buf);
	case Qdom:
		d = valdom(val);
		if(d->name)
			snprint(buf, sizeof(buf), "<domain %.*s>",
				 (int)d->name->len, d->name->s);
		else
			snprint(buf, sizeof(buf), "<domain %p>", d);
		return fmtputs0(vm, f, buf);
	case Qfd:
	case Qpair:
	case Qtab:
		hd = valhead(val);
		snprint(buf, sizeof(buf), "<%s %p>", qname[hd->qkind], hd);
		return fmtputs0(vm, f, buf);
	case Qxtn:
		str = fmtxtn(valxtn(val));
		return fmtputB(vm, f, str->s, str->len);
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
			if(fmtval(vm, f, v->vec[m]))
				return -1;
		}
		return fmtputs0(vm, f, " )");
	case Qlist:
		l = vallist(val);
		lx = l->x;
		if(fmtputs0(vm, f, "["))
			return -1;
		for(m = 0; m < listxlen(lx); m++){
			if(m > 0){
				if(fmtputs0(vm, f, ", "))
					return -1;
			}else{
				if(fmtputs0(vm, f, " "))
					return -1;
			}
			if(fmtval(vm, f, lx->val[lx->hd+m]))
				return -1;
		}
		return fmtputs0(vm, f, " ]");
	case Qrange:
		r = valrange(val);
 		snprint(buf, sizeof(buf),
			 "<range 0x%" PRIx64 " 0x%" PRIx64 ">",
			 r->beg->val, r->len->val);
		return fmtputs0(vm, f, buf);
	case Qstr:
		str = valstr(val);
		if(fmtputs0(vm, f, "\""))
			return -1;
		if(fmtputB(vm, f, str->s, str->len))
			return -1;
		return fmtputs0(vm, f, "\"");
	case Qrd:
		rd = valrd(val);
		if(fmtputs0(vm, f, "<rd "))
			return -1;
		if(fmtputs(vm, f, rd->name->s, rd->name->len))
			return -1;
		return fmtputs0(vm, f, ">");
	case Qrec:
		rec = valrec(val);
		rv = dovm(vm, rec->rd->fmt, 1, &val);
		if(rv->qkind != Qstr)
			vmerr(vm, "formatter for record type %.*s must "
			      "return a string",
			      (int)rec->rd->name->len, rec->rd->name->s);
		str = valstr(rv);
		return fmtputs(vm, f, str->s, str->len);
	default:
		snprint(buf, sizeof(buf), "<unhandled type %d>", val->qkind);
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
	Xtypename *t;
	char *fp;
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
	[Rs64be]['x'] = PRIx64,	[Rs64be]['X'] = PRIX64,	};

	t = chasetype(cv->type);
	fp = fmttab[t->rep][conv];
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
	
	switch(t->rep){
	case Ru08le:
	case Ru08be:
		snprint(buf, sizeof(buf), fmt, (u8)cv->val);
		break;
	case Rs08le:
	case Rs08be:
		snprint(buf, sizeof(buf), fmt, (s8)cv->val);
		break;
	case Ru16le:
	case Ru16be:
		snprint(buf, sizeof(buf), fmt, (u16)cv->val);
		break;
	case Rs16le:
	case Rs16be:
		snprint(buf, sizeof(buf), fmt, (s16)cv->val);
		break;
	case Ru32le:
	case Ru32be:
		snprint(buf, sizeof(buf), fmt, (u32)cv->val);
		break;
	case Rs32le:
	case Rs32be:
		snprint(buf, sizeof(buf), fmt, (s32)cv->val);
		break;
	case Ru64le:
	case Ru64be:
		snprint(buf, sizeof(buf), fmt, (u64)cv->val);
		break;
	case Rs64le:
	case Rs64be:
		snprint(buf, sizeof(buf), fmt, (s64)cv->val);
		break;
	default:
		fatal("bug");
	}
	return fmtputs(vm, f, buf, strlen(buf));
}

static int
fmtenconst(VM *vm, Fmt *f, Cval *cv)
{
	Xtypename *t;
	Vec *v;
	Cval *k;
	Str *s;
	u32 m;

	t = cv->type;
	while(t->tkind == Ttypedef)
		t = t->link;
	if(t->tkind != Tenum)
		return fmticval(vm, f, 'd', cv);
	for(m = 0; m < t->konst->len; m++){
		v = valvec(vecref(t->konst, m));
		k = valcval(vecref(v, 1));
		/* direct comparison is sane because
		 * enum consts all have the same
		 * type */
		if(cv->val == k->val){
			s = valstr(vecref(v, 0));
			return fmtputs(vm, f, s->s, s->len);
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
	Cval *cv, *ncv;
	Str *as, *ys;
	char *efmt;
	char ch;
	unsigned char c;
	Xtypename *xtn;
	Vec *vec;
	int i;
	Imm len;
	Val xargv[2];
	
	vpp = &argv[0];
	efmt = fmt+fmtlen;
	while(1){
		while(fmt < efmt && (ch = *fmt++) != '%')
			if(fmtputc(vm, f, ch))
				return;
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
			return;
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
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			c = cv->val;
			if(xisgraph(c) || xisspace(c))
				snprint(buf, sizeof(buf), "%c", c);
			else
				snprint(buf, sizeof(buf), "\\%.3o", c);
			if(fmtputs(vm, f, buf, strlen(buf)))
				return;
			break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmticval(vm, f, ch, cv))
				return;
			break;
		case 'e':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmtenconst(vm, f, cv))
				return;
			break;
		case 'p':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmticval(vm, f, 'x', cv))
				return;
			break;
		case 's':
		case 'b':
		case 'B':
			if(vp->qkind == Qstr){
				as = valstr(vp);
				if(ch == 's')
					len = xstrnlen(as->s, as->len);
				else
					len = as->len;
			}
			else if(vp->qkind == Qcval){
				cv = valcval(vp);
				if(!isstrcval(cv))
					goto badarg;
				if(cv->val == 0)
					as = mkstr0("(null)");
				else
					as = stringof(vm, cv);
				len = as->len;
			}else if(vp->qkind == Qnil){
				as = mkstr0("(nil)");
				len = as->len;
			}else
				goto badarg;
			if(ch == 'B'){
				if(fmtputB(vm, f, as->s, len))
					return;
			}else{
				if(fmtputs(vm, f, as->s, len))
					return;
			}
			break;
		case 't':
			if(vp->qkind == Qxtn)
				as = fmtxtn(valxtn(vp));
			else if(vp->qkind == Qvec){
				vec = valvec(vp);
				if(vec->len < 2)
					goto badarg;
				vq = vecref(vec, Typepos);
				if(vq->qkind != Qxtn)
					goto badarg;
				xtn = valxtn(vq);
				vq = vecref(vec, Idpos);
				if(vq->qkind == Qnil)
					as = fmtxtn(xtn);
				else if(vq->qkind == Qstr)
					as = fmtdecl(xtn, valstr(vq));
				else
					goto badarg;
			}else if(vp->qkind == Qcval){
				cv = valcval(vp);
				as = fmtxtn(cv->type);
			}else
				vmerr(vm, "bad operand to %%t");
			if(fmtputs(vm, f, as->s, as->len))
				return;
			break;
		case 'y':
			if(vp->qkind != Qcval)
				vmerr(vm, "bad operand to %%y");
			cv = valcval(vp);
			xargv[0] = mkvalns(cv->dom->ns);
			xargv[1] = vp;
			if(cv->dom->ns->lookaddr == 0)
				vmerr(vm, "name space does not"
				      " define lookaddr");
			vq = dovm(vm, cv->dom->ns->lookaddr, 2, xargv);
			cv = typecast(vm, cv->dom->ns->base[Vptr], cv);
			if(vq->qkind == Qnil){
				snprint(buf, sizeof(buf),
					 "0x%" PRIx64, cv->val);
				if(fmtputs(vm, f, buf, strlen(buf)))
					return;
				break;
			}else if(vq->qkind != Qvec)
			bady:
				vmerr(vm, "invalid response from lookaddr");
			vec = valvec(vq);
			if(vec->len < 3)
				goto bady;
			vq = vecref(vec, Idpos);
			if(vq->qkind != Qstr)
				goto bady;
			as = valstr(vq);
			vq = attroff(vecref(vec, Attrpos));
			if(vq->qkind != Qcval)
				goto bady;
			/* FIXME: too complicated */
			gcprotect(vm, cv);
			ncv = xcvalalu(vm, Isub, cv,
				       typecast(vm, cv->dom->ns->base[Vptr],
					       valcval(vq)));
			gcunprotect(vm, cv);
			if(ncv->val != 0){
				snprint(buf, sizeof(buf),
					 "+0x%" PRIx64, ncv->val);
				ys = mkstrn(vm, as->len+strlen(buf));
				memcpy(ys->s, as->s, as->len);
				memcpy(ys->s+as->len, buf, strlen(buf));
			}else
				ys = as;
			if(fmtputs(vm, f, ys->s, ys->len))
				return;
			break;
		default:
			vmerr(vm, "unrecognized format conversion: %%%c", ch);
		}
	}
	/* not reached */
badarg:
	vmerr(vm, "wrong type to %%%c conversion", ch);
}

static int
fmtstrflush(VM *vm, Fmt *f)
{
	u32 len;
	char *s;
	len = (u32)(uintptr_t)f->farg;
	s = f->start;
	f->start = erealloc(f->start, len, len*2);
	len *= 2;
	f->farg = (void*)(uintptr_t)len;
	f->to = f->start+(f->to-s);
	f->stop = f->start+len;
	return 0;
}

static Str*
dovsprinta(VM *vm, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	Fmt f;
	static u32 initlen = 128;
	memset(&f, 0, sizeof(f));
	f.start = emalloc(initlen);
	f.farg = (void*)(uintptr_t)initlen;
	f.to = f.start;
	f.stop = f.start+initlen;
	f.flush = fmtstrflush;
	dofmt(vm, &f, fmt, fmtlen, argc, argv);
	return mkstrk(f.start, f.to-f.start, Smalloc);
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
		gcprotect(vm, s);
		argv[0] = mkvalstr(s);
		r = dovm(vm, fd->u.cl.write, 1, argv);
		gcunprotect(vm, s);
		if(r->qkind != Qnil)
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
	if(argv[0]->qkind != Qstr)
		vmerr(vm, "operand 1 to printf must be a format string");
	fmts = valstr(argv[0]);
	dofdprint(vm, vmstdout(vm), fmts->s, fmts->len, argc-1, argv+1);
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
	if(argv[0]->qkind != Qfd)
		vmerr(vm, "operand 1 to fprintf must be a file descriptor");
	if(argv[1]->qkind != Qstr)
		vmerr(vm, "operand 2 to fprintf must be a format string");
	fd = valfd(argv[0]);
	fmts = valstr(argv[1]);
	dofdprint(vm, fd, fmts->s, fmts->len, argc-2, argv+2);
}

void
l1_sprintfa(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *fmts, *rs;
	if(argc < 1)
		vmerr(vm, "wrong number of arguments to sprintfa");
	if(argv[0]->qkind != Qstr)
		vmerr(vm, "operand 1 to sprintfa must be a format string");
	fmts = valstr(argv[0]);
	rs = dovsprinta(vm, fmts->s, fmts->len, argc-1, argv+1);
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
