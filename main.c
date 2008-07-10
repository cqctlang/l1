#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

static void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s <option>\n", argv0);
	fprintf(stderr, "present input code on stdin.\n");
	fprintf(stderr, "\t-h print this usage\n");
	fprintf(stderr, "\t-p dump IR and object code\n");
	fprintf(stderr, "\t-c do not compile IR code\n");
	fprintf(stderr, "\t-v do not execute object code\n");
	fprintf(stderr, "\t-e <file> read input from <file> instead of stdin\n");
	fprintf(stderr, "\t-t report timing statistics\n");
	exit(0);
}

int
main(int argc, char *argv[])
{
	Closure *entry;
	Expr *rv;
	VM *vm;
	Env *env;
	char *filename = 0;
	int defaultflags = (Fcompile|Fexec);
	int flags = defaultflags;
	int c;
	struct timeval beg, end;

	while(EOF != (c = getopt(argc, argv, "phcvte:"))){
		switch(c){
		case 'p':
			flags |= (Fprintir|Fprintobj);
			break;
		case 'c':
			flags &= ~(Fcompile);
			break;
		case 'e':
			filename = optarg;
			break;
		case 'v':
			flags &= ~(Fexec);
			break;
		case 't':
			flags |= Ftime;
			break;
		case 'h':
			usage(argv[0]);
			break;
		case '?':
			fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			usage(argv[0]);
			break;
		}
	}

	initparse();
	initcompile();
	initvm();

	env = mkenv();

	if(filename == 0)
		filename = stdinname;

	if(0 > doparse(filename))
		exit(0);

	if(flags&Fprintir){
		printf("source:\n");
		printexpr(ctx.el);
		printf("\n");
	}

	dotypes(ctx.el);
	rv = docompilec(ctx.el);
	if(rv == 0){
		freeexpr(ctx.el);
		goto out;
	}
	ctx.el = rv;
	docompile0(ctx.el);
	if(flags&Fprintir){
		printf("compile0:\n");
		printexpr(ctx.el);
		printf("\n");
	}

	if(flags&Fcompile){
		entry = compileentry(ctx.el, env, flags);
		if(flags&Fexec){
			vm = mkvm(env);
			if(vm == 0)
				goto out;
			if(flags&Ftime)
				gettimeofday(&beg, 0);
			if(!waserror(vm)){
				dovm(vm, entry, 0, 0);
				if(flags&Ftime){
					gettimeofday(&end, 0);
					tvdiff(&end, &beg, &end);
					printf("%lu usec\n",
					       1000000*end.tv_sec+end.tv_usec);
				}
				poperror(vm);
			}
			freevm(vm);
		}
	}else
		freeexpr(ctx.el);

out:
	freeenv(env);
	finivm();
	finicompile();
	finiparse();

	return 0;
}
