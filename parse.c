#include "sys.h"
#include "util.h"
#include "syscqct.h"

char cqctflags[256];

char* cbasename[Vnbase+1] = {
	[Vundef]              = "error!",
	[Vchar]               = "char",
	[Vshort]	      = "short",
	[Vint]		      = "int",
	[Vlong]		      = "long",
	[Vvlong]	      = "long long",
	[Vuchar]	      = "unsigned char",
	[Vushort]	      = "unsigned short",
	[Vuint]		      = "unsigned int",
	[Vulong]	      = "unsigned long",
	[Vuvlong]	      = "unsigned long long",
	[Vfloat]	      = "float",
	[Vdouble]	      = "double",
	[Vlongdouble]	      = "long double",
	[Vptr]		      = "$nsptr",
};

char* tkindstr[Tntkind] = {
	[Tarr]			= "array",
	[Tbase]			= "base",
	[Tenum]			= "enum",
	[Tfun]			= "function",
	[Tptr]			= "pointer",
	[Tstruct]		= "struct",
	[Ttypedef]		= "typedef",
	[Tunion]		= "union",
	[Tvoid]			= "void",
	[Tbitfield]		= "bitfield",
	[Tconst]		= "constant",
	[Txaccess]		= "xaccess",
	[Tundef]		= "undef",
};

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
	[Vulong][Elong]        = Vuvlong,

	[Vvlong][Eunsigned]    = Vuvlong,
	[Vvlong][Esigned]      = Vvlong,

	[Vdouble][Elong]       = Vlongdouble,

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

static Decl* dodecls(U *ctx, Expr *e);
static Decl* dodecl(U *ctx, Expr *e);
static Type* copytype(Type *t);

static HT *filenames;
static char **loadpath;

extern int yylex_destroy(void);

void
parseerror(U *ctx, char *fmt, ...)
{
	va_list args;

	if(ctx == 0){
		/* we're here because of a bison limitation.
		 * the caller is yyerror from one of our GLR merge
		 * resolvers, which do not convey %parse-param's
		 */
		va_start(args, fmt);
		vmsg(fmt, args);
		va_end(args);
		return;
	}

	if(ctx->inp)
		xprintf("%s:%u: ",
			ctx->inp->src.filename
			? ctx->inp->src.filename : "<stdin>",
			ctx->inp->src.line);
	va_start(args, fmt);
	vmsg(fmt, args);
	va_end(args);

	while(popyy(ctx))
		;
	yylex_destroy();
	longjmp(ctx->jmp, 1);
}

Lits*
mklits(char *s, unsigned len)
{
	Lits *lits;
	char *p;

	lits = emalloc(sizeof(Lits)+len);
	p = (char*)(lits+1);
	memcpy(p, s, len);
	lits->s = p;
	lits->len = len;
	return lits;
}

Lits*
copylits(Lits *lits)
{
	return mklits(lits->s, lits->len);
}

void
freelits(Lits *lits)
{
	efree(lits);
}

static Type*
newtype()
{
	Type *t;
	t = emalloc(sizeof(Type));
	return t;
}

static Decl*
newdecl()
{
	Decl *d;
	d = emalloc(sizeof(Decl));
	return d;
}

Expr*
newexprsrc(Src *src, unsigned kind, Expr *e1, Expr *e2, Expr *e3, Expr *e4)
{
	Expr *e;

	e = emalloc(sizeof(Expr));
	e->kind = kind;
	e->e1 = e1;
	e->e2 = e2;	
	e->e3 = e3;
	e->e4 = e4;

	if(src)
		e->src = *src;

	return e;
}

Expr*
newexpr(unsigned kind, Expr *e1, Expr *e2, Expr *e3, Expr *e4)
{
	return newexprsrc(0, kind, e1, e2, e3, e4);
}

Expr*
newbinopsrc(Src *src, unsigned kind, Expr *e1, Expr *e2)
{
	Expr *e;
	e = newexprsrc(src, Ebinop, e1, e2, 0, 0);
	e->op = kind;
	return e;
}

Expr*
newbinop(unsigned kind, Expr *e1, Expr *e2)
{
	return newbinopsrc(0, kind, e1, e2);
}

Expr*
newgopsrc(Src *src, unsigned kind, Expr *e1, Expr *e2)
{
	Expr *e;
	e = newexprsrc(src, Egop, e1, e2, 0, 0);
	e->op = kind;
	return e;
}

