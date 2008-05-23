typedef
enum {
	Iadd = 0,
	Iand,
	Ibox,
	Ibox0,
	Icall,
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
	Iencode,
	Iframe,
	Igc,
	Ihalt,
	Iinv,
	Iiscl,
	Iiscval,
	Iisnull,
	Iispair,
	Iisrange,
	Iisstr,
	Iistab,
	Iistn,
	Iistype,
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
	Inull,
	Ior,
	Inop,
	Iprint,
	Ipush,
	Irange,
	Irbeg,
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
	Itnx,
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
	Ocval,
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

typedef struct Head Head;
typedef struct Heap Heap;
struct Head {
	unsigned color;
	Heap *heap;
	Head *link;
	Head *slink;
	Head *alink;
	int state;
};

struct Heap {
	char *id;
	Head *alloc, *swept, *sweep, *free;
	unsigned sz;
	Head* (*iter)(Head *hd, Ictx *ictx);
	void (*free1)(Head *hd);
};

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
		Cval cval;
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
	char *label;
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
Closure* mkcl(Code *code, unsigned long entry, unsigned len, char *id);
void docompile0(Expr *e);
Closure* compileentry(NS *ns, Expr *el, Env *env, int flags);
void printcode(Code *code);
char* topvecid(unsigned idx, Topvec *tv);
Val* topvecval(unsigned idx, Topvec *tv);
Closure* haltthunk();
Closure* gcthunk();
Closure* dingthunk();
Closure* printthunk();
Closure* callcc();
Closure* carthunk();
Closure* cdrthunk();
Closure* consthunk();
Closure* nullthunk();
Closure* rangebegthunk();
Closure* rangelenthunk();
Closure* rangethunk();
Closure* iscvaluethunk();
Closure* isprocedurethunk();
Closure* isnullthunk();
Closure* ispairthunk();
Closure* israngethunk();
Closure* isstringthunk();
Closure* istablethunk();
Closure* istnthunk();
Closure* istypethunk();
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

Code* contcode();
void getcode(Code *code);
void putcode(Code *code);

Env* mkenv();
void freeenv(Env *env);
Val* envgetbind(Env *env, char *id);

void initvm();
void finivm();
VM* mkvm(Env*);
void freevm(VM*);
void dovm(VM* vm, Closure *cl);

Head* galloc(Heap *heap);
void freecode(Head *hd);
extern Heap heapcode;
