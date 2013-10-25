#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "amd64.h"
#include <assert.h>

xRand NONE = { .okind = opNone };
xRand RNONE = { .okind = opReg, .u.r = rNone };
xImm DNONE = { .size = Zero };
/* FIXME: steal that trick from luajit */
#define defreg(R) xRand R = { .okind = opReg, .u.r = r##R }
defreg(RAX);
defreg(RCX);
defreg(RDX);
defreg(RBX);
defreg(RSP);
defreg(RBP);
defreg(RSI);
defreg(RDI);
defreg(R8);
defreg(R9);
defreg(R10);
defreg(R11);
defreg(R12);
defreg(R13);
defreg(R14);
defreg(R15);
defreg(EAX);
defreg(ECX);
defreg(EDX);
defreg(EBX);
defreg(ESP);
defreg(EBP);
defreg(ESI);
defreg(EDI);
defreg(AX);
defreg(CX);
defreg(DX);
defreg(BX);
defreg(SP);
defreg(BP);
defreg(SI);
defreg(DI);
defreg(AL);
defreg(CL);
defreg(DL);
defreg(BL);
defreg(SPL);
defreg(BPL);
defreg(SIL);
defreg(DIL);
defreg(AH);
defreg(CH);
defreg(DH);
defreg(BH);
#undef defreg

static void
n1(NC *nc, xImm imm)
{
	emitu8(nc, imm.v.sbyte);
}

static void
n2(NC *nc, xImm imm)
{
	emitu16(nc, imm.v.sword);
}

static void
n4(NC *nc, xImm imm)
{
	emitu32(nc, imm.v.sdword);
}

static void
n8(NC *nc, xImm imm)
{
	emitu64(nc, imm.v.sqword);
}

#if 0
static void
emituptr(NC *nc, uptr w)
{
	nccap(nc, sizeof(w));
	*(uptr*)nc->p = w;
	nc->p += sizeof(w);
	nc->n += sizeof(w);
}

static void
nuptr(NC *nc, xImm imm)
{
	switch(imm.size) {
	case Byte:
		emituptr(nc, imm.v.ubyte);
		break;
	case Word:
		emituptr(nc, imm.v.uword);
		break;
	case DWord:
		emituptr(nc, imm.v.udword);
		break;
	case QWord:
		emituptr(nc, imm.v.uqword);
		break;
	case Zero:
	default:
		bug();
	}
}
#endif

xRand
immu64(u64 u)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = QWord;
	randimm(x).v.uqword = u;
	return x;
}

xRand
immu32(u32 u)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = DWord;
	randimm(x).v.udword = u;
	return x;
}

xRand
immu16(u16 u)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = Word;
	randimm(x).v.uword = u;
	return x;
}

xRand
immu8(u8 u)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = Byte;
	randimm(x).v.ubyte = u;
	return x;
}

xRand
imms64(s64 s)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = QWord;
	randimm(x).v.sqword = s;
	return x;
}

xRand
imms32(s32 s)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = DWord;
	randimm(x).v.sdword = s;
	return x;
}

xRand
imms16(s16 s)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = Word;
	randimm(x).v.sword = s;
	return x;
}

xRand
imms8(s8 s)
{
	xRand x;
	memset(&x, 0, sizeof(x));
	x.okind = opImm;
	randimm(x).size = Byte;
	randimm(x).v.sbyte = s;
	return x;
}

xRand
imm(u64 u)
{
	if(u > 0xffffffff)
		bug();
	return immu32(u);
}

xRand
byte(u64 u)
{
	if(u > 0xff)
		bug();
	return immu8(u);
}

static xImm
dispi(s32 d)
{
	xImm x;
	memset(&x, 0, sizeof(x));
	/* FIXME: check range */
	x.size = DWord;
	x.v.sdword = d;
	return x;
}

xRand
disp(s32 d)
{
	return imms32(d);
}

static int
isbyte(xImm imm)
{
	return imm.size == Byte;
}

static int
fitsbyte(xImm imm)
{
	return (imm.size == Byte
		|| (-128 <= imm.v.sqword && imm.v.sqword <= 127));
}

