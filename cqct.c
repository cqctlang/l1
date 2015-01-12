#include "sys.h"
#include "util.h"
#include "syscqct.h"

char **cqctloadpath;
char cqctflags[256];

int
cqcteval(VM *vm, char *s, char *src, Val *rv)
{
	Val cl;
	Val codeval = cqctcstrval(s);
	cl = cqctenvlook(vm, (char *)"eval");
	if(cl == 0)
		return -1;
	if (cqctcallfn(vm, cl, 1, &codeval, rv))
		return -1;
	return 0;
}

void
cqctinterrupt(VM *vm)
{
	vm->flags |= VMirq;
}

Cbase
cqctvalcbase(Val v)
{
	Cval *cv;
	Ctype *t;
	if(!Viskind(v, Qcval))
		return (Cbase)-1;
	cv = valcval(v);
	t = chasetype(cv->type);
	switch(t->tkind){
	case Tbase:
		return typecbase(t);
	case Tptr:
		return typecbase(cv->dom->ns->base[Vptr]);
	default:
		/* only scalar types in cvalues */
		bug();
	}
}

Val
cqctmkfd(Xfd *xfd, char *name)
{
	Fd *fd;
	Str *n;

	if(name)
		n = mkstr0(name);
	else
		n = mkstr0("");
	fd = mkfdfn(n, Fread|Fwrite, xfd);
	return mkvalfd(fd);
}

char*
cqctvalcstr(Val v)
{
	if(Viskind(v, Qstr))
		return str2cstr(valstr(v));
	if(Viskind(v, Qcid))
		return str2cstr(cid2str(valcid(v)));
	return 0;
}

char*
cqctvalcstrshared(Val v)
{
	if(Viskind(v, Qstr))
		return strdata(valstr(v));
	if(Viskind(v, Qcid))
		return ciddata(valcid(v));
	return 0;
}

uint64_t
cqctvalcstrlen(Val v)
{
	Str *s;
	if(!Viskind(v, Qstr))
		return 0;
	s = valstr(v);
	return (uint64_t)s->len;
}

Val
cqctcstrval(char *s)
{
	return mkvalstr(mkstr0(s));
}

Val
cqctcstrnval(char *s, uint64_t len)
{
	return mkvalstr(mkstr(s, len));
}

Val
cqctcstrvalshared(char *s)
{
	return mkvalstr(mkstrk(s, strlen(s), Sperm));
}

Val
cqctcstrnvalshared(char *s, uint64_t len)
{
	return mkvalstr(mkstrk(s, len, Sperm));
}

void
cqctfreecstr(char *s)
{
	efree(s);
}

void
cqctenvbind(VM *vm, char *name, Val v)
{
	envbind(vm->top, name, v);
}

void
cqctenvdel(VM *vm, char *name)
{
	envdel(vm->top, name);
}

Val
cqctenvlook(VM *vm, char *name)
{
	return envget(vm->top, mkcid0(name));
}

uint64_t
cqctlength(Val v)
{
	List *lst;
	Vec *vec;
	Str *str;
	Tab *tab;

	switch(Vkind(v)){
	default:
		return (uint64_t)-1;
	case Qlist:
		lst = vallist(v);
		return listlen(lst);
	case Qstr:
		str = valstr(v);
		return str->len;
	case Qvec:
		vec = valvec(v);
		return veclen(vec);
	case Qtab:
		tab = valtab(v);
		return tab->nent;
	}
}

Val
cqctmklist(uint64_t n)
{
	return mkvallist(mklistn(n));
}

Val
cqctlistref(Val l, uint64_t idx)
{
	List *lst;
	if(!Viskind(l, Qlist))
		return 0;
	lst = vallist(l);
	if(idx >= listlen(lst))
		return 0;
	return listref(lst, idx);
}

Val
cqctlistset(Val l, uint64_t idx, Val v)
{
	List *lst;
	if(!Viskind(l, Qlist))
		return 0;
	lst = vallist(l);
	if(idx >= listlen(lst))
		return 0;
	listset(lst, idx, v);
	return l;
}

Val*
cqctlistvals(Val v)
{
	List *l;
	if(!Viskind(v, Qlist))
		return 0;
	l = vallist(v);
	return &listdata(l)[l->h];
}

Val
cqctlistappend(Val l, Val v)
{
	if(!Viskind(l, Qlist))
		return 0;
	_listappend(vallist(l), v);
	return l;
}

Val
cqcttabget(Val l, Val k) {
	 Tab *tab;
	 if (!Viskind(l, Qtab))
		   return 0;
	 tab = valtab(l);
	 return tabget(tab, k);
}

Val
cqctmkvec(uint64_t n)
{
	return mkvalvec(mkvec(n));
}

Val
cqctvecref(Val o, uint64_t idx)
{
	Vec *vec;
	if(!Viskind(o, Qvec))
		return 0;
	vec = valvec(o);
	if(idx >= veclen(vec))
		return 0;
	return vecref(vec, idx);
}

Val
cqctvecset(Val o, uint64_t idx, Val v)
{
	Vec *vec;
	if(!Viskind(o, Qvec))
		return 0;
	vec = valvec(o);
	if(idx >= veclen(vec))
		return 0;
	vecset(vec, idx, v);
	return o;
}

Val*
cqctvecvals(Val v)
{
	Vec *vec;
	if(!Viskind(v, Qvec))
		return 0;
	vec = valvec(v);
	return vecdata(vec);
}

Val
cqctmkrange(Val b, Val l)
{
	if(!Viskind(b, Qcval) || !Viskind(l, Qcval))
		return 0;
	return mkvalrange(mkrange(valcval(b), valcval(l)));
}

Val
cqctrangebeg(Val o)
{
	Range *r;
	if(!Viskind(o, Qrange))
		return 0;
	r = valrange(o);
	return mkvalcval2(r->beg);
}

Val
cqctrangelen(Val o)
{
	Range *r;
	if(!Viskind(o, Qrange))
		return 0;
	r = valrange(o);
	return mkvalcval2(r->len);
}
