#include "sys.h"
#include "util.h"
#include "cqct.h"

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
	fprintf(stderr, "\t-g do not run gc in separate thread\n");

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
	Val v;
	Expr *e;
	VM *vm;
	Env *env;
	char *filename = 0;
	int c, wast;
	struct timeval beg, end;
	int dorepl;
	unsigned len;
	char *inbuf;

	cqctflags['c'] = 1;		/* compile */
	cqctflags['x'] = 1;		/* execute */
	cqctflags['g'] = 1;		/* gc in separate thread */
	dorepl = 1;
	while(EOF != (c = getopt(argc, argv, "bce:hopqtxg"))){
		switch(c){
		case 'o':
		case 'p':
		case 'q':
		case 'b':
		case 't':
			cqctflags[c] = 1;
			break;
		case 'c':
		case 'x':
		case 'g':
			cqctflags[c] = 0;
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

	env = cqctinit();
	if(cqctflags['x']){
		vm = cqctmkvm(env, cqctflags['g']);
		if(vm == 0){
			cqctfini(env);
			return -1;
		}
	}

	if(dorepl)
		if(setvbuf(stdin, 0, _IONBF, 0))
			fatal("cannot clear stdin buffering");

	do{
		inbuf = 0;
		if(dorepl){
			printf("; ");
			fflush(stdout);
			inbuf = readexpr(&len);
			if(inbuf == 0){
				printf("\n");
				break;
			}
		}

		if(filename)
			e = cqctparsefile(filename);
		else if(inbuf){
			e = cqctparsestr(inbuf);
			free(inbuf);
		}
		else
			fatal("bug");
		if(e == 0)
			continue;

		entry = cqctcompile(e, env);
		if(entry == 0){
			cqctfreeexpr(e);
			continue;
		}
		/* now storage for E is managed by cqct storage manager */

		if(cqctflags['x'] == 0)
			continue; /* just compiling */
		
		wast = cqctflags['t'];
		if(wast)
			gettimeofday(&beg, 0);
		if(cqctcallthunk(vm, entry, &v))
			continue; /* error */
		if(wast && cqctflags['t']){
			gettimeofday(&end, 0);
			tvdiff(&end, &beg, &end);
			printf("%lu usec\n",
			       1000000*end.tv_sec+end.tv_usec);
		}
		if(dorepl && v->qkind != Qnil){
			cqctprintval(vm, v);
			printf("\n");
		}
	}while(dorepl);

	if(cqctflags['x'])
		cqctfreevm(vm);
	cqctfini(env);
	return 0;
}
