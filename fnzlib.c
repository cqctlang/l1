#include "sys.h"
#include "util.h"
#include "syscqct.h"

#include <zlib.h>

static void
l1_zinflate(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *p, *len;
	z_stream z;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to zinflate");
	checkarg(vm, "zinflate", argv, 0, Qcval);
	checkarg(vm, "zinflate", argv, 1, Qcval);
	p = valcval(argv[0]);
	len = valcval(argv[1]);

	memset(&z, 0, sizeof(z));
	z.next_out = 

	switch(inflateInit(&z)){
	case Z_OK:
		break;
	case Z_MEM_ERROR:
		vmerr(vm, "unrecoverable zlib memory error");
	case Z_VERSION_ERROR:
		vmerr(vm, "unrecoverable zlib version error");
	default:
		vmerr(vm, "unrecoverable, unknown zlib error");
	}

	
}

void
fnzlib(Env *env)
{
}
