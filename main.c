#include "sys.h"
#include "util.h"
#include "cqct.h"

static void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s [flags] [-e <file>] \n", argv0);
	fprintf(stderr, "without -e, runs in interactive evaluation mode:\n");
	fprintf(stderr, "\ttype cinquecento expressions on stdin, "
		" followed by newline\n");
	fprintf(stderr, "\tto exit send SIGTERM (^c).\n");
	fprintf(stderr, "\nuser flags:\n");
	fprintf(stderr, "\t-h print this usage\n");
	fprintf(stderr, "\t-t report timing statistics\n");
	fprintf(stderr, "\t-e <file> read input from <file> "
			"instead of stdin\n");
	fprintf(stderr, "\t-m <N> limit heap to <N> megabytes\n");
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
			xfree(buf);
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
	int c;
	struct timeval beg, end;
	int dorepl;
	char opt[256];
	unsigned len;
	char *inbuf, *s;
	u64 heapmax;

	memset(opt, 0, sizeof(opt));
	opt['x'] = 1;		/* execute */
	opt['g'] = 1;		/* gc in separate thread */
	dorepl = 1;
	heapmax = 0;
	while(EOF != (c = getopt(argc, argv, "be:hopqtxgm:"))){
		switch(c){
		case 'o':
		case 'p':
		case 'q':
		case 'b':
			cqctflags[c] = 1;
			break;
		case 't':
			opt[c] = 1;
			break;
		case 'x':
		case 'g':
			opt[c] = 0;
			break;
		case 'e':
			dorepl = 0;
			filename = optarg;
			break;
		case 'm':
			heapmax = atoi(optarg);
			break;
		case 'h':
		case '?':
			usage(argv[0]);
		}
	}

	env = cqctinit(opt['g'], heapmax);
	if(opt['x']){
		vm = cqctmkvm(env);
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
			inbuf = readfile(filename);
		if(inbuf == 0){
			printf("%s: %s\n", filename, strerror(errno));
			continue;
		}
		e = cqctparsestr(inbuf, filename);
		xfree(inbuf);
		if(e == 0)
			continue;
		entry = cqctcompile(e, env);
		if(entry == 0){
			cqctfreeexpr(e);
			continue;
		}
		/* now storage for E is managed by cqct storage manager */

		if(opt['x'] == 0)
			continue; /* just compiling */
		
		if(opt['t'])
			gettimeofday(&beg, 0);
		if(cqctcallthunk(vm, entry, &v))
			continue; /* error */
		if(opt['t']){
			gettimeofday(&end, 0);
			tvdiff(&end, &beg, &end);
			printf("%lu usec\n",
			       1000000*end.tv_sec+end.tv_usec);
		}
		if(dorepl && v->qkind != Qnil){
			s = cqctsprintval(vm, v);
			printf("%s\n", s);
			cqctfreecstr(s);
		}
	}while(dorepl);

	if(opt['x'])
		cqctfreevm(vm);
	cqctfini(env);
	return 0;
}
