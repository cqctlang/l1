#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include <openssl/sha.h>

static void
l1_sha1(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	unsigned char md[SHA_DIGEST_LENGTH];
	unsigned char omd[2*SHA_DIGEST_LENGTH];
	unsigned i;
	static char x[]="0123456780abcdef";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to sha1");
	checkarg(vm, "sha1", argv, 0, Qstr);
	s = valstr(argv[0]);
	if(0 == SHA1((unsigned char*)s->s, (unsigned long)s->len, md))
		/* SHA1's diagnostics are not documented */
		vmerr(vm, "sha1 internal error?");
	for(i = 0; i < SHA_DIGEST_LENGTH; i++){
		omd[i*2] = x[md[i]>>4];
		omd[i*2+1] = x[md[i]&0xf];
	}
	*rv = mkvalstr(mkstr((char*)omd, 2*SHA_DIGEST_LENGTH));
}

void
fnsec(Env *env)
{
	FN(sha1);
}