Expr*
newgop(unsigned kind, Expr *e1, Expr *e2)
{
	return newgopsrc(0, kind, e1, e2);
}

void
freeexpr(Expr *e)
{
	Expr *p;

	if(e == 0)
		return;

	if(e->kind == Eelist){
		p = e;
		while(p->kind == Eelist){
			freeexpr(p->e1);
			e = p->e2;
			efree(p);
			p = e;
		}
		freeexpr(p);
		return;
	}

	switch(e->kind){
	case Eid:
		efree(e->id);
		break;
	case Econsts:
		freelits(e->lits);
		break;
	default:
		break;
	}
	freeexpr(e->e1);
	freeexpr(e->e2);
	freeexpr(e->e3);
	freeexpr(e->e4);
	if(e->xp)
		freeexprx(e);
	efree(e);
}

Expr*
copyexpr(Expr *e)
{
	Expr *ne;

	if(e == 0)
		return 0;

	ne = emalloc(sizeof(Expr));
	ne->kind = e->kind;
	switch(e->kind){
	case Eid:
		ne->id = xstrdup(e->id);
		break;
	case Econsts:
		ne->lits = copylits(e->lits);
		break;
	case Econst:
		ne->liti = e->liti;
		break;
	case Ebinop:
		ne->op = e->op;
		break;
	default:
		break;
	}

	if(e->xp)
		fatal("bug");

	ne->e1 = copyexpr(e->e1);
	ne->e2 = copyexpr(e->e2);
	ne->e3 = copyexpr(e->e3);
	ne->e4 = copyexpr(e->e4);
	return ne;
}

/* Reverse the order of elements in Eelist E. */
Expr*
invert(Expr *e)
{
	Expr *tl, *q, *p, *t;

	if(e->kind == Enull)
		return e;

	tl = q = e;
	p = q->e2;
	while(p->kind != Enull){
		t = p->e2;
		p->e2 = q;
		q = p;
		p = t;
	}
	tl->e2 = p;

	return q;
}

/* Flatten Eelist of Eelists into single Eelist */
Expr*
flatten(Expr *e)
{
	Expr *p, *q;
	Expr *nl;

	nl = nullelist();
	p = e;
	while(p->kind != Enull){
		q = p->e1;
		while(q->kind != Enull){
			nl = newexpr(Eelist, q->e1, nl, 0, 0);
			q->e1 = 0;
			q = q->e2;
		}
		p = p->e2;
	}
	freeexpr(e);
	return invert(nl);
}

Expr*
nullelist()
{
	return newexpr(Enull, 0, 0, 0, 0);
}

Expr*
ptrto(Expr *ptre, Expr *e)
{
	Expr *p;

	p = ptre;
	while(p->e1)
		p = p->e1;
	p->e1 = e;

	return ptre;
}

Expr*
doid(char *s)
{
	Expr *e;
	e = newexpr(Eid, 0, 0, 0, 0);
	e->id = xstrdup(s);
	return e;
}

Expr*
doidnsrc(Src *src, char *s, unsigned long len)
{
	Expr *e;
	e = newexprsrc(src, Eid, 0, 0, 0, 0);
	e->id = xstrndup(s, len);
	return e;
}

Expr*
doidn(char *s, unsigned long len)
{
	return doidnsrc(0, s, len);
}

Expr*
mkconst(Cbase type, Imm val)
{
	Expr *e;
	e = newexpr(Econst, 0, 0, 0, 0);
	e->liti.val = val;
	e->liti.base = type;
	return e;
}

Expr*
mkconstliti(Liti *liti)
{
	Expr *e;
	e = newexpr(Econst, 0, 0, 0, 0);
	e->liti = *liti;
	return e;
}

