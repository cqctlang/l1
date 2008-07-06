#include "sys.h"
#include "util.h"
#include "l1.h"


char* basename[Vnbase] = {
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
	[Vvoid]		      = "void",
/*
	[Vptr]		      = "void*",
*/
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

static Decl* dodecls(Expr *e);
static Decl* dodecl(Expr *e);
static char* fmtdecl(Decl *d);
static Type* copytype(Type *t);

static HT *filenames;

U ctx;
char *stdinname = "<stdin>";

extern int yylex_destroy(void);

void
parseerror(char *fmt, ...)
{
	va_list args;

	if(ctx.inp)
		fprintf(stderr, "%s:%u: ", ctx.inp->filename, ctx.inp->line);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);

	while(popyy())
		;
	yylex_destroy();
	longjmp(ctx.jmp, 1);
}

Lits*
mklits(char *s, unsigned len)
{
	Lits *lits;
	char *p;

	lits = xmalloc(sizeof(Lits)+len);
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
	free(lits);
}

static Type*
newtype()
{
	Type *t;
	t = xmalloc(sizeof(Type));
	return t;
}

static Decl*
newdecl()
{
	Decl *d;
	d = xmalloc(sizeof(Decl));
	return d;
}

Type*
basetype(unsigned base)
{
	Type *t;
	t = newtype();
	t->kind = Tbase;
	t->base = base;
	return t;
}

static void
initbase()
{


}

Expr*
newexpr(unsigned kind, Expr *e1, Expr *e2, Expr *e3, Expr *e4)
{
	Expr *e;

	e = xmalloc(sizeof(Expr));
	e->kind = kind;
	e->e1 = e1;
	e->e2 = e2;	
	e->e3 = e3;
	e->e4 = e4;

	if(ctx.inp){
		e->src.filename = ctx.inp->filename;
		e->src.line = ctx.inp->line;
	}

	return e;
}

Expr*
newbinop(unsigned kind, Expr *e1, Expr *e2)
{
	Expr *e;
	e = newexpr(Ebinop, e1, e2, 0, 0);
	e->op = kind;
	return e;
}

Expr*
newgop(unsigned kind, Expr *e1, Expr *e2)
{
	Expr *e;
	e = newexpr(Egop, e1, e2, 0, 0);
	e->op = kind;
	return e;
}