xRand
mkindirect(xRand b, xRand i, u8 s, s32 d)
{
	xRand x;

	/* express "none" with RNONE, not NONE */
	assert(isreg(b)); assert(isreg(i));

	x.okind = opMem;
	randbase(x) = randreg(b);
	randidx(x) = randreg(i);
	randscale(x) = s;
	randdisp(x) = dispi(d);

	/* canonicalize displacement */
	if(!isrnone(randbase(x)) && d == 0)
		randdisp(x) = DNONE;

	/* canonicalize EBP */
	if(!isrnone(randbase(x))
	   && isdnone(randdisp(x))
	   && (randreg(b) == rEBP
	       || randreg(b) == rRBP
	       || randreg(b) == rR13))
		randdisp(x) = dispi(0);

	/* check ESP */
	if(randreg(i) == rESP || randreg(i) == rRSP)
		bug();

	return x;
}

xRand
indirecti(xRand b, xRand i, s32 d)
{
	return mkindirect(b, i, 0, d);
}

xRand
indirect2i(xRand b, xRand i, s32 d)
{
	return mkindirect(b, i, 1, d);
}

xRand
indirect4i(xRand b, xRand i, s32 d)
{
	return mkindirect(b, i, 2, d);
}

xRand
indirect8i(xRand b, xRand i, s32 d)
{
	return mkindirect(b, i, 3, d);
}

xRand
indirectrip(s32 d)
{
	return mkindirect(RNONE, RNONE, 0, d);
}

xRand
indirect2(xRand b, xRand o)
{
	/* asm.cqct permits o to be displacement
	   (probably something like
	   mkindirect(b, RNONE, 0, o) */
	assert(isreg(b));
	switch(o.okind) {
	case opReg:
		return mkindirect(b, o, 0, 0);
	case opImm:
		return mkindirect(b, RNONE, 0, randimm(o).v.sdword);
	case opMem:
	default:
		bug();
	}
}

xRand
indirect1(xRand o)
{
	if(isimm(o))
		/* FIXME: this is for 64-bit.
		   see odd dependency in asm.cqct.
		   this case seems unused anyway. */
		return mkindirect(RNONE, RNONE, 0, randimm(o).v.sdword);
	else if(isreg(o))
		return mkindirect(o, RNONE, 0, 0);
	else
		bug();
}

static xOp
mkop(u8 bits)
{
	xOp op;
	op.extended = 0;
	op.op0 = bits;
	op.op1 = 0; /* unused */
	return op;
}

static xOp
mkxop(u8 bits)
{
	xOp op;
	op.extended = 1;
	op.op0 = 017;
	op.op1 = bits;
	return op;
}

static xOp
opcodeor(xOp in, u8 bits)
{
	xOp op;
	op = in;
	if(op.extended)
		op.op0 |= bits;	/* FIXME! this seems wrong.  should it be op1?
				   i.e., the actual opcode!?!
				   if so, fix in cqct as well. */
	else
		op.op0 |= bits;
	return op;
}

static int
isreg8(xReg r)
{
	return r >= rAL && r <= rBH;
}

static int
isreg8x(xReg r)
{
	return r >= rSPL && r <= rDIL;
}

static int
isreg16(xReg r)
{
	return r >= rAX && r < rDI;
}

static int
isreg32(xReg r)
{
	return r >= rEAX && r < rEDI;
}

static int
isreg64(xReg r)
{
	return r >= rRAX && r <= rR15;
}

static int
isrand64(xRand rand)
{
	if(isreg(rand))
		return isreg64(randreg(rand));
	else if(ismem(rand))
		return isreg64(randbase(rand)) || isreg64(randidx(rand));
	else
		return 0;
}

static int
_regbits(xReg r)
{
	if(isreg64(r))
		return r-rRAX;
	else if(isreg32(r))
		return r-rEAX;
	else if(isreg16(r))
		return r-rAX;
	else if(isreg8(r))
		return r-rAL;
	else if(isreg8x(r))
		return (r-rSPL)+4;
	else if(r == rNone)
		bug();
	/* FIXME: broken on high reg8 */
	bug();
}

static int
regbits(xReg r)
{
	return _regbits(r)&0x7;
}

static int
isxreg(xReg r)
{
	return isreg64(r) && _regbits(r) > 7;
}

static int
isxrand(xRand o)
{
	if(isreg(o))
		return isxreg(randreg(o));
	else if(ismem(o))
		return isxreg(randbase(o));
	else
		return 0;
}

static int
isxrand8(xRand o)
{
	if(isreg(o))
		switch(randreg(o)) {
		case rSPL:
		case rBPL:
		case rSIL:
		case rDIL:
			return 1;
		default:
			return 0;
		}
	return 0;
}

