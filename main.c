#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

char flags[256];

static void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s [flags] [-e <file>] \n", argv0);
	fprintf(stderr, "without -e, runs in interactive evaluation mode:\n");
	fprintf(stderr, "\tenter cinquecento expressions on stdin, "
			"followed by EOF (^d).\n");
	fprintf(stderr, "\tto exit send SIGTERM (^c).\n");
	fprintf(stderr, "\nuser flags:\n");
	fprintf(stderr, "\t-h print this usage\n");
	fprintf(stderr, "\t-t report timing statistics\n");
	fprintf(stderr, "\t-e <file> read input from <file> "
			"instead of stdin\n");
	fprintf(stderr, "\ndeveloper flags:\n");
	fprintf(stderr, "\t-o dump disassembled object code\n");
	fprintf(stderr, "\t-p dump IR at various stages\n");
	fprintf(stderr, "\t-q dump expanded cinquecento source\n");
	fprintf(stderr, "\t-b dump frame storage\n");
	fprintf(stderr, "\t-c do not compile expanded source\n");
	fprintf(stderr, "\t-v do not execute object code\n");

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
	int c;
	struct timeval beg, end;

	flags['c'] = 1;		/* compile */
	flags['x'] = 1;		/* execute */

	while(EOF != (c = getopt(argc, argv, "hoqbcxte:"))){
		switch(c){
		case 'o':
		case 'q':
		case 't':
		case 'b':
			flags[c] = 1;
			break;
		case 'c':
		case 'x':
			flags[c] = 0;
			break;
		case 'e':
			flags['e'] = 1;
			filename = optarg;
			break;
			break;
		case 'h':
			usage(argv[0]);
		case '?':
			fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			usage(argv[0]);
		}
	}

	initparse();
	initcompile();
	initvm();

	env = mkenv();
	if(flags['x'])
		vm = mkvm(env);
	if(vm == 0)
		goto out;

	if(filename == 0)
		filename = stdinname;

repl:
	if(flags['e'] == 0)
		printf(">>> ");

	if(0 > doparse(filename))
		exit(0);

	if(flags['p']){
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
	if(flags['p']){
		printf("compile0:\n");
		printexpr(ctx.el);
		printf("\n");
	}

	if(flags['q']){
		printf("transformed source:\n");
		printcqct(ctx.el);
		printf("\n");
	}

	if(flags['c']){
		entry = compileentry(ctx.el, env);
		if(flags['x']){
			if(flags['t'])
				gettimeofday(&beg, 0);
			if(!waserror(vm)){
				dovm(vm, entry, 0, 0);
				if(flags['t']){
					gettimeofday(&end, 0);
					tvdiff(&end, &beg, &end);
					printf("%lu usec\n",
					       1000000*end.tv_sec+end.tv_usec);
				}
				poperror(vm);
			}else
				vmreset(vm);
		}
	}else
		freeexpr(ctx.el);

	if(flags['e'] == 0){
		printvmac(vm);
		printf("\n");
		goto repl;
	}

out:
	freeenv(env);
	freevm(vm);
	finivm();
	finicompile();
	finiparse();

	return 0;
}