void
freeexpr(Expr *e)
{
	if(e == 0)
		return;
	switch(e->kind){
	case Eid:
		free(e->id);
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
	free(e);
}

Expr*
copyexpr(Expr *e)
{
	Expr *ne;

	if(e == 0)
		return 0;

	ne = xmalloc(sizeof(Expr));
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
doidn(char *s, unsigned long len)
{
	Expr *e;
	e = newexpr(Eid, 0, 0, 0, 0);
	e->id = xstrndup(s, len);
	return e;
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

static int
isoctdigit(int c)
{
	return ('0' <= c) && (c <= '7');
}

Expr*
doconst(char *s, unsigned long len)
{
	Imm n;
	enum { Rdec, Rhex, Roct } radix;
	enum { Snone=0, Su, Sl, Sul, Sll, Sull } suf;
	unsigned base, noct;
	char c, *p, *z;

	z = s+len;

	if(s[0] == 'L')
		parseerror("wide characters unsupported");

	/* char constant */
	if(s[0] == '\''){
		if(s[1] != '\\')
			return mkconst(Vchar, s[1]);

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
				while(isxdigit(*p)){
					c *= 16;
					if(isdigit(*p))
						c += *p - '0';
					else if('a' <= *p && *p <= 'f')
						c += *p - 'a' + 10;
					else
						c += *p - 'A' + 10;
					p++;
				}
			}else{
				noct = 0;
				while(noct < 3 && isoctdigit(*p)){
					c *= 8;
					c += *p - '0';
					p++;
					noct++;
				}
			}
			break;
		}
		return mkconst(Vchar, c);
	}

	if(strchr(s, '.'))
		parseerror("float pointer constants unsupported");

	/* integer constant */
	if(s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		radix = Rhex;
	else if(s[0] == '0')
		radix = Roct;
	else
		radix = Rdec;
	n = strtoull(s, &p, 0);

	suf = Snone;
	if(p == s)
		parseerror("bad integer constant");
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
				parseerror("bad use of constant suffix U");
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
				parseerror("bad use of constant suffix L");
			}
		else
			parseerror("bad integer constant suffix %c", *p);
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

	return mkconst(base, n);
}

Expr*
doconsts(char *s, unsigned long len)
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
				while(isxdigit(*r)){
					c *= 16;
					if(isdigit(*r))
						c += *r - '0';
					else if('a' <= *r && *r <= 'f')
						c += *r - 'a' + 10;
					else
						c += *r - 'A' + 10;
					r++;
				}
			}else{
				noct = 0;
				while(noct < 3 && isoctdigit(*r)){
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

	e = newexpr(Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, w-s);
	return e;
}

Expr*
dotick(Expr *dom, Expr *id)
{
	Expr *e;
	e = newexpr(Etick, dom, id, 0, 0);
	return e;
}

#if 0
static Expr*
recexprinc(Expr *e)
{
	switch(e->kind){
	case Econst:
		e->cval.val++;
		return e;
	case Ebinop:
		if(e->op != Eadd)
			fatal("bug"); /* incomplete implementation? */
		if(recexprinc(e->e1))
			return e;
		else if(recexprinc(e->e2))
			return e;
		else
			return 0;
	default:
		return 0;
	}
}

static Expr*
exprinc(Expr *e)
{
	Expr *ne, *rv;

	ne = copyexpr(e);
	rv = recexprinc(ne);
	if(rv)
		return rv;
	return newbinop(Eadd, mkconst(Vuint, 1), ne);
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
	if(e->kind != Eelist)
		fatal("bug");

	el = e->e1;
	if(el->kind != Eenumel)
		fatal("bug");

	en = xmalloc(sizeof(Enum));
	en->id = el->e1->id; /* steal */
	el->e1->id = NULL;
	if(el->e2){
		freeexpr(val);
		en->val = el->e2; /* steal */
		el->e2 = NULL;
	}else
		en->val = val;
	en->type = t;
/*	encstore(ctx.ns, en->id, en); */

	printf("enum %s ", en->id);
	printexpr(en->val);
	printf("\n");
	recenums(t, e->e2, exprinc(en->val));

	return 0;
}
#endif

static Enum*
enums(Type *t, Expr *e)
{
	if(e == NULL)
		return NULL;
#if 0
	return recenums(t, e, mkconst(Vuint, 0));
#endif
	return 0;
}

static Decl*
sufields(Type *su, Expr *e)
{
	Decl *hd, *p;

	if(e == NULL)
		return NULL;
	if(e->kind == Enull)
		return NULL;

	if(e->kind != Eelist)
		fatal("sufields expects an expression list");

	switch(e->e1->kind){
	case Efields:
		hd = dodecls(e->e1);
		if(e->e1->e3){
			hd->offs = e->e1->e3; /* steal */
			e->e1->e3 = NULL;
		}
		p = hd;
		while(p->link != NULL)
			p = p->link;
		p->link = sufields(su, e->e2);
		break;
	case Efieldoff:
		su->sz = e->e1->e1; /* steal */
		e->e1->e1 = NULL;
		return sufields(su, e->e2);
		break;
	default:
		fatal("unrecognized su declaration %d", e->e1->kind);
	}
	return hd;
}

static Decl*
params(Expr *e)
{
	Decl *hd;

	if(e->kind == Enull)
		return NULL;

	if(e->kind != Eelist)
		fatal("params expects an expression list");

	hd = dodecl(e->e1);
	hd->link = params(e->e2);

	return hd;
}

static unsigned
baselist(Expr *e)
{
	Expr *s;
	unsigned base;
	
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
				parseerror("bad type specifier");
			break;
		default:
			parseerror("bad type specifier");
		}
	}
	return base;
}