static void
emitopcode(NC *nc, xOp op)
{
	emitu8(nc, op.op0);
	if(op.extended)
		emitu8(nc, op.op1);
}

static int
sibpresent(xRand rand)
{
	switch(rand.okind) {
	case opMem:
		if(!isrnone(randidx(rand)) || randscale(rand) != 0)
			return 1;
		switch(randbase(rand)) {
		case rESP:
		case rRSP:
		case rR12:
			return 1;
		default:
			return 0;
		}
	case opReg:
		return 0;
	case opNone:
	case opImm:
	default:
		bug();
		return 0;
	}

}

static void
emitmodrm(NC *nc, u8 rbits, xRand rand)
{
	xImm d;
	u8 b, m, rm;

	/* reg */
	b = (rbits&0x7)<<3;

	/* mod */
	switch(rand.okind) {
	case opReg:
		m = 0x3;
		break;
	case opMem:
		if(!isrnone(randbase(rand))) {
			d = randdisp(rand);
			if(d.size == Zero)
				m = 0x0;
			else if(fitsbyte(d))
				m = 0x1;
			else
				m = 0x2;
		}else
			m = 0x0;
		break;
	case opImm:
	case opNone:
	default:
		bug();
	}
	b |= m<<6;

	/* r/m */
	switch(rand.okind) {
	case opMem:
		if(sibpresent(rand))
			rm = regbits(rRSP);
		else
			rm = (isrnone(randbase(rand))
			      ? regbits(rEBP) : regbits(randbase(rand)));
		break;
	case opReg:
		rm = regbits(randreg(rand));
		break;
	case opImm:
	case opNone:
	default:
		bug();
	}
	b |= rm;

	emitu8(nc, b);
}

static void
emitsib(NC *nc, xRand rand)
{
	u8 s, i, b;
	if(sibpresent(rand)) {
		s = randscale(rand);
		i = (isrnone(randidx(rand))
		     ? regbits(rESP) : regbits(randidx(rand)));
		b = (isrnone(randbase(rand))
		     ? regbits(rEBP) : regbits(randbase(rand)));
		emitu8(nc, s<<6|i<<3|b);
	}
}

static void
emitdisp(NC *nc, xRand rand)
{
	xImm imm;
	switch(rand.okind) {
	case opMem:
		imm = randdisp(rand);
		if(imm.size == Zero)
			return;
		if(!isrnone(randbase(rand)) && fitsbyte(imm))
			n1(nc, imm);
		else
			n4(nc, imm);
		break;
	case opReg:
		return;
	case opNone:
	case opImm:
		bug();
	}
}

static void
emitaddressing(NC *nc, u8 rbits, xRand rand)
{
	emitmodrm(nc, rbits, rand);
	emitsib(nc, rand);
	emitdisp(nc, rand);
}

static void
prefix16(NC *nc, xRand rand)
{
	if(isreg(rand) && isreg16(randreg(rand)))
		emitu8(nc, 0x66);
}

static void
prefixrex(NC *nc, xRand reg, xRand rand, u8 w)
{
	u8 rex;

	rex = REX;
	if(w && ((isnone(reg) && isrand64(rand)) || isrand64(reg)))
		/* FIXME: handler register-128 case */
		rex |= REXW;
	if(isxrand(reg))
		rex |= REXR;
	if(isxrand(rand))
		rex |= REXB;
	if(ismem(rand) && isxreg(randidx(rand)))
		rex |= REXX;

	/* FIXME: handle extended 8-bit register case */
	if(rex == REX && !isxrand8(reg) && !isxrand8(rand))
		; /* no rex prefix */
	else
		emitu8(nc, rex);
}

static void
shortop(NC *nc, xRand dst, u8 n, u8 w)
{
	assert(isreg(dst));
	prefix16(nc, dst);
	prefixrex(nc, NONE, dst, w);
	emitu8(nc, regbits(randreg(dst))+n);
}

static void
op1(NC *nc, xRand dst, u8 rbits, xOp op, u8 w)
{
	prefix16(nc, dst);
	prefixrex(nc, NONE, dst, w);
	emitopcode(nc, op);
	emitaddressing(nc, rbits, dst);
}

