#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "udis/udis86.h"

static void
udprint(ud_t *ud)
{
	xprintf("%016" PRIx64 " %-32s %s\n",
		ud_insn_off(ud),
		ud_insn_hex(ud),
		ud_insn_asm(ud));
}

void
disx86(unsigned char *s, unsigned char *e, unsigned inc)
{
	ud_t ud;
	unsigned i, m;
	unsigned char *p;

	ud_init(&ud);
	ud_set_mode(&ud, 64);
	ud_set_pc(&ud, 0);
	ud_set_syntax(&ud, UD_SYN_INTEL);

	if(inc)
		while(e > 0){
			ud_set_input_buffer(&ud, s, 16);
			if(!ud_disassemble(&ud))
				break;
			udprint(&ud);
			s += ud_insn_len(&ud);
			e--;
		}
	else if(e == 0)
		while(1){
			ud_set_input_buffer(&ud, s, 16);
			if(!ud_disassemble(&ud))
				break;
			udprint(&ud);
			m = ud_insn_len(&ud);
			s += m;
			if(ud.mnemonic == UD_Iret)
				break;
			p = ud_insn_ptr(&ud);
			for(i = 0; i < m; i++)
				if(p[i] != 0)
					break;
			if(i >= m)
				break;
		}
	else{
		ud_set_input_buffer(&ud, s, e-s);
		while(ud_disassemble(&ud))
			udprint(&ud);
	}
}

static void
l1_disx86(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Cval *p, *m;
	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to dis");
	if(argc == 1){
		checkarg(vm, argv, 0, Qstr);
		s = valstr(argv[0]);
	}else if(argc == 2){
		checkarg(vm, argv, 0, Qcval);
		checkarg(vm, argv, 1, Qcval);
		p = valcval(argv[0]);
		m = valcval(argv[1]);
		s = getbytes(vm, p, cvalu(m));
	}
	disx86((unsigned char*)strdata(s),
	       (unsigned char*)strdata(s)+s->len, 0);
}

void
fndis(Env env)
{
	FN(disx86);
}