static Type*
specifier(Expr *e)
{
	Type *t;
	Expr *dom, *id;

	t = newtype();
	switch(e->kind){
	case Ebase:
		/* < Ebase, baselist, dom, 0, 0 > */
		t->kind = Tbase;
		t->base = baselist(e->e1);
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
				t->field = sufields(t, e->e2);
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
declarator0(Type *bt, Expr *e)
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
		return declarator0(t, e->e1);
	case Earr:
		t = newtype();
		t->kind = Tarr;
		t->link = bt;
		t->cnt = e->e2;	/* steal */
		e->e2 = NULL;
		return declarator0(t, e->e1);
	case Efun:
		t = newtype();
		t->kind = Tfun;
		t->link = bt;
		t->param = params(e->e2);
		return declarator0(t, e->e1);
	default:
		fatal("bug");
		break;
	}
	return NULL;
}

static Decl*
declarator(Type *bt, Expr *e)
{
	Decl *d;

	if(e && e->kind == Ebits){
		if(e->e1)
			d = declarator0(bt, e->e1);
		else
			d = newdecl();
		d->bits = e->e2; /* steal */
		e->e2 = NULL;
	}else
		d = declarator0(bt, e);
	return d;
}

static int
needsparen(unsigned kind)
{
	switch(kind){
	case Tfun:
	case Tarr:
		return 1;
	default:
		return 0;
	}
}

static char*
fmtdecllist(Decl *p)
{
	char **ds, *buf, *bp;
	unsigned i, n;
	unsigned long m;
	Decl *q;

	n = 0;
	q = p;
	while(q){
		n++;
		q = q->link;
	}

	if(n == 0)
		return xstrdup("");

	ds = xmalloc(n*sizeof(char**));
	m = 1;			/* null */
	for(i = 0, q = p; i < n; i++, q = q->link){
		ds[i] = fmtdecl(q);
		m += strlen(ds[i]);
		if(i < n-1)
			m += 2;	/* comma, space */
	}

	buf = xmalloc(m);
	bp = buf;
	for(i = 0; i < n; i++){
		strcpy(bp, ds[i]);
		bp += strlen(ds[i]);
		if(i < n-1){
			strcpy(bp, ", ");
			bp += 2;
		}
	}
	*bp = 0;

	return buf;
}

/* o must be dynamically allocated; caller turns over its management
   to fmttype.  returned string is also dynamically allocated; caller
   must free it */
char*
fmttype(Type *t, char *o)
{
	char *buf, *w, *pl;
	unsigned m;

	switch(t->kind){
	case Tbase:
		m = strlen(basename[t->base])+1+strlen(o)+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s %s", basename[t->base], o);
		free(o);
		return buf;
	case Ttypedef:
		m = strlen(t->tid)+1+strlen(o)+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s %s", t->tid, o);
		free(o);
		return buf;
	case Tstruct:
	case Tunion:
		w = tkindstr[t->kind];
		if(t->tag){
			m = strlen(w)+1+strlen(t->tag)+1+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "%s %s %s", w, t->tag, o);
		}else{
			m = strlen(w)+1+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "%s %s", w, o);
		}
		free(o);
		return buf;
	case Tenum:
		if(t->tag){
			m = 4+1+strlen(t->tag)+1+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "enum %s %s", t->tag, o);
		}else{
			m = 4+1+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "enum %s", o);
		}
		free(o);
		return buf;
	case Tptr:
		m = 2+strlen(o)+1+1;
		buf = xmalloc(m);
		if(needsparen(t->link->kind))
			snprintf(buf, m, "(*%s)", o);
		else
			snprintf(buf, m, "*%s", o);
		free(o);
		return fmttype(t->link, buf);
	case Tarr:
		m = strlen(o)+2+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s[]", o);
		free(o);
		return fmttype(t->link, buf);
	case Tfun:
		pl = fmtdecllist(t->param);
		m = strlen(o)+1+strlen(pl)+1+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s(%s)", o, pl);
		free(o);
		free(pl);
		return fmttype(t->link, buf);
	default:
		fatal("bug");
	}
	return NULL;
}

