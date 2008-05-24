#include "sys.h"
#include "util.h"
#include "l1.h"

static unsigned basemod[Vnil+1][Enbase];
char* basename[Vnil+1];
Imm   basesize[Vnil+1];
Imm   ptrsize;

static Decl* dodecls(Expr *e);
static char* fmtdecl(Decl *d);
static Type* copytype(Type *t);

static HT *filenames;

U ctx;
char *stdinname = "<stdin>";
int yylex_destroy(void);

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

NS*
mkns()
{
	NS *ns;
	ns = xmalloc(sizeof(NS));
	ns->tagtab = mkht();
	ns->tidtab = mkht();
	ns->symtab = mkht();
	ns->enctab = mkht();
	return ns;
}

void
freens(NS *ns)
{
	freeht(ns->tagtab);
	freeht(ns->tidtab);
	freeht(ns->symtab);
	freeht(ns->enctab);
	free(ns);
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
	basename[Verr]                = "error!";
	basename[Vchar]               = "char";
	basename[Vshort]	      = "short";	     
	basename[Vint]		      = "int";
	basename[Vlong]		      = "long";
	basename[Vvlong]	      = "vlong";
	basename[Vuchar]	      = "uchar";
	basename[Vushort]	      = "ushort";
	basename[Vuint]		      = "uint";
	basename[Vulong]	      = "ulong";
	basename[Vuvlong]	      = "uvlong";
	basename[Vfloat]	      = "float";
	basename[Vdouble]	      = "double";
	basename[Vlongdouble]	      = "longdouble";
	basename[Vvoid]		      = "void";
	basename[Vnil]		      = "error!";

	basesize[Verr]                = -1;
	basesize[Vchar]               = 1;
	basesize[Vshort]	      = 2;
	basesize[Vint]		      = 4;
	basesize[Vlong]		      = 4;
	basesize[Vvlong]	      = 8;
	basesize[Vuchar]	      = 1;
	basesize[Vushort]	      = 2;
	basesize[Vuint]		      = 4;
	basesize[Vulong]	      = 4;
	basesize[Vuvlong]	      = 8;
	basesize[Vfloat]	      = 4;
	basesize[Vdouble]	      = 8;
	basesize[Vlongdouble]	      = 12;
	basesize[Vvoid]		      = 1;
	basesize[Vnil]		      = -1;

	ptrsize = 4;

	basemod[Vchar][Eunsigned]     = Vuchar;
	basemod[Vchar][Esigned]       = Vchar;

	basemod[Vshort][Eunsigned]    = Vushort;
	basemod[Vshort][Esigned]      = Vshort;
	basemod[Vshort][Eint]         = Vshort;
	basemod[Vushort][Eint]        = Vushort;

	basemod[Vint][Eunsigned]      = Vuint;
	basemod[Vint][Esigned]        = Vint;
	basemod[Vint][Elong]          = Vlong;
	basemod[Vuint][Elong]         = Vulong;
	
	basemod[Vlong][Eunsigned]     = Vulong;
	basemod[Vlong][Esigned]       = Vlong;
	basemod[Vlong][Elong]         = Vvlong;
	basemod[Vulong][Elong]        = Vuvlong;

	basemod[Vvlong][Eunsigned]    = Vuvlong;
	basemod[Vvlong][Esigned]      = Vvlong;

	basemod[Vdouble][Elong]       = Vlongdouble;

	basemod[Vnil][Echar]          = Vchar;
	basemod[Vnil][Edouble]        = Vdouble;	
	basemod[Vnil][Efloat]         = Vfloat;	
	basemod[Vnil][Eint]           = Vint;
	basemod[Vnil][Elong]          = Vlong;
	basemod[Vnil][Eshort]         = Vshort;
	basemod[Vnil][Esigned]        = Vint;
	basemod[Vnil][Eunsigned]      = Vuint;
	basemod[Vnil][Evoid]          = Vvoid;

	/* the rest are Verr, which we assume to be 0 */
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
freeexpr(Expr *e, void(*xfn)(Expr*))
{
	if(e == 0)
		return;
	switch(e->kind){
	case Eid:
	case Etick:
		free(e->id);
		break;
	case Econsts:
		freelits(e->lits);
		break;
	case Econst:
		freetype(e->cval.type, xfn);
		break;
	default:
		break;
	}
	freeexpr(e->e1, xfn);
	freeexpr(e->e2, xfn);
	freeexpr(e->e3, xfn);
	freeexpr(e->e4, xfn);
	if(xfn)
		xfn(e);
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
	case Etick:
		ne->id = xstrdup(e->id);
		break;
	case Econsts:
		ne->lits = copylits(e->lits);
		break;
	case Econst:
		ne->cval = e->cval;
		break;
	default:
		break;
	}

	ne->e1 = copyexpr(e->e1);
	ne->e2 = copyexpr(e->e2);
	ne->e3 = copyexpr(e->e3);
	ne->e4 = copyexpr(e->e4);
	return ne;
}

Type*
taglookup(NS *ns, char *tag)
{
	return hget(ns->tagtab, tag);
}

static void
tagstore(NS *ns, Type *t)
{
	hput(ns->tagtab, t->tag, t);
}

Type*
tidlookup(NS *ns, char *tid)
{
	return hget(ns->tidtab, tid);
}

