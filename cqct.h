#ifndef _BISONFLAW_CQCT_H_
#define _BISONFLAW_CQCT_H_

extern char cqctflags[];

typedef struct Closure Closure;
typedef struct Env Env;
typedef struct Expr Expr;
typedef struct VM VM;

int		cqctcallthunk(VM *vm, Closure *cl);
Closure*	cqctcompile(Expr *e, Env *env);
void		cqctfini(Env *env);
void		cqctfreeexpr(Expr *e);
void		cqctfreevm(VM *vm);
Env*		cqctinit();
VM*		cqctmkvm(Env *env);
Expr*		cqctparsefile(char *filename);
Expr*		cqctparsestr(char *str);
void		cqctprintvmac(VM *vm);

#endif /* _BISONFLAW_CQCT_H_ */
