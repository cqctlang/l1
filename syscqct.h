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
	Edefconst,
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

/* base C types */
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

/* type representations */ 
typedef
enum Rkind {
	Rundef,
	Ru08le,
	Ru16le,
	Ru32le,
	Ru64le,
	Rs08le,
	Rs16le,
	Rs32le,
	Rs64le,
	Ru08be,
	Ru16be,
	Ru32be,
	Ru64be,
	Rs08be,
	Rs16be,
	Rs32be,
	Rs64be,
	Rnrep, 
} Rkind;

/* ctypes */
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
	Tntkind=Tundef+1,	/* keep outside of Tkind */
};

enum {
	Tabinitsize=1024,	/* power of 2 */
	Listinitsize=16,
	Maxprintint=32,
	Typepos=0,
	Idpos=1,
	Offpos=2,
	Maxvms=1024,
	Errinitdepth=128,	/* initial max error stack */
	Maxtmp = 128,
	Maxstk = 1024,
	InsnAlloc = 10,
	AllocBatch = 128,
	Maxheap	= 5*1024*1024,
};

typedef struct Decl Decl;
typedef struct Enum Enum;
typedef struct Type Type;
typedef struct Ctl Ctl;
typedef struct Code Code;
typedef struct Topvec Topvec;
typedef struct Konst Konst;
typedef struct Konsti Konsti;

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

typedef struct As As;
typedef struct Box Box;
typedef struct Cval Cval;
typedef struct Dom Dom;
typedef struct Fd Fd;
typedef struct List List;
typedef struct Ns Ns;
typedef struct Pair Pair;
typedef struct Range Range;
typedef struct Str Str;
typedef struct Tab Tab;
typedef struct Vec Vec;
typedef struct Xtypename Xtypename;

typedef void (Cfn)(VM *vm, Imm argc, Val *argv, Val *rv);
typedef void (Ccl)(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv);

struct Cval {
	Head hd;
	Dom *dom;
	Xtypename *type;
	Imm val;
};

struct Closure {
	Head hd;
	Code *code;
	unsigned long entry;
	unsigned dlen;
	Val *display;
	char *id;
	Imm fp;			/* of continuation, always >0 */
	Cfn *cfn;
	Ccl *ccl;
};

struct Box {
	Head hd;
	Val v;
};

typedef struct Tabidx Tabidx;
struct Tabidx {
	u32 idx;
	Tabidx *link;
};

typedef
struct Tabx {
	u32 nxt, lim;
	u32 sz;
	Val *key;
	Val *val;
	Tabidx **idx;
} Tabx;

struct Tab {
	Head hd;
	u32 cnt;		/* key/val pairs stored */
	Tabx *x;		/* current storage, atomically swappable */
};

typedef
struct Listx {
	/* invariants:
	 *  if(hd == tl)
	 *  	list is empty.
	 *  else
	 *  	tl>hd and list has tl-hd elements,
	 *      hd points to first element in list, and
	 *	tl-1 points to last.
	 */
	u32 hd, tl, sz;
	Val *val;		/* atomically swappable */
	Val *oval;		/* buffer for gc-safe sliding */
} Listx;

struct List {
	Head hd;
	Listx *x;		/* current storage, atomically swappable */
};

struct As {
	Head hd;
	Closure *dispatch;
	Str *name;
};

typedef
struct Nssym {
	Imm addr;
	Vec *sym;
} Nssym;

struct Ns {
	Head hd;

	/* interface for all instances */
	Closure *enumsym;
	Closure *enumtype;
	Closure *looksym;
	Closure *looktype;
	Closure *lookaddr;
	Str *name;

	/* cached base type definition */
	Xtypename *base[Vnallbase];
};

struct Dom {
	Head hd;
	As *as;
	Ns *ns;
	Str *name;
};

struct Fd {
	Head hd;
	void (*close)(Fd *fd);
	int fd;
	Str *name;
	enum Fflag {
		Fclosed =	1,
		Fread =		Fclosed<<1,
		Fwrite =	Fread<<1,
	} flags;
};

struct Pair {
	Head hd;
	Val car;
	Val cdr;
};

