#include "sys.h"
#include "util.h"
#include "syscqct.h"

u32
hashstr(Val val)
{
	Str *s;
	s = valstr(val);
	return shash(strdata(s), s->len);
}

int
equalstrc(Str *a, char *b)
{
	if(a->len != strlen(b))
		return 0;
	return memcmp(strdata(a), b, a->len) ? 0 : 1;
}

int
equalstr(Str *a, Str *b)
{
	if(a->len != b->len)
		return 0;
	return memcmp(strdata(a), strdata(b), a->len) ? 0 : 1;
}

Str*
mkstr(char *s, Imm len)
{
	Str *str;
	str = mkstrn(len);
	memcpy(strdata(str), s, len);
	return str;
}

Str*
mkstr0(char *s)
{
	Str *str;
	Imm len;
	len = strlen(s);
	str = mkstrn(len);
	memcpy(strdata(str), s, len);
	return str;
}

Str*
mkstrk(char *s, Imm len, Skind skind)
{
	Strmmap *sm;
	Strperm *sp;
	Strmalloc *sa;

	switch(skind){
	case Sheap:
		return mkstr(s, len);
	case Smalloc:
		sa = (Strmalloc*)malv(Qstr, sizeof(Strmalloc));
		sa->s = s;
		sa->str.len = len;
		sa->str.skind = Smalloc;
		quard((Val)sa);
		return (Str*)sa;
	case Smmap:
		sm = (Strmmap*)malv(Qstr, sizeof(Strmmap));
		sm->mlen = len;
		sm->s = s;
		sm->str.len = len;
		sm->str.skind = Smmap;
		quard((Val)sm);
		return (Str*)sm;
	case Sperm:
		sp = (Strperm*)malv(Qstr, sizeof(Strperm));
		sp->s = s;
		sp->str.len = len;
		sp->str.skind = Sperm;
		return (Str*)sp;
	}
	fatal("bug");
}

Str*
mkstrn(Imm len)
{
	Str *str;
	str = (Str*)malv(Qstr, sizeof(Str)+len*sizeof(char));
	str->len = len;
	str->skind = Sheap;
	return str;
}

static Str*
mkstrmalloc(Imm len)
{
	Strmalloc *sa;
	sa = (Strmalloc*)malv(Qstr, sizeof(Strmalloc));
	sa->s = emalloc(len);
	sa->str.len = len;
	sa->str.skind = Smalloc;
	return (Str*)sa;
}

static Str*
mkstrext(void *p, Imm len)
{
	Strperm *sp;
	sp = (Strperm*)malv(Qstr, sizeof(Strperm));
	sp->s = p;
	sp->str.len = len;
	sp->str.skind = Sperm;
	return (Str*)sp;
}

static void
l1_malloc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *len;
	Str *s;
	As *as;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to malloc");
	checkarg(vm, "malloc", argv, 0, Qcval);
	len = valcval(argv[0]);
	if(!isnatcval(len))
		vmerr(vm, "malloc expects a non-negative length");
	s = mkstrmalloc(len->val);
	as = mkmas(s);
	*rv = mkvalcval(mkdom(litdom->ns, as, mkstr0("malloc")),
			mkptrxtn(litdom->ns->base[Vchar],
				 litdom->ns->base[Vptr]->rep),
			(uptr)strdata(s));
}

static void
l1_memset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	unsigned char b;
	Cval *tcv, *bcv, *lcv;
	Imm lim;

	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to memset");
	s = valstrorcval(vm, "memset", argv, 0);
	checkarg(vm, "memset", argv, 1, Qcval);
	bcv = valcval(argv[1]);
	b = bcv->val&0xff;
	if(argc == 3){
		checkarg(vm, "memset", argv, 2, Qcval);
		lcv = valcval(argv[2]);
		lim = lcv->val;
	}else
		lim = s->len;
	if(Vkind(argv[0]) == Qstr)
		memset(strdata(s), b, lim);
	else if(Vkind(argv[0]) == Qcval){
		/* FIXME: we shouldn't call valstrorcval just
		   to get lim in 2-arg case */
		s = mkstrmalloc(lim);
		memset(strdata(s), b, lim);
		tcv = valcval(argv[0]);
		callput(vm, tcv->dom->as, tcv->val, lim, s);
	}else
		fatal("bug");
}

