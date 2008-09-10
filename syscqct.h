#ifndef _BISONFLAW_SYSCQCT_H_
#define _BISONFLAW_SYSCQCT_H_

#include "cqct.h"

enum{
	Vintmax		= 2147483647ULL,
	Vuintmax	= 4294967295ULL,
	Vlongmax	= 2147483647ULL,
	Vulongmax	= 4294967295ULL,
	Vvlongmax	= 9223372036854775807ULL,
	Vuvlongmax	= 18446744073709551615ULL,
	Maxliti		= 24,	/* longest integer literal plus null */
};

typedef
enum{
	Echar=0,
	Edouble,
	Efloat,
	Eint,	
	Elong,
	Eshort,
	Esigned,
	Eunsigned,
	Evoid,
	Enbase,

	Eadd,			/* 10 */
	Eambig,
	Earef,
	Earr,
	Earrow,
	Eband,
	Ebase,
	Ebinop,
	Ebitfield,
	Eblock,
	Ebor,
	Ebreak,
	Ebxor,
	Ecall,
	Ecase,
	Ecast,
	Ecomma,
	Econd,
	Econst,
	Econsts,
	Econtainer,
	Econtinue,
	Edecl,			/* type specifier + 1 declarator */
	Edecls,			/* type specifier + list of declarators */
	Edefault,
	Edefine,
	Ederef,
	Ediv,
	Edo,
	Edot,
	Edotdot,
	Eelist,
	Eenum,
	Eenumel,
	Eeq,
	Efieldoff,
	Efields,
	Efor,
	Efun,
	Eg,
	Egop,
	Ege,
	Egt,
	Eid,
	Eif,
	Elambda,
	Eland,
	Ele,
	Elist,
	Elor,
	Elt,
	Emod,
	Emul,
	Ens,
	Eneq,
	Enop,
	Enil,
	Enull,
	Epostdec,
	Epostinc,
	Epredec,
	Epreinc,
	Eptr,
	Eref,
	Eret,
	Eshl,
	Eshr,
	Esizeofe,
	Esizeoft,
	Estruct,
	Esub,
	Eswitch,
	Etick,
	Etid,
	Etypedef,
	Etypeofe,
	Etypeoft,
	Euminus,
	Eunion,
	Eunot,
	Euplus,
	Eutwiddle,
	Ewhile,
	Excast,

	/* open-coded primitives */
	E_cval,
	E_ref,
	E_sizeof,
	Emax,
} Kind;

typedef
enum Cbase {
	Vundef=0,
	Vchar,
	Vshort,
	Vint,
	Vlong,
	Vvlong,
	Vuchar,
	Vushort,
	Vuint,
	Vulong,
	Vuvlong,
	Vfloat,
	Vdouble,
	Vlongdouble,
	Vnbase,
	Vptr = Vnbase,		/* alias for some other base type */
	Vvoid,
	Vnallbase,
} Cbase;

typedef
enum Tkind {
	Tvoid=0,
	Tbase,
	Tstruct,
	Tunion,
	Tenum,
	Tptr,
	Tarr,
	Tfun,
	Ttypedef,
	Tbitfield,		/* xtn only */
	Tconst,			/* xtn only */
	Tundef,			/* xtn only */
} Tkind;

enum {
	Tntkind = Tundef+1
};

typedef struct Decl Decl;
typedef struct Enum Enum;
typedef struct Type Type;

typedef
struct Src {
	char *filename;
	unsigned col, line;
} Src;

typedef
struct Lits {
	char *s;
	unsigned len;
} Lits;

typedef
struct Liti {
	Imm val;
	Cbase base;
} Liti;

struct Expr {
	Kind kind;

	char *id;		/* Eid, Etick */
	Lits *lits;		/* Econsts */
	Liti liti;		/* Econst */
	Kind op;		/* Ebinop, Egop */

	Expr *e1;
	Expr *e2;
	Expr *e3;
	Expr *e4;

	/* source */
	Src src;

	/* compiler-managed fields */
	void *xp;
	unsigned xn;
};

