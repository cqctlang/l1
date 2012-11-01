typedef
enum xReg {
	rNone,
	rRAX, rRCX, rRDX, rRBX, rRSP, rRBP, rRSI, rRDI,
	rR8, rR9, rR10, rR11, rR12, rR13, rR14, rR15,
	rEAX, rECX, rEDX, rEBX, rESP, rEBP, rESI, rEDI,
	rAX, rCX, rDX, rBX, rSP, rBP, rSI, rDI,
	rAL, rCL, rDL, rBL,
	rAH, rCH, rDH, rBH,
	rSPL, rBPL, rSIL, rDIL,
} xReg;

typedef
enum Prefix {
	REX  = 0x40,
	REXB = 0x41,
	REXX = 0x42,
	REXR = 0x44,
	REXW = 0x48,
} Prefix;

typedef
enum xOpkind {
	opNone,
	opReg,
	opMem,
	opImm,
} xOpkind;

typedef
enum xSize {
	Zero = 0,
	Byte = 8,
	Word = 16,
	DWord = 32,
	QWord = 64,
} xSize;

typedef
struct xImm {
	enum xSize size;
	union {
		u8	ubyte;
		s8	sbyte;
		u16	uword;
		s16	sword;
		u32	udword;
		s32	sdword;
		u64	uqword;
		s64	sqword;
	} v;
} xImm;

typedef
struct xMem {
	xReg base;
	xReg idx;
	u8 scale;
	xImm disp;
} xMem;

typedef
struct xRand {
	xOpkind okind;
	union{
		xReg r;
		xMem m;
		xImm i;
	} u;
} xRand;

#define isnone(o)    ((o).okind == opNone)
#define isreg(o)     ((o).okind == opReg)
#define ismem(o)     ((o).okind == opMem)
#define isimm(o)     ((o).okind == opImm)

#define isrnone(r)   ((r) == rNone)
#define isdnone(i)   ((i).size == Zero)

#define randreg(o)   ((o).u.r)
#define randbase(o)  ((o).u.m.base)
#define randidx(o)   ((o).u.m.idx)
#define randscale(o) ((o).u.m.scale)
#define randdisp(o)  ((o).u.m.disp)
#define randimm(o)   ((o).u.i)

typedef
struct xOp {
	u8 extended;
	u8 op0, op1;
} xOp;

#define isxop(op)    ((op).extended != 0)

extern xRand NONE;
extern xRand RNONE, RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
	R8, R9, R10, R11, R12, R13, R14, R15,
	EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
	AX, CX, DX, BX, SP, BP, SI, DI,
	AL, CL, DL, BL, SPL, BPL, SIL, DIL,
	AH, CH, DH, BH;

xRand		byte(u64 u);
xRand		disp(s32 d);
xRand		imm(u64 u);
xRand		imms8(s8 s);
xRand		imms16(s16 s);
xRand		imms32(s32 s);
xRand		imms64(s64 s);
xRand		immu8(u8 u);
xRand		immu16(u16 u);
xRand		immu32(u32 u);
xRand		immu64(u64 u);
xRand		indirect1(xRand o);
xRand		indirect2(xRand b, xRand o);
xRand		indirecti(xRand b, xRand i, s32 d);
xRand		indirect2i(xRand b, xRand i, s32 d);
xRand		indirect4i(xRand b, xRand i, s32 d);
xRand		indirect8i(xRand b, xRand i, s32 d);
xRand		indirectrip(s32 d);

void		ADC(NC *nc, xRand dst, xRand src);
void		ADD(NC *nc, xRand dst, xRand src);
void		AND(NC *nc, xRand dst, xRand src);
void		CALL(NC *nc, xRand dst);
void		CMP(NC *nc, xRand dst, xRand src);
void		CPUID(NC *nc);
void		DIV(NC *nc, xRand dst);
void		JMP(NC *nc, xRand dst);
void		IDIV(NC *nc, xRand dst);
void		IMUL1(NC *nc, xRand dst);
void		IMUL2(NC *nc, xRand dst, xRand src);
void		IMUL3(NC *nc, xRand dst, xRand src, xRand imm);
void		LEA(NC *nc, xRand dst, xRand src);
void		LEAVE(NC *nc);
void		MOV(NC *nc, xRand dst, xRand src);
void		MUL(NC *nc, xRand dst);
void		NEG(NC *nc, xRand dst);
void		NOP(NC *nc);
void		NOT(NC *nc, xRand dst);
void		OR(NC *nc, xRand dst, xRand src);
void		POP(NC *nc, xRand o);
void		PUSH(NC *nc, xRand o);
void		POPAD(NC *nc);
void		PUSHAD(NC *nc);
void		RCL(NC *nc, xRand dst, xRand n);
void		RCR(NC *nc, xRand dst, xRand n);
void		RDPMC(NC *nc);
void		RDTSC(NC *nc);
void		RET(NC *nc);
void		RETPOP(NC *nc, xRand imm);
void		ROL(NC *nc, xRand dst, xRand n);
void		ROR(NC *nc, xRand dst, xRand n);
void		SAR(NC *nc, xRand dst, xRand n);
void		SBB(NC *nc, xRand dst, xRand src);
void		SHL(NC *nc, xRand dst, xRand n);
void		SHR(NC *nc, xRand dst, xRand n);
void		SUB(NC *nc, xRand dst, xRand src);
void		TEST(NC *nc, xRand dst, xRand src);
void		XOR(NC *nc, xRand dst, xRand src);

void		JO(NC *nc, xRand dst);
void		JNO(NC *nc, xRand dst);
void		JB(NC *nc, xRand dst);
void		JAE(NC *nc, xRand dst);
void		JE(NC *nc, xRand dst);
void		JZ(NC *nc, xRand dst);
void		JNE(NC *nc, xRand dst);
void		JNZ(NC *nc, xRand dst);
void		JBE(NC *nc, xRand dst);
void		JA(NC *nc, xRand dst);
void		JS(NC *nc, xRand dst);
void		JNS(NC *nc, xRand dst);
void		JP(NC *nc, xRand dst);
void		JNP(NC *nc, xRand dst);
void		JL(NC *nc, xRand dst);
void		JGE(NC *nc, xRand dst);
void		JLE(NC *nc, xRand dst);
void		JG(NC *nc, xRand dst);

void		CMOVO(NC *nc, xRand dst, xRand src);
void		CMOVNO(NC *nc, xRand dst, xRand src);
void		CMOVB(NC *nc, xRand dst, xRand src);
void		CMOVAE(NC *nc, xRand dst, xRand src);
void		CMOVE(NC *nc, xRand dst, xRand src);
void		CMOVZ(NC *nc, xRand dst, xRand src);
void		CMOVNE(NC *nc, xRand dst, xRand src);
void		CMOVNZ(NC *nc, xRand dst, xRand src);
void		CMOVBE(NC *nc, xRand dst, xRand src);
void		CMOVA(NC *nc, xRand dst, xRand src);
void		CMOVS(NC *nc, xRand dst, xRand src);
void		CMOVNS(NC *nc, xRand dst, xRand src);
void		CMOVP(NC *nc, xRand dst, xRand src);
void		CMOVNP(NC *nc, xRand dst, xRand src);
void		CMOVL(NC *nc, xRand dst, xRand src);
void		CMOVGE(NC *nc, xRand dst, xRand src);
void		CMOVLE(NC *nc, xRand dst, xRand src);
void		CMOVG(NC *nc, xRand dst, xRand src);
