extern char cqctflags[];

typedef
enum {
	Qundef = 0,
	Qnil,
	Qnulllist,
	Qas,
	Qbox,
	Qcl,
	Qcode,
	Qcval,
	Qdom,
	Qfd,
	Qlist,
	Qns,
	Qpair,
	Qrange,
	Qstr,
	Qtab,
	Qvec,
	Qxtn,
	Qimm,
	Qnkind
} Qkind;

typedef struct Head Head;
typedef struct Heap Heap;
struct Head {
	Qkind qkind;
	unsigned color;
	unsigned inrootset;	/* gc: limit (racily) duplicates on rootsets */
	Heap *heap;
	Head *alink;
	Head *link;
	int state;		/* debugging */
};

typedef struct Head* Val;

typedef struct Closure Closure;
typedef struct Env Env;
typedef struct Expr Expr;
typedef struct VM VM;

int		cqctcallthunk(VM *vm, Closure *cl, Val *v);
Closure*	cqctcompile(Expr *e, Env *env);
void		cqctfini(Env *env);
void		cqctfreeexpr(Expr *e);
void		cqctfreevm(VM *vm);
void		cqctgcprotect(VM *vm, Val v);
void		cqctgcunprotect(VM *vm, Val v);
Env*		cqctinit();
VM*		cqctmkvm(Env *env, int gcthread);
Expr*		cqctparsefile(char *filename);
Expr*		cqctparsestr(char *filename, char *str);
void		cqctprintval(VM *vm, Val v);
char*		cqctsprintval(VM *vm, Val v);
int8_t		cqctval2int8(Val);
int16_t		cqctval2int16(Val);
int32_t		cqctval2int32(Val);
int64_t		cqctval2int64(Val);
char*		cqctval2cstr(Val);
uint8_t		cqctval2uint8(Val);	
uint16_t	cqctval2uint16(Val);
uint32_t	cqctval2uint32(Val);
uint64_t	cqctval2uint64(Val);
