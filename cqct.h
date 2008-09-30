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

int		cqctcallfn(VM *vm, Closure *cl, int argc, Val *argv, Val *rv);
int		cqctcallthunk(VM *vm, Closure *cl, Val *rv);
Closure*	cqctcompile(Expr *e, Env *env);
Val		cqctcstrval(char *s);
int		cqctfaulthook(void (*h)(void), int in);
void		cqctfini(Env *env);
void		cqctfreecstr(char *s);
void		cqctfreeexpr(Expr *e);
void		cqctfreevm(VM *vm);
void		cqctgcprotect(VM *vm, Val v);
void		cqctgcunprotect(VM *vm, Val v);
Env*		cqctinit(int gcthread, uint64_t heapmax);
Val		cqctint8val(int8_t);
Val		cqctint16val(int16_t);
Val		cqctint32val(int32_t);
Val		cqctint64val(int64_t);
VM*		cqctmkvm(Env *env);
Expr*		cqctparsefile(char *filename);
Expr*		cqctparsestr(char *str, char *whence);
char*		cqctsprintval(VM *vm, Val v);
Val		cqctuint8val(uint8_t);
Val		cqctuint16val(uint16_t);
Val		cqctuint32val(uint32_t);
Val		cqctuint64val(uint64_t);
int8_t		cqctvalint8(Val);
int16_t		cqctvalint16(Val);
int32_t		cqctvalint32(Val);
int64_t		cqctvalint64(Val);
char*		cqctvalcstr(Val);
uint8_t		cqctvaluint8(Val);	
uint16_t	cqctvaluint16(Val);
uint32_t	cqctvaluint32(Val);
uint64_t	cqctvaluint64(Val);