static void
l1_mkstrext(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *p, *l;
	void *a;
	Str *s;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mkstrext");
	checkarg(vm, "mkstrext", argv, 0, Qcval);
	checkarg(vm, "mkstrext", argv, 0, Qcval);
	p = valcval(argv[0]);
	l = valcval(argv[1]);
	a = (void*)(uptr)p->val;

	/* allocate before checking, in case allocation
	   modifies set of managed ranges */
	s = mkstrext(a, l->val);

	/* check for intersection with managed range */
	if(ismanagedrange(a, l->val))
		vmerr(vm, "range includes managed address space");

	*rv = mkvalstr(s);
}

static void
l1_mkstr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkstr");
	checkarg(vm, "mkstr", argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 1 to mkstr must be a non-negative integer");
	*rv = mkvalstr(mkstrn(cv->val));
}

static void
l1_strlen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to strlen");
	s = valstrorcval(vm, "strlen", argv, 0);
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], s->len);
}

static void
l1_substr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *b, *e;
	Str *s;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to substr");
	s = valstrorcval(vm, "substr", argv, 0);
	checkarg(vm, "substr", argv, 1, Qcval);
	checkarg(vm, "substr", argv, 2, Qcval);
	b = valcval(argv[1]);
	e = valcval(argv[2]);
	if(b->val > s->len)
		vmerr(vm, "substring out of bounds");
	if(e->val > s->len)
		vmerr(vm, "substring out of bounds");
	if(b->val > e->val)
		vmerr(vm, "substring out of bounds");
	*rv = mkvalstr(strslice(s, b->val, e->val));
}

void
l1_strref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *str;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to strref");
	checkarg(vm, "strref", argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to strref must be "
		      "a non-negative integer");
	str = valstrorcval(vm, "strref", argv, 0);
	if(cv->val >= str->len)
		vmerr(vm, "strref out of bounds");
	*rv = mkvallitcval(Vuchar, (u8)strdata(str)[cv->val]);
}

static char*
xmemmem(char *s1, Imm l1, char *s2, Imm l2)
{
	char *p, *e;
	if(l2 > l1)
		return 0;
	p = s1;
	e = s1+(l1-l2+1);
	while(p < e){
		if(!memcmp(p, s2, l2))
			return p;
		p++;
	}
	return 0;
}

// FIXME: the interface is broken
static void
l1_strstr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *s1, *s2, *p;
	Str *str1, *str2;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to strstr");
	str1 = valstrorcval(vm, "strstr", argv, 0);
	s1 = str2cstr(str1);
	str2 = valstrorcval(vm, "strstr", argv, 1);
	s2 = str2cstr(str2);
	if(Vkind(argv[0]) == Qcval && Vkind(argv[1]) == Qcval)
		p = strstr(s1, s2);
	else
		p = xmemmem(s1, str1->len, s2, str2->len);
	if(p)
		*rv = mkvallitcval(Vuvlong, p-s1);
	efree(s1);
	efree(s2);
}

static void
l1_strton(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Liti liti;
	Cval *cv;
	char *err;
	unsigned radix;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to strton");
	checkarg(vm, "strton", argv, 0, Qstr);
	radix = 0;
	if(argc == 2){
		checkarg(vm, "strton", argv, 1, Qcval);
		cv = valcval(argv[1]);
		if(!isnatcval(cv))
			vmerr(vm, "operand 2 to strton must be "
			      "non-negative");
		radix = cv->val;
	}

	s = valstrorcval(vm, "strton", argv, 0);
	if(!parseliti(strdata(s), s->len, &liti, radix, &err))
		*rv = mkvalcval(litdom, litdom->ns->base[liti.base], liti.val);
}

void
l1_strput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s, *t;
	Cval *off, *cv;
	Imm o;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to strput");
	if(Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to strput must be a string");
	if(Vkind(argv[1]) != Qcval)
		vmerr(vm, "operand 2 to strput must be an offset");
	if(Vkind(argv[2]) != Qstr && Vkind(argv[2]) != Qcval)
		vmerr(vm, "operand 3 to strput must be a string or character");
	s = valstr(argv[0]);
	off = valcval(argv[1]);
	o = off->val;		/* FIXME: use type */
	if(o >= s->len)
		vmerr(vm, "strput out of bounds");
	if(Vkind(argv[2]) == Qstr){
		t = valstr(argv[2]);
		if(o+t->len > s->len)
			vmerr(vm, "strput out of bounds");
		memcpy(strdata(s)+o, strdata(t), t->len);
	}else{
		cv = valcval(argv[2]);
		strdata(s)[o] = (char)cv->val;
	}
	USED(rv);
}

void
fnstr(Env *env)
{
	FN(malloc);
	FN(memset);
	FN(mkstr);
	FN(mkstrext);
	FN(strput);
	FN(strlen);
	FN(strref);
	FN(strstr);
	FN(strton);
	FN(substr);
}