int
parseliti(char *s, unsigned long len, Liti *liti, char **err)
{
	Imm n;
	enum { Rdec, Rhex, Roct } radix;
	enum { Snone=0, Su, Sl, Sul, Sll, Sull } suf;
	unsigned base, noct;
	char c, *p, *z;
	char buf[Maxliti];	/* stage for null-terminated strtoull input */

	if(len >= Maxliti){
		*err = "excessively long integer literal";
		return -1;
	}
		
	memcpy(buf, s, len);
	buf[len] = 0;
	s = buf;
	z = s+len;

	if(s[0] == 'L'){
		*err = "wide characters unsupported";
		return -1;
	}

	/* char constant */
	if(s[0] == '\''){
		if(s[1] != '\\'){
			c = s[1];
			goto achar;
		}

		switch(s[2]){
		case 'n':
			c = '\n';
			break;
		case 't':
			c = '\t';
			break;
		case 'v':
			c = '\v';
			break;
		case 'b':
			c = '\b';
			break;
		case 'r':
			c = '\r';
			break;
		case 'f':
			c = '\f';
			break;
		case 'a':
			c = '\a';
			break;
		case '\\':
			c = '\\';
			break;
		case '\?':
			c = '\?';
			break;
		case '\'':
			c = '\'';
			break;
		case '"':
			c = '\"';
			break;
		default:
			p = &s[2];
			c = 0;
			if(*p == 'x'){
				p++;
				while(xisxdigit(*p)){
					c *= 16;
					if(xisdigit(*p))
						c += *p - '0';
					else if('a' <= *p && *p <= 'f')
						c += *p - 'a' + 10;
					else
						c += *p - 'A' + 10;
					p++;
				}
			}else{
				noct = 0;
				while(noct < 3 && xisodigit(*p)){
					c *= 8;
					c += *p - '0';
					p++;
					noct++;
				}
			}
			break;
		}
	achar:
		liti->base = Vchar;
		liti->val = c;
		return 0;
	}

	if(strnchr(s, '.', len)){
		*err = "floating point constants unsupported";
		return -1;
	}

	/* integer constant */
	if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		radix = Rhex;
	else if(s[0] == '0')
		radix = Roct;
	else
		radix = Rdec;
	n = strtoull(s, &p, 0);

	suf = Snone;
	if(p == s){
		*err = "invalid integer constant";
		return -1;
	}
	while(p < z){
		if(*p == 'U' || *p == 'u')
			switch(suf){
			case Snone:
				suf = Su;
				break;
			case Sl:
				suf = Sul;
				break;
			case Sll:
				suf = Sull;
				break;
			default:
				*err = "invalid use of constant suffix U";
				return -1;
			}
		else if(*p == 'L' || *p == 'l')
			switch(suf){
			case Snone:
				suf = Sl;
				break;
			case Sl:
				suf = Sll;
				break;
			case Su:
				suf = Sul;
				break;
			case Sul:
				suf = Sull;
				break;
			default:
				*err = "invalid use of constant suffix L";
				return -1;
			}
		else{
			*err = "invalid integer constant";
			return -1;
		}
		p++;
	}
		
	if(radix == Rdec && suf == Snone){
		if(n <= Vintmax)
			base = Vint;
		else if(n <= Vlongmax)
			base = Vlong;
		else if(n <= Vulongmax)
			base = Vulong;
		else if(n <= Vvlongmax)
			base = Vvlong;
		else
			base = Vuvlong;
	}else if((radix == Roct || radix == Rhex) && (suf == Snone)){
		if(n <= Vintmax)
			base = Vint;
		else if(n <= Vuintmax)
			base = Vuint;
		else if(n <= Vlongmax)
			base = Vlong;
		else if(n <= Vulongmax)
			base = Vulong;
		else if(n <= Vvlongmax)
			base = Vvlong;
		else
			base = Vuvlong;
	}else if(suf == Su){
		if(n <= Vuintmax)
			base = Vuint;
		else if(n <= Vulongmax)
			base = Vulong;
		else
			base = Vuvlong;
	}else if(suf == Sul){
		if(n <= Vulongmax)
			base = Vulong;
		else
			base = Vuvlong;
	}else if(suf == Sull)
		base = Vuvlong;
	else if(suf == Sl){
		if(n <= Vlongmax)
			base = Vlong;
		else if(n <= Vulongmax)
			base = Vulong;
		else if(n <= Vvlongmax)
			base = Vvlong;
		else
			base = Vuvlong;
	}else if(suf == Sll){
		if(n <= Vvlongmax)
			base = Vvlong;
		else
			base = Vuvlong;
	}else
		fatal("bug");

	liti->base = base;
	liti->val = n;
	return 0;
}

Expr*
doconst(U *ctx, char *s, unsigned long len)
{
	Liti liti;
	char *err;
	if(0 != parseliti(s, len, &liti, &err))
		parseerror(ctx, err);
	return mkconstliti(&liti);
}

