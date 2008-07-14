typedef
enum {
	Iadd = 0,
	Iand,
	Ias,
	Ibin,
	Ibox,
	Ibox0,
	Icall,
	Icallc,
	Icallt,
	Icar,
	Icdr,
	Iclo,
	Icmpeq,
	Icmpgt,
	Icmpge,
	Icmplt,
	Icmple,
	Icmpneq,
	Icons,
	Icval,
	Iding,
	Idiv,
	Idom,
	Idomas,
	Idomns,
	Iencode,
	Iframe,
	Igc,
	Ihalt,
	Iinv,
	Iisas,
	Iiscl,
	Iiscval,
	Iisdom,
	Iisns,
	Iisnull,
	Iispair,
	Iisrange,
	Iisstr,
	Iistab,
	Iistn,
	Iisvec,
	Ijmp,
	Ijnz,
	Ijz,
	Ikg,
	Ikp,
	Ilenl,
	Ilens,
	Ilenv,
	Imod,
	Imov,
	Imul,
	Ineg,
	Inot,
	Ins,
	Insesym,
	Insetype,
	Inslsym,
	Insltype,
	Inull,
	Ior,
	Inop,
	Ipanic,
	Iprint,
	Ipush,
	Irange,
	Irbeg,
	Iref,
	Iret,
	Irlen,
	Ishl,
	Ishr,
	Isizeof,
	Islices,
	Istr,
	Isub,
	Itab,
	Itabdel,
	Itabenum,
	Itabget,
	Itabput,
	Itn,
	Ivec,
	Ivecl,
	Ivecref,
	Ivecset,
	Ivlist,
	Ivvec,
	Ixcast,
	Ixor,
	Iopmax         
} ikind;

#define TBITS(t,v) ((((t)&7)<<4)|((v)&15))
#define TBITSTYPE(tb) (((tb)>>4)&7)
#define TBITSBASE(tb) ((tb)&15)

typedef
enum {
	Rsp = 0,
	Rfp,
	Rpc,
	Rac,
	Rcl,
} Reg;

enum {
	Oloc = 0,
	Oliti,
	Olits,
	Onil,

	Lreg = 0,
	Lparam,
	Llocal,
	Ldisp,
	Ltopl,

	Clabel = 0,
	Clabelpair,

	Maxtmp = 128,
	Maxstk = 1024,
	InsnAlloc = 1024,
	AllocBatch = 128,
};

typedef
struct Ictx {
	u64 n;
	void *x;
} Ictx;

typedef struct Val Val;

typedef
struct Location {
	unsigned kind;
	unsigned indirect;
	unsigned idx;
	Val *val;
} Location;

typedef
struct Operand {
	unsigned okind;
	union{
		Location loc;
		Liti liti;
		Lits *lits;
	} u;
} Operand;

typedef struct Ctl Ctl;
typedef struct Code Code;
typedef struct Topvec Topvec;
typedef struct Konst Konst;

struct Ctl {
	unsigned ckind;

	/* ckind == Clabel */
	char *label;		/* for humans, only; (duplicates in code ok) */
	int used;
	unsigned long insn;

	/* ckind == Clabelpair */
	Ctl *l1, *l2;

	Ctl *link;
	Src *src;
	Code *code;
};

typedef
struct Insn {
	ikind kind;
	u8 bits;			/* typename/typedesc */
	void *go;
	Operand op1, op2, op3, dst;
	Ctl *dstlabel;
} Insn;

typedef struct Head Head;
typedef struct Heap Heap;
struct Head {
	unsigned color;
	unsigned inrootset;	/* racily limit duplicates on rootsets */
	Heap *heap;
	Head *alink;
	Head *link;
	int state;		/* debugging */
};

struct Code {
	Head hd;
	unsigned long refcnt;
	unsigned long ninsn;
	unsigned long maxinsn;
	unsigned long nconst;
	Insn *insn;
	Ctl **labels;
	Ctl *clist;
	Expr *src;
	Topvec *topvec;
	Konst *konst;
};

typedef struct Closure Closure;
typedef struct VM VM;
typedef struct Env Env;

void initcompile();
void finicompile();
Code* newcode();
Closure* mkcl(Code *code, unsigned long entry, unsigned len, char *id);
Expr* docompilec(Expr *e);
void docompile0(Expr *e);
Closure* compileentry(Expr *el, Env *env);
void printvmac(VM *vm);
char* topvecid(unsigned idx, Topvec *tv);
Val* topvecval(unsigned idx, Topvec *tv);
Closure* haltthunk();
Closure* gcthunk();
Closure* panicthunk();
Closure* dingthunk();
Closure* printthunk();
Closure* callcc();
Closure* carthunk();
Closure* cdrthunk();
Closure* consthunk();
Closure* mkasthunk();
Closure* mkdomthunk();
Closure* domasthunk();
Closure* domnsthunk();
Closure* mknsthunk();
Closure* nullthunk();
Closure* rangebegthunk();
Closure* rangelenthunk();
Closure* rangethunk();
Closure* iscvaluethunk();
Closure* isasthunk();
Closure* isdomthunk();
Closure* isnsthunk();
Closure* isnullthunk();
Closure* ispairthunk();
Closure* isprocedurethunk();
Closure* israngethunk();
Closure* isstringthunk();
Closure* istablethunk();
Closure* istnthunk();
Closure* isvectorthunk();
Closure* stringthunk();
Closure* strlenthunk();
Closure* substrthunk();
Closure* tablethunk();
Closure* tabinsertthunk();
Closure* tabdeletethunk();
Closure* tablookthunk();
Closure* tabenumthunk();
Closure* typenamexthunk();
Closure* mkvecthunk();
Closure* vectorthunk();
Closure* veclenthunk();
Closure* vecrefthunk();
Closure* vecsetthunk();

Code* callccode();
Code* contcode();

Env* mkenv();
void freeenv(Env *env);
Val* envgetbind(Env *env, char *id);

void initvm();
void finivm();
VM* mkvm(Env*);
void vmreset(VM *vm);
void freevm(VM*);
jmp_buf* _pusherror(VM *vm);
#define waserror(vm) (setjmp(*(_pusherror(vm))))
void nexterror(VM *vm) __attribute__((noreturn));
void poperror(VM *vm);
Val* dovm(VM* vm, Closure *cl, Imm argc, Val *argv);

void freecode(Head *hd);