struct Enum {
	char *id;
	Expr *val;
	Enum *link;
};

struct Decl {
	Type *type;
	char *id;
	Expr *offs;		/* offset */
	Decl *link;
};

struct Type {
	unsigned kind;
	unsigned base;		/* base, enum? */
	char *tid;		/* typedef */
	char *tag;		/* struct, union, enum */
	char *dom;		/* optional domain qualifier for any Type */
	Decl *field;		/* struct, union */
	Enum *en;		/* enum */
	Expr *sz;		/* struct, union */
	Expr *bitw, *bit0;	/* bitfield geometry */
	Decl *param;		/* func */
	Expr *cnt;		/* arr */
	Type *link;		/* typedef, ptr, arr, func(return type) */
};

enum {
	MaxIn	= 128,
};

typedef struct YYstate YYstate;

typedef
struct In {
	struct Src src;
	FILE *fp;
	YYstate *yy;
} In;

typedef
struct U {
	jmp_buf jmp;
	In in[MaxIn];
	In *inp;
	Expr *el;
} U;

extern U ctx;
extern char* S[];
extern char* basename[];
extern char* tkindstr[];

YYstate* mkyystate(FILE *fp);
YYstate* mkyystatestr(char *buf);
void freeyystate(YYstate *yy);
void setyystate(YYstate *yy);

int yyparse(U *ctx);
Expr* newexpr(unsigned, Expr*, Expr*, Expr*, Expr*);
Expr* newexprsrc(Src*, unsigned, Expr*, Expr*, Expr*, Expr*);
Expr* copyexpr(Expr *e);
Expr* newbinop(unsigned, Expr*, Expr*);
Expr* newbinopsrc(Src*, unsigned, Expr*, Expr*);
Expr* newgop(unsigned, Expr*, Expr*);
Expr* newgopsrc(Src*, unsigned, Expr*, Expr*);
Expr* mkconst(Cbase base, Imm val); /* rename newconst? */
void freeexpr(Expr*);
void freeenum(Enum *en);
void freeexprx(Expr *e);
Expr* invert(Expr*);
Expr* nullelist();
Expr* ptrto(Expr*, Expr*);
Expr* doid(char*);
Expr* doidn(char *s, unsigned long len);
Expr* doidnsrc(Src *src, char *s, unsigned long len);
int parseliti(char *s, unsigned long len, Liti *liti, char **err);
Expr* doconst(U *ctx, char*, unsigned long len);
Expr* doconsts(char*, unsigned long len);
Expr* doconstssrc(Src*, char*, unsigned long len);
Expr* dotick(Expr*, Expr*);
Expr* doticksrc(Src *src, Expr*, Expr*);
Lits* mklits(char*, unsigned len);
Lits* copylits(Lits *lits);
void freelits(Lits *lits);
char* fmttype(Type *t, char *o);

Expr* dotypes(U*, Expr*);
void dotop(U*, Expr*);

void printexpr(Expr*);
void printcqct(Expr*);

void pushyy(U *ctx, char *filename, char *inbuf);
int popyy(U *ctx);
void tryinclude(U *ctx, char *raw);
void parseerror(U *ctx, char *fmt, ...);

typedef
struct BFgeom {
	Imm bp, bs;
	Imm addr, cnt;
	unsigned isbe;
	int les, bes;
} BFgeom;
int bitfieldgeom(BFgeom *bfg);
Imm bitfieldget(char *s, BFgeom *bfg);
Imm bitfieldput(char *s, BFgeom *bfg, Imm val);

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))