Expr*
doconstssrc(Src *src, char *s, unsigned long len)
{
	Expr *e;
	int c;
	char *r, *w, *z;
	unsigned noct;

	r = s;
	w = s;
	z = s+len;
	r++;			/* skip opening quote */
	while(1){
		if(r[0] == '\"'){
			/* closing quote -- check for concontenation */
			while(++r < z){
				if(r[0] == '\"'){
					r++;
					break;
				}
			}
			if(r >= z)
				break;
		}

		if(r[0] != '\\'){
			*w++ = *r++;
			continue;
		}

		/* escape sequence */
		r++;
		switch(*r){
 		case 'n':
			c = '\n';
			r++;
			break;
		case 't':
			c = '\t';
			r++;
			break;
		case 'v':
			c = '\v';
			r++;
			break;
		case 'b':
			c = '\b';
			r++;
			break;
		case 'r':
			c = '\r';
			r++;
			break;
		case 'f':
			c = '\f';
			r++;
			break;
		case 'a':
			c = '\a';
			r++;
			break;
		case '\\':
			c = '\\';
			r++;
			break;
		case '\?':
			c = '\?';
			r++;
			break;
		case '\'':
			c = '\'';
			r++;
			break;
		case '"':
			c = '\"';
			r++;
			break;
		default:
			c = 0;
			if(*r == 'x'){
				r++;
				while(xisxdigit(*r)){
					c *= 16;
					if(xisdigit(*r))
						c += *r - '0';
					else if('a' <= *r && *r <= 'f')
						c += *r - 'a' + 10;
					else
						c += *r - 'A' + 10;
					r++;
				}
			}else{
				noct = 0;
				while(noct < 3 && xisodigit(*r)){
					c *= 8;
					c += *r - '0';
					r++;
					noct++;
				}
			}
			break;
		}
		*w++ = c;
	}

	e = newexprsrc(src, Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, w-s);
	return e;
}

Expr*
doconsts(char *s, unsigned long len)
{
	return doconstssrc(0, s, len);
}

Expr*
doticksrc(Src *src, Expr *dom, Expr *id)
{
	Expr *e;
	e = newexprsrc(src, Etick, dom, id, 0, 0);
	return e;
}

Expr*
dotick(Expr *dom, Expr *id)
{
	return doticksrc(0, dom, id);
}

static Expr*
exprinc(Expr *e)
{
	Expr *ne;

	ne = copyexpr(e);
	if(ne->kind == Econst){
		ne->liti.val++;
		return ne;
	}else if(ne->kind == Ebinop && ne->op == Eadd
		 && ne->e2->kind == Econst){
		/* match form generated by following case */
		ne->e2->liti.val++;
		return ne;
	}else
		return newbinop(Eadd, ne, mkconst(Vuint, 1));
}

void
freeenum(Enum *en)
{
	Enum *q;

	while(en){
		q = en->link;
		efree(en->id);
		freeexpr(en->val);
		efree(en);
		en = q;
	}
}

static Enum*
recenums(Type *t, Expr *e, Expr *val)
{
	Expr *el;
	Enum *en;

	if(e->kind == Enull){
		freeexpr(val);
		return NULL;
	}
	if(e->kind != Eelist)	/* FIXME delete */
		fatal("bug");

	el = e->e1;
	if(el->kind != Eenumel)	/* FIXME delete */
		fatal("bug");

	en = emalloc(sizeof(Enum));
	en->id = el->e1->id; /* steal */
	el->e1->id = NULL;
	if(el->e2){
		freeexpr(val);
		en->val = el->e2; /* steal */
		el->e2 = NULL;
	}else
		en->val = val;
	en->link = recenums(t, e->e2, exprinc(en->val));
	return en;
}

static Enum*
enums(Type *t, Expr *e)
{
	return recenums(t, e, mkconst(Vuint, 0));
}