static char*
fmtdecl(Decl *d)
{
	char *o;

	if(d->type == NULL)
		return NULL;

	if(d->id)
		o = xstrdup(d->id);
	else
		o = xstrdup("");

	return fmttype(d->type, o);
}

static void
printdecl(Decl *d)
{
	char *o;

	o = fmtdecl(d);
	printf("%s", o);
	free(o);
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
	nd->bits = copyexpr(dl->bits);
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
	switch(nt->kind){
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
dodecls(Expr *e)
{
	Type *t;
	Expr *dl;
	Decl *rv, *p;
	Decl **lp;

	t = specifier(e->e1);

	dl = e->e2;
	if(dl->kind != Enull){
		lp = &rv;
		while(dl->kind != Enull){
			if(lp != &rv)
				/* one copy of type per each declarator */
				t = copytype(t);
			p = declarator(t, dl->e1);
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
dodecl(Expr *e)
{
	Type *t;
	Decl *rv;

	t = specifier(e->e1);
	rv = declarator(t, e->e2);

	return rv;
}

Expr*
dotypes(Expr *e)
{
	if(e == 0)
		return 0;

	switch(e->kind){
	case Etypedef:
	case Edecl:
		e->xp = dodecl(e);
		break;
	case Edecls:
		e->xp = dodecls(e);
		break;
	default:
		dotypes(e->e1);
		dotypes(e->e2);
		dotypes(e->e3);
		dotypes(e->e4);
		break;
	}
	return e;
}

void
dotop(Expr *e)
{
	ctx.el = newexpr(Eelist, e, ctx.el, 0, 0);
}

void
initparse()
{
	filenames = mkht();
	initbase();
}

static void
freefilename(void *u, char *k, void *v)
{
	free(k);
}

void
finiparse()
{
	hforeach(filenames, freefilename, 0);
	freeht(filenames);
}

void
pushyy(char *filename)
{
	FILE *fp;
	char *keyed;

	if(filename == stdinname)
		fp = stdin;
	else
		fp = fopen(filename, "r");
	if(fp == 0)
		parseerror("cannot @include %s", filename);

	if(ctx.inp == 0)
		ctx.inp = ctx.in;
	else
		ctx.inp++;

	if(ctx.inp >= ctx.in+MaxIn)
		fatal("maximum include depth exceeded");

	ctx.inp->fp = fp;
	keyed = hget(filenames, filename);
	if(!keyed){
		keyed = xstrdup(filename);
		hput(filenames, keyed, 0);
	}
	ctx.inp->filename = keyed;
	ctx.inp->yy = mkyystate(fp);
	ctx.inp->line = 1;
	ctx.inp->col = 0;
	setyystate(ctx.inp->yy);
}

int
popyy()
{
	if(ctx.inp == 0)
		return 0;
	if(ctx.inp->fp != stdin)
		fclose(ctx.inp->fp);
	ctx.inp->filename = 0;
	freeyystate(ctx.inp->yy);
	if(ctx.inp == ctx.in){
		ctx.inp = 0;
		return 0;
	}
	ctx.inp--;
	setyystate(ctx.inp->yy);
	return 1;
}

void
tryinclude(char *raw)
{
	char *p, *q;
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

	switch(c){
	case '"':
		printf("include relative %s\n", p);
		break;
	case '<':
		printf("include system %s\n", p);
		break;
	}

	pushyy(p);
}

int
doparse(char *filename)
{
	ctx.el = nullelist();
	if(setjmp(ctx.jmp) == 0){
		pushyy(filename);
		yyparse();
	}else
		return -1;

	ctx.el = invert(ctx.el);
	popyy();
	/* FIXME: does bison normally call yylex_destroy implicitly?
	   http://www.mail-archive.com/bison-patches@gnu.org/msg01521.html
	*/
	yylex_destroy();
	return 0;
}
