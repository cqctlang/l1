#include "sys.h"
#include "util.h"
#include "syscqct.h"

static u32 cnt;

void
cwarn(U *ctx, Expr *e, char *fmt, ...)
{
	va_list args;
	if(e->src.line)
		cprintf(ctx->out,
			"%s:%u: warning: ",
			e->src.filename ? e->src.filename : "<stdin>",
			e->src.line);
	else
		cprintf(ctx->out,
			"<lost-location!>: warning: ");
	va_start(args, fmt);
	cvprintf(ctx->out, fmt, args);
	va_end(args);
}

void
cwarnln(U *ctx, Expr *e, char *fmt, ...)
{
	va_list args;
	if(e->src.line)
		cprintf(ctx->out,
			"%s:%u: warning: ",
			e->src.filename ? e->src.filename : "<stdin>",
			e->src.line);
	else
		cprintf(ctx->out,
			"<lost-location!>: warning: ");
	va_start(args, fmt);
	cvprintf(ctx->out, fmt, args);
	cprintf(ctx->out, "\n");
	va_end(args);
}

void
cerror(U *ctx, Expr *e, char *fmt, ...)
{
	va_list args;
	if(e->src.line)
		cprintf(ctx->out,
			"%s:%u: ",
			e->src.filename ? e->src.filename : "<stdin>",
			e->src.line);
	else
		cprintf(ctx->out, "<lost-location!>: ");
	va_start(args, fmt);
	cvprintf(ctx->out, fmt, args);
	cprintf(ctx->out, "\n");
	va_end(args);
	longjmp(ctx->jmp, 1);
}

void
cposterror(U *ctx, Expr *e, char *fmt, ...)
{
	va_list args;
	if(e->src.line)
		cprintf(ctx->out,
			"%s:%u: ",
			e->src.filename ? e->src.filename : "<stdin>",
			e->src.line);
	else
		cprintf(ctx->out, "<lost-location!>: ");
	va_start(args, fmt);
	cvprintf(ctx->out, fmt, args);
	cprintf(ctx->out, "\n");
	va_end(args);
	ctx->errors++;
}

static Expr*
Z0(unsigned kind)
{
	return newexpr(kind, 0, 0, 0, 0);
}

static Expr*
Z1(unsigned kind, Expr *e1)
{
	return newexpr(kind, e1, 0, 0, 0);
}

static Expr*
Z2(unsigned kind, Expr *e1, Expr *e2)
{
	return newexpr(kind, e1, e2, 0, 0);
}

Expr*
Zif(Expr *cond, Expr *true)
{
	return newexpr(Eif, cond, true, 0, 0);
}

Expr*
Zifelse(Expr *cond, Expr *true, Expr *false)
{
	return newexpr(Eif, cond, true, false, 0);
}

unsigned
elistlen(Expr *l)
{
	unsigned n;
	n = 0;
	while(l->kind != Enull){
		l = l->e2;
		n++;
	}
	return n;
}

Expr*
Zcons(Expr *hd, Expr *tl)
{
	return Z2(Eelist, hd, tl);
}

Expr*
Zset(Expr *l, Expr *r)
{
	return Z2(Eg, l, r);
}

Expr*
Zret(Expr *e)
{
	return Z1(Eret, e);
}

Expr*
Zsizeof(Expr *e)
{
	return Z1(E_sizeof, e);
}

Expr*
Zxcast(Expr *type, Expr *cval)
{
	return newbinop(Excast, type, cval);
}

Expr*
Zbinop(unsigned op, Expr *x, Expr *y)
{
       return newbinop(op, x, y);
}

Expr*
Zadd(Expr *x, Expr *y)
{
	return Zbinop(Eadd, x, y);
}

Expr*
Zsub(Expr *x, Expr *y)
{
	return Zbinop(Esub, x, y);
}

Expr*
Znot(Expr *e)
{
	return Z1(Eunot, e);
}

Expr*
Zcall(Expr *fn, unsigned narg, ...)
{
	Expr *e;
	va_list args;

	va_start(args, narg);
	e = nullelist();
	while(narg-- > 0)
		e = Zcons(va_arg(args, Expr*), e);
	va_end(args);
	return Z2(Ecall, fn, invert(e));
}

Expr*
Zapply(Expr *fn, Expr *args)
{
	return Z2(Ecall, fn, args);
}

Expr*
Zbind(Expr *id, Expr *e)
{
	return Zcons(id, Zcons(e, nullelist()));
}

Expr*
Zletrec(Expr *binds, Expr *body)
{
	return Z2(Eletrec, binds, body);
}

Expr*
Zconsts(char *s)
{
	Expr *e;
	e = newexpr(Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, strlen(s));
	return e;
}

Expr*
Zuint(Imm val)
{
	return mkconst(Vuint, val);
}

Expr*
Zint(Imm val)
{
	return mkconst(Vint, val);
}

Expr*
Znil(void)
{
	return Z0(Enil);
}

Expr*
Zstr(char *s)
{
	Expr *e;
	e = newexpr(Econsts, 0, 0, 0, 0);
	e->lits = mklits(s, strlen(s));
	return e;
}

Expr*
Zcval(Expr *dom, Expr *type, Expr *val)
{
	return newexpr(E_cval, dom, type, val, 0);
}