static Decl*
sufields(U *ctx, Type *su, Expr *e, Expr **sz)
{
	Decl *hd, *p;

	if(e == NULL)
		return NULL;
	if(e->kind == Enull){
		/* ascribe size 0 to `struct tag {}' */
		/* if we make aggregate size optional,
		   then (FIXME) do this only if sufields
		   was passed an empty list */
		if(*sz == 0)
			*sz = mkconst(Vint, 0);
		return NULL;
	}
	switch(e->e1->kind){
	case Ebitfield:
		hd = dodecl(ctx, e->e1);
		hd->offs = e->e1->e3; /* steal */
		e->e1->e3 = NULL;
		hd->type->bitw = e->e1->e4; /* steal */
		e->e1->e4 = NULL;
		hd->link = sufields(ctx, su, e->e2, sz);
		break;
	case Efields:
		hd = dodecls(ctx, e->e1);
		if(e->e1->e3){
			hd->offs = e->e1->e3; /* steal */
			e->e1->e3 = NULL;
		}
		p = hd;
		while(p->link != NULL)
			p = p->link;
		p->link = sufields(ctx, su, e->e2, sz);
		break;
	case Efieldoff:
		*sz = e->e1->e1; /* steal */
		e->e1->e1 = NULL;
		return sufields(ctx, su, e->e2, sz);
		break;
	default:
		fatal("unrecognized su declaration %d", e->e1->kind);
	}
	return hd;
}

static Decl*
params(U *ctx, Expr *e)
{
	Decl *hd;

	if(e->kind == Enull)
		return NULL;

	if(e->kind != Eelist)
		fatal("params expects an expression list");

	hd = dodecl(ctx, e->e1);
	hd->link = params(ctx, e->e2);

	return hd;
}

static Cbase
baselist(U *ctx, Expr *e)
{
	Expr *s;
	Cbase base;
	
	base = Vundef;
	while(e->kind != Enull){
		s = e->e1;
		e = e->e2;
		switch(s->kind){
		case Echar:
		case Edouble:
		case Efloat:
		case Eint:	
		case Elong:
		case Eshort:
		case Esigned:
		case Eunsigned:
		case Evoid:
			base = basemod[base][s->kind];
			/* FIXME: can we rely on parser to structure these
			   constructions, and eliminate Vundef? */
			if(base == Vundef)
				parseerror(ctx, "bad type specifier");
			break;
		default:
			parseerror(ctx, "bad type specifier");
		}
	}
	return base;
}

static Type*
specifier(U *ctx, Expr *e)
{
	Type *t;
	Expr *dom, *id;
	Cbase cb;

	t = newtype();
	switch(e->kind){
	case Ebase:
		/* < Ebase, baselist, dom, 0, 0 > */
		cb = baselist(ctx, e->e1);
		if(cb == Vvoid)
			t->kind = Tvoid;
		else{
			t->kind = Tbase;
			t->base = cb;
		}
		dom = e->e2;
		break;
	case Etid:
		/* < Etid, tid, dom, 0, 0 > */
		t->kind = Ttypedef;
		id = e->e1;
		t->tid = id->id; /* steal */
		id->id = 0;
		dom = e->e2;
		break;
	case Eenum:
	case Estruct:
	case Eunion:
		/* < kind, tag, decls, dom, 0 > */
		switch(e->kind){
		case Eenum:
			t->kind = Tenum;
			break;
		case Estruct:
			t->kind = Tstruct;
			break;
		case Eunion:
			t->kind = Tunion;
			break;
		default:
			fatal("bug");
		}
		id = e->e1;
		t->tag = id->id; /* steal */
		id->id = 0;
		dom = e->e3;
		if(e->e2){
			if(t->kind == Tenum)
				t->en = enums(t, e->e2);
			else
				t->field = sufields(ctx, t, e->e2, &t->sz);
		}
		break;
	default:
		fatal("bug");
	}
	
	if(dom){
		t->dom = dom->id; /* steal */
		dom->id = 0;
	}

	return t;
}

static Decl*
declarator(U *ctx, Type *bt, Expr *e)
{
	Type *t;
	Decl *d;

	if(e == NULL){
		d = newdecl();
		d->type = bt;
		return d;
	}

	switch(e->kind){
	case Eid:
		d = newdecl();
		d->type = bt;
		d->id = e->id;	/* steal */
		e->id = NULL;
		return d;
	case Eptr:
		t = newtype();
		t->kind = Tptr;
		t->link = bt;
		t->dom = xstrdup(bt->dom);
		return declarator(ctx, t, e->e1);
	case Earr:
		t = newtype();
		t->kind = Tarr;
		t->link = bt;
		t->cnt = e->e2;	/* steal */
		t->dom = xstrdup(bt->dom);
		e->e2 = NULL;
		return declarator(ctx, t, e->e1);
	case Efun:
		t = newtype();
		t->kind = Tfun;
		t->link = bt;
		t->param = params(ctx, e->e2);
		t->dom = xstrdup(bt->dom);
		return declarator(ctx, t, e->e1);
	default:
		fatal("bug");
		break;
	}
	return NULL;
}

