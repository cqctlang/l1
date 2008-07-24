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
	fprintf(stderr, "\type cinquecento expressions on stdin, "
		" followed by newline\n");
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
	fprintf(stderr, "\t-x do not execute object code\n");

	exit(0);
}

static char*
readexpr(unsigned *cp)
{
	char *buf;
	unsigned len, m, rv, cnt;

	len = 1024;
	buf = xmalloc(len);
	m = len-1;
	cnt = 0;
	while(1){
		if(m == 0){
			buf = xrealloc(buf, len, 2*len);
			len *= 2;
			m = len-1;
		}
		if(0 == fgets(buf+cnt, len, stdin)){
			free(buf);
			return 0;
		}
		rv = strlen(buf);
		if(buf[rv-1] == '\n')
			return buf;
		m -= rv;
		cnt += rv;
	}
}

int
main(int argc, char *argv[])
{
	Closure *entry;
	Expr *rv;
	VM *vm;
	Env *env;
	char *filename = 0;
	int c, wast;
	struct timeval beg, end;
	int dorepl;
	unsigned len;
	char *inbuf;

	flags['c'] = 1;		/* compile */
	flags['x'] = 1;		/* execute */
	dorepl = 1;
	while(EOF != (c = getopt(argc, argv, "bce:hopqtx"))){
		switch(c){
		case 'o':
		case 'p':
		case 'q':
		case 'b':
		case 't':
			flags[c] = 1;
			break;
		case 'c':
		case 'x':
			flags[c] = 0;
			break;
		case 'e':
			dorepl = 0;
			filename = optarg;
			break;
		case 'h':
		case '?':
			usage(argv[0]);
		}
	}

	initparse();
	initcompile();

	env = mkenv();
	if(flags['x']){
		initvm();
		vm = mkvm(env);
		if(vm == 0)
			goto out;
	}

	if(filename == 0)
		filename = stdinname;

	if(dorepl)
		if(setvbuf(stdin, 0, _IONBF, 0))
			fatal("cannot clear stdin buffering");

	do{
		inbuf = 0;
		if(dorepl){
			printf("; ");
			fflush(stdout);
			inbuf = readexpr(&len);
			if(inbuf == 0)
				break;
		}

		if(0 > doparse(filename, inbuf))
			continue;

		if(flags['p']){
			printf("source:\n");
			printexpr(ctx.el);
			printf("\n");
		}

		dotypes(ctx.el);
		rv = docompilec(ctx.el);
		if(rv == 0){
			freeexpr(ctx.el);
			continue;
		}
		ctx.el = rv;
		if(0 > docompile0(ctx.el)){
			freeexpr(ctx.el);
			continue;
		}
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
				wast = flags['t'];
				if(wast)
					gettimeofday(&beg, 0);
				if(!waserror(vm)){
					dovm(vm, entry, 0, 0);
					if(wast && flags['t']){
						gettimeofday(&end, 0);
						tvdiff(&end, &beg, &end);
						printf("%lu usec\n",
						       1000000*end.tv_sec
						       +end.tv_usec);
					}
					poperror(vm);
				}else
					vmreset(vm);
			}
		}else
			freeexpr(ctx.el);

		if(dorepl && flags['x'])
			printvmac(vm);
	}while(dorepl);
out:
	freeenv(env);
	if(flags['x']){
		freevm(vm);
		finivm();
	}
	finicompile();
	finiparse();

	return 0;
}