typedef
enum {
	Iadd = 0,
	Iand,
	Iargc,
	Ibin,
	Ibox,
	Ibox0,
	Icall,
	Icallc,
	Icallt,
	Iclo,
	Icmpeq,
	Icmpgt,
	Icmpge,
	Icmplt,
	Icmple,
	Icmpneq,
	Icval,
	Idata,
	Idiv,
	Iframe,
	Ihalt,
	Iinv,
	Ijmp,
	Ijnz,
	Ijz,
	Ikg,
	Ikp,
	Ilist,
	Imod,
	Imov,
	Imul,
	Ineg,
	Inot,
	Ior,
	Inop,
	Ipanic,
	Ipush,
	Ipushi,
	Iref,
	Iret,
	Ishl,
	Ishr,
	Isizeof,
	Isub,
	Ixcast,
	Ixor,
	Iopmax         
} ikind;

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
		Val liti;
		Lits *lits;
	} u;
} Operand;

typedef struct Ctl Ctl;
typedef struct Code Code;
typedef struct Topvec Topvec;
typedef struct Konst Konst;
typedef struct Konsti Konsti;

struct Ctl {
	unsigned ckind;

	/* ckind == Clabel */
	char *label;		/* for humans, only; (duplicates in code ok) */
	int used;
	unsigned long insn;
	void *insnx;

	/* ckind == Clabelpair */
	Ctl *l1, *l2;

	Ctl *link;
	Src *src;
	Code *code;
};

typedef
struct Insn {
	ikind kind;
	void *go;
	uintptr_t data;
	Operand op1, op2, op3, dst;
	Ctl *dstlabel;
} Insn;

struct Konst {
	HT *ht;
};

struct Konsti {
	HT *ht;
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
	Konsti *konsti;
};

void initcompile();
void finicompile();
Code* newcode();
Closure* mkcl(Code *code, unsigned long entry, unsigned len, char *id);
int docompilec(U *ctx, Expr *e);
int docompile0(U *ctx, Expr *e);
typedef Expr*(Pass)(U*, Expr*);
Expr* gentypename(Type *t, Pass*, U*);
Closure* compileentry(Expr *el, Env *env);
void printvmac(VM *vm);
char* topvecid(unsigned idx, Topvec *tv);
Val* topvecval(unsigned idx, Topvec *tv);
Imm valimm(Val v);
Closure* haltthunk();
Closure* callcc();
Closure* panicthunk();

Code* callccode();
Code* contcode();

Env* mktopenv();
void freeenv(Env *env);
Val* envgetbind(Env *env, char *id);

void initvm();
void finivm();
jmp_buf* _pusherror(VM *vm);
#define waserror(vm) (setjmp(*(_pusherror(vm))))
void nexterror(VM *vm) NORETURN;
void poperror(VM *vm);
Val dovm(VM* vm, Closure *cl, Imm argc, Val *argv);

Val mklitcval(Cbase base, Imm val);
void freecode(Head *hd);

/* cutil.c */
void cerror(U *ctx, Expr *e, char *fmt, ...) NORETURN;
Expr* Zadd(Expr *x, Expr *y);
Expr* Zapply(Expr *fn, Expr *args);
Expr* Zargs(unsigned n, ...);
Expr* Zbinop(unsigned op, Expr *x, Expr *y);
Expr* Zblock(Expr *locs, ...);
Expr* Zcall(Expr *fn, unsigned narg, ...);
Expr* Zcons(Expr *hd, Expr *tl);
Expr* Zconsts(char *s);
Expr* Zcval(Expr *dom, Expr *type, Expr *val);
Expr* Zif(Expr *cond, Expr *true);
Expr* Zifelse(Expr *cond, Expr *true, Expr *false);
Expr* Zlambda(Expr *args, Expr *body);
Expr* Zlambdn(Expr *args, Expr *body, Expr *name);
Expr* Zlocals(unsigned n, ...);
Expr* Znil();
Expr* Zrange(Expr *addr, Expr *sz);
Expr* Zref(Expr *dom, Expr *type, Expr *val);
Expr* Zret(Expr *e);
Expr* Zset(Expr *l, Expr *r);
Expr* Zsizeof(Expr *e);
Expr* Zstr(char *s);
Expr* Zsub(Expr *x, Expr *y);
Expr* Zuint(Imm val);
Expr* Zxcast(Expr *type, Expr *cval);

#endif /* _BISONFLAW_SYSCQCT_H_ */