static void
tidstore(NS *ns, char *tid, Type *t)
{
	hput(ns->tidtab, tid, t);
}

Decl*
symlookup(NS *ns, char *id)
{
	return hget(ns->symtab, id);
}

static void
symstore(NS *ns, char *id, Decl *d)
{
	hput(ns->symtab, id, d);
}

Enum*
enclookup(NS *ns, char *id)
{
	return hget(ns->enctab, id);
}

static void
encstore(NS *ns, char *id, Enum *en)
{
	hput(ns->enctab, id, en);
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

void
initcval(Cval *cval, Type *type, Imm val)
{
	cval->type = type;
	cval->val = val;
}

Expr*
mkconst(unsigned type, Imm val)
{
	Expr *e;
	e = newexpr(Econst, 0, 0, 0, 0);
	initcval(&e->cval, basetype(type), val);
	return e;
}

static int
isoctdigit(int c)
{
	return ('0' <= c) && (c <= '7');
}

Expr*
doconst(char *s)
{
	Imm n;
	enum { Rdec, Rhex, Roct } radix;
	enum { Snone=0, Su, Sl, Sul, Sll, Sull } suf;
	unsigned base, noct;
	char c, *p;

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
	while(*p != '\0'){
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
doconsts(char *s)
{
	Expr *e;
	int c;
	char *r, *w, *z;
	unsigned noct;

	r = s;
	w = s;
	z = s+strlen(s);
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
	free(s);
	return e;
}

Expr*
dotick(char *s)
{
	Expr *e;
	e = newexpr(Etick, 0, 0, 0, 0);
	e->id = xstrdup(s);
	return e;
}

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
		freeexpr(val, 0);
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
		freeexpr(val, 0);
		en->val = el->e2; /* steal */
		el->e2 = NULL;
	}else
		en->val = val;
	en->type = t;
	encstore(ctx.ns, en->id, en);

	printf("enum %s ", en->id);
	printexpr(en->val);
	printf("\n");
	recenums(t, e->e2, exprinc(en->val));

	return 0;
}

static Enum*
enums(Type *t, Expr *e)
{
	if(e == NULL)
		return NULL;
	return recenums(t, e, mkconst(Vuint, 0));
}

static Decl*
sufields(Expr *e)
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
		p = hd;
		while(p->link != NULL)
			p = p->link;
		p->link = sufields(e->e2);
		break;
	case Efieldoff:
		hd = newdecl();
		hd->offs = e->e1->e1; /* steal */
		e->e1->e1 = NULL;
		hd->link = sufields(e->e2);
		break;
	default:
		fatal("unrecognized su declaration %d", e->e1->kind);
	}
	return hd;
}

static Decl*
params(Expr *e)
{
	Decl *hd, *p;

	if(e->kind == Enull)
		return NULL;

	if(e->kind != Eelist)
		fatal("params expects an expression list");

	hd = dodecls(e->e1);
	p = hd;
	while(p->link != NULL)
		p = p->link;
	p->link = params(e->e2);

	return hd;
}

static Type*
speclist(Expr *e)
{
	Type *t;
	Expr *s, *tag, *dl;
	unsigned kind, base;

	t = NULL;
	base = Vnil;
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
			if(t)
				parseerror("bad type specifier");
			base = basemod[base][s->kind];
			if(base == Verr)
				parseerror("bad type specifier");
			break;
		case Eenum:
		case Estruct:
		case Eunion:
			if(t || base != Vnil)
				parseerror("bad type specifier");
			switch(s->kind){
			case Estruct:
				kind = Tstruct;
				break;
			case Eunion:
				kind = Tunion;
				break;
			case Eenum:
				kind = Tenum;
				break;
			default:
				fatal("bug");
				break;
			}

			tag = s->e1;
			dl = s->e2;

			t = newtype();
			t->kind = kind;
			if(tag){
				t->tag = tag->id; /* steal */
				tag->id = NULL;
			}
			if(dl){
				if(kind == Tenum)
					t->en = enums(t, dl);
				else
					t->field = sufields(dl);
			}
			break;
		case Eid:
			t = newtype();
			t->kind = Ttypedef;
			t->tid = s->id; /* steal */
			s->id = NULL;
			break;
		default:
			fatal("bug");
			break;
		}
	}

	if(base != Vnil)
		return basetype(base);

	if(t == NULL)
		fatal("unexpected specifier list");

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

	if(e->kind == Ebits){
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

/* o may be freed. */
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
		w = t->kind == Tstruct ? "struct" : "union";
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
	return t;
}

static Decl*
dodecls(Expr *e)
{
	Type *t;
	Expr *dl;
	Decl *rv, *p;
	Decl **lp;

	t = speclist(e->e1);

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
dotypedef(Expr *e)
{
	Type *t;
	Decl *rv;

	t = speclist(e->e1);
	rv = declarator(t, e->e2);

	/*	tidstore(ctx.ns, rv->id, rv->type); */

	return rv;
}

void
dotop(Expr *e)
{
	ctx.el = newexpr(Eelist, e, ctx.el, 0, 0);
}

Expr*
dotypes(Expr *e)
{
	if(e == 0)
		return 0;

	switch(e->kind){
	case Etypedef:
		e->xp = dotypedef(e);
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
tryinclude(NS *ns, char *raw)
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
	yylex_destroy();
	return 0;
}