static void
imm1x(NC *nc, xRand dst, xImm imm, u8 rbits, xOp op, u8 w)
{
	op1(nc, dst, rbits, op, w);
	n1(nc, imm);
}

static void
imm1(NC *nc, xRand dst, xImm imm, u8 rbits, xOp op, u8 w)
{
	if(isbyte(imm))
		imm1x(nc, dst, imm, rbits, op, w);
	else
		imm1x(nc, dst, imm, rbits, opcodeor(op, 1), w);
}

static void
imm4(NC *nc, xRand dst, xImm imm, u8 rbits, xOp op, u8 w)
{
	op1(nc, dst, rbits, opcodeor(op, 1), w);
	n4(nc, imm);
}

static void
imm14(NC *nc, xRand dst, xImm imm, u8 rbits, xOp op, u8 w)
{
	if(fitsbyte(imm))
		if(isbyte(imm))
			return imm1(nc, dst, imm, rbits, op, w);
		else
			return imm1(nc, dst, imm, rbits, opcodeor(op, 0x2), w);
	else
		return imm4(nc, dst, imm, rbits, op, w);
}

static void
op2x(NC *nc, xOp op, xRand dst, xRand src, u8 w)
{
	prefixrex(nc, dst, src, w);
	emitopcode(nc, op);
	emitaddressing(nc, regbits(randreg(dst)), src);
}

static void
op2(NC *nc, xOp op, xRand dst, xRand src, u8 w)
{
	xRand t;

	/* pick a direction */
	if(isreg(dst) && !isreg(src))
		op = opcodeor(op, 0x2);
	else{
		/* swapping looks odd, but these are really
		   positions in modrm byte */
		t = dst;
		dst = src;
		src = t;
	}

	/* pick a size */
	if(!isreg(dst) || !(isreg8(randreg(dst)) || isreg8x(randreg(dst))))
		op = opcodeor(op, 0x1);

	prefix16(nc, dst);
	op2x(nc, op, dst, src, w);
}

static void
movi(NC *nc, xRand dst, xImm srci)
{
	switch(dst.okind) {
	case opReg:
		if(isbyte(srci)) {
			shortop(nc, dst, 0xb0, REXW);
			n1(nc, srci);
		}else{
			shortop(nc, dst, 0xb8, REXW);
			if(isreg64(randreg(dst)))
				n8(nc, srci);
			else if(isreg32(randreg(dst)))
				n4(nc, srci);
			else if(isreg16(randreg(dst)))
				n2(nc, srci);
			else
				bug();
		}
		break;
	case opMem:
		if(isbyte(srci))
			imm1(nc, dst, srci, 000, mkop(0xc6), REXW);
		else
			imm4(nc, dst, srci, 000, mkop(0xc6), REXW);
		break;
	case opImm:
	case opNone:
		bug();
	}
}

void
MOV(NC *nc, xRand dst, xRand src)
{
	if(isreg(src) || ismem(src))
		op2(nc, mkop(0x88), dst, src, REXW);
	else
		movi(nc, dst, randimm(src));
}

void
ADD(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 000, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(000), dst, src, REXW);
	else
		bug();
}

void
OR(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 001, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(010), dst, src, REXW);
	else
		bug();
}

void
ADC(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 002, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(020), dst, src, REXW);
	else
		bug();
}

void
SBB(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 003, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(030), dst, src, REXW);
	else
		bug();
}

void
AND(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 004, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(040), dst, src, REXW);
	else
		bug();
}

void
SUB(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 005, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(050), dst, src, REXW);
	else
		bug();
}

void
XOR(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 006, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(060), dst, src, REXW);
	else
		bug();
}

void
CMP(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm14(nc, dst, randimm(src), 007, mkop(0x80), REXW);
	else if(isreg(src) || ismem(src))
		return op2(nc, mkop(070), dst, src, REXW);
	else
		bug();
}

void
TEST(NC *nc, xRand dst, xRand src)
{
	if(isimm(src))
		return imm4(nc, dst, randimm(src), 000, mkop(0xf7), REXW);
	else if(isreg(src))
		return op2(nc, mkop(0204), dst, src, REXW);
	else if(isreg(dst) && ismem(src))
		/* FIXME: sane? (o.w. there is no insn form) */
		return op2(nc, mkop(0204), src, dst, REXW);
	else
		bug();
}

void
LEA(NC *nc, xRand dst, xRand src)
{
	return op2(nc, mkop(0x8d), src, dst, REXW);
}