static Decl*
copydecls(Decl *dl)
{
	Decl *nd;

	if(dl == 0)
		return 0;
	nd = newdecl();
	nd->id = xstrdup(dl->id);
	nd->offs = copyexpr(dl->offs);
	nd->link = copydecls(dl->link);
	return nd;
}

static Type*
copytype(Type *t)
{
	Type *nt;

	if(t == 0)
		return 0;

	nt = newtype();
	nt->kind = t->kind;
	nt->dom = xstrdup(t->dom);
	nt->bitw = copyexpr(t->bitw);
	nt->bit0 = copyexpr(t->bit0);
	switch(nt->kind){
	case Tvoid:
		break;
	case Tbase:
		nt->base = t->base;
		break;
	case Ttypedef:
		nt->tid = xstrdup(t->tid);
		break;
	case Tstruct:
	case Tunion:
		nt->tag = xstrdup(t->tag);
		nt->field = copydecls(t->field);
		nt->sz = copyexpr(t->sz);
		break;
	case Tenum:
		fatal("define copytype on enum");
	case Tptr:
		nt->link = copytype(t->link);
		break;
	case Tfun:
		nt->link = copytype(t->link);
		nt->param = copydecls(t->param);
		break;
	case Tarr:
		nt->link = copytype(t->link);
		nt->cnt = copyexpr(t->cnt);
		break;
	}
	return nt;
}

static Decl*
dodecls(U *ctx, Expr *e)
{
	Type *t;
	Expr *dl;
	Decl *rv, *p;
	Decl **lp;

	t = specifier(ctx, e->e1);

	dl = e->e2;
	if(dl->kind != Enull){
		lp = &rv;
		while(dl->kind != Enull){
			if(lp != &rv)
				/* one copy of type per each declarator */
				t = copytype(t);
			p = declarator(ctx, t, dl->e1);
			*lp = p;
			lp = &p->link;
			dl = dl->e2;
		}
		*lp = NULL;
	}else{
		rv = newdecl();
		rv->type = t;
	}

	if(e->e3){
		if(rv == NULL)
			fatal("bug");
		rv->offs = e->e3; /* steal */
		e->e3 = NULL;
	}

	return rv;
}

static Decl*
dodecl(U *ctx, Expr *e)
{
	Type *t;
	Decl *rv;

	t = specifier(ctx, e->e1);
	rv = declarator(ctx, t, e->e2);

	return rv;
}

Expr*
dotypes(U *ctx, Expr *e)
{
	Expr *p;

	if(e == 0)
		return 0;

	switch(e->kind){
	case Etypedef:
	case Edecl:
		e->xp = dodecl(ctx, e);
		break;
	case Edecls:
		e->xp = dodecls(ctx, e);
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			dotypes(ctx, p->e1);
			p = p->e2;
		}
		break;
	default:
		dotypes(ctx, e->e1);
		dotypes(ctx, e->e2);
		dotypes(ctx, e->e3);
		dotypes(ctx, e->e4);
		break;
	}
	return e;
}

void
dotop(U *ctx, Expr *e)
{
	ctx->el = newexprsrc(&ctx->inp->src, Eelist, e, ctx->el, 0, 0);
}

static void
initparse()
{
	filenames = mkht();
}

static void
freefilename(void *u, char *k, void *v)
{
	efree(k);
}

static void
finiparse()
{
	hforeach(filenames, freefilename, 0);
	freeht(filenames);
}

static void
pushyy(U *ctx, char *filename, char *buf, int dofree)
{
	char *keyed;

	if(ctx->inp == 0)
		ctx->inp = ctx->in;
	else
		ctx->inp++;
	if(ctx->inp >= ctx->in+MaxIn)
		fatal("maximum include depth exceeded");
	keyed = 0;
	if(filename){
		keyed = hget(filenames, filename, strlen(filename));
		if(!keyed){
			keyed = xstrdup(filename);
			hput(filenames, keyed, strlen(keyed), keyed);
		}
	}
	ctx->inp->src.filename = keyed;
	ctx->inp->yy = mkyystatestr(buf);
	ctx->inp->src.line = 1;
	ctx->inp->src.col = 0;
	ctx->inp->dofree = dofree;
	ctx->inp->buf = buf;
	setyystate(ctx->inp->yy);
}