struct Range {
	Head hd;
	Cval *beg;
	Cval *len;
};

typedef
enum Skind {
	Sperm,			/* don't free */
	Smalloc,		/* free with free() */
	Smmap,			/* free with munmap() */
} Skind;

struct Str {
	Head hd;
	Skind skind;
	u64 len;
	size_t mlen;		/* Smmap size */
	char *s;
};

struct Vec {
	Head hd;
	Imm len;
	Val *vec;
};

struct Xtypename {
	Head hd;
	Tkind tkind;		/* = Tbase, Tstruct, ... */
	Cbase basename;		/* base (FIXME: rename cbase) */
	Rkind rep;		/* base, ptr, enum; = Ru08le ... */
	Str *tid;		/* typedef */
	Str *tag;		/* struct, union, enum */
	Val cnt;		/* arr */
	Val sz;			/* struct, union, bitfield */
	Val bit0;		/* bitfield */
	Xtypename *link;	/* ptr, arr, func, bitfield, enum, const */
	Vec *field;		/* struct, union */
	Vec *param;		/* func */
	Vec *konst;		/* enum (constants) */
};

enum{
	FmtWidth	= 1,
	FmtLeft		= FmtWidth << 1,
	FmtPrec		= FmtLeft << 1,
	FmtSharp	= FmtPrec << 1,
	FmtSpace	= FmtSharp << 1,
	FmtSign		= FmtSpace << 1,
	FmtZero		= FmtSign << 1,

	FmtFlag		= FmtZero << 1
};

typedef struct Fmt Fmt;
struct Fmt {
	char *start, *to, *stop;
	int (*flush)(Fmt*);
	void *farg;
	int width;
	int prec;
	unsigned int flags;
};

enum {
	MaxIn	= 128,
};

typedef struct YYstate YYstate;

typedef
struct In {
	struct Src src;
	char *buf;
	int dofree;
	YYstate *yy;
} In;

typedef
struct U {
	jmp_buf jmp;
	In in[MaxIn];
	In *inp;
	Expr *el;
} U;

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

typedef
struct BFgeom {
	Imm bp, bs;
	Imm addr, cnt;
	unsigned isbe;
	int les, bes;
} BFgeom;

typedef struct Root Root;
struct Root {
	Head *hd;
	Root *link;
};

typedef
struct Rootset {
	Root *roots;
	Root *last;
	Root *before_last;
	Root *this;
	Root *free;
} Rootset;

typedef
struct Err {
	jmp_buf esc;
	unsigned pdepth;	/* vm->pdepth when error label set */
} Err;

typedef struct Xenv Xenv;
struct Xenv {
	HT *ht;
	Xenv *link;
};

struct Env {
	HT *var;
	Xenv *con;
};

struct VM {
	Val stack[Maxstk];
	Dom *litdom;
	Ns *litns;
	Xtypename **litbase;	/* always points to litns->base */
	Str *sget, *sput, *smap;/* cached dispatch operands */
	Fd *stdout, *stdin;
	Root **prot;		/* stack of lists of GC-protected objects */
	Rootset rs;		/* Root free list for prot */
	unsigned pdepth, pmax;	/* # live and max prot lists  */
	Env *top;
	Imm sp, fp, pc;
	Closure *clx;
	Insn *ibuf;
	Val ac, cl;
	Err *err;		/* stack of error labels */
	unsigned edepth, emax;	/* # live and max error labels */
	Tab *prof;
};

extern char* S[];
extern char* basename[];
extern char* tkindstr[];
extern VM*   vms[];

/* c.l */
void		freeyystate(YYstate *yy);
YYstate*	mkyystate(FILE *fp);
YYstate*	mkyystatestr(char *buf);
void		setyystate(YYstate *yy);