void
JMP(NC *nc, xRand dst)
{
	if(isimm(dst)) {
		emitu8(nc, 0xe9);
		n4(nc, randimm(dst));
	}else
		op1(nc, dst, 004, mkop(0xff), REXW);
}

void
CALL(NC *nc, xRand dst)
{
	if(isimm(dst)) {
		emitu8(nc, 0xe8);
		n4(nc, randimm(dst));
	}else
		op1(nc, dst, 002, mkop(0xff), REXW);
}

static void
jcc(NC *nc, u8 op, xRand dst)
{
	if(!isimm(dst))
		bug();
	emitopcode(nc, mkxop(op));
	n4(nc, randimm(dst));
}

void	JO(NC *nc, xRand dst)	{ jcc(nc, 0x80, dst); }
void	JNO(NC *nc, xRand dst)	{ jcc(nc, 0x81, dst); }
void	JB(NC *nc, xRand dst)	{ jcc(nc, 0x82, dst); }
void	JAE(NC *nc, xRand dst)	{ jcc(nc, 0x83, dst); }
void	JE(NC *nc, xRand dst)	{ jcc(nc, 0x84, dst); }
void	JZ(NC *nc, xRand dst)	{ JE(nc, dst); }
void	JNE(NC *nc, xRand dst)	{ jcc(nc, 0x85, dst); }
void	JNZ(NC *nc, xRand dst)	{ JNE(nc, dst); }
void	JBE(NC *nc, xRand dst)	{ jcc(nc, 0x86, dst); }
void	JA(NC *nc, xRand dst)	{ jcc(nc, 0x87, dst); }
void	JS(NC *nc, xRand dst)	{ jcc(nc, 0x88, dst); }
void	JNS(NC *nc, xRand dst)	{ jcc(nc, 0x89, dst); }
void	JP(NC *nc, xRand dst)	{ jcc(nc, 0x8a, dst); }
void	JNP(NC *nc, xRand dst)	{ jcc(nc, 0x8b, dst); }
void	JL(NC *nc, xRand dst)	{ jcc(nc, 0x8c, dst); }
void	JGE(NC *nc, xRand dst)	{ jcc(nc, 0x8d, dst); }
void	JLE(NC *nc, xRand dst)	{ jcc(nc, 0x8e, dst); }
void	JG(NC *nc, xRand dst)	{ jcc(nc, 0x8f, dst); }

static void
movcc(NC *nc, u8 op, xRand dst, xRand src)
{
	op2x(nc, mkxop(op), dst, src, REXW);
}

void	CMOVO(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x40, dst, src); }
void	CMOVNO(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x41, dst, src); }
void	CMOVB(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x42, dst, src); }
void	CMOVAE(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x43, dst, src); }
void	CMOVE(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x44, dst, src); }
void	CMOVZ(NC *nc, xRand dst, xRand src)	{ CMOVE(nc, dst, src); }
void	CMOVNE(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x45, dst, src); }
void	CMOVNZ(NC *nc, xRand dst, xRand src)	{ CMOVNE(nc, dst, src); }
void	CMOVBE(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x46, dst, src); }
void	CMOVA(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x47, dst, src); }
void	CMOVS(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x48, dst, src); }
void	CMOVNS(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x49, dst, src); }
void	CMOVP(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x4a, dst, src); }
void	CMOVNP(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x4b, dst, src); }
void	CMOVL(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x4c, dst, src); }
void	CMOVGE(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x4d, dst, src); }
void	CMOVLE(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x4e, dst, src); }
void	CMOVG(NC *nc, xRand dst, xRand src)	{ movcc(nc, 0x4f, dst, src); }

void
PUSH(NC *nc, xRand o)
{
	switch(o.okind) {
	case opReg:
		shortop(nc, o, 0x50, 0);
		break;
	case opMem:
		op1(nc, o, 006, mkop(0xff), 0);
		break;
	case opImm:
		emitu8(nc, 0x68);
		n4(nc, randimm(o));
		break;
	default:
		bug();
	}
}

void
POP(NC *nc, xRand o)
{
	switch(o.okind) {
	case opReg:
		shortop(nc, o, 0x58, 0);
		break;
	case opMem:
		op1(nc, o, 000, mkop(0x8f), 0);
		break;
	case opImm:
	default:
		bug();
	}
}

