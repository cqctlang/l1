#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "x/include/lib9.h"
#include "x/include/libsec.h"

static void
l1_sha1(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	unsigned char md[SHA1dlen];
	unsigned char omd[2*SHA1dlen];
	unsigned i;
	static char x[]="0123456780abcdef";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to sha1");
	checkarg(vm, "sha1", argv, 0, Qstr);
	s = valstr(argv[0]);
	sha1((unsigned char*)strdata(s), (unsigned long)s->len, md, 0);
	for(i = 0; i < SHA1dlen; i++){
		omd[i*2] = x[md[i]>>4];
		omd[i*2+1] = x[md[i]&0xf];
	}
	*rv = mkvalstr(mkstr((char*)omd, 2*SHA1dlen));
}

void
fnsec(Env *env)
{
	FN(sha1);
}
