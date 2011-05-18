#include "sys.h"
#include "util.h"
#include "syscqct.h"

Val syms;

Cid*
mkcidstr(Str *str)
{
	char *s;
	Cid *id;
	Imm len, m;
	Val v;

	v = tabget(valtab(syms), mkvalstr(str));
	if(v)
		return valcid(v);
	s = strdata(str);
	len = str->len;
	if(s[len-1] == '\0')
		m = len;
	else
		m = len+1;
	id = (Cid*)malv(Qcid, sizeof(Cid)+m*sizeof(char));
	id->len = m;
	memcpy(ciddata(id), s, len);
	tabput(valtab(syms), mkvalstr(str), mkvalcid(id));
	return id;
}

Cid*
mkcid(char *s, Imm len)
{
	Str *str;
	str = mkstr(s, len);
	return mkcidstr(str);
}

Cid*
mkcid0(char *s)
{
	Str *str;
	str = mkstr0(s);
	return mkcidstr(str);
}

static void
l1_mkcid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkcid");
	checkarg(vm, "mkcid", argv, 0, Qstr);
	*rv = mkvalcid(mkcidstr(valstr(argv[0])));
}

void
fncid(Env *env)
{
	FN(mkcid);
}

void
initcid()
{
	syms = mkvaltab(mktab());
}

void
finicid()
{
	syms = Xnil;
}