void
PUSHAD(NC *nc)
{
	emitu8(nc, 0x60);
}

void
POPAD(NC *nc)
{
	emitu8(nc, 0x61);
}

static void
shift(NC *nc, u8 obits, xRand dst, xRand src)
{
	if(isreg(src) && randreg(src) == rCL)
		return op1(nc, dst, obits, mkop(0xd3), REXW);
	else if(isimm(src))
		return imm1(nc, dst, randimm(src), obits, mkop(0xc0), REXW);
}

void	ROL(NC *nc, xRand dst, xRand n)		{ shift(nc, 000, dst, n); }
void	ROR(NC *nc, xRand dst, xRand n)		{ shift(nc, 001, dst, n); }
void	RCL(NC *nc, xRand dst, xRand n)		{ shift(nc, 002, dst, n); }
void	RCR(NC *nc, xRand dst, xRand n)		{ shift(nc, 003, dst, n); }
void	SHL(NC *nc, xRand dst, xRand n)		{ shift(nc, 004, dst, n); }
void	SHR(NC *nc, xRand dst, xRand n)		{ shift(nc, 005, dst, n); }
void	SAR(NC *nc, xRand dst, xRand n)		{ shift(nc, 007, dst, n); }

void
NOT(NC *nc, xRand dst)
{
	op1(nc, dst, 002, mkop(0xf7), REXW);
}

void
NEG(NC *nc, xRand dst)
{
	op1(nc, dst, 003, mkop(0xf7), REXW);
}

void
MUL(NC *nc, xRand dst)
{
	op1(nc, dst, 004, mkop(0xf7), REXW);
}

void
IMUL3(NC *nc, xRand dst, xRand src, xRand imm)
{
	xImm i;
	if(!isimm(imm))
		bug();
	i = randimm(imm);
	if(fitsbyte(i)) {
		op2(nc, mkop(0x6a), src, dst, REXW);
		n1(nc, i);
	}else{
		op2(nc, mkop(0x68), src, dst, REXW);
		n4(nc, i);
	}
}

void
IMUL2(NC *nc, xRand dst, xRand src)
{
	if(isreg(src) || ismem(src))
		op2x(nc, mkxop(0257), dst, src, REXW);
	else if(isimm(src))
		IMUL3(nc, dst, dst, src);
	else
		bug();
}

void
IMUL1(NC *nc, xRand dst)
{
	op1(nc, dst, 005, mkop(0xf7), REXW);
}

void
DIV(NC *nc, xRand dst)
{
	op1(nc, dst, 006, mkop(0xf7), REXW);
}

void
IDIV(NC *nc, xRand dst)
{
	op1(nc, dst, 007, mkop(0xf7), REXW);
}

void
NOP(NC *nc)
{
	emitu8(nc, 0x90);
}

void
RDTSC(NC *nc)
{
	emitu8(nc, 0x0f);
	emitu8(nc, 0x31);
}

void
RDPMC(NC *nc)
{
	emitu8(nc, 0x0f);
	emitu8(nc, 0x33);
}

void
LEAVE(NC *nc)
{
	emitu8(nc, 0xc9);
}

void
RET(NC *nc)
{
	emitu8(nc, 0xc3);
}

void
RETPOP(NC *nc, xRand imm)
{
	if(!isimm(imm))
		bug();
	emitu8(nc, 0xc2);
	emitu16(nc, randimm(imm).v.uword);
}

void
CPUID(NC *nc)
{
	emitopcode(nc, mkxop(0xa2));
}

static void
printbytes(void *buf, u32 m)
{
	u32 i;
	unsigned char *p;
	for(i = 0, p = buf; i < m; i++, p++)
		printf("\\x\%02x", *p);
}

static void
check(NC *p, char *form, char *result, u32 m)
{
	printf("%s\n", form);
	disx86(p->buf, p->buf+p->n, 0);
	if(p->n != m || memcmp(p->buf, result, p->n)) {
		printf("failed %s\n", form);
		printf("\texpected "); printbytes(result, m);
		printf("\n");
		printf("\treceived "); printbytes(p->buf, p->n);
		printf("\n");
	}
}

#define test(form, result)						\
{									\
	ncclr(p);							\
	form;								\
	check(p, #form, result, sizeof(result)-1);			\
}

int
testamd64()
{
	NC *p;
	p = mknc();
#include "amd64.tests.h"
	freenc(p);
	return 0;
}
