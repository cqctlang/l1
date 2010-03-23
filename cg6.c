#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum
{
	MaxInsn6 = 15,
	RexW = 0x48,
};

typedef
enum Reg
{
	Rrax = 0,
	Rrcx,
	Rrdx,
	Rrbx,
	Rrsp,
	Rrbp,
	Rrsi,
	Rrdi,
	Rr8,
	Rr9,
	Rr10,
	Rr11,
	Rr12,
	Rr13,
	Rr14,
	Rr15,
} Reg;

static char*
nextx(Code *code)
{
again:
	if(code->nx+MaxInsn6 >= code->maxx){
		code->x = erealloc(code->x, code->maxx, 2*code->maxx);
		code->maxx *= 2;
		goto again;
	}
	return code->x+code->nx;
}

static void
movm2rax(Code *c, void *addr)
{
	char *x;
	u64 *p;
	x = nextx(c);

	x[0] = RexW;     // mov m64,%rax
	x[1] = 0xa1;
	p = (u64*)&x[2];
	*p = (u64)addr;
	c->nx += 10;
}

static void
movr2r(Code *c, Reg src, Reg dst)
{
	u8 modrm;
	char *x;
	
	if(src == dst)
		return;

	if(src >= Rr8 || dst >= Rr8)
		fatal("unsupported operand");
	
	x = nextx(c);
	x[0] = RexW;                // mov r/m,r
	x[1] = 0x8b;
	x[2] = (3<<6)|(dst<<6)|src;
	c->nx += 3;
}

static void
getval(Code *c, Location *l, Reg reg)
{
	Val p;

	switch(LOCKIND(loc->loc)){
	case Lreg:
		switch(LOCIDX(loc->loc)){
		case Rac:
			fatal("unimplemented");
		case Rsp:
		case Rfp:
		case Rpc:
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		fatal("unimplemented");
	case Llocal:
		fatal("unimplemented");
	case Ldisp:
		fatal("unimplemented");
	case Ltopl:
		p = *(loc->var->val);
		if(Vkind(p) == Qundef)
			vmerr(vm, "reference to unbound variable: %s",
			      loc->var->id);
		return valcval(p);
	default:
		fatal("bug");
	}
	return 0; /* not reached */
	
}

static void
getvalrand(Code *c, Operand *r, Reg reg)
{
	Val v;
	switch(r->okind){
	case Oloc:
		
		break;
	case Okon:
		v = r->u.kon;
		movm2rax(v, Rax);
		movr2r(Rax, reg);
		/* FIXME: copy string constants in case they are mutated */
		break;
	case Onil:
		fatal("unimplemented");
		break;
	default:
		fatal("bug");
	}
}

static void
insn6(Code *c, Insn *i)
{
	switch(i->kind){
	case Inop:
		break;
	case Iargc:
		break;
	case Imov:
		getvalrand(c, &i->op1, Rax);
		putvalrand(c, Rax, &i->dst);
		break;
	case Ispec:
	case Icallc:
	case Iinv:
	case Ineg:
	case Inot:
	case Iadd:
	case Iand:
	case Idiv:
	case Imod:
	case Imul:
	case Ior:
	case Ishl:
	case Ishr:
	case Isub:
	case Ixor:
	case Icmplt:
	case Icmple:
	case Icmpgt:
	case Icmpge:
	case Icmpeq:
	case Icmpneq:
	case Isubsp:
	case Imov:
	case Ipush:
	case Ipushi:
	case Ivargc:
	case Icall:
	case Icallt:
	case Iframe:
	case Ipanic:
	case Ihalt:
	case Iret:
	case Ijmp:
	case Ijnz:
	case Ijz:
	case Iclo:
	case Ikg:
	case Ikp:
	case Ibox:
	case Ibox0:
	case Icval:
	case Iref:
	case Ixcast:
	case Ilist:
	case Isizeof:
		printinsn(i);
		fatal("insn6: unimplemented instruction");
	default:
		fatal("insn6: unrecognized insn %d", i->kind);
	}

}

void
cg6(Code *c)
{
	unsigned i;

	for(i = 0; i < c->ninsn; i++)
		insn6(c, &c->insn[i]);
	xprintf("disasm 0x%p 0x%p\n", c->x, c->x+c->nx);
}
