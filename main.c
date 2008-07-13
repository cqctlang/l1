#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

char flags[256];

static void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s <option>\n", argv0);
	fprintf(stderr, "present input code on stdin.\n");
	fprintf(stderr, "\t-h print this usage\n");
	fprintf(stderr, "\t-p print IR and object code\n");
	fprintf(stderr, "\t-q print expanded cinquecento source\n");
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
	int c;
	struct timeval beg, end;

	flags['c'] = 1;		/* compile */
	flags['x'] = 1;		/* execute */

	while(EOF != (c = getopt(argc, argv, "hpqcxte:"))){
		switch(c){
		case 'p':
		case 'q':
		case 't':
			flags[c] = 1;
			break;
		case 'c':
		case 'x':
			flags[c] = 0;
			break;
		case 'e':
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

	if(filename == 0)
		filename = stdinname;

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
			vm = mkvm(env);
			if(vm == 0)
				goto out;
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