Expr*
Zref(Expr *dom, Expr *type, Expr *val)
{
	return newexpr(E_ref, dom, type, val, 0);
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
G(char *s)
{
	Expr *e;
	if(cqctflags['r'])
		return doid(s);
	e = newexpr(Eid, 0, 0, 0, 0);
	e->id = emalloc(strlen(s)+2);
	memcpy(e->id+1, s, strlen(s));
	e->id[0] = '%';
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
Ztg(char *id, Expr *v)
{
	Expr *e;
	e = Z1(E_tg, v);
	e->id = xstrdup(id);
	return e;
}

Expr*
Ztid(char *id)
{
	Expr *e;
	e = Z0(E_tid);
	e->id = xstrdup(id);
	return e;
}

Expr*
Zlocals(unsigned n, ...)
{
	unsigned m;
	va_list args;
	Expr *l;

	l = nullelist();
	va_start(args, n);
	for(m = 0; m < n; m++)
		l = Zcons(doid(va_arg(args, char*)), l);
	va_end(args);

	return invert(l);
}

Expr*
Zlambda(Expr *args, Expr *body)
{
	return newexpr(Elambda, args, body, 0, 0);
}

Expr*
Zlambdn(Expr *args, Expr *body, Expr *name)
{
	return newexpr(Elambda, args, body, name, 0);
}

Expr*
Zvararg(Expr *id)
{
	return Zcons(id, Zcons(Z0(Eellipsis), nullelist()));
}

Expr*
Zblock(Expr *locs, ...)
{
	Expr *se, *te;
	va_list args;

	te = nullelist();
	va_start(args, locs);
	while(1){
		se = va_arg(args, Expr*);
		if(se == 0)
			break;
		te = Zcons(se, te);
	}
	return newexpr(Eblock, locs, invert(te), 0, 0);
}

Expr*
Zscope(Expr *block)
{
	if(block->kind != Eblock)
		fatal("bug");
	return Z1(Escope, block);
}

Expr*
Zids2strs(Expr *l)
{
	Expr *te;
	te = nullelist();
	while(l->kind == Eelist){
		te = Z2(Eelist, Zconsts(l->e1->id), te);
		l = l->e2;
	}
	return Zapply(G("list"), invert(te));
}

Expr*
Zkon(Val v)
{
	Expr *e;
	e = newexpr(Ekon, 0, 0, 0, 0);
	if(v == 0)
		fatal("bug");
	e->xp = v;
	return e;
}

Expr*
Zlabel(char *l)
{
	Expr *e;
	e = Z1(Elabel, Z0(Enop));
	e->id = xstrdup(l);
	return e;
}

Expr*
Zgoto(char *l)
{
	Expr *e;
	e = Z0(Egoto);
	e->id = xstrdup(l);
	return e;
}

// FIXME: fix interface: pass string, not Eid; rationalize with Zlabel
Expr*
Zlabelsrc(Src *src, Expr *id, Expr *s)
{
	Expr *e;
	e = Z1(Elabel, s);
	e->id = xstrdup(id->id);
	putsrc(e, src);
	freeexpr(id);
	return e;
}

// FIXME: fix interface: pass string, not Eid
Expr*
Zgotosrc(Src *src, Expr *id)
{
	Expr *e;
	e = Zgoto(id->id);
	freeexpr(id);
	putsrc(e, src);
	return e;
}

void
putsrc(Expr *e, Src *src)
{
	Expr *p;

	if(e == 0)
		return;

	if(e->src.line != 0)
		return;
	e->src = *src;
	switch(e->kind){
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			putsrc(p->e1, src);
			p = p->e2;
			if(p->src.line != 0)
				return;
			p->src = *src;
		}
		break;
	default:
		putsrc(e->e1, src);
		putsrc(e->e2, src);
		putsrc(e->e3, src);
		putsrc(e->e4, src);
		break;
	}
}

void
resetuniqid()
{
	cnt = 0;
}

/* FIXME:  we don't actually need to compute the name
   unless we want to see it. */
Expr*
uniqid(char *id)
{
	unsigned len;
	char *p;
	len = strlen(id)+1+10+1;
	p = emalloc(len);
	snprint(p, len, "%s.%u", id, cnt);
	if(cnt+1 < cnt)
		fatal("too many identifiers");
	cnt++;
	return doid(p);
}

int
vmember(Expr *e, Expr *l)
{
	Expr *p;

	p = l;
	while(p->kind == Eelist){
		if(p->e1->kind == Eid && !strcmp(e->id, p->e1->id))
			return 1;
		p = p->e2;
	}
	return 0;
}

Expr*
vinsert(Expr *e, Expr *vs)
{
	if(vmember(e, vs))
		return vs;
	else
		return Zcons(copyexpr(e), vs);
}

Expr*
vunion(Expr *a, Expr *b)
{
	Expr *p, *rv;

	rv = copyexpr(b);
	p = a;
	while(p->kind == Eelist){
		if(p->e1->kind == Eid && !vmember(p->e1, rv))
			rv = Zcons(copyexpr(p->e1), rv);
		p = p->e2;
	}
	return rv;
}

Expr*
vintersect(Expr *a, Expr *b)
{
	Expr *p, *rv;

	rv = nullelist();
	p = a;
	while(p->kind == Eelist){
		if(p->e1->kind == Eid && vmember(p->e1, b))
			rv = Zcons(copyexpr(p->e1), rv);
		p = p->e2;
	}
	return rv;
}

/* a - b */
Expr*
vdiff(Expr *a, Expr *b)
{
	Expr *p, *rv;

	rv = nullelist();
	p = a;
	while(p->kind == Eelist){
		if(p->e1->kind == Eid && !vmember(p->e1, b))
			rv = Zcons(copyexpr(p->e1), rv);
		p = p->e2;
	}
	return rv;
}

int
visempty(Expr *a)
{
	return a->kind == Enull;
}

