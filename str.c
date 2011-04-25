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
Strcmp(Str *s1, Str *s2)
{
	unsigned char *p1, *p2;
	int cmp;
	Imm l1, l2;

	p1 = (unsigned char*)strdata(s1);
	p2 = (unsigned char*)strdata(s2);
	l1 = s1->len;
	l2 = s2->len;
	cmp = memcmp(p1, p2, MIN(l1, l2));
	if(cmp != 0)
		return cmp;
	if(l1 == l2)
		return 0;
	else if(l1 < l2)
		return -1;
	else
		return 1;
}

int
equalstr(Str *a, Str *b)
{
	if(a->len != b->len)
		return 0;
	return Strcmp(a, b) ? 0 : 1;
}

Str*
strcopy(Str *s)
{
	return mkstr(strdata(s), s->len);
}

char*
str2cstr(Str *str)
{
	char *s;
	s = emalloc(str->len+1);
	memcpy(s, strdata(str), str->len);
	return s;
}

Str*
strslice(Str *str, Imm beg, Imm end)
{
	return mkstr(strdata(str)+beg, end-beg);
}

Str*
strconcat(Str *s1, Str *s2)
{
	Str *s;
	s = mkstrn(s1->len+s2->len);
	memcpy(strdata(s), strdata(s1), s1->len);
	memcpy(strdata(s)+s1->len, strdata(s2), s2->len);
	return s;
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
	return mkstrk(emalloc(len), len, Smalloc);
}

static void
l1__malloc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *len;
	void *p;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to malloc");
	checkarg(vm, "malloc", argv, 0, Qcval);
	len = valcval(argv[0]);
	if(!isnatcval(len))
		vmerr(vm, "malloc expects a non-negative length");
	p = malloc(len->val);

	/* FIXME: might be nice for ns to have cached void* */
	*rv = mkvalcval(litdom,
			mkptrxtn(mkvoidxtn(),
				 litdom->ns->base[Vptr]->rep),
			(uptr)p);
}

static void
l1__free(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *p;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to malloc");
	checkarg(vm, "malloc", argv, 0, Qcval);
	p = valcval(argv[0]);
	free((void*)(uptr)p->val);
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
	FN(_free);
	FN(_malloc);
	FN(memset);
	FN(mkstr);
	FN(strput);
	FN(strlen);
	FN(strref);
	FN(strstr);
	FN(strton);
	FN(substr);
}