/* parse.c */
Expr*		copyexpr(Expr *e);
Lits*		copylits(Lits *lits);
Expr*		doconst(U *ctx, char*, unsigned long len);
Expr*		doconsts(char*, unsigned long len);
Expr*		doconstssrc(Src*, char*, unsigned long len);
Expr*		doid(char*);
Expr*		doidn(char *s, unsigned long len);
Expr*		doidnsrc(Src *src, char *s, unsigned long len);
Expr*		dotick(Expr*, Expr*);
Expr*		doticksrc(Src *src, Expr*, Expr*);
void		dotop(U*, Expr*);
Expr*		dotypes(U*, Expr*);
char*		fmttype(Type *t, char *o);
Expr*		flatten(Expr *e);
void		freeenum(Enum *en);
void		freeexpr(Expr*);
void		freeexprx(Expr *e);
void		freelits(Lits *lits);
Expr*		invert(Expr*);
Expr*		mkconst(Cbase base, Imm val); /* rename newconst? */
Lits*		mklits(char*, unsigned len);
Expr*		newbinop(unsigned, Expr*, Expr*);
Expr*		newbinopsrc(Src*, unsigned, Expr*, Expr*);
Expr*		newexpr(unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newexprsrc(Src*, unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newgop(unsigned, Expr*, Expr*);
Expr*		newgopsrc(Src*, unsigned, Expr*, Expr*);
Expr*		nullelist();
int		parseliti(char *s, unsigned long len, Liti *liti, char **err);
Expr*		ptrto(Expr*, Expr*);
void		parseerror(U *ctx, char *fmt, ...);
int		popyy(U *ctx);
void		tryinclude(U *ctx, char *raw);
int		yyparse(U *ctx);

/* printexpr.c */
void		printcqct(Expr*);
void		printexpr(Expr*);
void		printdecl(Decl *d);
void		printvars(Expr *e);

/* bitfield.c */
int		bitfieldgeom(BFgeom *bfg);
Imm		bitfieldget(char *s, BFgeom *bfg);
Imm		bitfieldput(char *s, BFgeom *bfg, Imm val);

/* compile.c */
Closure*	callcc();
Code*		callccode();
Closure*	compileentry(Expr *el, Env *env);
Code*		contcode();
void		finicompile();
void		freeconst(void *u, char *id, void *v);
void		freexenv(Xenv *xe);
Closure*	haltthunk();
void		initcompile();
Xenv*		mkxenv(Xenv *link);
Closure*	panicthunk();
char*		topvecid(unsigned idx, Topvec *tv);
Val*		topvecval(unsigned idx, Topvec *tv);
void*		xenvlook(Xenv *xe, char *id);
void		xenvbind(Xenv *xe, char *id, void *v);
void		xenvforeach(Xenv *xe, void (*f)(void *u, char *k, void *v),
			    void *u);
unsigned long	xenvsize(Xenv *xe);

/* compilec.c */
int		docompilec(U *ctx, Expr *e);

/* compile0.c */
int		docompile0(U *ctx, Expr *e);
Expr*		gentypename(Type *t, Expr*(*)(U*, Expr*), U*);

/* vm.c */
void		builtinfn(Env *env, char *name, Closure *cl);
void		checkarg(VM *vm, char *f, Val *argv,
			 unsigned arg, Qkind qkind);
void		dofmt(VM *vm, Fmt *f, char *fmt, Imm fmtlen,
		      Imm argc, Val *argv);
Val		dovm(VM* vm, Closure *cl, Imm argc, Val *argv);
int		envbinds(Env *env, char *id);
Val*		envgetbind(Env *env, char *id);
void		freeenv(Env *env);
void		initvm(int gcthread, u64 heapmax);
void		finivm();
int		freecode(Head *hd);
void		heapfree(Head *p);
Closure*	mkcfn(char *id, Cfn *cfn);
Closure*	mkcl(Code *code, unsigned long entry, unsigned len, char *id);
Cval*		mkcval(Dom *dom, Xtypename *type, Imm val);
Fd*		mkfd(Str *name, int xfd, int flags, void (*free)(Fd *fd));
Cval*		mklitcval(Cbase base, Imm val);
Range*		mkrange(Cval *beg, Cval *len);
As*		mksas(Str *s);
Str*		mkstr(char *s, Imm len);
Str*		mkstrlits(Lits *lits);
Str*		mkstr0(char *s);
Str*		mkstrk(char *s, Imm len, Skind skind);
Str*		mkstrn(VM *vm, Imm len);
Tab*		mktab();
Env*		mktopenv();
Val		mkvalas(As *as);
Val		mkvalbox(Val boxed);
Val		mkvalcl(Closure *cl);
Val		mkvalcval(Dom *dom, Xtypename *t, Imm imm);
Val		mkvalcval2(Cval *cv);
Val		mkvaldom(Dom *dom);
Val		mkvalfd(Fd *fd);
Val		mkvalimm(Dom *dom, Xtypename *t, Imm imm);
Val		mkvallist(List *lst);
Val		mkvallitcval(Cbase base, Imm imm);
Val		mkvalns(Ns *ns);
Val		mkvalpair(Val car, Val cdr);
Val		mkvalrange(Cval *beg, Cval *len);
Val		mkvalrange2(Range *r);
Val		mkvalstr(Str *str);
Val		mkvaltab(Tab *tab);
Val		mkvalvec(Vec *vec);
Val		mkvalxtn(Xtypename *xtn);
As*		mkzas(VM *vm, Imm len);
Code*		newcode();
void		nexterror(VM *vm) NORETURN;
void		poperror(VM *vm);
void		printvmac(VM *vm);
jmp_buf*	_pusherror(VM *vm);
char*		str2cstr(Str *str);
Val		tabget(Tab *tab, Val keyv);
void		tabput(VM *vm, Tab *tab, Val keyv, Val val);
Imm		valimm(Val v);
void		vmerr(VM *vm, char *fmt, ...) NORETURN;
void		vmreset(VM *vm);
Fd*		vmstdout(VM *vm);
#define valas(v)	((As*)(v))
#define valcval(v)	((Cval*)(v))
#define valcl(v)	((Closure*)(v))
#define valdom(v)	((Dom*)(v))
#define valfd(v)	((Fd*)(v))
#define vallist(v)	((List*)(v))
#define valns(v)	((Ns*)(v))
#define valpair(v)	((Pair*)(v))
#define valrange(v)	((Range*)(v))
#define valstr(v)	((Str*)(v))
#define valtab(v)	((Tab*)(v))
#define valvec(v)	((Vec*)(v))
#define valxtn(v)	((Xtypename*)(v))
#define valboxedcval(b)	((Cval*)((Box*)(b))->v)
#define waserror(vm) (setjmp(*(_pusherror(vm))))
#define FN(name) builtinfn(env, #name, mkcfn(#name, l1_##name))

typedef int(*Freeheadfn)(Head*);
Freeheadfn getfreeheadfn(Qkind qkind);
void setfreeheadfn(Qkind qkind, Freeheadfn free1);

extern		void fns(Env*);

/* cutil.c */
void		cerror(U *ctx, Expr *e, char *fmt, ...) NORETURN;
void		cwarn(Expr *e, char *fmt, ...);
Expr*		Zadd(Expr *x, Expr *y);
Expr*		Zapply(Expr *fn, Expr *args);
Expr*		Zargs(unsigned n, ...);
Expr*		Zbinop(unsigned op, Expr *x, Expr *y);
Expr*		Zblock(Expr *locs, ...);
Expr*		Zcall(Expr *fn, unsigned narg, ...);
Expr*		Zcons(Expr *hd, Expr *tl);
Expr*		Zconsts(char *s);
Expr*		Zcval(Expr *dom, Expr *type, Expr *val);
Expr*		Zif(Expr *cond, Expr *true);
Expr*		Zifelse(Expr *cond, Expr *true, Expr *false);
Expr*		Zlambda(Expr *args, Expr *body);
Expr*		Zlambdn(Expr *args, Expr *body, Expr *name);
Expr*		Zlocals(unsigned n, ...);
Expr*		Znil();
Expr*		Zrange(Expr *addr, Expr *sz);
Expr*		Zref(Expr *dom, Expr *type, Expr *val);
Expr*		Zret(Expr *e);
Expr*		Zset(Expr *l, Expr *r);
Expr*		Zsizeof(Expr *e);
Expr*		Zstr(char *s);
Expr*		Zsub(Expr *x, Expr *y);
Expr*		Zuint(Imm val);
Expr*		Zxcast(Expr *type, Expr *cval);

#endif /* _BISONFLAW_SYSCQCT_H_ */
