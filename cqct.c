#include "sys.h"
#include "util.h"
#include "syscqct.h"

char **cqctloadpath;
char cqctflags[256];

void
cqctgcenable(VM *vm)
{
	gcenable();
}

void
cqctgcdisable(VM *vm)
{
	gcdisable();
}

void
cqctinterrupt(VM *vm)
{
	if(vm->flags&VMirq)
		fatal("hard interrupt");
	vm->flags |= VMirq;
}


Cbase
cqctvalcbase(Val v)
{
	Cval *cv;
	Ctype *t;
	if(Vkind(v) != Qcval)
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
	if(Vkind(v) != Qstr)
		return 0;
	return str2cstr(valstr(v));
}

char*
cqctvalcstrshared(Val v)
{
	Str *s;
	if(Vkind(v) != Qstr)
		return 0;
	s = valstr(v);
	return strdata(s);
}

uint64_t
cqctvalcstrlen(Val v)
{
	Str *s;
	if(Vkind(v) != Qstr)
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
cqctenvbind(Env *env, char *name, Val v)
{
	envbind(env, name, v);
}

Val
cqctenvlook(Env *env, char *name)
{
	return envget(env, mkcid0(name));
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
	if(Vkind(l) != Qlist)
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
	if(Vkind(l) != Qlist)
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
	if(Vkind(v) != Qlist)
		return 0;
	l = vallist(v);
	return &listdata(l)[l->h];
}

Val
cqctlistappend(Val l, Val v)
{
	if(Vkind(l) != Qlist)
		return 0;
	_listappend(vallist(l), v);
	return l;
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
	if(Vkind(o) != Qvec)
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
	if(Vkind(o) != Qvec)
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
	if(Vkind(v) != Qvec)
		return 0;
	vec = valvec(v);
	return vecdata(vec);
}

Val
cqctmkrange(Val b, Val l)
{
	if(Vkind(b) != Qcval || Vkind(l) != Qcval)
		return 0;
	return mkvalrange(mkrange(valcval(b), valcval(l)));
}

Val
cqctrangebeg(Val o)
{
	Range *r;
	if(Vkind(o) != Qrange)
		return 0;
	r = valrange(o);
	return mkvalcval2(r->beg);
}

Val
cqctrangelen(Val o)
{
	Range *r;
	if(Vkind(o) != Qrange)
		return 0;
	r = valrange(o);
	return mkvalcval2(r->len);
}

Env*
cqctinit(char **lp, Xfd *in, Xfd *out, Xfd *err)
{
	Xfd xfd[3];

	cqctloadpath = copystrv(lp);
	if(in == 0){
		in = &xfd[0];
		memset(in, 0, sizeof(Xfd));
		in->read = xfdread;
		in->fd = 0;
	}
	if(out == 0){
		out = &xfd[1];
		memset(out, 0, sizeof(Xfd));
		out->write = xfdwrite;
		out->fd = 1;
	}
	if(err == 0){
		err = &xfd[2];
		memset(err, 0, sizeof(Xfd));
		err->write = xfdwrite;
		err->fd = 2;
	}
	initio(in, out, err);
	initmem();
	initparse();
	initcid();
	inittype();
	initcg();
	initnc();
	initvm();
	return mktoplevel();
}

void
cqctfini(Env *top)
{
	efree(cqctloadpath);
	freeenv(top);
	finivm();
	finicg();
	finitype();
	finicid();
	finiparse();
	finimem();
	finiio();
}