int
popyy(U *ctx)
{
	if(ctx->inp == 0)
		return 0;
	ctx->inp->src.filename = 0;
	freeyystate(ctx->inp->yy);
	if(ctx->inp->dofree)
		efree(ctx->inp->buf);
	if(ctx->inp == ctx->in){
		ctx->inp = 0;
		return 0;
	}
	ctx->inp--;
	setyystate(ctx->inp->yy);
	return 1;
}

void
tryinclude(U *ctx, char *raw)
{
	char *p, *q, *path, *buf, *full, **lp;
	unsigned len;
	int c, f;

	f = '"';
	len = strlen(raw);
	p = strchr(raw, '"');
	if(!p){
		p = strchr(raw, '<');
		f = '>';
	}
	if(!p)
		fatal("bug");

	c = *p++;
	q = strchr(p, f);
	if(!q)
		fatal("bug");
	*q = 0;

	buf = 0;
	if(f == '>' && p[0] != '/' && p[0] != '.'){
		/* use load path */
		lp = loadpath;
		while(*lp){
			len = strlen(*lp)+1+strlen(p)+1;
			full = emalloc(len);
			snprintf(full, len, "%s/%s", *lp, p);
			buf = readfile(full);
			if(buf)
				break;
			lp++;
		}
	}else{
		if(p[0] != '/' && ctx->inp->src.filename){
			path = xstrdup(ctx->inp->src.filename);
			len = strlen(path)+1+strlen(p)+1;
			full = emalloc(len);
			snprintf(full, len, "%s/%s", dirname(path), p);
			efree(path);
		}else
			full = xstrdup(p);
		buf = readfile(full);
	}
	if(buf == 0)
		parseerror(ctx, "cannot @include %s", p);
	pushyy(ctx, full, buf, 1);
	efree(full);
}

static Expr*
doparse(char *buf, char *whence)
{
	U ctx;

	memset(&ctx, 0, sizeof(ctx));
	ctx.el = nullelist();
	if(setjmp(ctx.jmp) == 0){
		pushyy(&ctx, whence, buf, 0);
		if(yyparse(&ctx) != 0)
			fatal("parse error");
	}else
		return 0;

	ctx.el = invert(ctx.el);
	popyy(&ctx);
	/* FIXME: does bison normally call yylex_destroy implicitly?
	   http://www.mail-archive.com/bison-patches@gnu.org/msg01521.html
	*/
	yylex_destroy();
	return ctx.el;
}

Expr*
cqctparsefile(char *filename)
{
	Expr *e;
	char *str;
	str = readfile(filename);
	if(str == 0)
		return 0;
	e = doparse(str, filename);
	efree(str);
	return e;
}

Expr*
cqctparsestr(char *str, char *whence)
{
	return doparse(str, whence);
}

Closure*
cqctcompile(Expr *e, Toplevel *top, char *argsid)
{
	U ctx;

	if(cqctflags['p']){
		xprintf("input:\n");
		printexpr(e);
		xprintf("\n");
	}
	memset(&ctx, 0, sizeof(ctx));
	dotypes(&ctx, e);
	if(docompilec(&ctx, e) != 0)
		return 0;
	if(docompile0(&ctx, e) != 0)
		return 0;
	if(cqctflags['p']){
		xprintf("compile0:\n");
		printexpr(e);
		xprintf("\n");
	}
	return compileentry(e, top, argsid);
}

static char**
copyargv(char **lp)
{
	unsigned n, i, nlp;
	char **p, **rv, *s;

	n = 0;
	nlp = 0;

	if(lp != 0){
		p = lp;
		while(*p){
			n += strlen(*p)+1;
			nlp++;
			p++;
		}
	}
	rv = emalloc((nlp+1)*sizeof(char*)+n);
	p = rv;
	s = (char*)(rv+nlp+1);
	for(i = 0; i < nlp; i++){
		p[i] = s;
		strcpy(s, lp[i]);
		s += strlen(lp[i])+1;
	}
	p[nlp] = 0;

	return rv;
}

Toplevel*
cqctinit(int gcthread, u64 heapmax, char **lp)
{
	loadpath = copyargv(lp);
	initparse();
	initcompile();
	initvm(gcthread, heapmax);
	return mktoplevel();
}

void
cqctfini(Toplevel *top)
{
	efree(loadpath);
	freetoplevel(top);
	finivm();
	finicompile();
	finiparse();
}

void
cqctfreeexpr(Expr *e)
{
	freeexpr(e);
}
